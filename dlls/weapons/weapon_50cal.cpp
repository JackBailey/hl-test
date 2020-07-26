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
#include "soundent.h"
#include "gamerules.h"

enum _50cal_e
{
	_50CAL_IDLE1 = 0,
	_50CAL_IDLE2,
	_50CAL_HOLSTER,
	_50CAL_DRAW,
	_50CAL_FIRE
};

#define	FIRE_DELAY	0.1;


class C50cal : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 3; }
	int GetItemInfo(ItemInfo *p);
	int AddToPlayer( CBasePlayer *pPlayer );

	void PrimaryAttack( void );
	BOOL Deploy( void );
	void WeaponIdle( void );
	int m_iShell;
	int m_iLink;

	BOOL HasAmmo( void )
	{
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
			return FALSE;
		return TRUE;
	}

	void UseAmmo( int count )
	{
		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= count )
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= count;
		else
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
	}
};

LINK_ENTITY_TO_CLASS( weapon_50cal, C50cal );


void C50cal::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_50cal"); // hack to allow for old names
	Precache( );
	SET_MODEL(ENT(pev), "models/w_50cal.mdl");
	m_iId = WEAPON_50CAL;

	m_iDefaultAmmo = _50CAL_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}


void C50cal::Precache( void )
{
	PRECACHE_MODEL("models/v_50cal.mdl");
	PRECACHE_MODEL("models/w_50cal.mdl");
	PRECACHE_MODEL("models/p_9mmAR.mdl");

	m_iShell = PRECACHE_MODEL ("models/saw_shell.mdl");
	m_iLink = PRECACHE_MODEL ("models/saw_link.mdl");
	
	PRECACHE_SOUND ("weapons/50cal.wav");

	PRECACHE_SOUND ("weapons/357_cock1.wav");
}

int C50cal::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "50cal";
	p->iMaxAmmo1 = _50CAL_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = WEAPON_NOCLIP;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 2;
	p->iPosition = 3;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_50CAL;
	p->iWeight = _50CAL_WEIGHT;

	return 1;
}

int C50cal::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

BOOL C50cal::Deploy( )
{
	return DefaultDeploy( "models/v_50cal.mdl", "models/p_9mmAR.mdl", _50CAL_DRAW, "mp5" );
}


void C50cal::PrimaryAttack()
{
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.15;
		return;
	}

	if ( !HasAmmo() )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = gpGlobals->time + 0.15;
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	UseAmmo( 1 );

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	SendWeaponAnim( _50CAL_FIRE );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/50cal.wav", VOL_NORM, ATTN_NORM, 0, RANDOM_FLOAT(95, 105));

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	Vector	vecShellVelocity = m_pPlayer->pev->velocity 
							 + gpGlobals->v_right * RANDOM_FLOAT(50,70) 
							 + gpGlobals->v_up * RANDOM_FLOAT(100,150) 
							 + gpGlobals->v_forward * 25;
	EjectBrass ( pev->origin + m_pPlayer->pev->view_ofs
					+ gpGlobals->v_up * -12 
					+ gpGlobals->v_forward * 20 
					+ gpGlobals->v_right * 4, vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL);

	Vector	vecLinkVelocity = m_pPlayer->pev->velocity 
							 + gpGlobals->v_right * RANDOM_FLOAT(50,70) 
							 + gpGlobals->v_up * RANDOM_FLOAT(100,150) 
							 + gpGlobals->v_forward * 25;
	EjectBrass ( pev->origin + m_pPlayer->pev->view_ofs
					+ gpGlobals->v_up * -12 
					+ gpGlobals->v_forward * 20 
					+ gpGlobals->v_right * 4, vecLinkVelocity, pev->angles.y, m_iLink, TE_BOUNCE_NULL);
	
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	
	m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_50CAL, 1 );

	if ( !HasAmmo() )
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = m_flNextPrimaryAttack + FIRE_DELAY;
	if (m_flNextPrimaryAttack < gpGlobals->time)
		m_flNextPrimaryAttack = gpGlobals->time + FIRE_DELAY;

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );

	m_pPlayer->pev->punchangle.x = RANDOM_FLOAT( -5.0, 5.0 );
	m_pPlayer->pev->punchangle.y = RANDOM_FLOAT( -2.5, 2.5 );
}


void C50cal::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	switch (RANDOM_LONG (0, 1))
	{
	case 0:
		SendWeaponAnim( _50CAL_IDLE1 );
		break;
	case 1:
		SendWeaponAnim( _50CAL_IDLE2 );
		break;
	}

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (20, 25);
}



class C50calAmmoClip : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_50cal_ammo.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_50cal_ammo.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = (pOther->GiveAmmo( AMMO_50CALBOX_GIVE, "50cal", _50CAL_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};

LINK_ENTITY_TO_CLASS( ammo_50cal, C50calAmmoClip );
