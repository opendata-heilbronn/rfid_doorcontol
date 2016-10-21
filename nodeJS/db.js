var sqlite3 = require('sqlite3').verbose();
var db;

function open(path)
{
  db = new sqlite3.Database(path);
  init();
}

function init()
{
  db.run("CREATE TABLE IF NOT EXISTS 'access' ('cardID'	TEXT UNIQUE,	'name'	TEXT, 'accessStatus'	INTEGER,	PRIMARY KEY(cardID));");
}

function close()
{
  db.close();
}

function createAccess(cardID)
{
  db.run("INSERT INTO access (cardID, accessStatus) VALUES ($cardID, 1)", {$cardID: cardID}, function(err){
    if (err) {
      console.log("Error in createAccess  (cardID: <"+cardID+">)  " + err);
    }
  })
}

function getAccessStatus(cardID, callback)
{
  db.get("SELECT accessStatus FROM access WHERE cardID = ?", cardID, function(err, row){
    if(err){
      console.log(err);
    }
    else {
      if (row == undefined) {
        callback(-1);
      }
      else {
        callback(row.accessStatus);
      }
    }
  });
}

function setName(cardID, name)
{
  db.run("UPDATE access SET name = ? WHERE cardID = ?", [name, cardID]);
}

function getName(cardID, callback)
{
  db.get("SELECT name FROM access WHERE cardID = ?", cardID, function(err, row){
    if(err){
      console.log(err);
    }
    else {
      callback(row.name);
    }
  });
}



module.exports = {open, close, createAccess, getAccessStatus, setName, getName};
