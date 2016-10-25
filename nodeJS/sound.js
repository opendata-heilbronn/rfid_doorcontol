const config = require('./config');

const player = require('play-sound')(opts = {});
const fs = require('fs');


var availableFiles = [];
var currentlyPlaying;

fs.readdir(config.sound.folder, (err, files) => {
  files.forEach(file => {
    availableFiles.push(file);
  });
});

function stopPlaying()
{
  if(currentlyPlaying != undefined)
    currentlyPlaying.kill();
}

function playSoundFile(fileToPlay)
{
  availableFiles.forEach(file => {
    var f = file.substring(0, file.lastIndexOf('.'));
    if (f.toLowerCase() == fileToPlay.toLowerCase())
    {
      stopPlaying();
      var fullPath = config.sound.folder + "/" + file;
      currentlyPlaying = player.play(fullPath, {timeout: config.sound.maxLength}, function(err){
        if(err && !err.killed)
          console.log(err);
      });
    }
  });
}

module.exports = {playSoundFile};
