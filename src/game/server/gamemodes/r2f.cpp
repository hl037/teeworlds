/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>

#include <game/mapitems.h>

#include <game/server/entities/character.h>
#include <game/server/entities/flag.h>
#include <game/server/player.h>
#include <game/server/gamecontext.h>
#include "r2f.h"

CGameControllerR2F::CGameControllerR2F(class CGameContext *pGameServer)
: IGameController(pGameServer)
{
	m_apFlags[0] = 0;
	m_apFlags[1] = 0;
	m_pGameType = "R2F";
	m_GameFlags = GAMEFLAG_TEAMS|GAMEFLAG_FLAGS;
}

bool CGameControllerR2F::OnEntity(int Index, vec2 Pos)
{
	if(IGameController::OnEntity(Index, Pos))
		return true;
   

	int Team = -1;
	if(Index == ENTITY_FLAGSTAND_RED) Team = TEAM_RED;
	if(Index == ENTITY_FLAGSTAND_BLUE) Team = TEAM_BLUE;
	if(Team == -1 || m_apFlags[Team])
		return false;

   GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG,"game",  "Entity flag");
	CFlag *F = new CFlag(&GameServer()->m_World, Team);
	F->m_StandPos = Pos;
	F->m_Pos = Pos;
	m_apFlags[Team] = F;
	GameServer()->m_World.InsertEntity(F);
   return true;
}

int CGameControllerR2F::OnCharacterDeath(CCharacter * pVictim, CPlayer * pKiller, int Weapon)
{
	IGameController::OnCharacterDeath(pVictim, pKiller, Weapon);
   if(Weapon == WEAPON_SELF)
		pVictim->GetPlayer()->m_RespawnTick = Server()->Tick()+Server()->TickSpeed()*3.0f;
   else if(Weapon == ENTITY_FLAGSTAND_RED || Weapon == ENTITY_FLAGSTAND_BLUE)
      pVictim->GetPlayer()->m_RespawnTick = Server()->Tick()+min(1.0f,Server()->TickSpeed()*0.1f);
   else
      pVictim->GetPlayer()->m_RespawnTick = Server()->Tick()+Server()->TickSpeed()*2.0f;
	return 0;
}

void CGameControllerR2F::DoWincheck()
{
	if(m_GameOverTick == -1 && !m_Warmup)
	{
		// check score win condition
		if((g_Config.m_SvScorelimit > 0 && (m_aTeamscore[TEAM_RED] >= g_Config.m_SvScorelimit || m_aTeamscore[TEAM_BLUE] >= g_Config.m_SvScorelimit)) ||
			(g_Config.m_SvTimelimit > 0 && (Server()->Tick()-m_RoundStartTick) >= g_Config.m_SvTimelimit*Server()->TickSpeed()*60))
		{
			if(m_SuddenDeath)
			{
				if(m_aTeamscore[TEAM_RED]/100 != m_aTeamscore[TEAM_BLUE]/100)
					EndRound();
			}
			else
			{
				if(m_aTeamscore[TEAM_RED] != m_aTeamscore[TEAM_BLUE])
					EndRound();
				else
					m_SuddenDeath = 1;
			}
		}
	}
}

void CGameControllerR2F::Snap(int SnappingClient)
{
	IGameController::Snap(SnappingClient);

	CNetObj_GameData *pGameDataObj = (CNetObj_GameData *)Server()->SnapNewItem(NETOBJTYPE_GAMEDATA, 0, sizeof(CNetObj_GameData));
	if(!pGameDataObj)
		return;

	pGameDataObj->m_TeamscoreRed = m_aTeamscore[TEAM_RED];
	pGameDataObj->m_TeamscoreBlue = m_aTeamscore[TEAM_BLUE];

	pGameDataObj->m_FlagCarrierRed = FLAG_ATSTAND;
   pGameDataObj->m_FlagCarrierBlue = FLAG_ATSTAND;
}

void CGameControllerR2F::Tick()
{
	IGameController::Tick();

	if(GameServer()->m_World.m_ResetRequested || GameServer()->m_World.m_Paused)
		return;

	for(int fi = 0; fi < 2; fi++)
	{
		CFlag *F = m_apFlags[fi];

		if(!F)
			continue;

      CCharacter *apCloseCCharacters[MAX_CLIENTS];
      int Num = GameServer()->m_World.FindEntities(F->m_Pos, CFlag::ms_PhysSize, (CEntity**)apCloseCCharacters, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
      for(int i = 0; i < Num; i++)
      {
         if(!apCloseCCharacters[i]->IsAlive() || apCloseCCharacters[i]->GetPlayer()->GetTeam() == TEAM_SPECTATORS || GameServer()->Collision()->IntersectLine(F->m_Pos, apCloseCCharacters[i]->m_Pos, NULL, NULL))
            continue;

         if(apCloseCCharacters[i]->GetPlayer()->GetTeam() != F->m_Team)
         {
            // score
            if(F->m_AtStand)
            {
               m_aTeamscore[fi^1]+=10;
            }

            apCloseCCharacters[i]->GetPlayer()->m_Score += 10;

            char aBuf[256];
            str_format(aBuf, sizeof(aBuf), "player scored='%d:%s'",
               apCloseCCharacters[i]->GetPlayer()->GetCID(),
               Server()->ClientName(apCloseCCharacters[i]->GetPlayer()->GetCID()));
            GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
            
            apCloseCCharacters[i]->Die(0, (i == TEAM_RED ? ENTITY_FLAGSTAND_RED : ENTITY_FLAGSTAND_BLUE), SOUND_CTF_CAPTURE);
         }
      }
   }
}
