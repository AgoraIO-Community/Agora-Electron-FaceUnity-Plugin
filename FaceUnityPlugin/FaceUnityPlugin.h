//
//  FaceUnityPlugin.h
//  FaceUnityPlugin
//
//  Created by 张乾泽 on 2019/8/6.
//  Copyright © 2019 Agora Corp. All rights reserved.
//

#ifndef FaceUnityPlugin_h
#define FaceUnityPlugin_h

#include "IVideoFramePlugin.h"
#include <string>
#include <thread>
#include <vector>
#include "FUConfig.h"
#include "common/rapidjson/document.h"
#include "common/rapidjson/writer.h"


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
};

class FaceUnityPlugin : public IVideoFramePlugin
{
public:
    FaceUnityPlugin();
    ~FaceUnityPlugin();
    virtual bool onPluginCaptureVideoFrame(VideoPluginFrame* videoFrame) override;
    virtual bool onPluginRenderVideoFrame(unsigned int uid, VideoPluginFrame* videoFrame) override;
    
    virtual bool load(const char* path) override;
    virtual bool unLoad() override;
    virtual bool enable() override;
    virtual bool disable() override;
    virtual bool setParameter(const char* param) override;
    virtual void release() override;
protected:
    bool initOpenGL();
    void videoFrameData(VideoPluginFrame* videoFrame, unsigned char *yuvData);
    unsigned char *yuvData(VideoPluginFrame* videoFrame);
    int yuvSize(VideoPluginFrame* videoFrame);
    
    std::string folderPath;
    char* auth_package;
    int auth_package_size;
#if defined(_WIN32)
    int videoFrameThreadId;
#else
    pthread_t previousThreadId;
#endif
    bool mNeedUpdateFUOptions = true;
    bool mLoaded = false;
    bool mNeedUpdateBundles = false;
    FaceUnityPluginStatus status = FACEUNITY_PLUGIN_STATUS_STOPPED;
    std::vector<FaceUnityBundle> bundles;
    std::unique_ptr<int> items;
};

#define READ_DOUBLE_VALUE_PARAM(d, name, newvalue) \
if(d.HasMember(name)) { \
    Value& value = d[name]; \
    if(!value.IsNumber()) { \
        return false; \
    } \
    newvalue = value.GetDouble(); \
}

#endif /* FaceUnityPlugin_h */
