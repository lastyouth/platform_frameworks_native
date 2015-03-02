#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <utils/RefBase.h>
#include <utils/Errors.h>
#include <utils/String8.h>
#include <utils/String16.h>

#include "IDudiLowManagerService.h"

namespace android{
	class DudiLowManagerService : public BnDudiLowManagerService
	{
	public:
		static void instantiate();

		virtual status_t print(const char* str);
		virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags);
		virtual String8 getEncodedInputEventInternal();
		virtual status_t postEncodedInputEventInternal(String8 target);

	private:
		DudiLowManagerService();

		virtual ~DudiLowManagerService();
		Vector<String8> mInputEvents;
	};	

};
