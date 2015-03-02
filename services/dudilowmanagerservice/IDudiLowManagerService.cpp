#include "IDudiLowManagerService.h"

namespace android{

	enum{
	HW_DUDILOWMANAGERSERVICE = IBinder::FIRST_CALL_TRANSACTION,
	DUDIPOSTINPUTEVENT = 0x10001,
	DUDIGETINPUTEVENT = 0x10002,
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
		virtual String8 getEncodedInputEventInternal()
		{
			Parcel data,reply;
			
			data.writeInterfaceToken(IDudiLowManagerService::getInterfaceDescriptor());
			
			status_t status = remote()->transact(DUDIGETINPUTEVENT,data,&reply);
			
			if(status != NO_ERROR)
			{
				return String8(reply.readCString());
			}
			else
			{
				return String8("null");
			}
		}
		virtual status_t postEncodedInputEventInternal(String8 target)
		{
			Parcel data,reply;
			
			data.writeInterfaceToken(IDudiLowManagerService::getInterfaceDescriptor());
			data.writeCString(target.string());
			
			status_t status = remote()->transact(DUDIPOSTINPUTEVENT,data,&reply);
			
			int32_t ret = reply.readInt32();
			
			if(status != NO_ERROR)
			{
				return ret;
			}
			else
			{
				return status;
			}
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
		case DUDIGETINPUTEVENT:
			{
				CHECK_INTERFACE(IDudiLowManagerService,data,reply);

				reply->writeCString(getEncodedInputEventInternal().string());
				
				return NO_ERROR;
			}
			break;
		case DUDIPOSTINPUTEVENT:
			{
				CHECK_INTERFACE(IDudiLowManagerService,data,reply);
				
				const char* str = data.readCString();
			
				status_t status = postEncodedInputEventInternal(String8(str));
				
				return status;
			}
			break;
		default:
			return BBinder::onTransact(code,data,reply,flags);
		}
	}
};

