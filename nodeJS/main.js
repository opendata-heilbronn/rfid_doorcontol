/*
You need to install libnfc (e.g. sudo apt install libnfc5 libnfc-dev)
and maybe: https://gist.github.com/danieloneill/3be43d5374c80d89ea73

https://www.npmjs.com/package/nodeacr122u
https://www.npmjs.com/package/sqlite3
https://www.npmjs.com/package/rpi-gpio

*/
const config = require('./config');
const reader = require('./reader');
const db = require('./db');
const gpio = require('./gpio');
const api = require('./rest');
const sound = require('./sound.js');

var log = console.log;

console.log = function(){
  //log(new Date.no)
  log.apply(console, [new Date().toISOString()].concat("    " + arguments['0']));
};

console.log("Starting nodeJS RFID Doorcontrol");
console.log("ATTENTION: Log Timestamp is in UTC (GMT+0)");

//------------ RFID Reader init ------------//
//reader.open(rfidCallback);

//------------ DB init ------------//
db.open(config.sqlite.path);

//db.logAccess("asdf", 1);

/*db.createAccess("asdf"); //Temporary test code, remove before prod
db.getAccessStatus("asdf", function(status){
  console.log(status);
});
db.setName("asdf", "Herr Asdf");
db.getName("asdf", function(name){
  console.log(name);
})*/

setTimeout(function () {
  sound.playSoundFile('imperial_march')
}, 10);


function rfidCallback(data)
{
  console.log("Detected Tag: " + data);
  //TODO data handling with real tags
  var cardID = data//.something
  gpio.openWithAuth(cardID);
  /*getAccessStatus(cardID, function(status){
    if (status >= 1) {
      gpio.openDoor();
    }
    db.logAccess(cardID, status);
  })*/
}
