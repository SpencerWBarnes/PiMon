// Executes when the user clicks on a stream's button
function selectStream(newStream) {
  let currentStream = getCurrentStream();
  // No change
  if (currentStream == newStream) return;

  // Change button's CSS to not be active
  if (currentStream != '')
    document.getElementById(currentStream).classList.remove("active");

  // Change selected stream's button's CSS to be active
  document.getElementById(newStream).classList.add("active");
  // Update stored stream data, and update webpage
  setCurrentStream(newStream);
  updateOutDisplay(newStream);
}

// Display the selected stream's data to the screen
function updateOutDisplay(steamName) {
  // Set display's value to the data in the stream
  $outDisplay.val(logStreams[steamName]);

  // Autoscroll
  if ($inAutoScroll.hasClass('active'))
    $outDisplay.scrollTop($outDisplay[0].scrollHeight);
}


// Determine if the data is stale
function connectionAlive(deltaMilliseconds = 1000) {
  let currentTime = (new Date()).getTime();
  // If too long as passed since watchDog is reset, incoming data is stale
  return currentTime - watchDogTimer < deltaMilliseconds;
}

// Update webpage with data
function update() {
  // Get each message
  let messages = xhr.responseText.split('\n');
  // For each message that has not been analyzed (last message is empty, hence -1)
  messages.slice(position, -1).forEach(function (messageString) {

    // Convert JSON message into an object
    const messageJson = JSON.parse(messageString);

    // If the message's ack was the same as the last message, it is stale
    if (messageJson.hasOwnProperty("ack") && ackId == messageJson["ack"]) {
      // No new data, may be dead
      if (!connectionAlive()) {
        // Remove 'alive' property from heart icon
        heart.classList.remove("alive");
      }
      // WatchDog was not gone off, but nothing to update
      // Exit function
      return;
    }
    // New message, restart WatchDog
    else if (messageJson.hasOwnProperty("ack")) {
      ackId = messageJson["ack"];
      watchDogTimer = (new Date()).getTime();
    }
    
    // New message means connection is alive
    heart.classList.add("alive");

    // clear the numeric and boolean data displays
    $intDisplay.val('');
    $boolDisplay.val('');

    // interperate the data
    interpretData(messageJson);
  });
  position = messages.length - 1;
}

// Interprets new data object
function interpretData(messageJson) {
  // For each piece of data
  for (const key in messageJson) {
    // Skip ack, no need to display
    if (key == "ack") continue;

    const data = messageJson[key];
    const dataType = typeof(data);

    // JavaScript only has four data types: Bool, Num, String, Object
    switch (dataType) {
      case "boolean":
        // Append each new bool data to the display
        $boolDisplay.val($boolDisplay.val() + key + ": " + data + "\n");
        continue;

      case "number":
        // Append each new numeric data to the display
        $intDisplay.val($intDisplay.val() + key + ": " + data + "\n");
        continue;
        
      case "string":
        // Modify the approperiate stream
        interpretLogData(key, data);
        // Update the stream display in case it was the one changed
        updateOutDisplay(getCurrentStream());
        continue;
    }

    // Anything here must be a higher order object

    // If it is a numeric data with a unit
    if (data.hasOwnProperty("units")) {
      $intDisplay.val($intDisplay.val() + key + ": " + data["data"] + " " + data["units"] + "\n");
      continue;
    }

    // Final default, unknown object format
    // Anything here must be a un-united higher order object, unable to handle
    let dataString = "";
    // Build a string containing all of the object's data
    for (const property in data) {
      dataString += property + ": " + data[property] + ", ";
    }

    // Display the object's data as a stream, lets user see the malformed data
    interpretLogData(key, dataString);
    updateOutDisplay(getCurrentStream());
  }
}

// Append log data to the appropriate stream
function interpretLogData(logName, data, limit = 2048) {
  // If stream does not have key or nav pill yet
  if (!logStreams.hasOwnProperty(logName)) {
    logStreams[logName] = "";

    // Manually add pill to html, ain't pretty but it is clean
    const newNavPill = 
    `<li class="nav-item">
      <p class="nav-link mb-0" id="` + logName + `" onclick="selectStream(this.innerText)">` + logName + `</p>
    </li>`

    document.getElementById("logNavBar").innerHTML += newNavPill;
  }
  
  // Prepend data with time stamp
  const dateObj = new Date();
  const time = "[" +dateObj.getMinutes()+ ":" +dateObj.getSeconds()+ "." +Math.floor(dateObj.getMilliseconds()/100)+ "]";
  
  // Append new data to bottom of stream
  const value = logStreams[logName] + time + ': ' + String(data) + '\n';

  // Trim stream data to 'limit' number of characters, smaller memory footprint
  logStreams[logName] = value.substring(value.length - limit);
}

function getCurrentStream() {
  return logStreams["currentStream"];
}
function setCurrentStream(streamName) {
  logStreams["currentStream"] = streamName;
}
