#ifndef IGAMERECORD_H
#define IGAMERECORD_H


class IGameRecord
{
public:
   enum
   {
      NAME_MAX_SIZE = 4 * 4 + 1,
      CLAN_MAX_SIZE = 3 * 4 + 1,
   };
   
   struct PlayerInfo
   {
      char m_PlayerName[NAME_MAX_SIZE];
      char m_Clan[CLAN_MAX_SIZE];
      int m_Country;
      
   };

   IGameRecord();
   ~IGameRecord();
};

#endif // IGAMERECORD_H
