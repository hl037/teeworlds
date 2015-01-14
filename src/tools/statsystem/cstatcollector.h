#ifndef CSTATCOLLECTOR_H
#define CSTATCOLLECTOR_H

#include <engine/shared/demo.h>
#include <game/generated/protocol.h>
#include <engine/client.h>
#include <engine/console.h>


class CStatCollector : public CDemoPlayer::IListner
{
	enum
	{
		SNAP_CURRENT=IClient::SNAP_CURRENT,
		SNAP_PREV=IClient::SNAP_PREV
	};
   
	enum
	{
		NUM_SNAPSHOT_TYPES=2,
		PREDICTION_MARGIN=1000/50/2, // magic network prediction value
	};
   
   class CDemoPlayer * m_pDemoPlayer;
   
	CSnapshotStorage::CHolder *m_aSnapshots[NUM_SNAPSHOT_TYPES];
   char m_Buffers[NUM_SNAPSHOT_TYPES][2][CSnapshot::MAX_SIZE];
   
   //class CSnapshotDelta m_SnapshotDelta;
   
   
	CNetObjHandler m_NetObjHandler;

	int m_PrevGameTick;
	int m_CurGameTick;
   
	IConsole *m_pConsole;
   
public:
   
	inline int GameTick() const { return m_CurGameTick; }
   
public:
   CStatCollector(CSnapshotDelta * pSnapShotDelta);
   ~CStatCollector();
   
	virtual int SnapNumItems(int SnapID);
	virtual void * SnapFindItem(int SnapID, int Type, int ID);
	virtual void * SnapGetItem(int SnapID, int Index, IClient::CSnapItem *pItem);
	virtual void SnapInvalidateItem(int SnapID, int Index);
   inline void setDemoPlayer(CDemoPlayer * pDemoPlayer) { m_pDemoPlayer = pDemoPlayer; }
   inline void setConsole(IConsole * pConsole) { m_pConsole = pConsole; }

	// virtual void SnapSetStaticsize(int ItemType, int Size);
   
   virtual void OnDemoPlayerSnapshot(void *pData, int Size);
	virtual void OnDemoPlayerMessage(void *pData, int Size);
   
protected:
   virtual void OnNewSnapshot();
};

#endif // CSTATCOLLECTOR_H
