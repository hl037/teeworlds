/* (c) Léo Flaventin Hauchecorne. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>

#include <game/mapitems.h>

#include <game/server/entities/character.h>
#include <game/server/entities/flag.h>
#include <game/server/entities/lasercell.h>
#include <game/server/player.h>
#include <game/server/gamecontext.h>
#include "ktp.h"

CGameControllerKTP::CGameControllerKTP(class CGameContext *pGameServer)
: IGameController(pGameServer)
{
	m_pGameType = "KTP";
	m_GameFlags = GAMEFLAG_TEAMS|GAMEFLAG_FLAGS;
	m_NbSpot = 0;
}

bool CGameControllerKTP::OnEntity(int Index, vec2 Pos)
{
	if(IGameController::OnEntity(Index, Pos))
		return true;

	if(Index != ENTITY_STAY || m_NbSpot == MAX_STAY_SPOTS)
	{
		return false;
	}
	m_Spots[m_NbSpot].m_Cell = new CLaserCell(&GameServer()->m_World, Pos, 3);
	m_Spots[m_NbSpot].m_Team = -1;
	m_Spots[m_NbSpot].m_Ticks = 0;
	GameServer()->m_World.InsertEntity(m_Spots[m_NbSpot].m_Cell);
	++m_NbSpot;
	return true;
}

void CGameControllerKTP::DoWincheck()
{
	if(m_GameOverTick == -1 && !m_Warmup)
	{
		// check score win condition
		if((g_Config.m_SvScorelimit > 0 && (m_aTeamscore[TEAM_RED] >= g_Config.m_SvScorelimit || m_aTeamscore[TEAM_BLUE] >= g_Config.m_SvScorelimit)) ||
			(g_Config.m_SvTimelimit > 0 && (Server()->Tick()-m_RoundStartTick) >= g_Config.m_SvTimelimit*Server()->TickSpeed()*60))
		{
			if(m_aTeamscore[TEAM_RED]/10 != m_aTeamscore[TEAM_BLUE]/10)
				EndRound();
			else
				m_SuddenDeath = 1;
		}
	}
}

bool CGameControllerKTP::CanBeMovedOnBalance(int ClientID)
{
	CCharacter* Character = GameServer()->m_apPlayers[ClientID]->GetCharacter();
	if(Character)
	{
		for(int i = 0; i < m_NbSpot; ++i)
		{
			if(m_Spots[i].m_Cell->IsInCell(Character->m_Pos))
				return false;
		}
	}
	return true;
}

void CGameControllerKTP::Snap(int SnappingClient)
{
	IGameController::Snap(SnappingClient);

	CNetObj_GameData *pGameDataObj = (CNetObj_GameData *)Server()->SnapNewItem(NETOBJTYPE_GAMEDATA, 0, sizeof(CNetObj_GameData));
	if(!pGameDataObj)
		return;

	pGameDataObj->m_TeamscoreRed = m_aTeamscore[TEAM_RED];
	pGameDataObj->m_TeamscoreBlue = m_aTeamscore[TEAM_BLUE];

	pGameDataObj->m_FlagCarrierRed = 0;
	pGameDataObj->m_FlagCarrierBlue = 0;
}

void CGameControllerKTP::Tick()
{
	IGameController::Tick();

	if(GameServer()->m_World.m_ResetRequested || GameServer()->m_World.m_Paused)
		return;
	
	for(int i = 0; i < m_NbSpot; ++i)
	{
		CCharacter *apCloseCCharacters[MAX_CLIENTS/2+1];
		int Num = GameServer()->m_World.FindEntities(m_Spots[i].m_Cell->m_Pos, CFlag::ms_PhysSize, (CEntity**)apCloseCCharacters, MAX_CLIENTS/2+1, CGameWorld::ENTTYPE_CHARACTER);
		if(Num == 0 || !m_Spots[i].m_Cell->IsInCell(apCloseCCharacters[0]->m_Pos))
		{
			m_Spots[i].m_Team = -1;
			m_Spots[i].m_Ticks = 0;
			continue;
		}
		m_Spots[i].m_Team = apCloseCCharacters[0]->GetPlayer()->GetTeam();
		int j;
		for(j = 1; j < Num && m_Spots[i].m_Cell->IsInCell(apCloseCCharacters[j]->m_Pos); ++j)
		{
			if(m_Spots[i].m_Team != apCloseCCharacters[j]->GetPlayer()->GetTeam())
			{
				m_Spots[i].m_Team = -1;
				m_Spots[i].m_Ticks = 0;
				break;
			}
		}
		if(m_Spots[i].m_Team != -1)
		{
			++m_Spots[i].m_Ticks;
			if(!(m_Spots[i].m_Ticks % (Server()->TickSpeed()/10)))
			{
				++m_aTeamscore[m_Spots[i].m_Team];
				for(int k=0 ; k<j ; ++k)
				{
					++apCloseCCharacters[k]->GetPlayer()->m_Score;
				}
				if(!(m_Spots[i].m_Ticks % Server()->TickSpeed()))
				{
					for(int c = 0; c < MAX_CLIENTS; c++)
					{
						CPlayer *pPlayer = GameServer()->m_apPlayers[c];
						if(!pPlayer)
							continue;

						if(      pPlayer->GetTeam() == TEAM_SPECTATORS 
						      && pPlayer->m_SpectatorID != SPEC_FREEVIEW
						      && GameServer()->m_apPlayers[pPlayer->m_SpectatorID]
						      && GameServer()->m_apPlayers[pPlayer->m_SpectatorID]->GetTeam() == m_Spots[i].m_Team
							)
							GameServer()->CreateSoundGlobal(SOUND_CTF_GRAB_PL, c);
						else if(pPlayer->GetTeam() == m_Spots[i].m_Team)
							GameServer()->CreateSoundGlobal(SOUND_CTF_GRAB_PL, c);
						else
							GameServer()->CreateSoundGlobal(SOUND_CTF_CAPTURE, c);
					}
				}
			}
		}
	}
}
