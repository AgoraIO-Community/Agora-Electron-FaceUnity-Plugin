import path from "path";
import React, { useState, useEffect } from "react";
import AgoraRtcEngine from 'agora-electron-sdk';
import { makeStyles } from "@material-ui/core/styles";
import Grid from "@material-ui/core/Grid";
import Typography from "@material-ui/core/Typography";
import Container from "@material-ui/core/Container";
import AppBar from "@material-ui/core/AppBar";
import Toolbar from "@material-ui/core/Toolbar";

import BasicForm from "./components/_BasicForm";

const useStyles = makeStyles(theme => ({
  root: {
    flexGrow: 1,
    padding: 12
  },
  title: {
    fontWeight: 400
  },
  divider: {
    marginBottom: "32px"
  }
}));

const defaultState: {
  appId: string;
  channel: string;
  token?: string;
  fuAuth: string;
} = {
  appId: "",
  channel: "",
  token: undefined,
  fuAuth: "[]"
};

function App(props: any) {
  const classes = useStyles();
  const [isLoading, setIsLoading] = useState(false);
  const [isJoined, setIsJoined] = useState(false);
  const [fuEnabled, setFuEnabled] = useState(false);
  const [config, setConfig] = useState(defaultState);
  const [rtcEngine, setRtcEngine] = useState<AgoraRtcEngine|null>(null)

  // handle input change
  const handleChange = (key: string, value: any) => {
    setConfig(config => Object.assign({}, config, {
      [key]: value
    }))
  };
  // handle button click
  const handleClick = (key: string) => {
    switch (key) {
      case "join":
        join()
        break;
      case "enableFu":
        enableFu()
        break;
      case "leave":
        leave()
        break;
      case "disableFu":
        disableFu()
        break;
    }
  };

  const createContainer = (uid: string | number) => {
    let element = document.createElement('div')
    element.id = `video-${uid}`
    Object.assign(element.style, {
      width: '160px',
      height: '120px'
    })
    return element
  }

  const getFuPlugin = () => {
    return getRtcEngine().getPlugins().find(plugin => plugin.id === 'fu-mac')
  }

  const getRtcEngine = () => {
    if (rtcEngine) {
      return rtcEngine
    } else {
      const engine = new AgoraRtcEngine();
      engine.initialize(config.appId);
      engine.initializePluginManager()
      engine.registerPlugin({
        id: 'fu-mac',
        // @ts-ignore
        path: path.resolve(__static, 'fu-mac/libFaceUnityPlugin.dylib')
      })
      const plugin = engine.getPlugins().find(plugin => plugin.id === 'fu-mac')
      if (plugin) {
        plugin.setParameter(JSON.stringify({
          "plugin.fu.authdata": JSON.parse(config.fuAuth)
        }))
      }
      setRtcEngine(engine)
      return engine
    }
    
  }
  // intialize AgoraRtcEngine and do join
  const join = () => {
    const rtcEngine = getRtcEngine()
    rtcEngine.setChannelProfile(1);
    rtcEngine.setClientRole(1);
    rtcEngine.setAudioProfile(0, 1);
    rtcEngine.enableVideo();
    rtcEngine.enableWebSdkInteroperability(true);
    rtcEngine.setVideoEncoderConfiguration({
      width: 640,
      height: 480
    });
    // @ts-ignore
    rtcEngine.joinChannel(config.token || null, config.channel, '',  Number(`${new Date().getTime()}`.slice(7)));
    rtcEngine.on('joinedchannel', (uid: number) => {
      console.log(`local joined`)
      const target = createContainer('local')
      const grid = document.querySelector('#container')
      grid && grid.append(target)
      rtcEngine.setupLocalVideo(target)
    });
    rtcEngine.on('userjoined', (uid: number) => {
      console.log(`${uid} joined`)
      const target = createContainer(uid)
      const grid = document.querySelector('#container')
      grid && grid.append(target)
      rtcEngine.subscribe(uid, target)
    })
    rtcEngine.on('removestream', (uid:number) => {
      console.log(`${uid} left`)
      const target = document.querySelector(`#video-${uid}`)
      target && target.remove()
    })
    rtcEngine.on('leavechannel', () => {
      console.log(`local left`)
      const target = document.querySelector('#video-local')
      target && target.remove()
    })
    rtcEngine.on('error', err => {
      console.log(err)
    })
    rtcEngine.on('executefailed', (funcName: string) => {
      console.log(`${funcName} failed to execute`)
    })

    setIsJoined(true)
  }

  const leave = () => {
    const rtcEngine = getRtcEngine();
    rtcEngine.leaveChannel();
    rtcEngine.releasePluginManager();
    rtcEngine.release();
    setIsJoined(false)
    // rtcEngine.releasePluginManager();
    // rtcEngine.release()
  }


  const enableFu = () => {
    const plugin = getFuPlugin()
    if(plugin) {
      plugin.enable();
      setFuEnabled(true)
    }
  }

  const disableFu = () => {
    const plugin = getFuPlugin()
    if(plugin) {
      plugin.disable();
      setFuEnabled(false)
    }
  }


  return (
    <React.Fragment>
      {/* header */}
      <AppBar color="primary">
        <Toolbar>
          <Typography className={classes.title} variant="h6">
            Agora with FaceUnity
          </Typography>
        </Toolbar>
      </AppBar>
      <Toolbar className={classes.divider} />

      {/* body */}
      <Container>
        <Grid container spacing={3}>
          {/* form */}
          <Grid item xs={12} md={4}>
            <BasicForm
              onClick={handleClick}
              onChange={handleChange}
              {...config}
              fuEnabled={fuEnabled}
              isJoined={isJoined}
              isLoading={isLoading}
            />
          </Grid>

          {/* display area */}
          <Grid item xs={12} md={8}>
            <div id="container">

            </div>
          </Grid>
        </Grid>
      </Container>
    </React.Fragment>
  );
}

export default App;
