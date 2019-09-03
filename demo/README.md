[中文版本](./README.zh.md)

## Get started

1. Create folder `static` at /demo. (e.g, /demo/static)
> Since electron-webpack will create an environment variable `__static` which refer to this folder

2. Download prebuilt plugin from [Release](https://github.com/AgoraIO-Community/Agora-Electron-FaceUnity-Plugin/releases) into `static`
> Bundles from Release contains dynamic library built from C++ code and necessary resources (images, etc).

3. Get Agora App ID and FaceUnity Authdata
> Maybe your should go for official site for agora/faceunity to get these info.

4. Modify function `getRtcEngine` under `/src/renderer/App.tsx`, following is our default example (in a macos-env):
``` javascript
  engine.registerPlugin({
    id: 'fu-mac',
    // @ts-ignore
    path: path.resolve(__static, 'fu-mac/libFaceUnityPlugin.dylib')
  })
```

5. Run `npm install` and `npm run dev` to startup this demo.

6. Fill in form with your App ID and FaceUnity Authdata