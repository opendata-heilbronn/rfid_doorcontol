/*
You need to install libnfc (e.g. sudo apt install libnfc5 libnfc-dev)
and maybe: https://gist.github.com/danieloneill/3be43d5374c80d89ea73

https://www.npmjs.com/package/nodeacr122u
https://www.npmjs.com/package/sqlite3
https://www.npmjs.com/package/rpi-gpio

*/
var config = require('./config');
var reader = require('./reader');
var db = require('./db');
var gpio = require('./gpio');
var api = require('./rest');

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


function rfidCallback(data)
{
  console.log("Detected Tag: " + data);
  //TODO data handling with real tags
  var cardID = data//.something
  getAccessStatus(cardID, function(status){
    if (status >= 1) {
      gpio.openDoor();
    }
    db.logAccess(cardID, status);
  })
}
