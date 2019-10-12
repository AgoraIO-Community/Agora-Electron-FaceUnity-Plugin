wget https://github.com/Faceunity/FULiveDemoMac/releases/download/v6.2.0/FaceUnity-SDK-Mac-v6.2.0.zip -O FaceUnity.zip
unzip -u FaceUnity.zip
mkdir -p ../FULive
mv FaceUnity-SDK-Mac FaceUnityPlugin/FULive/mac
xcodebuild -project FaceUnityPlugin.xcodeproj
cd build
mv ../../FULive/mac/Resources ./Release/assets
zip -r fu-mac.zip Release