import React from "react";
import { createMuiTheme } from "@material-ui/core/styles";
import { ThemeProvider } from "@material-ui/styles";

const defaultTheme = createMuiTheme({
  palette: {
    primary: {
      main: "#1E88E5"
    },
    secondary: {
      main: "#FF4081"
    }
  }
});

export const DefaultThemeProvider: React.FC = props => {
  return (
    <ThemeProvider theme={defaultTheme}>
      {props.children}
    </ThemeProvider>
  )
}

const darkTheme = createMuiTheme({
  palette: {
    type: "dark"
  }
})

export const DarkThemeProvider: React.FC = props => {
  return (
    <ThemeProvider theme={darkTheme}>
      {props.children}
    </ThemeProvider>
  )
}

