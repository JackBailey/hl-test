/***
*	
*	Copyright (c) 2020, Jay Foremska.
*	
*	Use and modification of this code is allowed as long
*	as credit is provided! Enjoy!
*	
****/
//=========================================================
// Panthereye - big, red, jumpy boye
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"monsters.h"

//=========================================================
// Animation Events
//=========================================================
#define	PANTHEREYE_AE_MELEE1	( 1 )
#define	PANTHEREYE_AE_MELEE2	( 2 )
#define	PANTHEREYE_AE_LUNGE		( 3 )

#define	PANTHEREYE_FLINCH_DELAY	3	// at most one flinch every X seconds

//=========================================================
// Class Definition
//=========================================================
class CPanthereye : public CBaseMonster
{
public:
	void Spawn (void);
	void Precache (void);
	int Classify (void);
	void SetYawSpeed (void);
	void AlertSound (void);
	void IdleSound (void);
	void AttackSound (void);
	void PainSound (void);
	void HandleAnimEvent (MonsterEvent_t* pEvent);
	void Killed (entvars_t* pevAttacker, int iGib);
	void Melee (void);
	void Lunge (void);

	static const char *pAlertSounds[];
	static const char *pIdleSounds[];
	static const char *pAttackSounds[];
	static const char *pPainSounds[];
	static const char *pSwingSounds[];
	static const char *pHitSounds[];

	BOOL CheckRangeAttack1 (float flDot, float flDist);
	BOOL CheckMeleeAttack1 (float flDot, float flDist);

	virtual int		Save(CSave& save);
	virtual int		Restore(CRestore& restore);
	static	TYPEDESCRIPTION m_SaveData[];

private:
	int m_iLungeState;
};

TYPEDESCRIPTION CPanthereye::m_SaveData[] =
{
	DEFINE_FIELD (CPanthereye, m_iLungeState, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE (CPanthereye, CBaseMonster);

LINK_ENTITY_TO_CLASS ( monster_panthereye, CPanthereye );

//=========================================================
// Spawn
//=========================================================
void CPanthereye::Spawn( )
{
	Precache();

	SET_MODEL (ENT(pev), "models/panther.mdl");

	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);
}

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CPanthereye::Classify( void )
{
	return	CLASS_ALIEN_MONSTER;
}
