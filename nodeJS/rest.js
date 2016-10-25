const express = require('express');
const app = express();
const bodyParser = require('body-parser');

const db = require('./db');
const gpio = require('./gpio');

app.use(bodyParser.json());
app.use(function(req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Credentials", "true");
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    next();
});

const server = require('http').createServer(app).listen(3010);


app.get('/api/accessLog', function(req, res){
  var ip = req.connection.remoteAddress;
  console.log("<API> "+ip+" \"GET "+req.url+"\"");
  db.getAccessLog(function(accesses){
    res.send(accesses);
  });
});

app.get('/api/accessLog/:numResults', function(req, res){
  var ip = req.connection.remoteAddress;
  console.log("<API> "+ip+" \"GET "+req.url+"\"");
  db.getAccessLog(function(accesses){
    res.send(accesses);
  }, req.params.numResults);
});

app.post('/api/openDoor', function(req, res){
  var ip = req.connection.remoteAddress;
  var cardID = req.body.cardID;
  if (cardID == undefined)
  {
    console.log("<API> "+ip+" \"GET "+req.url+"\" missing_cardID");
    res.status(400);
    var response = {status: "failure", message: "no cardID parameter found"};
    res.send(JSON.stringify(response));
  }
  else {
    db.getAccessStatus(cardID, function(status) {
      if (status >= 1) {
        gpio.openDoor();
        console.log("<API> "+ip+" \"GET "+req.url+"\" opening_door");
        var response = {status: "success", message: "opening door"};
        res.send(JSON.stringify(response));
      }
      else {
        res.status(403);
        console.log("<API> "+ip+" \"GET "+req.url+"\" unauthorized_cardID");
        var response = {status: "failure", message: "unauthorized cardID"};
        res.send(JSON.stringify(response));
      }
      db.logAccess(cardID, status, "webAPI - " + ip);
    })

  }
});
