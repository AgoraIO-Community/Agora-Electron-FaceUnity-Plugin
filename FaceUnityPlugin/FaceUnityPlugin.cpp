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
        // 0. check if need to clean up fu
        if (status == FACEUNITY_PLUGIN_STATUS_STOPPING) {
            fuDestroyAllItems();
            fuOnDeviceLost();
            fuOnCameraChange();
            fuDestroyLibData();
            mNamaInited = false;
            mNeedUpdateFUOptions = true;
            status = FACEUNITY_PLUGIN_STATUS_STOPPED;
        }
        
        if(status == FACEUNITY_PLUGIN_STATUS_STOPPED) {
            break;
        }
        
        
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
            mNamaInited = true;
        }
        
        
        // 3. beauty params
        // check if options needs to be updated
//        if (mNeedUpdateFUOptions) {
//            fuItemSetParams(mBeautyHandles, "filter_name", const_cast<char*>(filter_name.c_str()));
//            fuItemSetParamd(mBeautyHandles, "filter_level", filter_level);
//            fuItemSetParamd(mBeautyHandles, "color_level", color_level);
//            fuItemSetParamd(mBeautyHandles, "red_level", red_level);
//            fuItemSetParamd(mBeautyHandles, "blur_level", blur_level);
//            fuItemSetParamd(mBeautyHandles, "skin_detect", skin_detect);
//            fuItemSetParamd(mBeautyHandles, "nonshin_blur_scale", nonshin_blur_scale);
//            fuItemSetParamd(mBeautyHandles, "heavy_blur", heavy_blur);
//            fuItemSetParamd(mBeautyHandles, "face_shape", face_shape);
//            fuItemSetParamd(mBeautyHandles, "face_shape_level", face_shape_level);
//            fuItemSetParamd(mBeautyHandles, "eye_enlarging", eye_enlarging);
//            fuItemSetParamd(mBeautyHandles, "cheek_thinning", cheek_thinning);
//            fuItemSetParamd(mBeautyHandles, "cheek_v", cheek_v);
//            fuItemSetParamd(mBeautyHandles, "cheek_narrow", cheek_narrow);
//            fuItemSetParamd(mBeautyHandles, "cheek_small", cheek_small);
//            fuItemSetParamd(mBeautyHandles, "cheek_oval", cheek_oval);
//            fuItemSetParamd(mBeautyHandles, "intensity_nose", intensity_nose);
//            fuItemSetParamd(mBeautyHandles, "intensity_forehead", intensity_forehead);
//            fuItemSetParamd(mBeautyHandles, "intensity_mouth", intensity_mouth);
//            fuItemSetParamd(mBeautyHandles, "intensity_chin", intensity_chin);
//            fuItemSetParamd(mBeautyHandles, "change_frames", change_frames);
//            fuItemSetParamd(mBeautyHandles, "eye_bright", eye_bright);
//            fuItemSetParamd(mBeautyHandles, "tooth_whiten", tooth_whiten);
//            fuItemSetParamd(mBeautyHandles, "is_beauty_on", is_beauty_on);
//            mNeedUpdateFUOptions = false;
//        }
        if(mNeedUpdateBundles) {
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
                    strncpy(mPropertyName, propertyName, MAX_PROPERTY_NAME);
                    const Value& propertyValue = itr->value;
                    if(propertyValue.IsNumber()) {
                        fuItemSetParamd(items_ptr[i], mPropertyName, propertyValue.GetDouble());
                    } else if(propertyValue.IsString()){
                        char mPropertyValue[MAX_PROPERTY_VALUE];
                        strncpy(mPropertyValue, propertyValue.GetString(), MAX_PROPERTY_VALUE);
                        fuItemSetParams(items_ptr[i], mPropertyName, mPropertyValue);
                    }
                }
                
                i++;
            }
            mNeedUpdateBundles = false;
        }
        
        // 4. make it beautiful
        unsigned char *in_ptr = yuvData(videoFrame);
        fuRenderItemsEx2(
                         FU_FORMAT_I420_BUFFER, reinterpret_cast<int*>(in_ptr),
                         FU_FORMAT_I420_BUFFER, reinterpret_cast<int*>(in_ptr),
                         videoFrame->width, videoFrame->height,
                         mFrameID++, items.get(), (int)bundles.size(),
                         NAMA_RENDER_FEATURE_FULL, NULL);
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
    
    if(d.HasMember("plugin.fu.param.filter_name")) {
        Value& value = d["plugin.fu.param.filter_name"];
        if(!value.IsString()) {
            return false;
        }
        std::string sName(value.GetString());
        filter_name = sName;
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
    }
    
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.filter_level", filter_level)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.color_level", color_level)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.red_level", red_level)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.blur_level", blur_level)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.skin_detect", skin_detect)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.nonshin_blur_scale", nonshin_blur_scale)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.heavy_blur", heavy_blur)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.face_shape", face_shape)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.face_shape_level", face_shape_level)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.eye_enlarging", eye_enlarging)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.cheek_thinning", cheek_thinning)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.cheek_v", cheek_v)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.cheek_narrow", cheek_narrow)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.cheek_small", cheek_small)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.cheek_oval", cheek_oval)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.intensity_nose", intensity_nose)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.intensity_forehead", intensity_forehead)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.intensity_mouth", intensity_mouth)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.intensity_forehead", intensity_forehead)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.intensity_chin", intensity_chin)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.change_frames", change_frames)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.eye_bright", eye_bright)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.tooth_whiten", tooth_whiten)
    READ_DOUBLE_VALUE_PARAM(d, "plugin.fu.param.is_beauty_on", is_beauty_on)

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
        mNeedUpdateBundles = true;
    }
    
    // reset mNeedUpdateFUOptions
    mNeedUpdateFUOptions = true;
    
    return false;
}

void FaceUnityPlugin::release()
{
    fuOnDeviceLost();
    fuDestroyAllItems();
    mNamaInited = false;
    mNeedUpdateFUOptions = true;
    delete[] auth_package;
    folderPath = "";
}

IVideoFramePlugin* createVideoFramePlugin()
{
    return new FaceUnityPlugin();
}
