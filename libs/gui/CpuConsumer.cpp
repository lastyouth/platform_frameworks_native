/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "CpuConsumer"
//#define ATRACE_TAG ATRACE_TAG_GRAPHICS

#include <cutils/compiler.h>
#include <utils/Log.h>
#include <gui/CpuConsumer.h>

//#define CC_LOGV(x, ...) ALOGV("[%s] " x, mName.string(), ##__VA_ARGS__)
//#define CC_LOGD(x, ...) ALOGD("[%s] " x, mName.string(), ##__VA_ARGS__)
//#define CC_LOGI(x, ...) ALOGI("[%s] " x, mName.string(), ##__VA_ARGS__)
#define CC_LOGW(x, ...) ALOGW("[%s] " x, mName.string(), ##__VA_ARGS__)
#define CC_LOGE(x, ...) ALOGE("[%s] " x, mName.string(), ##__VA_ARGS__)

namespace android {

CpuConsumer::CpuConsumer(const sp<IGraphicBufferConsumer>& bq,
        size_t maxLockedBuffers, bool controlledByApp) :
    ConsumerBase(bq, controlledByApp),
    mMaxLockedBuffers(maxLockedBuffers),
    mCurrentLockedBuffers(0)
{
    // Create tracking entries for locked buffers
    mAcquiredBuffers.insertAt(0, maxLockedBuffers);

    mConsumer->setConsumerUsageBits(GRALLOC_USAGE_SW_READ_OFTEN);
    mConsumer->setMaxAcquiredBufferCount(static_cast<int32_t>(maxLockedBuffers));
}

CpuConsumer::~CpuConsumer() {
    // ConsumerBase destructor does all the work.
}



void CpuConsumer::setName(const String8& name) {
    Mutex::Autolock _l(mMutex);
    mName = name;
    mConsumer->setConsumerName(name);
}

status_t CpuConsumer::setDefaultBufferSize(uint32_t width, uint32_t height)
{
    Mutex::Autolock _l(mMutex);
    return mConsumer->setDefaultBufferSize(width, height);
}

status_t CpuConsumer::setDefaultBufferFormat(PixelFormat defaultFormat)
{
    Mutex::Autolock _l(mMutex);
    return mConsumer->setDefaultBufferFormat(defaultFormat);
}

status_t CpuConsumer::lockNextBuffer(LockedBuffer *nativeBuffer) {
    status_t err;

    if (!nativeBuffer) return BAD_VALUE;
    if (mCurrentLockedBuffers == mMaxLockedBuffers) {
        CC_LOGW("Max buffers have been locked (%zd), cannot lock anymore.",
                mMaxLockedBuffers);
        return NOT_ENOUGH_DATA;
    }

    BufferQueue::BufferItem b;

    Mutex::Autolock _l(mMutex);

    err = acquireBufferLocked(&b, 0);
    if (err != OK) {
        if (err == BufferQueue::NO_BUFFER_AVAILABLE) {
            return BAD_VALUE;
        } else {
            CC_LOGE("Error acquiring buffer: %s (%d)", strerror(err), err);
            return err;
        }
    }

    int buf = b.mBuf;

    void *bufferPointer = NULL;
    android_ycbcr ycbcr = android_ycbcr();

    if (b.mFence.get()) {
        if (mSlots[buf].mGraphicBuffer->getPixelFormat() ==
                HAL_PIXEL_FORMAT_YCbCr_420_888) {
            err = mSlots[buf].mGraphicBuffer->lockAsyncYCbCr(
                GraphicBuffer::USAGE_SW_READ_OFTEN,
                b.mCrop,
                &ycbcr,
                b.mFence->dup());

            if (err != OK) {
                CC_LOGE("Unable to lock YCbCr buffer for CPU reading: %s (%d)",
                        strerror(-err), err);
                return err;
            }
            bufferPointer = ycbcr.y;
        } else {
            err = mSlots[buf].mGraphicBuffer->lockAsync(
                GraphicBuffer::USAGE_SW_READ_OFTEN,
                b.mCrop,
                &bufferPointer,
                b.mFence->dup());

            if (err != OK) {
                CC_LOGE("Unable to lock buffer for CPU reading: %s (%d)",
                        strerror(-err), err);
                return err;
            }
        }
    } else {
        if (mSlots[buf].mGraphicBuffer->getPixelFormat() ==
                HAL_PIXEL_FORMAT_YCbCr_420_888) {
            err = mSlots[buf].mGraphicBuffer->lockYCbCr(
                GraphicBuffer::USAGE_SW_READ_OFTEN,
                b.mCrop,
                &ycbcr);

            if (err != OK) {
                CC_LOGE("Unable to lock YCbCr buffer for CPU reading: %s (%d)",
                        strerror(-err), err);
                return err;
            }
            bufferPointer = ycbcr.y;
        } else {
            err = mSlots[buf].mGraphicBuffer->lock(
                GraphicBuffer::USAGE_SW_READ_OFTEN,
                b.mCrop,
                &bufferPointer);

            if (err != OK) {
                CC_LOGE("Unable to lock buffer for CPU reading: %s (%d)",
                        strerror(-err), err);
                return err;
            }
        }
    }

    size_t lockedIdx = 0;
    for (; lockedIdx < static_cast<size_t>(mMaxLockedBuffers); lockedIdx++) {
        if (mAcquiredBuffers[lockedIdx].mSlot ==
                BufferQueue::INVALID_BUFFER_SLOT) {
            break;
        }
    }
    assert(lockedIdx < mMaxLockedBuffers);

    AcquiredBuffer &ab = mAcquiredBuffers.editItemAt(lockedIdx);
    ab.mSlot = buf;
    ab.mBufferPointer = bufferPointer;
    ab.mGraphicBuffer = mSlots[buf].mGraphicBuffer;

    nativeBuffer->data   =
            reinterpret_cast<uint8_t*>(bufferPointer);
    nativeBuffer->width  = mSlots[buf].mGraphicBuffer->getWidth();
    nativeBuffer->height = mSlots[buf].mGraphicBuffer->getHeight();
    nativeBuffer->format = mSlots[buf].mGraphicBuffer->getPixelFormat();
    nativeBuffer->stride = (ycbcr.y != NULL) ?
            static_cast<uint32_t>(ycbcr.ystride) :
            mSlots[buf].mGraphicBuffer->getStride();

    nativeBuffer->crop        = b.mCrop;
    nativeBuffer->transform   = b.mTransform;
    nativeBuffer->scalingMode = b.mScalingMode;
    nativeBuffer->timestamp   = b.mTimestamp;
    nativeBuffer->frameNumber = b.mFrameNumber;

    nativeBuffer->dataCb       = reinterpret_cast<uint8_t*>(ycbcr.cb);
    nativeBuffer->dataCr       = reinterpret_cast<uint8_t*>(ycbcr.cr);
    nativeBuffer->chromaStride = static_cast<uint32_t>(ycbcr.cstride);
    nativeBuffer->chromaStep   = static_cast<uint32_t>(ycbcr.chroma_step);

    mCurrentLockedBuffers++;

    return OK;
}

status_t CpuConsumer::unlockBuffer(const LockedBuffer &nativeBuffer) {
    Mutex::Autolock _l(mMutex);
    size_t lockedIdx = 0;

    void *bufPtr = reinterpret_cast<void *>(nativeBuffer.data);
    for (; lockedIdx < static_cast<size_t>(mMaxLockedBuffers); lockedIdx++) {
        if (bufPtr == mAcquiredBuffers[lockedIdx].mBufferPointer) break;
    }
    if (lockedIdx == mMaxLockedBuffers) {
        CC_LOGE("%s: Can't find buffer to free", __FUNCTION__);
        return BAD_VALUE;
    }

    return releaseAcquiredBufferLocked(lockedIdx);
}

status_t CpuConsumer::releaseAcquiredBufferLocked(size_t lockedIdx) {
    status_t err;
    int fd = -1;

    err = mAcquiredBuffers[lockedIdx].mGraphicBuffer->unlockAsync(&fd);
    if (err != OK) {
        CC_LOGE("%s: Unable to unlock graphic buffer %zd", __FUNCTION__,
                lockedIdx);
        return err;
    }
    int buf = mAcquiredBuffers[lockedIdx].mSlot;
    if (CC_LIKELY(fd != -1)) {
        sp<Fence> fence(new Fence(fd));
        addReleaseFenceLocked(
            mAcquiredBuffers[lockedIdx].mSlot,
            mSlots[buf].mGraphicBuffer,
            fence);
    }

    // release the buffer if it hasn't already been freed by the BufferQueue.
    // This can happen, for example, when the producer of this buffer
    // disconnected after this buffer was acquired.
    if (CC_LIKELY(mAcquiredBuffers[lockedIdx].mGraphicBuffer ==
            mSlots[buf].mGraphicBuffer)) {
        releaseBufferLocked(
                buf, mAcquiredBuffers[lockedIdx].mGraphicBuffer,
                EGL_NO_DISPLAY, EGL_NO_SYNC_KHR);
    }

    AcquiredBuffer &ab = mAcquiredBuffers.editItemAt(lockedIdx);
    ab.mSlot = BufferQueue::INVALID_BUFFER_SLOT;
    ab.mBufferPointer = NULL;
    ab.mGraphicBuffer.clear();

    mCurrentLockedBuffers--;
    return OK;
}

void CpuConsumer::freeBufferLocked(int slotIndex) {
    ConsumerBase::freeBufferLocked(slotIndex);
}

} // namespace android
