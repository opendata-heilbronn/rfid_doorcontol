var config = require('./config');
var gpio = require('rpi-gpio');
var doorPin = config.gpio.doorPin; //pin of the door buzzer relay
var buzzTime = config.gpio.buzzTime; //time for the door buzzer to open in ms
var disableGPIO = config.gpio.disableGPIO; //disable all gpio calls (for developing purposes)


//------------ GPIO init ------------//
if (!disableGPIO)
{
  gpio.setup(doorPin, gpio.DIR_OUT);
  gpio.write(doorPin, false); //initialize pin as output in the off state
}

function openDoor()
{
  gpio.write(doorPin, true);
  setTimeout(function(){
    gpio.write(doorPin, false);
  }, buzzTime);
}

module.exports = {openDoor};
