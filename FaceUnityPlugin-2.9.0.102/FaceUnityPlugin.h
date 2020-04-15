//
//  FaceUnityPlugin.h
//  FaceUnityPlugin
//
//  Created by 张乾泽 on 2019/8/6.
//  Copyright © 2019 Agora Corp. All rights reserved.
//

#ifndef FaceUnityPlugin_h
#define FaceUnityPlugin_h

#include "IAVFramePlugin.h"
#include <string>
#include <thread>
#include <vector>
#include "FUConfig.h"
#include "common/rapidjson/document.h"
#include "common/rapidjson/writer.h"
#include "common/loguru.hpp"

using namespace rapidjson;

enum FaceUnityPluginStatus
{
    FACEUNITY_PLUGIN_STATUS_STOPPED = 0,
    FACEUNITY_PLUGIN_STATUS_STOPPING = 1,
    FACEUNITY_PLUGIN_STATUS_STARTED = 2
};

struct FaceUnityBundle {
    std::string bundleName;
    std::string options;
    bool updated;
};

class FaceUnityPlugin : public IAVFramePlugin
{
public:
    FaceUnityPlugin();
    ~FaceUnityPlugin();
    virtual bool onPluginCaptureVideoFrame(VideoPluginFrame* videoFrame) override;
    virtual bool onPluginRenderVideoFrame(unsigned int uid, VideoPluginFrame* videoFrame) override;
    
    virtual bool onPluginRecordAudioFrame(AudioPluginFrame* audioFrame) override;
    virtual bool onPluginPlaybackAudioFrame(AudioPluginFrame* audioFrame) override;
    virtual bool onPluginMixedAudioFrame(AudioPluginFrame* audioFrame) override;
    virtual bool onPluginPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioPluginFrame* audioFrame) override;
    
    virtual int load(const char* path) override;
    virtual int unLoad() override;
    virtual int enable() override;
    virtual int disable() override;
    virtual int setParameter(const char* param) override;
    virtual const char* getParameter(const char* key) override;
    virtual int release() override;
protected:
    bool initOpenGL();
    void videoFrameData(VideoPluginFrame* videoFrame, unsigned char* yuvData);
    void yuvData(VideoPluginFrame* srcVideoFrame, VideoPluginFrame* dstVideoFrame);
    int yuvSize(VideoPluginFrame* videoFrame);
    void checkCreateVideoFrame(VideoPluginFrame* videoFrame);
    void initCacheVideoFrame(VideoPluginFrame* dstVideoFrame, VideoPluginFrame* srcVideoFrame);
    void memsetCacheBuffer(VideoPluginFrame* videoFrame);
    void releaseCacheBuffer(VideoPluginFrame* videoFrame);
    std::string folderPath;
    char* auth_package;
    int auth_package_size;
#if defined(_WIN32)
    int previousThreadId;
#else
    uint64_t previousThreadId;
#endif
    bool mLoaded = false;
    bool mNeedLoadBundles = true;
    bool mNeedUpdateBundles = true;
    bool mReleased = false;
    FaceUnityPluginStatus status = FACEUNITY_PLUGIN_STATUS_STOPPED;
    std::vector<FaceUnityBundle> bundles;
    std::unique_ptr<int> items;
    VideoPluginFrame* cacheYuvVideoFramePtr;
};

#define READ_DOUBLE_VALUE_PARAM(d, name, newvalue) \
if(d.HasMember(name)) { \
    Value& value = d[name]; \
    if(!value.IsNumber()) { \
        return false; \
    } \
    newvalue = value.GetDouble(); \
}

#if defined(_WIN32)
/*
 * '_cups_strlcpy()' - Safely copy two strings.
 */

size_t                    /* O - Length of string */
strlcpy(char *dst,        /* O - Destination string */
    const char *src,        /* I - Source string */
    size_t size)        /* I - Size of destination string buffer */
{
    size_t    srclen;            /* Length of source string */


    /*
    * Figure out how much room is needed...
    */
    size--;

    srclen = strlen(src);

    /*
    * Copy the appropriate amount...
    */

    if (srclen > size)
        srclen = size;

    memcpy(dst, src, srclen);
    dst[srclen] = '\0';

    return (srclen);
}

#endif

#endif /* FaceUnityPlugin_h */
