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
#include "FUConfig.h"


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
    int videoFrameThreadId;
    bool mNeedUpdateFUOptions = true;
    bool mLoaded = false;
    
    
    std::string filter_name = "origin";
    double filter_level = 1.0;
    double color_level = 0.2;
    double red_level = 0.5;
    double blur_level = 6.0;
    double skin_detect = 0.0;
    double nonshin_blur_scale = 0.45;
    double heavy_blur = 0;
    double face_shape = 3;
    double face_shape_level = 1.0;
    double eye_enlarging = 0.5;
    double cheek_thinning = 0.0;
    double cheek_v = 0.0;
    double cheek_narrow = 0.0;
    double cheek_small = 0.0;
    double cheek_oval = 0.0;
    double intensity_nose = 0.0;
    double intensity_forehead = 0.5;
    double intensity_mouth = 0.5;
    double intensity_chin = 0.0;
    double change_frames = 0.0;
    double eye_bright = 1.0;
    double tooth_whiten = 1.0;
    double is_beauty_on = 1.0;
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
