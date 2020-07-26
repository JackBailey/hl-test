/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"

enum shotgun_e {
	SHOTGUN_IDLE = 0,
	SHOTGUN_FIRE,
	SHOTGUN_FIRE2,
	SHOTGUN_RELOAD,
	SHOTGUN_PUMP,
	SHOTGUN_START_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_HOLSTER,
	SHOTGUN_IDLE4,
	SHOTGUN_IDLE_DEEP
};

class CShotgun : public CBasePlayerWeapon
{
public:
	int		Save( CSave &save );
	int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	void Spawn( void );
	void Precache( void );
	int iItemSlot( ) { return 3; }
	int GetItemInfo(ItemInfo *p);
	int AddToPlayer( CBasePlayer *pPlayer );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( );
	void Reload( void );
	void WeaponIdle( void );
	int m_fInReload;
	float m_flNextReload;
	int m_iShell;
	float m_flPumpTime;

	// jay - new shell ejection
	bool m_bFiredAlt;
	Vector VecShellVelocity;
	Vector VecShellVelocity1;
	Vector VecShellVelocity2;
	void ItemPostFrame( void );
	void EjectShells( void );
	void Holster( void );
};
LINK_ENTITY_TO_CLASS( weapon_shotgun, CShotgun );


TYPEDESCRIPTION	CShotgun::m_SaveData[] = 
{
	DEFINE_FIELD( CShotgun, m_flNextReload, FIELD_TIME ),
	DEFINE_FIELD( CShotgun, m_fInReload, FIELD_INTEGER ),
	DEFINE_FIELD( CShotgun, m_flNextReload, FIELD_TIME ),
	// DEFINE_FIELD( CShotgun, m_iShell, FIELD_INTEGER ),
	DEFINE_FIELD( CShotgun, m_flPumpTime, FIELD_TIME ),
	DEFINE_FIELD( CShotgun, m_bFiredAlt, FIELD_BOOLEAN ),	// jay - new shell ejection
};
IMPLEMENT_SAVERESTORE( CShotgun, CBasePlayerWeapon );



void CShotgun::Spawn( )
{
	Precache( );
	m_iId = WEAPON_SHOTGUN;
	SET_MODEL(ENT(pev), "models/w_shotgun.mdl");

	m_iDefaultAmmo = SHOTGUN_DEFAULT_GIVE;

	FallInit();// get ready to fall
}


void CShotgun::Precache( void )
{
	PRECACHE_MODEL("models/v_shotgun.mdl");
	PRECACHE_MODEL("models/w_shotgun.mdl");
	PRECACHE_MODEL("models/p_shotgun.mdl");

	m_iShell = PRECACHE_MODEL ("models/shotgunshell.mdl");// shotgun shell

	PRECACHE_SOUND("items/9mmclip1.wav");              

	PRECACHE_SOUND ("weapons/dbarrel1.wav");//shotgun
	PRECACHE_SOUND ("weapons/sbarrel1.wav");//shotgun

	PRECACHE_SOUND ("weapons/reload1.wav");	// shotgun reload
	PRECACHE_SOUND ("weapons/reload2.wav");	// jay - unused shotgun reload
	PRECACHE_SOUND ("weapons/reload3.wav");	// shotgun reload
	
	PRECACHE_SOUND ("weapons/357_cock1.wav"); // gun empty sound
	PRECACHE_SOUND ("weapons/foley/sg_pump.wav");	// cock gun
}

int CShotgun::AddToPlayer( CBasePlayer *pPlayer )
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


int CShotgun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "buckshot";
	p->iMaxAmmo1 = BUCKSHOT_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = SHOTGUN_MAX_CLIP;
	p->iSlot = 2;
	p->iPosition = 1;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SHOTGUN;
	p->iWeight = SHOTGUN_WEIGHT;

	return 1;
}



BOOL CShotgun::Deploy( )
{
	return DefaultDeploy( "models/v_shotgun.mdl", "models/p_shotgun.mdl", SHOTGUN_DRAW, "shotgun" );
}

// jay - new shell ejection
void CShotgun::EjectShells( void )
{
	if (!m_bFiredAlt)
		EjectBrass (m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -12 + gpGlobals->v_forward * 20 + gpGlobals->v_right * 4, 
					VecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHOTSHELL);
	else
	{
		EjectBrass (m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -12 + gpGlobals->v_forward * 20 + gpGlobals->v_right * 4, 
					VecShellVelocity1, pev->angles.y, m_iShell, TE_BOUNCE_SHOTSHELL);
		EjectBrass (m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -12 + gpGlobals->v_forward * 20 + gpGlobals->v_right * 4, 
					VecShellVelocity2, pev->angles.y, m_iShell, TE_BOUNCE_SHOTSHELL);
		m_bFiredAlt = false;
	}

	m_flPumpTime = 0;
}

void CShotgun::ItemPostFrame( void )
{
	if (m_flPumpTime && m_flPumpTime < gpGlobals->time)
		EjectShells();

	CBasePlayerWeapon::ItemPostFrame();	// call vanilla ItemPostFrame as well
}

void CShotgun::Holster( )
{
	if (m_flPumpTime)
	{
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/foley/sg_pump.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		EjectShells();
	}

	CBasePlayerWeapon::Holster();	// call vanilla Holster as well, doesn't *seem* to do anything but do it just in case
}

void CShotgun::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.15;
		return;
	}

	if (m_iClip <= 0)
	{
		Reload( );
		if (m_iClip == 0)
			PlayEmptySound( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	SendWeaponAnim( SHOTGUN_FIRE );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	VecShellVelocity = m_pPlayer->pev->velocity 
					 + gpGlobals->v_right * RANDOM_FLOAT(50,70) 
					 + gpGlobals->v_up * RANDOM_FLOAT(100,150) 
					 + gpGlobals->v_forward * 25;

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/sbarrel1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0x1f));
	

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
 
	m_pPlayer->FireBullets( 6, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0 );	// jay - always use singleplayer spread

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	//if (m_iClip != 0)
		m_flPumpTime = gpGlobals->time + 0.5;

	m_flNextPrimaryAttack = gpGlobals->time + 1;	// jay - changed delay from 0.75
	m_flNextSecondaryAttack = gpGlobals->time + 1;	// jay - changed delay from 0.75
	if (m_iClip != 0)
		m_flTimeWeaponIdle = gpGlobals->time + 5.0;
	else
		m_flTimeWeaponIdle = 1;	// jay - changed delay from 0.75
	m_fInReload = 0;

	m_pPlayer->pev->punchangle.x -= 5;
}


void CShotgun::SecondaryAttack( void )
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.15;
		return;
	}

	if (m_iClip <= 1)
	{
		Reload( );
		PlayEmptySound( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip -= 2;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	SendWeaponAnim( SHOTGUN_FIRE2 );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
		
	VecShellVelocity1 = m_pPlayer->pev->velocity 
					 + gpGlobals->v_right * RANDOM_FLOAT(50,70) 
					 + gpGlobals->v_up * RANDOM_FLOAT(100,150) 
					 + gpGlobals->v_forward * 25;
	VecShellVelocity2 = m_pPlayer->pev->velocity 
					 + gpGlobals->v_right * RANDOM_FLOAT(50,70) 
					 + gpGlobals->v_up * RANDOM_FLOAT(100,150) 
					 + gpGlobals->v_forward * 15;

	m_bFiredAlt = true;	// jay - new shell ejection

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dbarrel1.wav", RANDOM_FLOAT(0.98, 1.0), ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));
	
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	
	m_pPlayer->FireBullets( 12, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0 );	// jay - always use singleplayer spread

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	// if (m_iClip != 0)
		m_flPumpTime = gpGlobals->time + 0.95;

	m_flNextPrimaryAttack = gpGlobals->time + 1.6;	// jay - changed delay from 1.5
	m_flNextSecondaryAttack = gpGlobals->time + 1.6;	// jay - changed delay from 1.5
	if (m_iClip != 0)
		m_flTimeWeaponIdle = gpGlobals->time + 6.0;
	else
		m_flTimeWeaponIdle = 1.6;	// jay - changed delay from 1.5

	m_fInReload = 0;

	m_pPlayer->pev->punchangle.x -= 10;
}


void CShotgun::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == SHOTGUN_MAX_CLIP)
		return;

	if (m_flNextReload > gpGlobals->time)
		return;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > gpGlobals->time)
		return;

	// check to see if we're ready to reload
	if (m_fInReload == 0)
	{
		SendWeaponAnim( SHOTGUN_START_RELOAD );
		m_fInReload = 1;
		m_pPlayer->m_flNextAttack = gpGlobals->time + 0.75;	// jay - changed delay from 0.6
		m_flTimeWeaponIdle = gpGlobals->time + 0.75;	// jay - changed delay from 0.6
		m_flNextPrimaryAttack = gpGlobals->time + 0.75;	// jay - changed delay from 1
		m_flNextSecondaryAttack = gpGlobals->time + 0.75;	// jay - changed delay from 1
		return;
	}
	else if (m_fInReload == 1)
	{
		if (m_flTimeWeaponIdle > gpGlobals->time)
			return;
		// was waiting for gun to move to side
		m_fInReload = 2;

		switch (RANDOM_LONG (0, 2))
		{
		case 0:
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
			break;
		case 1:
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
			break;
		case 2:
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload3.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
			break;
		}

		SendWeaponAnim( SHOTGUN_RELOAD );

		m_flNextReload = gpGlobals->time + 0.5;
		m_flTimeWeaponIdle = gpGlobals->time + 0.5;
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInReload = 1;
	}
}


void CShotgun::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if (m_flTimeWeaponIdle < gpGlobals->time)
	{
		if (m_iClip == 0 && m_fInReload == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			Reload( );
		}
		else if (m_fInReload != 0)
		{
			if (m_iClip != 8 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			{
				Reload( );
			}
			else
			{
				// reload debounce has timed out
				SendWeaponAnim( SHOTGUN_PUMP );
				m_fInReload = 0;
				m_flTimeWeaponIdle = gpGlobals->time + 1;	// jay - changed delay from 1.5
			}
		}
		else
		{
			// jay - new idle anim handling; a switch removes the need for iAnim and flRand
			switch (RANDOM_LONG (0, 2))
			{
			case 0:
				m_flTimeWeaponIdle = gpGlobals->time + (20.0/9.0);
				SendWeaponAnim( SHOTGUN_IDLE );
				break;
			case 1:
				m_flTimeWeaponIdle = gpGlobals->time + (20.0/9.0);
				SendWeaponAnim( SHOTGUN_IDLE4 );
				break;
			case 2:
				m_flTimeWeaponIdle = gpGlobals->time + (60.0/12.0);
				SendWeaponAnim( SHOTGUN_IDLE_DEEP );
				break;
			}
		}
	}
}



class CShotgunAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_shotbox.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_shotbox.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_BUCKSHOTBOX_GIVE, "buckshot", BUCKSHOT_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_buckshot, CShotgunAmmo );


