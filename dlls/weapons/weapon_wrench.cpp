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
#include "gamerules.h"

#define WRENCH_BODYHIT_VOLUME	512
#define WRENCH_WALLHIT_VOLUME	128

#define	WRENCH_DELAY	0.75


enum wrench_e
{
	WRENCH_IDLE1 = 0,
	WRENCH_IDLE2,
	WRENCH_IDLE3,
	WRENCH_DRAW,
	WRENCH_HOLSTER,
	WRENCH_ATTACK1HIT,
	WRENCH_ATTACK1MISS,
	WRENCH_ATTACK2HIT,
	WRENCH_ATTACK2MISS,
	WRENCH_ATTACK3HIT,
	WRENCH_ATTACK3MISS,
	WRENCH_ALT_WINDUP,
	WRENCH_ALT_HIT,
	WRENCH_ALT_MISS
};


class CWrench : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 1; }
	void EXPORT SwingAgain( void );
	void EXPORT Smack( void );
	int GetItemInfo(ItemInfo *p);
	
	void FindHullIntersection( const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity );
	void PrimaryAttack( void );
	BOOL Swing( bool bFirst );
	BOOL Deploy( void );
	void Holster( void );

	int m_iSwing;
	TraceResult m_trHit;
};

LINK_ENTITY_TO_CLASS( weapon_wrench, CWrench );

void CWrench::Spawn( )
{
	Precache();
	m_iId = WEAPON_WRENCH;
	SET_MODEL(ENT(pev), "models/w_pipe_wrench.mdl");
	m_iClip = WEAPON_NOCLIP;

	FallInit();// get ready to fall down.
}

void CWrench::Precache( )
{
	PRECACHE_MODEL( "models/w_pipe_wrench.mdl" );
	PRECACHE_MODEL( "models/v_pipe_wrench.mdl" );
	PRECACHE_MODEL( "models/p_crowbar.mdl" );

	PRECACHE_SOUND( "weapons/pwrench_miss1.wav" );
	PRECACHE_SOUND( "weapons/pwrench_miss2.wav" );
	PRECACHE_SOUND( "weapons/pwrench_hit1.wav" );
	PRECACHE_SOUND( "weapons/pwrench_hit2.wav" );
	PRECACHE_SOUND( "weapons/pwrench_hitbod1.wav" );
	PRECACHE_SOUND( "weapons/pwrench_hitbod2.wav" );
	PRECACHE_SOUND( "weapons/pwrench_hitbod3.wav" );

	PRECACHE_SOUND( "weapons/pwrench_big_miss.wav" );
	PRECACHE_SOUND( "weapons/pwrench_big_hit1.wav" );
	PRECACHE_SOUND( "weapons/pwrench_big_hit2.wav" );
	PRECACHE_SOUND( "weapons/pwrench_big_hitbod1.wav" );
	PRECACHE_SOUND( "weapons/pwrench_big_hitbod2.wav" );
}

int CWrench::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = WEAPON_NOCLIP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = WEAPON_NOCLIP;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iId = WEAPON_WRENCH;
	p->iWeight = WRENCH_WEIGHT;
	return 1;
}

BOOL CWrench::Deploy( )
{
	return DefaultDeploy( "models/v_pipe_wrench.mdl", "models/p_crowbar.mdl", WRENCH_DRAW, "crowbar" );
}

void CWrench::Holster( )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
	SendWeaponAnim( WRENCH_HOLSTER );
}

void CWrench::FindHullIntersection( const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity )
{
	int			i, j, k;
	float		distance;
	float		*minmaxs[2] = {mins, maxs};
	TraceResult tmpTrace;
	Vector		vecHullEnd = tr.vecEndPos;
	Vector		vecEnd;

	distance = 1e6f;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	UTIL_TraceLine( vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &tmpTrace );
	if ( tmpTrace.flFraction < 1.0 )
	{
		tr = tmpTrace;
		return;
	}

	for ( i = 0; i < 2; i++ )
	{
		for ( j = 0; j < 2; j++ )
		{
			for ( k = 0; k < 2; k++ )
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];

				UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, pEntity, &tmpTrace );
				if ( tmpTrace.flFraction < 1.0 )
				{
					float thisDistance = (tmpTrace.vecEndPos - vecSrc).Length();
					if ( thisDistance < distance )
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}

void CWrench::PrimaryAttack( )
{
	if( !Swing( true ) )
	{
		SetThink(&CWrench::SwingAgain );
		pev->nextthink = UTIL_WeaponTimeBase() + 0.1;
	}
}

void CWrench::Smack( )
{
	DecalGunshot( &m_trHit, BULLET_PLAYER_CROWBAR );
}

void CWrench::SwingAgain( )
{
	Swing( false );
}

BOOL CWrench::Swing( bool bFirst )
{
	bool bDidHit = false;

	TraceResult tr;

	UTIL_MakeVectors (m_pPlayer->pev->v_angle);
	Vector vecSrc	= m_pPlayer->GetGunPosition( );
	Vector vecEnd	= vecSrc + gpGlobals->v_forward * 32;

	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

	if ( tr.flFraction >= 1.0 )
	{
		UTIL_TraceHull( vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT( m_pPlayer->pev ), &tr );
		if ( tr.flFraction < 1.0 )
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
			if ( !pHit || pHit->IsBSPModel() )
				FindHullIntersection( vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict() );
			vecEnd = tr.vecEndPos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}

	if ( tr.flFraction >= 1.0 )
	{
		if (bFirst)
		{
			// miss
			switch( (m_iSwing++) % 3 )
			{
			case 0:
				SendWeaponAnim( WRENCH_ATTACK1MISS ); break;
			case 1:
				SendWeaponAnim( WRENCH_ATTACK2MISS ); break;
			case 2:
				SendWeaponAnim( WRENCH_ATTACK3MISS ); break;
			}
			m_flNextPrimaryAttack = gpGlobals->time + WRENCH_DELAY;
			m_flNextSecondaryAttack = gpGlobals->time + WRENCH_DELAY;

			// play wiff or swish sound
			switch( RANDOM_LONG( 0, 1 ) )
			{
			case 0:
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/pwrench_miss1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				break;
			case 1:
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/pwrench_miss2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				break;
			}

			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		}
	}
	else
	{
		// hit
		bDidHit = true;

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		switch( (m_iSwing++) % 3 )
		{
		case 0:
			SendWeaponAnim( WRENCH_ATTACK1HIT ); break;
		case 1:
			SendWeaponAnim( WRENCH_ATTACK2HIT ); break;
		case 2:
			SendWeaponAnim( WRENCH_ATTACK3HIT ); break;
		}

		m_flNextPrimaryAttack = gpGlobals->time + WRENCH_DELAY;
		m_flNextSecondaryAttack = gpGlobals->time + WRENCH_DELAY;

		ClearMultiDamage( );
		pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgWrench, gpGlobals->v_forward, &tr, DMG_CLUB );
		ApplyMultiDamage( m_pPlayer->pev, m_pPlayer->pev );

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		int fHitWorld = TRUE;

		if (pEntity)
		{
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
			{
				switch( RANDOM_LONG( 0, 2 ) )
				{
				case 0:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/pwrench_hitbod1.wav", VOL_NORM, ATTN_NORM);
					break;
				case 1:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/pwrench_hitbod2.wav", VOL_NORM, ATTN_NORM);
					break;
				case 2:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/pwrench_hitbod3.wav", VOL_NORM, ATTN_NORM);
					break;
				}

				m_pPlayer->m_iWeaponVolume = WRENCH_BODYHIT_VOLUME;

				if (!pEntity->IsAlive() )
					return TRUE;
				else
					flVol = 0.1;

				fHitWorld = FALSE;
			}
		}

		// play texture hit sound
		// UNDONE: Calculate the correct point of intersection when we hit with the hull instead of the line

		if (fHitWorld)
		{
			float fvolbar = TEXTURETYPE_PlaySound(&tr, vecSrc, vecSrc + (vecEnd-vecSrc)*2, BULLET_PLAYER_CROWBAR);

			if ( g_pGameRules->IsMultiplayer() )
			{
				// override the volume here, cause we don't play texture sounds in multiplayer, 
				// and fvolbar is going to be 0 from the above call.

				fvolbar = 1;
			}

			// also play wrench strike
			switch( RANDOM_LONG( 0, 1 ) )
			{
			case 0:
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/pwrench_hit1.wav", VOL_NORM, ATTN_NORM);
				break;
			case 1:
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/pwrench_hit2.wav", VOL_NORM, ATTN_NORM);
				break;
			}
		}

		// delay the decal a bit
		m_trHit = tr;
		SetThink( &CWrench::Smack );
		pev->nextthink = gpGlobals->time + 0.2;

		m_pPlayer->m_iWeaponVolume = flVol * WRENCH_WALLHIT_VOLUME;
	}
	return bDidHit;
}
