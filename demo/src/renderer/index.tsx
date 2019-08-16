import React from "react";
import ReactDOM from "react-dom";
import "./index.css";
import App from "./App";
import {DefaultThemeProvider} from './configures/_configureTheme'

ReactDOM.render(
  <DefaultThemeProvider>
      <App />
  </DefaultThemeProvider>,
  document.getElementById("app")
);
