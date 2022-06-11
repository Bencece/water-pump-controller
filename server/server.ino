#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  pinMode(5, OUTPUT);
  digitalWrite(5, 1);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  // Match the request
  int val;
  if (req.indexOf(F("/on")) != -1) {
    val = 0;
  } else if (req.indexOf(F("/off")) != -1) {
    val = 1;
  } else {
    Serial.println(F("invalid request"));
    val = digitalRead(5);
  }

  // Set LED according to the request
  digitalWrite(5, val);

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html lang='hu'><head>    <meta charset='UTF-8'>    <meta http-equiv='X-UA-Compatible' content='IE=edge'>    <meta name='viewport' content='width=device-width, initial-scale=1.0'>    <title>Szivattyú</title>    <style>        .buttON {            background-color: #4CAF50;            border: none;            color: white;            padding: 15px 32px;            text-align: center;            text-decoration: none;            display: inline-block;            font-size: 16px;            margin: 4px 2px;            cursor: pointer;        }        .buttOFF {            background-color: #f44336;            border: none;            color: white;            padding: 15px 32px;            text-align: center;            text-decoration: none;            display: inline-block;            font-size: 16px;            margin: 4px 2px;            cursor: pointer;        }        .center {            margin: auto;            width: 60%;            padding: 10px;            text-align: center;        }    </style></head><body>    <div class='center'>"));
  client.print(F("<button class=""buttON"" onclick=""window.location.href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/on';"">BE</button><button class=""buttOFF"" onclick=""window.location.href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/off';"">KI</button></div></body></html>"));

  Serial.println(F("Disconnecting from client"));
}
