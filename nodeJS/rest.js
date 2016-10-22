const express = require('express');
const app = express();
const bodyParser = require('body-parser');

var db = require('./db');
var gpio = require('./gpio');

app.use(bodyParser.json());
app.use(function(req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Credentials", "true");
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    next();
});

const server = require('http').createServer(app).listen(3010);


app.get('/api/accessLog', function(req, res){
  db.getAccessLog(function(accesses){
    res.send(accesses);
    gpio.openDoor();
  });
});

app.get('/api/accessLog/:numResults', function(req, res){
  db.getAccessLog(function(accesses){
    res.send(accesses);
  }, req.params.numResults);
});
