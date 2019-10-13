cp -rf ../FULive/win/assets ./Release/.
rm -rf FaceUnityPlugin
7z a -r -tzip fu-win.zip ./Release
