#include "db.h"

int CGameRecord::CompareInfo(const PlayerInfo * pInfo1, const CGameRecord::DBPlayerInfo * pInfo2)
{
   int r;
   r = str_comp(pInfo1->m_pPlayerName, pInfo2->m_aPlayerName);
   if(r) return r;
   r = str_comp(pInfo1->m_pClan, pInfo2->m_aClan);
   if(r) return r;
   return pInfo1->m_Country == pInfo2->m_Country ? 1 : 0;
}

int CGameRecord::CompareInfo(const PlayerInfo * pInfo1, const PlayerInfo * pInfo2)
{
   int r;
   r = str_comp(pInfo1->m_pPlayerName, pInfo2->m_pPlayerName);
   if(r) return r;
   r = str_comp(pInfo1->m_pClan, pInfo2->m_pClan);
   if(r) return r;
   return pInfo1->m_Country == pInfo2->m_Country ? 1 : 0;
}

int CGameRecord::CompareInfo(const CGameRecord::DBPlayerInfo * pInfo1, const CGameRecord::DBPlayerInfo * pInfo2)
{
   int r;
   r = str_comp(pInfo1->m_aPlayerName, pInfo2->m_aPlayerName);
   if(r) return r;
   r = str_comp(pInfo1->m_aClan, pInfo2->m_aClan);
   if(r) return r;
   return pInfo1->m_Country == pInfo2->m_Country ? 1 : 0;
}

CGameRecord::DBPlayerInfo *CGameRecord::FindDBPlayerInfo(const PlayerInfo * pInfo)
{
   int i;
   int end = m_aDBPlayerInfos.size();
   for(i = 0 ; i < end ; ++i)
   {
      if(CompareInfo(pInfo, m_aDBPlayerInfos[i]))
      {
         return m_aDBPlayerInfos[i];
      }
   }
   return 0;
}

CGameRecord::NameNetAddr *CGameRecord::TakeNameNetAddr(char * pName)
{
   std::list<NameNetAddr *>::iterator it = m_aNameNetAddr.begin();
   std::list<NameNetAddr *>::iterator end = m_aNameNetAddr.end();
   for( ; it != end ; ++it)
   {
      NameNetAddr * pPair = *it;
      if(str_comp(pName, pPair->m_aPlayerName))
      {
         m_aNameNetAddr.erase(it);
         return pPair;
      }
   }
   return 0;
}

CGameRecord::CGameRecord() :
m_Current(0)
{
   mem_zero(m_aPlayers, sizeof(m_aPlayers));
}

CGameRecord::~CGameRecord()
{
   
}

void CGameRecord::beginSnapshot()
{
   for(int i = 0 ; i < MAX_CLIENTS ; ++i)
   {
      if(m_aPlayers[m_Current][i])
      {
         m_aPlayers[m_Current][i]->ID = -1;
      }
   }
}

void CGameRecord::addNameNetAddrPair(char * pPlayerName, const NETADDR & NetAddr)
{
   NameNetAddr * pPair = new NameNetAddr;
   str_copy(pPair->m_aPlayerName, pPlayerName, sizeof(pPair->m_aPlayerName));
   mem_copy(&pPair->m_NetAddr, &NetAddr, sizeof(pPair->m_NetAddr));
   m_aNameNetAddr.push_back(pPair);
}

void CGameRecord::setPlayer(int ClientID, PlayerInfo * pPlayerInfo)
{
   // Is it laready ok ?
   if(   ClientID < 0 
      || MAX_CLIENTS <= ClientID 
      || (m_aPlayers[m_Current][ClientID] != 0 && CompareInfo(pPlayerInfo, m_aPlayers[m_Current][ClientID]))
   )
      return;
   int i;
   DBPlayerInfo * pTmpInfo = FindDBPlayerInfo(pPlayerInfo);
   
   // Info found
   if(pTmpInfo != NULL)
   {
      m_aPlayers[m_Current^1][ClientID] = pTmpInfo;
      pTmpInfo->ID = ClientID;
   }
   
   // Check if nick already exists
   for(i = 0 ; i < MAX_CLIENTS ; ++i)
   {
      if(str_comp(m_aPlayers[m_Current][i]->m_aPlayerName, pPlayerInfo->m_pPlayerName))
      {
         pTmpInfo = new DBPlayerInfo;
         m_aDBPlayerInfos.push_back(pTmpInfo);
         
         str_copy(pTmpInfo->m_aPlayerName, pPlayerInfo->m_pPlayerName, sizeof(pTmpInfo->m_aPlayerName));
         str_copy(pTmpInfo->m_aClan, pPlayerInfo->m_pClan, sizeof(pTmpInfo->m_aClan));
         pTmpInfo->m_Country = pPlayerInfo->m_Country;
         
         mem_copy(&pTmpInfo->m_NetAddr, &m_aPlayers[m_Current][i]->m_NetAddr, sizeof(pTmpInfo->m_NetAddr));
         
         pTmpInfo->m_pPlayer = m_aPlayers[m_Current][i]->m_pPlayer;
         pTmpInfo->m_pPlayer->m_aInfos.push_back(pTmpInfo);
         
         m_aPlayers[m_Current^1][ClientID] = pTmpInfo;
         pTmpInfo->ID = ClientID;
         return;
      }
   }
   
   // Else create new one
   pTmpInfo = new DBPlayerInfo;
   m_aDBPlayerInfos.push_back(pTmpInfo);
   
   str_copy(pTmpInfo->m_aPlayerName, pPlayerInfo->m_pPlayerName, sizeof(pTmpInfo->m_aPlayerName));
   str_copy(pTmpInfo->m_aClan, pPlayerInfo->m_pClan, sizeof(pTmpInfo->m_aClan));
   pTmpInfo->m_Country = pPlayerInfo->m_Country;
   
   NameNetAddr * pPair = TakeNameNetAddr(pTmpInfo->m_aPlayerName);
   if(pPair != 0)
   {
      mem_copy(&pTmpInfo->m_NetAddr, &pPair->m_NetAddr, sizeof(pTmpInfo->m_NetAddr));
      delete pPair;
   }
   
   DBPlayer * pPlayer = new DBPlayer;
   m_aDBPlayer.push_back(pPlayer);
   str_copy(pPlayer->m_aPlayerName, pTmpInfo->m_aPlayerName, sizeof(pPlayer->m_aPlayerName));
   pPlayer->m_aInfos.push_back(pTmpInfo);
   pTmpInfo->m_pPlayer = pPlayer;
   
   m_aPlayers[m_Current^1][ClientID] = pTmpInfo;
   pTmpInfo->ID = ClientID;
   return;
}

void CGameRecord::newAlias(int ClientID, PlayerInfo * pNewInfo)
{
   DBPlayerInfo * pTmpInfo = FindDBPlayerInfo(pNewInfo);
   if(pTmpInfo != 0)
   {
      if(m_aPlayers[m_Current][ClientID]->m_pPlayer != pTmpInfo->m_pPlayer)
      {
         DBPlayer * src, * dest;
         if(m_aPlayers[m_Current][ClientID]->m_pPlayer->m_aInfos.size() < pTmpInfo->m_pPlayer->m_aInfos.size())
         {
            src = m_aPlayers[m_Current][ClientID]->m_pPlayer;
            dest = pTmpInfo->m_pPlayer;
         }
         else
         {
            dest = m_aPlayers[m_Current][ClientID]->m_pPlayer;
            src = pTmpInfo->m_pPlayer;
         }
         dest->m_aInfos.insert(dest->m_aInfos.end(), src->m_aInfos.begin(), src->m_aInfos.end());
         m_aDBPlayer.erase(std::find(m_aDBPlayer.begin(), m_aDBPlayer.end(), src));
         delete src;
         m_aPlayers[m_Current][ClientID]->m_pPlayer = dest;
         pTmpInfo->m_pPlayer = dest;
      }
   }
   else
   {
      pTmpInfo = new DBPlayerInfo;
      m_aDBPlayerInfos.push_back(pTmpInfo);
      
      str_copy(pTmpInfo->m_aPlayerName, pNewInfo->m_pPlayerName, sizeof(pTmpInfo->m_aPlayerName));
      str_copy(pTmpInfo->m_aClan, pNewInfo->m_pClan, sizeof(pTmpInfo->m_aClan));
      pTmpInfo->m_Country = pNewInfo->m_Country;
      
      mem_copy(&pTmpInfo->m_NetAddr, &m_aPlayers[m_Current][ClientID]->m_NetAddr, sizeof(pTmpInfo->m_NetAddr));
      
      pTmpInfo->m_pPlayer = m_aPlayers[m_Current][ClientID]->m_pPlayer;
      pTmpInfo->m_pPlayer->m_aInfos.push_back(pTmpInfo);
      
   }
   m_aPlayers[m_Current][ClientID]->ID = -1;
   m_aPlayers[m_Current][ClientID] = pTmpInfo;
   m_aPlayers[m_Current][ClientID]->ID = ClientID;
}

void CGameRecord::endSnapshot()
{
   mem_zero(m_aPlayers[m_Current], sizeof(m_aPlayers[m_Current]));
   m_Current ^= 1;
}

int CGameRecord::addKill(int KillerID, int KilledID, int Weapon)
{
   if(   KillerID < 0 
      || MAX_CLIENTS < KillerID 
      || !m_aPlayers[m_Current][KillerID]
      || KilledID < 0 
      || MAX_CLIENTS < KilledID 
      || !m_aPlayers[m_Current][KilledID]
   )
   {
      return -1;
   }
   DBKill * pKill = new DBKill;
   pKill->m_pKiller = m_aPlayers[m_Current][KillerID];
   pKill->m_pKilled = m_aPlayers[m_Current][KilledID];
   pKill->Weapon = Weapon;
   m_aKill.push_back(pKill);
   return 0;
}
