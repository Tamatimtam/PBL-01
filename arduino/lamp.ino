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





//PIN INSTANCE
const int relay_pin = D3;                               //RELAY PIN

//VARIABLES
int ledState = 0;                                       //led status

//OBJECT INSTANCE FOR LIBRARIES
ESP8266WiFiMulti    WiFiMulti;                          // Wifi Object
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

//SERIAL BEGIN
  Serial.begin(115200);
  delay(500);

//SETUP relay_pin
pinMode(relay_pin, OUTPUT);
digitalWrite(relay_pin, LOW); // set relay to off

//WIFI
  WiFiMulti.addAP("test", "tamatama");
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println(WiFi.localIP());




  // Set up web server routes
  server.on("/", HTTP_GET, handleRoot);
  //RELAY / LAMP
  server.on("/turn_on_led", HTTP_GET, toggleLED);
  server.on("/get_led_status", HTTP_GET, getLEDStatus); 

  // Start server
  server.begin();

  Serial.println("Web server started");
}




void loop() {
  server.handleClient();
}
