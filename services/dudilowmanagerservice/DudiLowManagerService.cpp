#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <binder/Parcel.h>
#include <binder/IPCThreadState.h>
#include <utils/String16.h>
#include <utils/threads.h>

#include "DudiLowManagerService.h"

#define LOG_TAG "DudiLowManagerService"

namespace android{

	void DudiLowManagerService::instantiate()
	{
		ALOGI("DudiLowManagerService is instantiated");
		int r = defaultServiceManager()->addService(String16("DudiLowManagerService"),new DudiLowManagerService());
	
		ALOGI("DudiLowManagerService returned : %d",r);
	}

	status_t DudiLowManagerService::print(const char* str)
	{
		ALOGI("DudiLowManagerService.cpp - print : str -> %s",str);
		printf("%s\n",str);
		return NO_ERROR;
	}

	DudiLowManagerService::DudiLowManagerService()
	{
		ALOGI("DudiLowManagerService is Created");
	}

	DudiLowManagerService::~DudiLowManagerService()
	{
	}

	status_t DudiLowManagerService::onTransact(uint32_t code, const Parcel& data,Parcel* reply,uint32_t flags)
	{
		switch(code)
		{
		case IBinder::FIRST_CALL_TRANSACTION:
			{
				ALOGI("DudiLowManagerService.cpp - onTransact");
				CHECK_INTERFACE(IDudiLowManagerService,data,reply);
				const char* str;
				str = data.readCString();
				reply->writeInt32(print(str));
				return NO_ERROR;
			}
			break;
		default:
			return BnDudiLowManagerService::onTransact(code,data,reply,flags);
		}	
	}
};
