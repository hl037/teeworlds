#ifndef IGAMERECORD_H
#define IGAMERECORD_H

#include <base/system.h>
#include <engine/shared/protocol.h>
#include <algorithm>
#include <vector>
#include <list>

typedef struct PlayerInfo
{
   char * m_pPlayerName;
   char * m_pClan;
   int m_Country;
}PlayerInfo;


class CGameRecord
{
public:
   enum
   {
      NAME_MAX_SIZE = 4 * 4 + 1,
      CLAN_MAX_SIZE = 3 * 4 + 1,
   };
   
protected:
   struct DBPlayerInfo;
   
   typedef struct DBPlayer
   {
      char m_aPlayerName[NAME_MAX_SIZE];
      std::vector<struct DBPlayerInfo *> m_aInfos;
   }DBPlayer;
   
   typedef struct DBPlayerInfo
   {
      char m_aPlayerName[NAME_MAX_SIZE];
      char m_aClan[CLAN_MAX_SIZE];
      int m_Country;
      int ID;
      NETADDR m_NetAddr;
      DBPlayer *m_pPlayer;
   }DBPlayerInfo;
   
   typedef struct DBKill
   {
      DBPlayerInfo * m_pKiller;
      DBPlayerInfo * m_pKilled;
      int Weapon;
   }DBKill;
   
   typedef struct NameNetAddr
   {
      char m_aPlayerName[NAME_MAX_SIZE];
      NETADDR m_NetAddr;
   }NameNetAddr;
   
   std::list<NameNetAddr *> m_aNameNetAddr;
   std::vector<DBPlayerInfo *> m_aDBPlayerInfos;
   std::vector<DBPlayer *> m_aDBPlayer;
   std::list<DBKill *> m_aKill;
   
   
protected:
   // The current array of clients (by ID)
   DBPlayerInfo * m_aPlayers[2][MAX_CLIENTS];
   int m_Current;
   
   int CompareInfo(const PlayerInfo * pInfo1, const DBPlayerInfo * pInfo2);
   int CompareInfo(const PlayerInfo * pInfo1, const PlayerInfo * pInfo2);
   int CompareInfo(const DBPlayerInfo * pInfo1, const DBPlayerInfo * pInfo2);
   
   DBPlayerInfo * FindDBPlayerInfo(const PlayerInfo * pInfo);
   NameNetAddr * TakeNameNetAddr(char * pName);

public:
   CGameRecord();
   ~CGameRecord();
   
   void beginSnapshot();
   void addNameNetAddrPair(char * pPlayerName, const NETADDR & NetAddr);
   void setPlayer(int ClientID, PlayerInfo * pPlayerInfo);
   void newAlias(int ClientID, PlayerInfo * pNewInfo);
   void endSnapshot();
   int addKill(int KillerID, int KilledID, int Weapon);
};

#endif // IGAMERECORD_H
