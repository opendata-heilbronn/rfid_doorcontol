var sqlite3 = require('sqlite3').verbose();
var db;

function open(path)
{
  db = new sqlite3.Database(path);
  init();
}

function init()
{
  db.run(`CREATE TABLE IF NOT EXISTS 'access' (
    'cardID'	TEXT UNIQUE,
    'name'	TEXT,
    'accessStatus'	INTEGER,
    PRIMARY KEY(cardID)
  );`);
  db.run(`CREATE TABLE IF NOT EXISTS 'accessLog' (
  	'accessID'	INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,
  	'cardID'	INTEGER NOT NULL,
  	'timestamp'	TEXT NOT NULL,
  	'accessStatus'	INTEGER
  );`);
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

function getCardInfo(cardID, callback)
{
  db.get("SELECT cardID, name, accessStatus FROM access WHERE cardID = ?", cardID, function(err, row){
    if(err){
      console.log(err);
    }
    else {
      callback(row);
    }
  });
}

function getAccessStatus(cardID, callback)
{
  getCardInfo(cardID, function(data){
    if(data == undefined) { //if no row found
      callback(-1); //return "not found" status code
    }
    else {
      callback(data.accessStatus); //return access status code from database
    }
  });
}

function setName(cardID, name)
{
  db.run("UPDATE access SET name = ? WHERE cardID = ?", [name, cardID]);
}

function getName(cardID, callback)
{
  getCardInfo(cardID, function(data){
    if(data == undefined) { //if no row found
      callback(undefined); //return "not found"
    }
    else {
      callback(data.name); //return access status code from database
    }
  });
}

function logAccess(cardID, accessGranted) { //uses current time of function call to insert as timestamp
  db.run("INSERT INTO accessLog (cardID, accessStatus, timestamp) VALUES (?, ?, datetime('now', 'localtime'))", [cardID, accessGranted]);
}



module.exports = {open, close, createAccess, getAccessStatus, setName, getName, logAccess};
