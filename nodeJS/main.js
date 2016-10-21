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

db.open(dbPath);
db.createAccess("asdf");
db.getAccessStatus("asdf", function(status){
  console.log(status);
});

db.setName("asdf", "Herr Asdf");
db.getName("asdf", function(name){
  console.log(name);
})

//reader.open(rfidCallback);

function rfidCallback(data)
{
  console.log("Detected Tag: " + data);
}
