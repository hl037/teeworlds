
#include <engine/shared/demo.h>
#include <engine/storage.h>
#include <engine/console.h>
#include <engine/shared/config.h>
#include <engine/shared/network.h>
#include "cstatcollector.h"


static CDemoPlayer *CreateDemoPlayer(CSnapshotDelta * pSnapshotDelta, CDemoPlayer::IListner * pListener)
{
   CDemoPlayer * pDemoPlayer = new CDemoPlayer(pSnapshotDelta);
   pDemoPlayer->SetListner(pListener);
   return pDemoPlayer;
}

int main(int argc, const char * argv[])
{
	dbg_logger_stdout();
   //Initialisation
	IKernel *pKernel = IKernel::Create();
	IStorage *pStorage = CreateStorage("Teeworlds", IStorage::STORAGETYPE_CLIENT, argc, argv); // ignore_convention
   IConsole *pConsole = CreateConsole(CFGFLAG_CLIENT);
   CNetBase::Init();
   CSnapshotDelta SnapshotDelta;
   
   //Stat collector
   CStatCollector StatCollector(&SnapshotDelta);
   
   CDemoPlayer *pDemoPlayer = CreateDemoPlayer(&SnapshotDelta, &StatCollector);
   StatCollector.setDemoPlayer(pDemoPlayer);
   StatCollector.setConsole(pConsole);
   
	{
		bool RegisterFail = false;
      RegisterFail = RegisterFail || !pKernel->RegisterInterface(pStorage);
      RegisterFail = RegisterFail || !pKernel->RegisterInterface(pDemoPlayer);
      RegisterFail = RegisterFail || !pKernel->RegisterInterface(pConsole);
      
		if(RegisterFail)
			return -1;
   }
   
   //Process args (load all demos)
   
   const char ** arg = argv+1;
   --argc;
   
   dbg_msg("Statsystem", "START");
   while(argc)
   {
      
      if(pDemoPlayer->Load(pStorage, pConsole, *arg, IStorage::TYPE_ALL) != 0)
      {
         --argc; ++arg;
         continue;
      }
      dbg_msg("Statsystem", "Demo %s loaded succesfuly", *arg);
      pDemoPlayer->Play();
      while(!pDemoPlayer->BaseInfo()->m_Paused)
      {
         pDemoPlayer->NextFrame();
      }
      dbg_msg("Statsystem", "END OF DEMO %s", *arg);
      --argc; ++arg;
   }
   
   return 0;
}
