/* (c) Léo Flaventin Hauchecorne. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include <game/server/entities/lasercell.h>

CLaserCell::CLaserCell(CGameWorld *pGameWorld, vec2 Pos, int size)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_LASERCELL)
{
	m_Pos = Pos;
	m_Size = size;
	int size2 = size*16;
	m_aLines[0] = new CLaserLine(pGameWorld, Pos + vec2(size2,size2-16), Pos + vec2(size2,-size2));
	m_aLines[1] = new CLaserLine(pGameWorld, Pos + vec2(size2-16,-size2), Pos + vec2(-size2,-size2));
	m_aLines[2] = new CLaserLine(pGameWorld, Pos + vec2(-size2,16-size2), Pos + vec2(-size2,size2));
	m_aLines[3] = new CLaserLine(pGameWorld, Pos + vec2(16-size2,size2), Pos + vec2(size2,size2));
}

CLaserCell::~CLaserCell()
{
	for(int i=0 ; i<4 ; ++i)
	{
		delete m_aLines[i];
	}
}

void CLaserCell::Reset()
{
	
}

void CLaserCell::Snap(int SnappingClient)
{
	for(int i=0 ; i<4 ; ++i)
	{
		m_aLines[i]->Snap(SnappingClient);
	}
}

bool CLaserCell::IsInCell(vec2 Pos)
{
	Pos -= m_Pos;
	float s = m_Size*16;
	return -s < Pos.x && Pos.x < s && -s < Pos.y && Pos.y < s;
}
