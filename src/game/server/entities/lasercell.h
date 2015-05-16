/* (c) Léo Flaventin Hauchecorne. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_LASERCELL_H
#define GAME_SERVER_ENTITIES_LASERCELL_H

#include <game/server/entity.h>
#include <game/server/entities/laserline.h>

class CLaserCell : public CEntity
{
public:
	CLaserCell(CGameWorld *pGameWorld, vec2 Pos, int size);
	virtual ~CLaserCell();

	virtual void Reset();
	virtual void Snap(int SnappingClient);
	
	inline int Size(){return m_Size;}
	
	bool IsInCell(vec2 Pos);

private:
	CLaserLine * m_aLines[4];
	vec2 m_To;
	int m_Size;

};

#endif
