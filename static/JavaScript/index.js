function selectStream(newStream) {
  let currentStream = getCurrentStream();
  // No change
  if (currentStream == newStream) return;

  if (currentStream != '')
    document.getElementById(currentStream).classList.remove("active");

  document.getElementById(newStream).classList.add("active");
  setCurrentStream(newStream);
  updateOutDisplay(newStream);
}

function updateOutDisplay(streamName) {
  $outDisplay.val(logStreams[streamName]);

  if ($inAutoScroll.hasClass('active'))
    $outDisplay.scrollTop($outDisplay[0].scrollHeight);
}

function connectionAlive(deltaMilliseconds = 1000) {
  let currentTime = (new Date()).getTime();
  return currentTime - watchDogTimer < deltaMilliseconds;
}

// update display, with delay
function update() {
  let messages = xhr.responseText.split('\n');
  messages.slice(position, -1).forEach(function (messageString) {

    // interpret the JSON object string
    const messageJson = JSON.parse(messageString);

    if (messageJson.hasOwnProperty("ack") && ackId == messageJson["ack"]) {
      // No new data, may be dead
      if (!connectionAlive()) {
        heart.classList.remove("alive");
      }
      // Still live, but no updates
      return;
    }
    // New message, refresh heartbeat
    else if (messageJson.hasOwnProperty("ack")) {
      ackId = messageJson["ack"];
      watchDogTimer = (new Date()).getTime();
    }
    
    heart.classList.add("alive");

    // clear the displays
    $intDisplay.val('');
    $boolDisplay.val('');

    // interperate the sensors
    interpretData(messageJson);
  });
  position = messages.length - 1;
}




function updateGraphStream_Keys(sensorKey){
  // This adds newly found sensors to the GraphStream dictionary
  // If key already exists, continue
  // else, add the new key to the dictionary
  var exists = false;
  for (const key in GraphStream){
    if (key == sensorKey){
      exists = true;
      break;
    }

    else {
      exists = false;
    }
  }

  if (exists == false){
    GraphStream[sensorKey] = [];
  }

}

function writeGraphStream_ValuestoNull(){
  // This writes Nulls to each key within the GraphStream dictionary
  // If the key gets updated within the messageJson, the Null is overwritten
  // If the key is not within the messageJson, the Null remains

  // for key in graphstream
  // if size of array is larger than (x)
  // shift() array
  // push() null
  // else
  // push() null
}

function updateGraphStream_ValueArrays(key, value){
  // This function updates the array for each key within the GraphStream Dictionary
  // This should overwrite the Nulls with the current data if it exists

  // pop() the null value off the end of the key array
  // push() the value to the key array
}






function interpretData(messageJson) {

  // for each message, append null to each key currently in the GraphStream
  // This makes sure that they are set before the newest message is parsed
  // the actual values for the GraphStream are updated during the case statement
  writeGraphStream_ValuestoNull();

  for (const key in messageJson) {
    // Skip ack
    if (key == "ack") continue;

    const data = messageJson[key];
    const dataType = typeof(data);

    // This is where we should call the following functions
    // updateGraphStream_Keys()
    updateGraphStream_Keys(key);


    switch (dataType) {
      case "boolean":
        $boolDisplay.val($boolDisplay.val() + key + ": " + data + "\n");
        updateGraphStream_ValueArrays(key, data);
        continue;

      case "number":
        $intDisplay.val($intDisplay.val() + key + ": " + data + "\n");
        updateGraphStream_ValueArrays(key, data);
        continue;
        
      case "string":
        // Modify approperiate stream
        interpretLogData(key, data);
        // Reflow stream in case it was changed
        updateOutDisplay(getCurrentStream());
        continue;
    }
    // Higher order objects
    if (data.hasOwnProperty("units")) {
      $intDisplay.val($intDisplay.val() + key + ": " + data["data"] + " " + data["units"] + "\n");
      updateGraphStream_ValueArrays(key, data);
      continue;
    }

    // Final default, unknown object format
    let dataString = "";
    for (const property in data) {
      dataString += property + ": " + data[property] + ", ";
    }
    // Modify approperiate stream
    interpretLogData(key, dataString);
    // Reflow stream in case it was changed
    updateOutDisplay(getCurrentStream());
  }
}

function interpretLogData(logName, data, limit = 2048) {
  // Does not have key or nav pill yet
  if (!logStreams.hasOwnProperty(logName)) {
    logStreams[logName] = "";

    const newNavPill = 
    `<li class="nav-item">
      <p class="nav-link mb-0" id="` + logName + `" onclick="selectStream(this.innerText)">` + logName + `</p>
    </li>`

    document.getElementById("logNavBar").innerHTML += newNavPill;
  }
  
  const dateObj = new Date();
  const time = "[" +dateObj.getMinutes()+ ":" +dateObj.getSeconds()+ "." +Math.floor(dateObj.getMilliseconds()/100)+ "]";
  // Concat old and new data
  const value = logStreams[logName] + time + ': ' + String(data) + '\n';
  // Store only the 'limit' number of characters
  logStreams[logName] = value.substring(value.length - limit);
}

function getCurrentStream() {
  return logStreams["currentStream"];
}
function setCurrentStream(streamName) {
  logStreams["currentStream"] = streamName;
}
