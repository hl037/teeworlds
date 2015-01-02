#include "cstatcollector.h"
#include <engine/client.h>


CStatCollector::CStatCollector()
{
   for(int i=0 ; i<NUM_SNAPSHOT_TYPES ; ++i)
   {
      m_aSnapshots[i] = new CSnapshotStorage::CHolder;
   }
   
	m_aSnapshots[SNAP_CURRENT]->m_pSnap = (CSnapshot *)m_Buffers[SNAP_CURRENT][0];
	m_aSnapshots[SNAP_CURRENT]->m_pAltSnap = (CSnapshot *)m_Buffers[SNAP_CURRENT][1];
	m_aSnapshots[SNAP_CURRENT]->m_SnapSize = 0;
	m_aSnapshots[SNAP_CURRENT]->m_Tick = -1;

	m_aSnapshots[SNAP_PREV]->m_pSnap = (CSnapshot *)m_Buffers[SNAP_PREV][0];
	m_aSnapshots[SNAP_PREV]->m_pAltSnap = (CSnapshot *)m_Buffers[SNAP_PREV][1];
	m_aSnapshots[SNAP_PREV]->m_SnapSize = 0;
	m_aSnapshots[SNAP_PREV]->m_Tick = -1;

}

CStatCollector::~CStatCollector()
{
   
}

int CStatCollector::SnapNumItems(int SnapID)
{
	dbg_assert(SnapID >= 0 && SnapID < NUM_SNAPSHOT_TYPES, "invalid SnapID");
	if(!m_aSnapshots[SnapID])
		return 0;
	return m_aSnapshots[SnapID]->m_pSnap->NumItems();
}


void *CStatCollector::SnapGetItem(int SnapID, int Index, IClient::CSnapItem *pItem)
{
	CSnapshotItem *i;
	dbg_assert(SnapID >= 0 && SnapID < NUM_SNAPSHOT_TYPES, "invalid SnapID");
	i = m_aSnapshots[SnapID]->m_pAltSnap->GetItem(Index);
	pItem->m_DataSize = m_aSnapshots[SnapID]->m_pAltSnap->GetItemSize(Index);
	pItem->m_Type = i->Type();
	pItem->m_ID = i->ID();
	return (void *)i->Data();
}

void CStatCollector::SnapInvalidateItem(int SnapID, int Index)
{
	CSnapshotItem *i;
	dbg_assert(SnapID >= 0 && SnapID < NUM_SNAPSHOT_TYPES, "invalid SnapID");
	i = m_aSnapshots[SnapID]->m_pAltSnap->GetItem(Index);
	if(i)
	{
		if((char *)i < (char *)m_aSnapshots[SnapID]->m_pAltSnap || (char *)i > (char *)m_aSnapshots[SnapID]->m_pAltSnap + m_aSnapshots[SnapID]->m_SnapSize)
			m_pConsole->Print(IConsole::OUTPUT_LEVEL_DEBUG, "client", "snap invalidate problem");
		if((char *)i >= (char *)m_aSnapshots[SnapID]->m_pSnap && (char *)i < (char *)m_aSnapshots[SnapID]->m_pSnap + m_aSnapshots[SnapID]->m_SnapSize)
			m_pConsole->Print(IConsole::OUTPUT_LEVEL_DEBUG, "client", "snap invalidate problem");
		i->m_TypeAndID = -1;
	}
}

void *CStatCollector::SnapFindItem(int SnapID, int Type, int ID)
{
	// TODO: linear search. should be fixed.
	int i;

	if(!m_aSnapshots[SnapID])
		return 0x0;

	for(i = 0; i < m_aSnapshots[SnapID]->m_pSnap->NumItems(); i++)
	{
		CSnapshotItem *pItem = m_aSnapshots[SnapID]->m_pAltSnap->GetItem(i);
		if(pItem->Type() == Type && pItem->ID() == ID)
			return (void *)pItem->Data();
	}
	return 0x0;
}

// void CStatCollector::SnapSetStaticsize(int ItemType, int Size)
// {
// 	m_SnapshotDelta.SetStaticsize(ItemType, Size);
// }




void CStatCollector::OnDemoPlayerSnapshot(void * pData, int Size)
{
   // update ticks, they could have changed
   const CDemoPlayer::CPlaybackInfo *pInfo = m_pDemoPlayer->Info();
	CSnapshotStorage::CHolder *pTemp;
	m_CurGameTick = pInfo->m_Info.m_CurrentTick;
	m_PrevGameTick = pInfo->m_PreviousTick;

	// handle snapshots
	pTemp = m_aSnapshots[SNAP_PREV];
	m_aSnapshots[SNAP_PREV] = m_aSnapshots[SNAP_CURRENT];
	m_aSnapshots[SNAP_CURRENT] = pTemp;

	mem_copy(m_aSnapshots[SNAP_CURRENT]->m_pSnap, pData, Size);
	mem_copy(m_aSnapshots[SNAP_CURRENT]->m_pAltSnap, pData, Size);

	OnNewSnapshot();
}

void CStatCollector::OnDemoPlayerMessage(void * pData, int Size)
{
   
}

void CStatCollector::OnNewSnapshot()
{
	//m_NewTick = true;
   dbg_msg("Statsystem", "New snap!!!\n");

//	// secure snapshot
//	{
//		int Num = SnapNumItems(IClient::SNAP_CURRENT);
//		for(int Index = 0; Index < Num; Index++)
//		{
//			IClient::CSnapItem Item;
//			void *pData = SnapGetItem(IClient::SNAP_CURRENT, Index, &Item);
//			if(m_NetObjHandler.ValidateObj(Item.m_Type, pData, Item.m_DataSize) != 0)
//			{
//				if(g_Config.m_Debug)
//				{
//					char aBuf[256];
//					str_format(aBuf, sizeof(aBuf), "invalidated index=%d type=%d (%s) size=%d id=%d", Index, Item.m_Type, m_NetObjHandler.GetObjName(Item.m_Type), Item.m_DataSize, Item.m_ID);
//					Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
//				}
//				SnapInvalidateItem(IClient::SNAP_CURRENT, Index);
//			}
//		}
//	}
//
//	ProcessEvents();
//
//	// go trough all the items in the snapshot and gather the info we want
//	{
//		m_Snap.m_aTeamSize[TEAM_RED] = m_Snap.m_aTeamSize[TEAM_BLUE] = 0;
//
//		int Num = SnapNumItems(IClient::SNAP_CURRENT);
//		for(int i = 0; i < Num; i++)
//		{
//			IClient::CSnapItem Item;
//			const void *pData = Client()->SnapGetItem(IClient::SNAP_CURRENT, i, &Item);
//
//			if(Item.m_Type == NETOBJTYPE_CLIENTINFO)
//			{
//				const CNetObj_ClientInfo *pInfo = (const CNetObj_ClientInfo *)pData;
//				int ClientID = Item.m_ID;
//				IntsToStr(&pInfo->m_Name0, 4, m_aClients[ClientID].m_aName);
//				IntsToStr(&pInfo->m_Clan0, 3, m_aClients[ClientID].m_aClan);
//				m_aClients[ClientID].m_Country = pInfo->m_Country;
//				IntsToStr(&pInfo->m_Skin0, 6, m_aClients[ClientID].m_aSkinName);
//
//				m_aClients[ClientID].m_UseCustomColor = pInfo->m_UseCustomColor;
//				m_aClients[ClientID].m_ColorBody = pInfo->m_ColorBody;
//				m_aClients[ClientID].m_ColorFeet = pInfo->m_ColorFeet;
//
//				// prepare the info
//				if(m_aClients[ClientID].m_aSkinName[0] == 'x' || m_aClients[ClientID].m_aSkinName[1] == '_')
//					str_copy(m_aClients[ClientID].m_aSkinName, "default", 64);
//
//				m_aClients[ClientID].m_SkinInfo.m_ColorBody = m_pSkins->GetColorV4(m_aClients[ClientID].m_ColorBody);
//				m_aClients[ClientID].m_SkinInfo.m_ColorFeet = m_pSkins->GetColorV4(m_aClients[ClientID].m_ColorFeet);
//				m_aClients[ClientID].m_SkinInfo.m_Size = 64;
//
//				// find new skin
//				m_aClients[ClientID].m_SkinID = g_GameClient.m_pSkins->Find(m_aClients[ClientID].m_aSkinName);
//				if(m_aClients[ClientID].m_SkinID < 0)
//				{
//					m_aClients[ClientID].m_SkinID = g_GameClient.m_pSkins->Find("default");
//					if(m_aClients[ClientID].m_SkinID < 0)
//						m_aClients[ClientID].m_SkinID = 0;
//				}
//
//				if(m_aClients[ClientID].m_UseCustomColor)
//					m_aClients[ClientID].m_SkinInfo.m_Texture = g_GameClient.m_pSkins->Get(m_aClients[ClientID].m_SkinID)->m_ColorTexture;
//				else
//				{
//					m_aClients[ClientID].m_SkinInfo.m_Texture = g_GameClient.m_pSkins->Get(m_aClients[ClientID].m_SkinID)->m_OrgTexture;
//					m_aClients[ClientID].m_SkinInfo.m_ColorBody = vec4(1,1,1,1);
//					m_aClients[ClientID].m_SkinInfo.m_ColorFeet = vec4(1,1,1,1);
//				}
//
//				m_aClients[ClientID].UpdateRenderInfo();
//
//			}
//			else if(Item.m_Type == NETOBJTYPE_PLAYERINFO)
//			{
//				const CNetObj_PlayerInfo *pInfo = (const CNetObj_PlayerInfo *)pData;
//
//				m_aClients[pInfo->m_ClientID].m_Team = pInfo->m_Team;
//				m_aClients[pInfo->m_ClientID].m_Active = true;
//				m_Snap.m_paPlayerInfos[pInfo->m_ClientID] = pInfo;
//				m_Snap.m_NumPlayers++;
//
//				if(pInfo->m_Local)
//				{
//					m_Snap.m_LocalClientID = Item.m_ID;
//					m_Snap.m_pLocalInfo = pInfo;
//
//					if(pInfo->m_Team == TEAM_SPECTATORS)
//					{
//						m_Snap.m_SpecInfo.m_Active = true;
//						m_Snap.m_SpecInfo.m_SpectatorID = SPEC_FREEVIEW;
//					}
//				}
//
//				// calculate team-balance
//				if(pInfo->m_Team != TEAM_SPECTATORS)
//					m_Snap.m_aTeamSize[pInfo->m_Team]++;
//
//			}
//			else if(Item.m_Type == NETOBJTYPE_CHARACTER)
//			{
//				const void *pOld = Client()->SnapFindItem(IClient::SNAP_PREV, NETOBJTYPE_CHARACTER, Item.m_ID);
//				m_Snap.m_aCharacters[Item.m_ID].m_Cur = *((const CNetObj_Character *)pData);
//				if(pOld)
//				{
//					m_Snap.m_aCharacters[Item.m_ID].m_Active = true;
//					m_Snap.m_aCharacters[Item.m_ID].m_Prev = *((const CNetObj_Character *)pOld);
//
//					if(m_Snap.m_aCharacters[Item.m_ID].m_Prev.m_Tick)
//						Evolve(&m_Snap.m_aCharacters[Item.m_ID].m_Prev, Client()->PrevGameTick());
//					if(m_Snap.m_aCharacters[Item.m_ID].m_Cur.m_Tick)
//						Evolve(&m_Snap.m_aCharacters[Item.m_ID].m_Cur, Client()->GameTick());
//				}
//			}
//			else if(Item.m_Type == NETOBJTYPE_SPECTATORINFO)
//			{
//				m_Snap.m_pSpectatorInfo = (const CNetObj_SpectatorInfo *)pData;
//				m_Snap.m_pPrevSpectatorInfo = (const CNetObj_SpectatorInfo *)Client()->SnapFindItem(IClient::SNAP_PREV, NETOBJTYPE_SPECTATORINFO, Item.m_ID);
//
//				m_Snap.m_SpecInfo.m_SpectatorID = m_Snap.m_pSpectatorInfo->m_SpectatorID;
//			}
//			else if(Item.m_Type == NETOBJTYPE_GAMEINFO)
//			{
//				static bool s_GameOver = 0;
//				m_Snap.m_pGameInfoObj = (const CNetObj_GameInfo *)pData;
//				if(!s_GameOver && m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_GAMEOVER)
//					OnGameOver();
//				else if(s_GameOver && !(m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_GAMEOVER))
//					OnStartGame();
//				s_GameOver = m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_GAMEOVER;
//			}
//			else if(Item.m_Type == NETOBJTYPE_GAMEDATA)
//			{
//				m_Snap.m_pGameDataObj = (const CNetObj_GameData *)pData;
//				m_Snap.m_GameDataSnapID = Item.m_ID;
//				if(m_Snap.m_pGameDataObj->m_FlagCarrierRed == FLAG_TAKEN)
//				{
//					if(m_FlagDropTick[TEAM_RED] == 0)
//						m_FlagDropTick[TEAM_RED] = Client()->GameTick();
//				}
//				else if(m_FlagDropTick[TEAM_RED] != 0)
//						m_FlagDropTick[TEAM_RED] = 0;
//				if(m_Snap.m_pGameDataObj->m_FlagCarrierBlue == FLAG_TAKEN)
//				{
//					if(m_FlagDropTick[TEAM_BLUE] == 0)
//						m_FlagDropTick[TEAM_BLUE] = Client()->GameTick();
//				}
//				else if(m_FlagDropTick[TEAM_BLUE] != 0)
//						m_FlagDropTick[TEAM_BLUE] = 0;
//			}
//			else if(Item.m_Type == NETOBJTYPE_FLAG)
//				m_Snap.m_paFlags[Item.m_ID%2] = (const CNetObj_Flag *)pData;
//		}
//	}
//
//	// setup local pointers
//	if(m_Snap.m_LocalClientID >= 0)
//	{
//		CSnapState::CCharacterInfo *c = &m_Snap.m_aCharacters[m_Snap.m_LocalClientID];
//		if(c->m_Active)
//		{
//			m_Snap.m_pLocalCharacter = &c->m_Cur;
//			m_Snap.m_pLocalPrevCharacter = &c->m_Prev;
//			m_LocalCharacterPos = vec2(m_Snap.m_pLocalCharacter->m_X, m_Snap.m_pLocalCharacter->m_Y);
//		}
//		else if(Client()->SnapFindItem(IClient::SNAP_PREV, NETOBJTYPE_CHARACTER, m_Snap.m_LocalClientID))
//		{
//			// player died
//			m_pControls->OnPlayerDeath();
//		}
//	}
//	else
//	{
//		m_Snap.m_SpecInfo.m_Active = true;
//		if(Client()->State() == IClient::STATE_DEMOPLAYBACK && DemoPlayer()->GetDemoType() == IDemoPlayer::DEMOTYPE_SERVER &&
//			m_DemoSpecID != SPEC_FREEVIEW && m_Snap.m_aCharacters[m_DemoSpecID].m_Active)
//			m_Snap.m_SpecInfo.m_SpectatorID = m_DemoSpecID;
//		else
//			m_Snap.m_SpecInfo.m_SpectatorID = SPEC_FREEVIEW;
//	}
//
//	// clear out unneeded client data
//	for(int i = 0; i < MAX_CLIENTS; ++i)
//	{
//		if(!m_Snap.m_paPlayerInfos[i] && m_aClients[i].m_Active)
//			m_aClients[i].Reset();
//	}
//
//	// update friend state
//	for(int i = 0; i < MAX_CLIENTS; ++i)
//	{
//		if(i == m_Snap.m_LocalClientID || !m_Snap.m_paPlayerInfos[i] || !Friends()->IsFriend(m_aClients[i].m_aName, m_aClients[i].m_aClan, true))
//			m_aClients[i].m_Friend = false;
//		else
//			m_aClients[i].m_Friend = true;
//	}
//
//	// sort player infos by score
//	mem_copy(m_Snap.m_paInfoByScore, m_Snap.m_paPlayerInfos, sizeof(m_Snap.m_paInfoByScore));
//	for(int k = 0; k < MAX_CLIENTS-1; k++) // ffs, bubblesort
//	{
//		for(int i = 0; i < MAX_CLIENTS-k-1; i++)
//		{
//			if(m_Snap.m_paInfoByScore[i+1] && (!m_Snap.m_paInfoByScore[i] || m_Snap.m_paInfoByScore[i]->m_Score < m_Snap.m_paInfoByScore[i+1]->m_Score))
//			{
//				const CNetObj_PlayerInfo *pTmp = m_Snap.m_paInfoByScore[i];
//				m_Snap.m_paInfoByScore[i] = m_Snap.m_paInfoByScore[i+1];
//				m_Snap.m_paInfoByScore[i+1] = pTmp;
//			}
//		}
//	}
//	// sort player infos by team
//	int Teams[3] = { TEAM_RED, TEAM_BLUE, TEAM_SPECTATORS };
//	int Index = 0;
//	for(int Team = 0; Team < 3; ++Team)
//	{
//		for(int i = 0; i < MAX_CLIENTS && Index < MAX_CLIENTS; ++i)
//		{
//			if(m_Snap.m_paPlayerInfos[i] && m_Snap.m_paPlayerInfos[i]->m_Team == Teams[Team])
//				m_Snap.m_paInfoByTeam[Index++] = m_Snap.m_paPlayerInfos[i];
//		}
//	}
//
//	CTuningParams StandardTuning;
//	CServerInfo CurrentServerInfo;
//	Client()->GetServerInfo(&CurrentServerInfo);
//	if(CurrentServerInfo.m_aGameType[0] != '0')
//	{
//		if(str_comp(CurrentServerInfo.m_aGameType, "DM") != 0 && str_comp(CurrentServerInfo.m_aGameType, "TDM") != 0 && str_comp(CurrentServerInfo.m_aGameType, "CTF") != 0)
//			m_ServerMode = SERVERMODE_MOD;
//		else if(mem_comp(&StandardTuning, &m_Tuning, sizeof(CTuningParams)) == 0)
//			m_ServerMode = SERVERMODE_PURE;
//		else
//			m_ServerMode = SERVERMODE_PUREMOD;
//	}
//
//	// add tuning to demo
//	if(DemoRecorder()->IsRecording() && mem_comp(&StandardTuning, &m_Tuning, sizeof(CTuningParams)) != 0)
//	{
//		CMsgPacker Msg(NETMSGTYPE_SV_TUNEPARAMS);
//		int *pParams = (int *)&m_Tuning;
//		for(unsigned i = 0; i < sizeof(m_Tuning)/sizeof(int); i++)
//			Msg.AddInt(pParams[i]);
//		Client()->SendMsg(&Msg, MSGFLAG_RECORD|MSGFLAG_NOSEND);
//	}
}
