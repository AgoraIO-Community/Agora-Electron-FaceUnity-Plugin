//
//  FaceUnityPlugin.cpp
//  FaceUnityPlugin
//
//  Created by 张乾泽 on 2019/8/6.
//  Copyright © 2019 Agora Corp. All rights reserved.
//
#include "FaceUnityPlugin.h"
#include <string.h>
#include <string>
#include "funama.h"
#include "FUConfig.h"
#include "Utils.h"
#include <stdlib.h>
#include <iostream>
#ifdef _WIN32
#include "windows.h"
#pragma comment(lib, "nama.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#else
#include <dlfcn.h>
#include <OpenGL/OpenGL.h>
#include <unistd.h>
#endif // WIN32


#define MAX_PATH 512
#define MAX_PROPERTY_NAME 256
#define MAX_PROPERTY_VALUE 512

static bool mNamaInited = false;
static int mFrameID = 0;

using namespace rapidjson;

#if defined(_WIN32)
PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),
    1u,
    PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW,
    PFD_TYPE_RGBA,
    32u,
    0u, 0u, 0u, 0u, 0u, 0u,
    8u,
    0u,
    0u,
    0u, 0u, 0u, 0u,
    24u,
    8u,
    0u,
    PFD_MAIN_PLANE,
    0u,
    0u, 0u };
#endif

FaceUnityPlugin::FaceUnityPlugin():cacheYuvVideoFramePtr(NULL)
{
    
}

FaceUnityPlugin::~FaceUnityPlugin()
{
    releaseCacheBuffer(cacheYuvVideoFramePtr);
}

bool FaceUnityPlugin::initOpenGL()
{
#ifdef _WIN32
	HWND hw = CreateWindowExA(
		0, "EDIT", "", ES_READONLY,
		0, 0, 1, 1,
		NULL, NULL,
		GetModuleHandleA(NULL), NULL);
	HDC hgldc = GetDC(hw);
	int spf = ChoosePixelFormat(hgldc, &pfd);
    if(spf == 0) {
        return false;
    }
	int ret = SetPixelFormat(hgldc, spf, &pfd);
    if(!ret) {
        return false;
    }
	HGLRC hglrc = wglCreateContext(hgldc);
	wglMakeCurrent(hgldc, hglrc);

	//hglrc就是创建出的OpenGL context
	printf("hw=%08x hgldc=%08x spf=%d ret=%d hglrc=%08x\n",
		hw, hgldc, spf, ret, hglrc);
#else
	CGLPixelFormatAttribute attrib[13] = { kCGLPFAOpenGLProfile,
		(CGLPixelFormatAttribute)kCGLOGLPVersion_Legacy,
		kCGLPFAAccelerated,
		kCGLPFAColorSize, (CGLPixelFormatAttribute)24,
		kCGLPFAAlphaSize, (CGLPixelFormatAttribute)8,
		kCGLPFADoubleBuffer,
		kCGLPFASampleBuffers, (CGLPixelFormatAttribute)1,
		kCGLPFASamples, (CGLPixelFormatAttribute)4,
		(CGLPixelFormatAttribute)0 };
	CGLPixelFormatObj pixelFormat = NULL;
	GLint numPixelFormats = 0;
	CGLContextObj cglContext1 = NULL;
	CGLChoosePixelFormat(attrib, &pixelFormat, &numPixelFormats);
	CGLError err = CGLCreateContext(pixelFormat, NULL, &cglContext1);
	CGLSetCurrentContext(cglContext1);
	if (err) {
		return false;
	}
#endif
	return true;
}

void FaceUnityPlugin::yuvData(VideoPluginFrame* srcVideoFrame, VideoPluginFrame* dstVideoFrame)
{
    int ysize = srcVideoFrame->yStride * srcVideoFrame->height;
    int usize = srcVideoFrame->uStride * srcVideoFrame->height / 2;
    int vsize = srcVideoFrame->vStride * srcVideoFrame->height / 2;
    memcpy(static_cast<unsigned char*>(dstVideoFrame->buffer), srcVideoFrame->yBuffer, ysize);
    memcpy(static_cast<unsigned char*>(dstVideoFrame->buffer) + ysize, srcVideoFrame->uBuffer, usize);
    memcpy(static_cast<unsigned char*>(dstVideoFrame->buffer) + ysize + usize, srcVideoFrame->vBuffer, vsize);
}

int FaceUnityPlugin::yuvSize(VideoPluginFrame* videoFrame)
{
    int ysize = videoFrame->yStride * videoFrame->height;
    int usize = videoFrame->uStride * videoFrame->height / 2;
    int vsize = videoFrame->vStride * videoFrame->height / 2;
    return ysize + usize + vsize;
}


void FaceUnityPlugin::initCacheVideoFrame(VideoPluginFrame* dstVideoFrame, VideoPluginFrame* srcVideoFrame)
{
    dstVideoFrame->width = srcVideoFrame->width;
    dstVideoFrame->height = srcVideoFrame->height;
    dstVideoFrame->yStride = srcVideoFrame->yStride;
    dstVideoFrame->uStride = srcVideoFrame->uStride;
    dstVideoFrame->vStride = srcVideoFrame->vStride;
    dstVideoFrame->buffer = malloc(yuvSize(dstVideoFrame));
}

void FaceUnityPlugin::checkCreateVideoFrame(VideoPluginFrame* videoFrame)
{
    if (!cacheYuvVideoFramePtr)
    {
        cacheYuvVideoFramePtr = new VideoPluginFrame();
        initCacheVideoFrame(cacheYuvVideoFramePtr, videoFrame);
        return;
    }

    // if video resolution change, we need to resize videoPtr
    if (cacheYuvVideoFramePtr->width != videoFrame->width
    || cacheYuvVideoFramePtr->height != videoFrame->height
    || cacheYuvVideoFramePtr->yStride != videoFrame->yStride
    || cacheYuvVideoFramePtr->uStride != videoFrame->uStride
    || cacheYuvVideoFramePtr->vStride != videoFrame->vStride)
    {
        releaseCacheBuffer(cacheYuvVideoFramePtr);
        cacheYuvVideoFramePtr = new VideoPluginFrame();
        initCacheVideoFrame(cacheYuvVideoFramePtr, videoFrame);
    }
}

void FaceUnityPlugin::memsetCacheBuffer(VideoPluginFrame* videoFrame)
{
    memset(videoFrame->buffer, 0, sizeof(videoFrame->buffer));
}

void FaceUnityPlugin::releaseCacheBuffer(VideoPluginFrame* videoFrame)
{
    if (videoFrame)
    {
        if (videoFrame->buffer)
        {
            delete videoFrame->buffer;
            videoFrame->buffer = NULL;
        }
        delete videoFrame;
        videoFrame = NULL;
    }
}

void FaceUnityPlugin::videoFrameData(VideoPluginFrame* videoFrame, unsigned char *yuvData)
{
    int ysize = videoFrame->yStride * videoFrame->height;
    int usize = videoFrame->uStride * videoFrame->height / 2;
    int vsize = videoFrame->vStride * videoFrame->height / 2;
    
    memcpy(videoFrame->yBuffer, yuvData,  ysize);
    memcpy(videoFrame->uBuffer, yuvData + ysize, usize);
    memcpy(videoFrame->vBuffer, yuvData + ysize + usize, vsize);
}

bool FaceUnityPlugin::onPluginRenderVideoFrame(unsigned int uid, VideoPluginFrame *videoFrame)
{
    return true;
}

bool FaceUnityPlugin::onPluginRecordAudioFrame(AudioPluginFrame *audioFrame)
{
    return true;
}

bool FaceUnityPlugin::onPluginPlaybackAudioFrame(AudioPluginFrame *audioFrame)
{
    return true;
}

bool FaceUnityPlugin::onPluginMixedAudioFrame(AudioPluginFrame *audioFrame)
{
    return true;
}

bool FaceUnityPlugin::onPluginPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioPluginFrame *audioFrame)
{
    return true;
}

bool FaceUnityPlugin::onPluginCaptureVideoFrame(VideoPluginFrame *videoFrame)
{
    if(auth_package_size == 0){
        return false;
    }

    if(mReleased) {
        if(mNamaInited) {
            fuDestroyAllItems();
            mNamaInited = false;
            //no need to update bundle option as bundles will be reloaded anyway
            mNeedUpdateBundles = false;
            //need to reload bundles once resume from stopping
            mNeedLoadBundles = true;
        }

        LOG_F(INFO, "FU Plugin Marked as released, skip");
        return true;
    }

    do {
#if defined(_WIN32)
        int tid = GetCurrentThreadId();
#else
        uint64_t tid;
        pthread_threadid_np(NULL, &tid);
#endif
        if(tid != previousThreadId && mNamaInited) {
            fuOnDeviceLost();
            mNamaInited = false;
            //no need to update bundle option as bundles will be reloaded anyway
            mNeedUpdateBundles = false;
            //need to reload bundles once resume from stopping
            mNeedLoadBundles = true;
        }
        previousThreadId = tid;
        
        
        // 2. initialize if not yet done
        if (!mNamaInited) {
            //load nama and initialize
            std::string assets_dir = folderPath + assets_dir_name + file_separator;
            std::string g_fuDataDir = assets_dir;
            std::vector<char> v3data;
            if (false == Utils::LoadBundle(g_fuDataDir + g_v3Data, v3data)) {
                break;
            }
            if (false == initOpenGL()) {
                break;
            }
            //CheckGLContext();
            fuSetup(reinterpret_cast<float*>(&v3data[0]), v3data.size(), NULL, auth_package, auth_package_size);
            LOG_F(INFO, "FaceUnitySDK: %s", fuGetVersion());
            mNamaInited = true;
        }
        
        
        if(mNeedLoadBundles) {
            fuDestroyAllItems();
            std::string assets_dir = folderPath + assets_dir_name + file_separator;
            std::string g_fuDataDir = assets_dir;
            items.reset(new int[bundles.size()]);
            int i = 0;
            int* items_ptr = items.get();
            for(auto t=bundles.begin(); t!=bundles.end(); ++t){
                std::vector<char> propData;
                if (false == Utils::LoadBundle(g_fuDataDir + t->bundleName, propData)) {
                    continue;
                }
    
                int handle = fuCreateItemFromPackage(&propData[0], (int)propData.size());
                items_ptr[i] = handle;
                
                //load options
                Document d;
                d.Parse(t->options.c_str());
                for (Value::ConstMemberIterator itr = d.MemberBegin();
                     itr != d.MemberEnd(); ++itr)
                {
                    const char* propertyName = itr->name.GetString();
                    char mPropertyName[MAX_PROPERTY_NAME];
                    int result = -1;
                    strlcpy(mPropertyName, propertyName, MAX_PROPERTY_NAME);
                    const Value& propertyValue = itr->value;
                    if(propertyValue.IsNumber()) {
                        result = fuItemSetParamd(items_ptr[i], mPropertyName, propertyValue.GetDouble());

                        LOG_F(INFO, "LoadPlugin: setting %s %f, result: %d", mPropertyName, propertyValue.GetDouble(), result);
                    } else if(propertyValue.IsString()){
                        char mPropertyValue[MAX_PROPERTY_VALUE];
                        strlcpy(mPropertyValue, propertyValue.GetString(), MAX_PROPERTY_VALUE);
                        result = fuItemSetParams(items_ptr[i], mPropertyName, mPropertyValue);
                        
                        LOG_F(INFO, "LoadPlugin: setting %s %s, result: %d", mPropertyName, mPropertyValue, result);
                    } else if(propertyValue.IsArray()){
                        int valueLength = propertyValue.Capacity();
                        double* values = new double[valueLength];
                        for (int i = 0; i < valueLength; i++) {
                            values[i] = propertyValue[i].GetDouble();
                        }
                    }
                }
                
                i++;
            }
            mNeedLoadBundles = false;
        }
        
        if(mNeedUpdateBundles) {
            int i = 0;
            int* items_ptr = items.get();
            for(auto t=bundles.begin(); t!=bundles.end(); ++t){
                if(!t->updated) {
                    continue;
                }
                //load options
                Document d;
                d.Parse(t->options.c_str());
                for (Value::ConstMemberIterator itr = d.MemberBegin();
                     itr != d.MemberEnd(); ++itr)
                {
                    const char* propertyName = itr->name.GetString();
                    char mPropertyName[MAX_PROPERTY_NAME];
                    strlcpy(mPropertyName, propertyName, MAX_PROPERTY_NAME);
                    const Value& propertyValue = itr->value;
                    int result = -1;
                    if(propertyValue.IsNumber()) {
                        result = fuItemSetParamd(items_ptr[i], mPropertyName, propertyValue.GetDouble());
                        
                        LOG_F(INFO, "UpdatePlugin: setting %s %f, result: %d", mPropertyName, propertyValue.GetDouble(), result);
                    } else if(propertyValue.IsString()){
                        char mPropertyValue[MAX_PROPERTY_VALUE];
                        strlcpy(mPropertyValue, propertyValue.GetString(), MAX_PROPERTY_VALUE);
                        result = fuItemSetParams(items_ptr[i], mPropertyName, mPropertyValue);
                        
                        LOG_F(INFO, "UpdatePlugin: setting %s %s, result: %d", mPropertyName, mPropertyValue, result);
                    } else if(propertyValue.IsArray()){
                        int valueLength = propertyValue.Capacity();
                        double* values = new double[valueLength];
                        for (int i = 0; i < valueLength; i++) {
                            values[i] = propertyValue[i].GetDouble();
                        }
                    }
                }
                t->updated = false;
                i++;
            }
            mNeedUpdateBundles = false;
        }
        // 4. make it beautiful
        checkCreateVideoFrame(videoFrame);
        yuvData(videoFrame, cacheYuvVideoFramePtr);
        try {
            fuRenderItemsEx2(
                             FU_FORMAT_I420_BUFFER, reinterpret_cast<int*>(cacheYuvVideoFramePtr->buffer),
                             FU_FORMAT_I420_BUFFER, reinterpret_cast<int*>(cacheYuvVideoFramePtr->buffer),
                             videoFrame->width, videoFrame->height,
                             mFrameID++, items.get(), (int)bundles.size(),
                             NAMA_RENDER_FEATURE_FULL, NULL);
        } catch (...) {
            
        }
        
        int err = fuGetSystemError();
        if(err != 0){
            LOG_F(INFO, "fuSystemError: %d", err);
        }
        
        videoFrameData(videoFrame, reinterpret_cast<unsigned char*>(cacheYuvVideoFramePtr->buffer));
        memsetCacheBuffer(cacheYuvVideoFramePtr);
    } while(false);
    
    return true;
}

int FaceUnityPlugin::load(const char *path)
{
    if(mLoaded) {
        return -101;
    }
    
    
    loguru::add_file("pluginfu.log", loguru::Append, loguru::Verbosity_MAX);
    
    std::string sPath(path);
    folderPath = sPath;
    
    mLoaded = true;
    mReleased = false;
    status = FACEUNITY_PLUGIN_STATUS_STARTED;
    return 0;
}

int FaceUnityPlugin::unLoad()
{
    if(!mLoaded) {
        return false;
    }
    
    delete[] auth_package;
    
    mLoaded = false;
    return 0;
}


int FaceUnityPlugin::enable()
{
    do {
        
    } while (false);
    return 0;
}


int FaceUnityPlugin::disable()
{
    return 0;
}


int FaceUnityPlugin::setParameter(const char *param)
{
    Document d;
    d.Parse(param);
    
    if(d.HasParseError()) {
        return -100;
    }
    
    
    if(d.HasMember("plugin.fu.authdata")) {
        Value& authdata = d["plugin.fu.authdata"];
        if(!authdata.IsArray()) {
            return -101;
        }
        auth_package_size = authdata.Capacity();
        auth_package = new char[auth_package_size];
        for (int i = 0; i < auth_package_size; i++) {
            auth_package[i] = authdata[i].GetInt();
        }
    }
    
    if(d.HasMember("plugin.fu.switch_camera")) {
        Value& value = d["plugin.fu.switch_camera"];
        if(!value.IsBool()) {
            return -101;
        }
        if(value.GetBool()) {
            status = FACEUNITY_PLUGIN_STATUS_STOPPING;
        } else {
            status = FACEUNITY_PLUGIN_STATUS_STARTED;
        }
    }

    if(d.HasMember("plugin.fu.bundles.load")) {
        bundles.clear();
        Value& bundlesData = d["plugin.fu.bundles.load"];
        if(!bundlesData.IsArray()) {
            return -101;
        }
        
        int bundleLength = bundlesData.Capacity();
        for (int i = 0; i < bundleLength; i++) {
            Value& bundleData = bundlesData[i];
            if(!bundleData.HasMember("bundleName") || !bundleData.HasMember("bundleOptions")) {
                return -101;
            }
            Value& bundleName = bundleData["bundleName"];
            if(!bundleName.IsString()){
                return -101;
            }
            FaceUnityBundle bundle;
            bundle.bundleName = bundleName.GetString();
            
            StringBuffer sb;
            Writer<StringBuffer> writer(sb);
            bundleData["bundleOptions"].Accept(writer);
            
            bundle.options = sb.GetString();
            
            bundles.push_back(bundle);
        }
        mNeedLoadBundles = true;
    }
    
    if(d.HasMember("plugin.fu.bundles.update")) {
        Value& updateBundleData = d["plugin.fu.bundles.update"];
        if(!updateBundleData.IsObject()) {
            return -101;
        }
        
        if(!updateBundleData.HasMember("bundleName") || !updateBundleData.HasMember("bundleOptions")) {
            return -101;
        }
        
        std::string bundleName = updateBundleData["bundleName"].GetString();
        
        for(auto t=bundles.begin(); t!=bundles.end(); ++t){
            if(!bundleName.compare(t->bundleName)) {
                StringBuffer sb;
                Writer<StringBuffer> writer(sb);
                updateBundleData["bundleOptions"].Accept(writer);
                t->options = sb.GetString();
                t->updated = true;
            }
        }
        // reset mNeedUpdateBundles
        mNeedUpdateBundles = true;
    }
    
    
    return 0;
}

const char* FaceUnityPlugin::getParameter(const char *key)
{
    return "";
}


int FaceUnityPlugin::release()
{
    mReleased = true;
    delete[] auth_package;
    folderPath = "";
    delete this;
    return 0;
}

IAVFramePlugin* createAVFramePlugin()
{
    return new FaceUnityPlugin();
}
