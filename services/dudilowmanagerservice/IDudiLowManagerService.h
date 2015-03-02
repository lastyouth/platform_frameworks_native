#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/Log.h>
#include <utils/String8.h>
#include <utils/String16.h>

#define LOG_TAG "DudiLowManagerService"


namespace android{

	class IDudiLowManagerService : public IInterface{
		public:
		
		DECLARE_META_INTERFACE(DudiLowManagerService);

		virtual status_t print(const char *str)=0;
		virtual String8 getEncodedInputEventInternal() = 0;
		virtual status_t postEncodedInputEventInternal(String8 target) = 0;
	};
	
	/*class BpDudiLowManagerService : public BpInterface<IDudiLowManagerService>
	{
		public:
		BpDudiLowManagerService (const sp<android::IBinder>& impl);
		virtual status_t print(const char* str);
	};*/
	
	class BnDudiLowManagerService : public BnInterface<IDudiLowManagerService>
	{
		public:
		virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);
	
	};

};



