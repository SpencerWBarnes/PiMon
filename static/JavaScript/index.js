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




function updateGraphStream()





function interpretData(messageJson) {
  for (const key in messageJson) {
    // Skip ack
    if (key == "ack") continue;

    const data = messageJson[key];
    const dataType = typeof(data);

    switch (dataType) {
      case "boolean":
        $boolDisplay.val($boolDisplay.val() + key + ": " + data + "\n");
        continue;

      case "number":
        $intDisplay.val($intDisplay.val() + key + ": " + data + "\n");
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
