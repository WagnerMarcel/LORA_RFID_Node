var ttn = require("ttn")
var http = require('http')

var appID = "dhbw_lora_wan_node"
var accessKey = "ttn-account-v2.ZPRzCqpIXqulZFCzhmNQHCS7RQ7-9bI_X1U6Shrifyg"

var message = ""
var id = ""

var server = http.createServer(function (request, response) {
  response.writeHead(200, {"Content-Type": "text/plain"});
  response.write(message);
  response.end(id);
});

// Listen on port 8000, IP defaults to 127.0.0.1
server.listen(8000);

ttn.data(appID, accessKey)
  .then(function (client) {
    message += "Connected!\n"
    client.on("uplink", function (devID, payload) {
      console.log("Received uplink from ", devID)
      message += "Recieved uplink from " + devID + "\n"
      console.log(payload)
      id = payload.payload_fields.text

    if(id.includes("0123456789")){
      console.log("True");
      client.send(devID, "True", 1);
    }else {
      console.log("False");
      client.send(devID, "False", 1);
    }
    })
  })
  .catch(function (error) {
    console.error("Error", error)
    process.exit(1)
  })
