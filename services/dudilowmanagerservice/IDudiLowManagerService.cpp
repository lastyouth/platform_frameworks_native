#include "IDudiLowManagerService.h"

namespace android{

	enum{
	HW_DUDILOWMANAGERSERVICE = IBinder::FIRST_CALL_TRANSACTION,
	};

	

	class BpDudiLowManagerService : public BpInterface<IDudiLowManagerService>
	{
		public:
	
		BpDudiLowManagerService(const sp<android::IBinder>& impl) : BpInterface<IDudiLowManagerService>(impl)
		{
		}
	
		virtual status_t print(const char* str)
		{
			ALOGI("IDudiLowManagerService.cpp - print");
			Parcel data,reply;
	
			data.writeInterfaceToken(IDudiLowManagerService::getInterfaceDescriptor());
			data.writeCString(str);
	
			status_t status = remote()->transact(HW_DUDILOWMANAGERSERVICE,data,&reply);
			
			if(status != NO_ERROR)
			{
			}
			else
			{
				status = reply.readInt32();
			}
			
			return status;
		}
	};
	
	IMPLEMENT_META_INTERFACE(DudiLowManagerService,"android.os.IDudiLowManagerService");
		
	status_t BnDudiLowManagerService::onTransact(uint32_t code,const Parcel &data,Parcel* reply, uint32_t flags)
	{
		switch(code)
		{
		case HW_DUDILOWMANAGERSERVICE:
			{
				ALOGI("IDudiLowManagerService.cpp - onTransact");
				CHECK_INTERFACE(IDudiLowManagerService,data,reply);
				const char* str;
				str = data.readCString();
				reply->writeInt32(print(str));
				return NO_ERROR;
			}
			break;
		default:
			return BBinder::onTransact(code,data,reply,flags);
		}
	}
};

