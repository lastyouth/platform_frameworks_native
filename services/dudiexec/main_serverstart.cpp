#include <stdio.h>
#include <unistd.h>
#include <grp.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include "../dudilowmanagerservice/DudiLowManagerService.h"

using namespace android;

int main(int argc, char* argv[])
{
	sp<ProcessState> proc(ProcessState::self());
	sp<IServiceManager> sm = defaultServiceManager();
	
	DudiLowManagerService::instantiate();
	
	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();
	
	return 0;
}

