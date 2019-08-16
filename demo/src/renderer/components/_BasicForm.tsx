import React from "react";
import { makeStyles } from "@material-ui/core/styles";
import Card from "@material-ui/core/Card";
import CardActions from "@material-ui/core/CardActions";
import CardContent from "@material-ui/core/CardContent";
import Button from "@material-ui/core/Button";
import TextField from "@material-ui/core/TextField";


const useStyles = makeStyles(theme => ({
  advanceSettings: {
    marginTop: 16
  },
  buttonContainer: {
    display: "flex",
    justifyContent: "space-around"
  },
  buttonItem: {
    width: "38.2%"
  }
}));

export interface BasicFormProps {
  appId: string;
  channel: string;
  fuAuth: string;
  token?: string;

  onChange: (key: string, value: any) => void;
  onClick: (key: string) => void;

  fuEnabled: boolean;
  isJoined: boolean;
  isLoading: boolean;
}

const BasicForm: React.FC<BasicFormProps> = props => {
  const classes = useStyles();
  const { fuEnabled, isLoading, isJoined } = props;

  const handleChange = (key: string) => (e: React.ChangeEvent<unknown>) => {
    return props.onChange(key, (e.target as HTMLInputElement).value);
  };

  const handleClick = (key: string) => (e: React.ChangeEvent<unknown>) => {
    return props.onClick(key);
  };

  const JoinLeaveBtn = () => {
    return (
      <Button
        className={classes.buttonItem}
        color={isJoined ? "secondary" : "primary"}
        onClick={isJoined ? handleClick("leave") : handleClick("join")}
        variant="contained"
        disabled={isLoading}
      >
        {isJoined ? "Leave" : "Join"}
      </Button>
    );
  };

  const FUToggleBtn = () => {
    return (
      <Button
        className={classes.buttonItem}
        color={fuEnabled ? "secondary" : "default"}
        onClick={
          fuEnabled ? handleClick("disableFu") : handleClick("enableFu")
        }
        variant="contained"
        disabled={!isJoined || isLoading}
      >
        {fuEnabled ? "Disable FU" : "Enable FU"}
      </Button>
    );
  };

  return (
    <React.Fragment>
      <Card>
        <CardContent>
          <form noValidate autoComplete="off">
            <TextField
              required
              value={props.appId}
              onChange={handleChange("appId")}
              id="appId"
              label="App ID"
              fullWidth
              margin="normal"
            />
            <TextField
              required
              value={props.channel}
              onChange={handleChange("channel")}
              id="channel"
              label="Channel"
              fullWidth
              margin="normal"
            />

            <TextField
              value={props.token}
              onChange={handleChange("token")}
              id="token"
              label="Token"
              fullWidth
              margin="normal"
            />


            <TextField
              value={props.fuAuth}
              onChange={handleChange("fuAuth")}
              id="fuAuth"
              label="FU Auth"
              fullWidth
              margin="normal"
            />
          </form>
        </CardContent>
        <CardActions className={classes.buttonContainer}>
          <JoinLeaveBtn />
          <FUToggleBtn />
        </CardActions>
      </Card>

      {/* advanced settings */}
      {/* <DarkThemeProvider>
        <ExpansionPanel className={classes.advanceSettings}>
          <ExpansionPanelSummary expandIcon={<ExpandMoreIcon />}>
            <Typography>Advanced Settings</Typography>
          </ExpansionPanelSummary>
          <ExpansionPanelDetails>

          </ExpansionPanelDetails>
        </ExpansionPanel>
      </DarkThemeProvider> */}
    </React.Fragment>
  );
};

export default BasicForm;
