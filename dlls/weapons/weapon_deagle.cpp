/***
*
*	Copyright (c) 2020, Jay Foremska.
*
*	Use and modification of this code is allowed as long
*	as credit is provided! Enjoy!
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"

enum deagle_e {
	DEAGLE_IDLE1 = 0,
	DEAGLE_IDLE2,
	DEAGLE_IDLE3,
	DEAGLE_IDLE4,
	DEAGLE_IDLE5,
	DEAGLE_FIRE,
	DEAGLE_FIRE_EMPTY,
	DEAGLE_RELOAD_EMPTY,
	DEAGLE_RELOAD,
	DEAGLE_DRAW,
	DEAGLE_HOLSTER
};

class CDeagle : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 2; }
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void DeagleFire( float flSpread, float flCycleTime, BOOL fUseAutoAim );
	BOOL Deploy( void );
	void Reload( void );
	void WeaponIdle( void );
	int m_iShell;
};

LINK_ENTITY_TO_CLASS( weapon_deagle, CDeagle );


void CDeagle::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_deagle"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_DEAGLE;
	SET_MODEL(ENT(pev), "models/w_desert_eagle.mdl");

	m_iDefaultAmmo = DEAGLE_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}


void CDeagle::Precache( void )
{
	PRECACHE_MODEL("models/v_desert_eagle.mdl");
	PRECACHE_MODEL("models/w_desert_eagle.mdl");
	PRECACHE_MODEL("models/p_9mmhandgun.mdl");

	m_iShell = PRECACHE_MODEL ("models/shell.mdl");// brass shell

	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("items/9mmclip2.wav");

	PRECACHE_SOUND ("weapons/deagle.wav");//handgun
}

int CDeagle::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "357";
	p->iMaxAmmo1 = _357_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = WEAPON_NOCLIP;
	p->iMaxClip = DEAGLE_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 2;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_DEAGLE;
	p->iWeight = DEAGLE_WEIGHT;

	return 1;
}

BOOL CDeagle::Deploy( )
{
	return DefaultDeploy( "models/v_desert_eagle.mdl", "models/p_9mmhandgun.mdl", DEAGLE_DRAW, "onehanded" );
}

void CDeagle::PrimaryAttack( void )
{
	DeagleFire( 0.01, 0.4, true );
}

void CDeagle::DeagleFire( float flSpread , float flCycleTime, BOOL fUseAutoAim )
{
	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = gpGlobals->time + 0.2;
		}

		return;
	}

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	if (m_iClip != 0)
		SendWeaponAnim( DEAGLE_FIRE );
	else
		SendWeaponAnim( DEAGLE_FIRE_EMPTY );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
		
	Vector	vecShellVelocity = m_pPlayer->pev->velocity 
							 + gpGlobals->v_right * RANDOM_FLOAT(50,70) 
							 + gpGlobals->v_up * RANDOM_FLOAT(100,150) 
							 + gpGlobals->v_forward * 25;
	EjectBrass ( pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -12 + gpGlobals->v_forward * 32 + gpGlobals->v_right * 6 , vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL ); 

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	EMIT_SOUND_DYN( ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/deagle.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming;
	
	if ( fUseAutoAim )
	{
		vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );
	}
	else
	{
		vecAiming = gpGlobals->v_forward;
	}

	m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, Vector( flSpread, flSpread, flSpread ), 8192, BULLET_PLAYER_DEAGLE, 1 );
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + flCycleTime;

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );

	m_pPlayer->pev->punchangle.x -= 3.25;
}


void CDeagle::Reload( void )
{
	int iResult;

	if (m_iClip == 0)
		iResult = DefaultReload(DEAGLE_MAX_CLIP, DEAGLE_RELOAD_EMPTY, 1.5 );
	else
		iResult = DefaultReload(DEAGLE_MAX_CLIP, DEAGLE_RELOAD, 1.5 );

	if (iResult)
	{
		m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
	}
}



void CDeagle::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	// only idle if the slid isn't back
	if (m_iClip != 0)
	{
		switch( RANDOM_LONG( 0, 4 ) )
		{
		case 0:
			SendWeaponAnim( DEAGLE_IDLE1 );
			m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT( 90.0, 120.0 );
			break;
		case 1:
			SendWeaponAnim( DEAGLE_IDLE2 );
			m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT( 90.0, 120.0 );
			break;
		case 2:
			SendWeaponAnim( DEAGLE_IDLE3 );
			m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT( 90.0, 120.0 );
			break;
		case 3:
			SendWeaponAnim( DEAGLE_IDLE4 );
			m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT( 90.0, 120.0 );
			break;
		case 4:
			SendWeaponAnim( DEAGLE_IDLE5 );
			m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT( 90.0, 120.0 );
			break;
		}
	}
}
