cp -rf ../FULive/win/assets ./Release/.
curl -L -o face_beautification.bundle https://github.com/Faceunity/FULivePC/raw/v6.2-release/assets/face_beautification.bundle
cp -rf ./face_beautification.bundle ./Release/assets/.
rm -rf FaceUnityPlugin
7z a -r -tzip fu-win.zip ./Release
