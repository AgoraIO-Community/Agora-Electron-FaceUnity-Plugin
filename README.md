# FaceUnity Plugin for Agora Electron SDK
> This repo provide a beauty plugin for Agora Electron SDK. Learn [how it works](https://github.com/AgoraIO/Electron-SDK/wiki/How-plugins-work).

# How to build - Mac
- There is a release.sh file under [FaceUnityPlugin/mac](https://github.com/AgoraIO-Community/Agora-Electron-FaceUnity-Plugin/tree/master/FaceUnityPlugin/mac)
- Run the script, it will automatically download FaceUnity Library and build the dll/dylib for you
- FaceUnity 6.2.0 will be installed by default, for now you may need to change the script yourself to work with other FaceUnity versions

# How to build - Win
- There is a release.sh file under [FaceUnityPlugin/win](https://github.com/AgoraIO-Community/Agora-Electron-FaceUnity-Plugin/tree/master/FaceUnityPlugin/win)
- Run the script, it will automatically download FaceUnity Library
- Open the .sln file in the same folder and build
- FaceUnity 6.2.0 will be installed by default, for now you may need to change the script yourself to work with other FaceUnity versions

## Get Started
Please take a look at readme in [/demo](https://github.com/AgoraIO-Community/Agora-Electron-FaceUnity-Plugin/tree/master/FaceUnityPlugin) to learn how to integrate this plugin with agora-electron-sdk to realize beauty in your application.

## APIs
### plugin.fu.authdata
set face unity authdata, please ensure you set this before enable plugin
```
plugin.setParameter(JSON.stringify({
  "plugin.fu.authdata": []
}))
```

### plugin.fu.bundles.load
load a set of face unity bundle and bundle options, for which options the bundle has, please look at face unity documentation instead
```
plugin.setParameter(JSON.stringify({"plugin.fu.bundles.load": [{
    bundleName: "face_beautification.bundle",
    bundleOptions: {
    }
}]}))
```

### plugin.fu.bundles.load
update one face unity bundle option, the bundle has to be loaded first. using this api is more efficient if you need to frequently update plugin options. for which options the bundle has, please look at face unity documentation instead
```
plugin.setParameter(JSON.stringify({"plugin.fu.bundles.update": {
    bundleName: "face_beautification.bundle",
    bundleOptions: {
    }
}}))
```

### plugin.fu.switch_camera
call this api to set switch_camera to true before you switch channel when face unity is on. listen to firstLocalVideoFrame event to set switch_camera to false
```
plugin.setParameter(JSON.stringify({"plugin.fu.switch_camera": true}))
```
