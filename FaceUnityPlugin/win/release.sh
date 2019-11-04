curl -L -o FaceUnityAll.zip https://github.com/Faceunity/FULivePC/releases/download/v6.3-release/LibUpdate_6.3.2_ALL_20190916003804-2.zip
unzip -u FaceUnityAll.zip
unzip -u LibUpdate_6.3.2_tf_phy_20190916003804-2.zip
mkdir -p ../FULive/win
mv LibUpdate_6.3.2_tf_phy_20190916003804-2/* ../FULive/win/.
