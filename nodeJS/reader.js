var ACR122U = require('nodeacr122u');


var reader = ACR122U.create();


function open(callback)
{
  reader.open('acr122_usb:', function(obj, err) {
      if( err ) {
          console.log("ACR122U: Failed to open device: "+err);
          return false;
      }
      reader['data'] = function(data) {
          var dataString = JSON.stringify(data, null, 2)
          //console.log("ACR122U event: "+dataString);
          callback(dataString);
      };
  });
}

module.exports = {open};
