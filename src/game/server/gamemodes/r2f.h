/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMEMODES_R2F_H
#define GAME_SERVER_GAMEMODES_R2F_H
#include <game/server/gamecontroller.h>
#include <game/server/entity.h>
#include <vector>

class CGameControllerR2F : public IGameController
{
public:
	std::vector<class CFlag *> m_apFlags;

	CGameControllerR2F(class CGameContext *pGameServer);
	virtual void DoWincheck();
	virtual void Snap(int SnappingClient);
	virtual void Tick();

	virtual bool OnEntity(int Index, vec2 Pos);
	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon);
};

#endif

