##Plugin API

```
Step by step to use beauty plugin:
 
1: const engine = new AgoraRtcEngine()
   engine.initialize(config.appId)
   engine.initializePluginManager()

2: let libPath = path.resolve(__static, 'fu-mac/libFaceUnityPlugin.dylib')
  let result = engine.registerPlugin({
    id: 'fu-mac',
    path: libPath
  })

3: plugin.setParameter(...)

4: const plugin = getRtcEngine().getPlugins().find(plugin => plugin.id === 'fu-mac')

  if(plugin) {
      plugin.enable()
  }

5: if(plugin) {
    plugin.disable()
  }
    
6: engine.releasePluginManager()
```


*****
### - initializePluginManager(): number;

 
- It will init plugin manager moudle for rtcEngine. 
- @note
- You need to call it after you initialize the rtcEngine.
- You need to call releasePluginManager() to release the memory when you do not want to use this manager anymore.
- @return 
- 0: call api success;
- < 0: call api fail;

```
samplecode:

const engine = new AgoraRtcEngine()
engine.initialize(config.appId)
engine.initializePluginManager()
```
*****
### - releasePluginManager(): number;


- It will help you to release the plugin manager,you can not call any api of plugin after you call releasePluginManager()
- @return
- 0: call api success;
- < 0: call api fail;

*****
### - registerPlugin(info: PluginInfo): number;


- You can registe a plugin into the rtcEngine.
- @note
- You need to call initializePluginManager() first and then call this api.

- @parameter
- PluginInfo {
-     id: string;
-     path: string;
- }

- id：It is the unique ID of this plugin，you can get this plugin by this id.
- path: The lib path of this plugin.
- @return
- 0: call api success
- <0: call api fail


```
samplecode:

let libPath = path.resolve(__static, 'fu-mac/libFaceUnityPlugin.dylib')
  let result = engine.registerPlugin({
    id: 'fu-mac',
    path: libPath
  })
```
*****
### - unregisterPlugin(pluginId: string): number;


- You can unregiste a plugin by this api.
- @parameter
- pluginId: The unique ID of this plugin which you set by registerPlugin() before.
- @return
- 0: call api success
- <0: call api fail

*****
### - getPlugins(): Plugin[];

- You can get all plugin info which you registe by registerPlugin(info: PluginInfo).
- @return
- Plugin[]: Return a list which contain all of the Plugin Object which you registe before.

*****
### - setParameter: (param: string) => number;

- You can set beauty authdata and beauty by this api.
- @note
- You must call setParameter(param: string) to set authdata and beauty parameters.
- @return
- 0: call api success
- <0: call api fail

```
samplecode:

const plugin = engine.getPlugins().find(plugin => plugin.id === 'fu-mac')
if (plugin) {
<!-- set beauty plugin authdata -->
plugin.setParameter(JSON.stringify({
  "plugin.fu.authdata": JSON.parse(config.fuAuth)
}))

plugin.setParameter(JSON.stringify({"plugin.fu.bundles.load": [{
  <!-- choose bundle, the bundle resource belong to the package which you download from release -->
  bundleName: "face_beautification.bundle",
  bundleOptions: {
    "filter_name": "tokyo",
    "filter_level": 1.0,    
    "color_level": 0.2,
    "red_level": 0.5,
    "blur_level": 6.0,
    "skin_detect": 0.0,
    "nonshin_blur_scale": 0.45,
    "heavy_blur": 0,
    "face_shape": 3,
    "face_shape_level": 1.0,
    "eye_enlarging": 0.5,
    "cheek_thinning": 0.0,
    "cheek_v": 0.0,
    "cheek_narrow": 0.0,
    "cheek_small": 0.0,
    "cheek_oval": 0.0,
    "intensity_nose": 0.0,
    "intensity_forehead": 0.5,
    "intensity_mouth": 0.5,
    "intensity_chin": 0.0,
    "change_frames": 0.0,
    "eye_bright": 1.0,
    "tooth_whiten": 1.0,
    "is_beauty_on": 1.0
  }
  
  ** Parameter Range:
  * "filter_name" : "origin", "delta", "electric", "slowlived", "tokyo", "warm"
  * "filter_level": [0, 1]
  * "color_level" : [0, 1]
  * "red_level" : [0, 1]
  * "blur_level" : [0, 1]
  * "skin_detect" : [0, 1]
  * "nonshin_blur_scale" : [0, 1]
  * "heavy_blur": [0,1]
  * "face_shape" : [0, 1]
  * "face_shape_level" : [0, 1]
  * "eye_enlarging" : [0, 1]
  * "cheek_thinning" : [0, 1]
  * "cheek_v" : [0, 1]
  * "cheek_narrow" : [0, 1]
  * "cheek_small" : [0, 1
  * "cheek_oval" : [0, 1]
  * "intensity_nose" : [0, 1]
  * "intensity_forehead" : [0, 1]
  * "intensity_mouth" : [0, 1]
  * "intensity_chin" : [0, 1]
  * "change_frames" : [0, 1]
  * "eye_bright" : [0, 1]
  * "tooth_whiten" : [0, 1]
  * "is_beauty_on" : [0, 1]
  **********************
}]}))
```

*****
### - enable: () => number;

- You can enable this plugin by this api, because the default settings is disable when you registe this Plugin.
- @note
- You need to call setParameter(param: string) to set the beauty param fisrt.
- @return 
- 0: call api success
- <0: call api fail


```
samplecode:

const plugin = getRtcEngine().getPlugins().find(plugin => plugin.id === 'fu-mac')
if(plugin) {
  plugin.enable()
}
```

*****
### - disable: () => number;

- You can pause this plugin by this api and you can resume it by enable();
- @return
- 0: call api success
- <0: call api fail


```
samplecode:

const plugin = getRtcEngine().getPlugins().find(plugin => plugin.id === 'fu-mac')
if(plugin) {
  plugin.disable()
}
```

