/* (c) Léo Flaventin Hauchecorne. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_LASERLINE_H
#define GAME_SERVER_ENTITIES_LASERLINE_H

#include <game/server/entity.h>

class CLaserLine : public CEntity
{
public:
	CLaserLine(CGameWorld *pGameWorld, vec2 From, vec2 To);

	virtual void Reset();
	virtual void Snap(int SnappingClient);

private:
	vec2 m_To;
};

#endif
