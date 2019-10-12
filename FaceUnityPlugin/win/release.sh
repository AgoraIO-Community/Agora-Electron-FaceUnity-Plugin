curl -L -o FaceUnityAll.zip https://github.com/Faceunity/FULivePC/releases/download/v6.2-release/LibUpdate_6.2.0_ALL.docs._20190708165436_0.zip
unzip -u FaceUnityAll.zip
unzip -u LibUpdate_6.2.0_tf_phy_20190708165436_0.zip
mkdir -p ../FULive/win
rm -rf ../FULive/win
mv LibUpdate_6.2.0_tf_phy_20190708165436_0 ../FULive/win
