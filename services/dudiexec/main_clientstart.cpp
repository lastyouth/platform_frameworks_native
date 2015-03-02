#include <stdio.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include "../dudilowmanagerservice/DudiLowManagerService.h"

using namespace android;

int main(int argc, char* argv[])
{
	sp<IServiceManager> sm = defaultServiceManager();
	
	sp<IBinder> b;
	
	sp<IDudiLowManagerService> sDudiService;
	
	do
	{
		b = sm->getService(String16("DudiLowManagerService"));
		
		if(b != 0)
		{
			break;
		}
		
		usleep(500000);
	}while(true);
	
	printf("DudiLowManagerService is Working\n");
	
	sDudiService = interface_cast<IDudiLowManagerService>(b);
	
	sDudiService->postEncodedInputEventInternal(String8("Don't be shy!!!"));
	
	return 0;
}

