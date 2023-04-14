const express = require("express");
const compression = require();

const app = express();

app.use(compression());
app.use(express.static("./dist"));

app.listen(80, () => {
  console.log("server is running at http://127.0.0.1");
});
