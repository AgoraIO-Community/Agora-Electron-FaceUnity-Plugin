wget https://github.com/Faceunity/FULiveDemoMac/releases/download/v6.2.0/FaceUnity-SDK-Mac-v6.2.0.zip -O FaceUnity.zip
unzip -u FaceUnity.zip
mkdir -p ../FULive
rm -rf ../FULive/mac
mv FaceUnity-SDK-Mac ../FULive/mac
rm -rf build
xcodebuild -project FaceUnityPlugin.xcodeproj
cd build
mv ../../FULive/mac/Resources ./Release/.
zip -r fu-mac.zip Release
