/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMEMODES_STAY_H
#define GAME_SERVER_GAMEMODES_STAY_H
#include <game/server/gamecontroller.h>
#include <game/server/entity.h>

#define MAX_STAY_SPOTS MAX_CLIENTS/2

class CGameControllerKTP : public IGameController
{
public:
	struct {
		class CLaserCell * m_Cell;
		int m_Team;
		int m_Ticks;
	} m_Spots[MAX_STAY_SPOTS];
	int m_NbSpot;

	CGameControllerKTP(class CGameContext *pGameServer);
	virtual void DoWincheck();
	virtual bool CanBeMovedOnBalance(int ClientID);
	virtual void Snap(int SnappingClient);
	virtual void Tick();

	virtual bool OnEntity(int Index, vec2 Pos);
};

#endif

