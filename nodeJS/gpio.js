const config = require('./config');
const db = require('./db');
const gpio = require('rpi-gpio');
const doorPin = config.gpio.doorPin; //pin of the door buzzer relay
const buzzTime = config.gpio.buzzTime; //time for the door buzzer to open in ms
const disableGPIO = config.gpio.disableGPIO; //disable all gpio calls (for developing purposes)


//------------ GPIO init ------------//
if (!disableGPIO)
{
  gpio.setup(doorPin, gpio.DIR_OUT);
  gpio.write(doorPin, false); //initialize pin as output in the off state
}

function openWithAuth(cardID)
{
  getAccessStatus(cardID, function(status){
    if (status >= 1) {
      gpio.openDoor();
    }
    db.logAccess(cardID, status, "reader");
  })
}

function openDoor()
{
  if (!disableGPIO)
    gpio.write(doorPin, true);
  else
    console.log("<GPIO> doorPin: HIGH");
  setTimeout(function(){
    if (!disableGPIO)
      gpio.write(doorPin, false);
    else
      console.log("<GPIO> doorPin: LOW");
  }, buzzTime);
}

module.exports = {openDoor, openWithAuth};
