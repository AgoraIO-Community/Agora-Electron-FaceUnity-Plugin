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

FaceUnityPlugin::FaceUnityPlugin()
{
}

FaceUnityPlugin::~FaceUnityPlugin()
{
    
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
	int ret = SetPixelFormat(hgldc, spf, &pfd);
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

unsigned char *FaceUnityPlugin::yuvData(VideoPluginFrame* videoFrame)
{
    int ysize = videoFrame->yStride * videoFrame->height;
    int usize = videoFrame->uStride * videoFrame->height / 2;
    int vsize = videoFrame->vStride * videoFrame->height / 2;
    unsigned char *temp = (unsigned char *)malloc(ysize + usize + vsize);
    
    memcpy(temp, videoFrame->yBuffer, ysize);
    memcpy(temp + ysize, videoFrame->uBuffer, usize);
    memcpy(temp + ysize + usize, videoFrame->vBuffer, vsize);
    return (unsigned char *)temp;
}

int FaceUnityPlugin::yuvSize(VideoPluginFrame* videoFrame)
{
    int ysize = videoFrame->yStride * videoFrame->height;
    int usize = videoFrame->uStride * videoFrame->height / 2;
    int vsize = videoFrame->vStride * videoFrame->height / 2;
    return ysize + usize + vsize;
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

bool FaceUnityPlugin::onPluginCaptureVideoFrame(VideoPluginFrame *videoFrame)
{
    if(auth_package_size == 0){
        return false;
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
            initOpenGL();
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
        unsigned char *in_ptr = yuvData(videoFrame);
        try {
            fuRenderItemsEx2(
                             FU_FORMAT_I420_BUFFER, reinterpret_cast<int*>(in_ptr),
                             FU_FORMAT_I420_BUFFER, reinterpret_cast<int*>(in_ptr),
                             videoFrame->width, videoFrame->height,
                             mFrameID++, items.get(), (int)bundles.size(),
                             NAMA_RENDER_FEATURE_FULL, NULL);
        } catch (...) {
            
        }
        
        int err = fuGetSystemError();
        if(err != 0){
            LOG_F(INFO, "fuSystemError: %d", err);
        }
        
        videoFrameData(videoFrame, in_ptr);
        delete in_ptr;
    } while(false);
    
    return true;
}

bool FaceUnityPlugin::load(const char *path)
{
    if(mLoaded) {
        return false;
    }
    
    
    loguru::add_file("pluginfu.log", loguru::Append, loguru::Verbosity_MAX);
    
    std::string sPath(path);
    folderPath = sPath;
    
    mLoaded = true;
    status = FACEUNITY_PLUGIN_STATUS_STARTED;
    return true;
}

bool FaceUnityPlugin::unLoad()
{
    if(!mLoaded) {
        return false;
    }
    
    delete[] auth_package;
    
    mLoaded = false;
    return true;
}


bool FaceUnityPlugin::enable()
{
    do {
        
    } while (false);
    return true;
}


bool FaceUnityPlugin::disable()
{
    return true;
}


bool FaceUnityPlugin::setParameter(const char *param)
{
    Document d;
    d.Parse(param);
    
    if(d.HasParseError()) {
        return false;
    }
    
    
    if(d.HasMember("plugin.fu.authdata")) {
        Value& authdata = d["plugin.fu.authdata"];
        if(!authdata.IsArray()) {
            return false;
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
            return false;
        }
        if(value.GetBool()) {
            status = FACEUNITY_PLUGIN_STATUS_STOPPING;
        } else {
            status = FACEUNITY_PLUGIN_STATUS_STARTED;
        }
        switching = value.GetBool();
    }

    if(d.HasMember("plugin.fu.bundles.load")) {
        bundles.clear();
        Value& bundlesData = d["plugin.fu.bundles.load"];
        if(!bundlesData.IsArray()) {
            return false;
        }
        
        int bundleLength = bundlesData.Capacity();
        for (int i = 0; i < bundleLength; i++) {
            Value& bundleData = bundlesData[i];
            if(!bundleData.HasMember("bundleName") || !bundleData.HasMember("bundleOptions")) {
                return false;
            }
            Value& bundleName = bundleData["bundleName"];
            if(!bundleName.IsString()){
                return false;
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
            return false;
        }
        
        if(!updateBundleData.HasMember("bundleName") || !updateBundleData.HasMember("bundleOptions")) {
            return false;
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
    
    
    return true;
}

void FaceUnityPlugin::release()
{
    if(mNamaInited) {
        fuOnDeviceLost();
        fuDestroyAllItems();
    }
    mNamaInited = false;
    mNeedUpdateBundles = true;
    mNeedLoadBundles = true;
    delete[] auth_package;
    folderPath = "";
}

IVideoFramePlugin* createVideoFramePlugin()
{
    return new FaceUnityPlugin();
}
