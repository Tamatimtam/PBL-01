#include <WebSockets4WebServer.h>
#include <SocketIOclient.h>
#include <WebSockets.h>
#include <WebSocketsVersion.h>
#include <WebSocketsServer.h>
#include <WebSocketsClient.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <Hash.h>
#include <DHT.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRutils.h>  
#include <ir_Panasonic.h>



//DEFINE FOR WEBSOCKET VARIABLES
String old_value, value, DHTTemp = "";

//DEFINE FOR DHT
#define DHTPIN D2
#define DHTTYPE DHT22 

//PIN INSTANCE
const uint16_t kIrLed = D5;                             //AC Pin
const int relay_pin = D3;                               //RELAY PIN

//VARIABLES
int ledState = 0;                                       //led status
bool acState = false;                                   //AC status
int userTemperature = 25;                               //AC Temperature

//OBJECT INSTANCE FOR LIBRARIES
ESP8266WiFiMulti    WiFiMulti;                          // Wifi Object
ESP8266WebServer    server(80);                         // Web Server Object
WebSocketsServer    webSocket = WebSocketsServer(81);   //Websocket Object
IRac ac(kIrLed);                                        //AC IR object
DHT dht(DHTPIN, DHTTYPE);                               //DHT Object








//WEBSOCKET DEFAULTS
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // send message to client
        webSocket.sendTXT(num, "0");
      }
      break;

    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      // send message to client
      // webSocket.sendTXT(num, "message here");
      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
      
    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\n", num, length);
      hexdump(payload, length);
      // send message to client
      // webSocket.sendBIN(num, payload, length);
      break;
  }

}




//AC FUNCTIONS
void ACSetTemp() {

  if (server.hasArg("temperature")) { //check if the route contains a temperature argument
      userTemperature = server.arg("temperature").toInt();  //put the argument from route to an int variable
      ac.next.degrees = userTemperature; //put variable into ac parameter function

      // Toggle A/C state
      ac.sendAc(); //send IR data
      server.send(200, "text/html", "Temperature Set to " + String(userTemperature) + " degrees"); //Send succes message
  }else{ //No parameters given
    server.send(400, "text/html", "Bad Request: No temperature parameter provided"); //send error
       }
}

void ACUp() {

  userTemperature++;
  ac.next.degrees = userTemperature;
  ac.sendAc();
    server.send(200, "text/html", String(userTemperature));
}

void ACDown() {

  userTemperature--;
  ac.next.degrees = userTemperature;
  ac.sendAc();
    server.send(200, "text/html", String(userTemperature));
}

void ACTurnOn() {
  // Set temperature based on user input
  ac.next.degrees = userTemperature;

  // Toggle A/C state
  acState = !acState;       //Local Variable
  ac.next.power = acState; //library variable
  
  
  // Send Data to AC via IR
    ac.sendAc();
    server.send(200, "text/html", acState ? "AC Is ON!" : "AC Is OFF!");
}

void getACStatus() {
    server.send(200, "text/plain", (acState == 1) ? "On" : "Off");
}

void getACTemp() {
      server.send(200, "text/plain", String(userTemperature));
}

void DHTSend() {} //NOT YET IMPLEMENTED




//LAMP FUNCTIONS
void toggleLED() {
    ledState = 1 - ledState; // Toggle the LED state (0 to 1 or 1 to 0)

    digitalWrite(relay_pin, ledState); // Set the LED state
    server.send(200, "text/plain", ledState ? "LED turned on" : "LED turned off");
}

void getLEDStatus() {
    server.send(200, "text/plain", (ledState == 1) ? "On" : "Off");
}




//WEB SERVER FUNCTIONS
void handleRoot() {
  String buttonText = acState ? "Turn Off A/C" : "Turn On A/C";
  String html = "<html><body>"
                "<button onclick=\"window.location.href='/acControl'\">" + buttonText + "</button>"
                "<br><br>"
                "Set Temperature: <input type='number' id='temperatureInput' value='" + String(userTemperature) + "'>"
                "<br>"
                "Current Temperature: <span id='currentTemp'>" + String(userTemperature) + "</span>"
                "<br>"
                "AC Status: <span id='acStatus'>" + String(acState ? "ON" : "OFF") + "</span>"
                "<script>"
                " function setTemperature() {"
                "var temperatureInput = document.getElementById('temperatureInput').value;"
                "window.location.href = '/setTemperature?temperature=' + temperatureInput;"
                "}"
                "</script>"
                "<button onclick=\"setTemperature()\">Set</button>"
                "<button onclick=\"window.location.href='/setTemperature'\">" + "Ubah Suhu AC" + "</button>";
                

  // LED BUTTONS
  html += "<br> <br>";
  html += "<h1>Control LED</h1>";
  html += "<form method='get' action='/turn_on_led'>";
  html += "<button>Toggle LED</button>";
  html += "</form>";
  html += "<p>LED Status: " + String(ledState ? "ON" : "OFF") + "</p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}




  //404 
  void handleNotFound() {
  server.send(404,   "text/html", "<html><body><p>404 Error</p></body></html>" );
}





//SETUP
void setup() {

    #pragma region //DEFAULT AC PARAMETERS
ac.next.protocol = decode_type_t::LG;
ac.next.model = 1;  // Some A/Cs have different models. Try just the first.
ac.next.mode = stdAc::opmode_t::kCool;  // Run in cool mode initially.
ac.next.celsius = true;  // Use Celsius for temp units. False = Fahrenheit
ac.next.fanspeed = stdAc::fanspeed_t::kMedium;  // Start the fan at medium.
ac.next.swingv = stdAc::swingv_t::kOff;  // Don't swing the fan up or down.
ac.next.swingh = stdAc::swingh_t::kOff;  // Don't swing the fan left or right.
ac.next.light = false;  // Turn off any LED/Lights/Display that we can.
ac.next.beep = false;  // Turn off any beep from the A/C if we can.
ac.next.econo = false;  // Turn off any economy modes if we can.
ac.next.filter = false;  // Turn off any Ion/Mold/Health filters if we can.
ac.next.turbo = false;  // Don't use any turbo/powerful/etc modes.
ac.next.quiet = false;  // Don't use any quiet/silent/etc modes.
ac.next.sleep = -1;  // Don't set any sleep time or modes.
ac.next.clean = false;  // Turn off any Cleaning options if we can.
ac.next.clock = -1;  // Don't set any current time if we can avoid it.
#pragma endregion

//SERIAL BEGIN
  Serial.begin(115200);
  delay(500);

//SETUP relay_pin
pinMode(relay_pin, OUTPUT);
digitalWrite(relay_pin, LOW); // set relay to off

//SETUP DHT
dht.begin();

//WIFI
  WiFiMulti.addAP("test", "tamatama");
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println(WiFi.localIP());




//WEBSOCKETS
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);





  // Set up web server routes
  //AC
  server.on("/", HTTP_GET, handleRoot);
  server.on("/acControl", HTTP_GET, ACTurnOn);
  server.on("/setTemperature", HTTP_GET, ACSetTemp);
  server.on("/get_AC_status", HTTP_GET, getACStatus); 
  server.on("/get_AC_temp", HTTP_GET, getACTemp); 
  server.on("/ACUp", HTTP_GET, ACUp); 
  server.on("/ACDown", HTTP_GET, ACDown);  
  //RELAY / LAMP
  server.on("/turn_on_led", HTTP_GET, toggleLED);
  server.on("/get_led_status", HTTP_GET, getLEDStatus); 

  // Start server
  server.begin();

  Serial.println("Web server started");
}





void handleUserMessage() {
  delay(1000);
     DHTTemp = dht.readTemperature();
    webSocket.broadcastTXT(DHTTemp);
    Serial.println("User message broadcasted: " + DHTTemp);
  server.send(200, "text/html", "Message received and broadcasted successfully");
}




void loop() {
  
  webSocket.loop();
  server.handleClient();
  handleUserMessage();
  

}
