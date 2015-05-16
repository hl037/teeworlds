/* (c) Léo Flaventin Hauchecorne. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "laserline.h"

CLaserLine::CLaserLine(CGameWorld * pGameWorld, vec2 From, vec2 To)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_LASERLINE)
{
	m_Pos = From;
	m_To = To;
}

void CLaserLine::Reset()
{
	
}

void CLaserLine::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));
	if(!pObj)
		return;
	
	pObj->m_X = (int)m_To.x;
	pObj->m_Y = (int)m_To.y;
	pObj->m_FromX = (int)m_Pos.x;
	pObj->m_FromY = (int)m_Pos.y;
	pObj->m_StartTick = Server()->Tick();;
}
