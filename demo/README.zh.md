[English Version](./README.md)

## 快速开始

1. 在demo根目录下创建`static`目录
> electron-webpack 会创建一个 __static 环境变量指向该目录

2.从[Release](https://github.com/AgoraIO-Community/Agora-Electron-FaceUnity-Plugin/releases)下载预编译好的插件到`static`目录
> 你所下载内容中包含编译好的动态库(.dll/.dylib)以及FaceUnity本身所需要的一些资源文件（图片贴纸等）

3. 取得Agora App ID 和 FaceUnity 的 AuthData
> 可以前去他们的官网，或者联系相关人员获得。

4. 修改 `getRtcEngine` 函数，该函数位于`/src/renderer/App.tsx`, 默认是这样的（我们使用macos时的例子，使用windows的需要修改相应动态库路径）:
``` javascript
  engine.registerPlugin({
    id: 'fu-mac',
    // @ts-ignore
    path: path.resolve(__static, 'fu-mac/libFaceUnityPlugin.dylib')
  })
```

5. 运行 `npm install` 命令安装依赖后，运行`npm run dev`启动开发环境的应用

6. 填写表单（使用之前获得的Agora AppID 和 FaceUnity AuthData）