var config = {};
config.gpio = {
  doorPin: 15,            //pin of the door buzzer relay
  buzzTime: 3000,         //time for the door buzzer to open in ms
  disableGPIO: 1          //disable all gpio calls (for developing purposes)
};
config.sqlite = {
  path: "access.sqlite"   //database file name
};
config.sound = {
  folder: "sounds",       //folder in which the sound files are stored
  maxLength: 10000        //maximum play time for a sound file
};

module.exports = config;
