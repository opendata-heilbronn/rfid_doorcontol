/*
You need to install libnfc (e.g. sudo apt install libnfc5 libnfc-dev)
https://gist.github.com/danieloneill/3be43d5374c80d89ea73

https://www.npmjs.com/package/nodeacr122u
https://www.npmjs.com/package/sqlite3
https://www.npmjs.com/package/rpi-gpio

*/
var reader = require('./reader');

var db = require('./db');
var dbPath = "access.sqlite";

var gpio = require('rpi-gpio');
var doorPin = 15; //pin of the door buzzer relay
var buzzTime = 3000; //time for the door buzzer to open in ms
var disableGPIO = 1; //disable all gpio calls (for developing purposes)

//------------ RFID Reader init ------------//
//reader.open(rfidCallback);

//------------ DB init ------------//
db.open(dbPath);
/*db.createAccess("asdf"); //Temporary test code, remove before prod
db.getAccessStatus("asdf", function(status){
  console.log(status);
});
db.setName("asdf", "Herr Asdf");
db.getName("asdf", function(name){
  console.log(name);
})*/

//------------ GPIO init ------------//
if (!disableGPIO)
{
  gpio.setup(doorPin, gpio.DIR_OUT);
  gpio.write(doorPin, false); //initialize pin as output in the off state
}


function rfidCallback(data)
{
  console.log("Detected Tag: " + data);
  //TODO data handling with real tags
  var cardID = data//.something
  getAccessStatus(cardID, function(status){
    if (status >= 1) {
      openDoor();
    }
  })
}


function openDoor()
{
  gpio.write(doorPin, true);
  setTimeout(function(){
    gpio.write(doorPin, false);
  }, buzzTime);
}
