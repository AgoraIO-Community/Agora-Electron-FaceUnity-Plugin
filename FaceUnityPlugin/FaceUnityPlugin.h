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
    bool switching = false;
#if defined(_WIN32)
    int previousThreadId;
#else
    uint64_t previousThreadId;
#endif
    bool mLoaded = false;
    bool mNeedLoadBundles = true;
    bool mNeedUpdateBundles = true;
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

#if defined(_WIN32)
/*
 * '_cups_strlcpy()' - Safely copy two strings.
 */

size_t					/* O - Length of string */
strlcpy(char *dst,		/* O - Destination string */
	const char *src,		/* I - Source string */
	size_t size)		/* I - Size of destination string buffer */
{
	size_t	srclen;			/* Length of source string */


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
