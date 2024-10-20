

#include <WebSockets4WebServer.h>
#include <ESP8266HTTPClient.h>
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
#include <espnow.h>


// DEFINE MAC ADDRESS OF RECEIVER
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// DEFINE A DATASTRUCTURE
typedef struct ESPNowStruct {
    int command; 
    int data;
} ESPNowStruct;

ESPNowStruct DataClassify;

// PIN INSTANCE
const uint16_t kIrLed = D5;  // AC Pin
const int relay_pin = D3;    // RELAY PIN

// VARIABLES
int ledState1 = 0;          // led status
int ledState2 = 0;          // led status
bool acState1 = false;      // AC status
bool acState2 = false;      // AC status
int userTemperature = 25;  // AC Temperature
const char* serverName = "http://192.168.47.77/acControl1"; // Replace with your receiving ESP IP address and endpoint

// OBJECT INSTANCE FOR LIBRARIES
ESP8266WiFiMulti WiFiMulti;           // Wifi Object
ESP8266WebServer server(80);          // Web Server Object
IRac ac(kIrLed);                      // AC IR object


//CALLBACKS
// DEFINE Function when data is recieved
void OnDataRecv(uint8_t* mac_addr, uint8_t* incomingData, uint8_t len) {
    memcpy(&DataClassify, incomingData, sizeof(DataClassify));
    unsigned long receiveTime = millis();  // Capture the current time
    Serial.print("ESPNOW MESSAGE RECEIVED");

    int32_t rssi = WiFi.RSSI();
    rssi = -1 * rssi;  // Multiply by -1 to reverse the logic
    Serial.print("RSSI: -");
    Serial.print(rssi);
    Serial.println("dBm");
    // Process received data
    switch (DataClassify.command) {
        case 1: // Toggle AC power
            acState = !acState;
            ac.next.power = acState;
            ac.sendAc();
            Serial.print("AC successfully turned to: ");
            Serial.println(acState ? "ON" : "OFF");
            break;
        case 2: // AC up command
            userTemperature = DataClassify.data;
            ac.next.degrees = userTemperature;
            ac.sendAc();
            Serial.print("AC temperature increased to: ");
            Serial.println(userTemperature);
            break;
        case 3: // AC down command
            userTemperature = DataClassify.data;
            ac.next.degrees = userTemperature;
            ac.sendAc();
            Serial.print("AC temperature decreased to: ");
            Serial.println(userTemperature);
            break;
        case 4: // LAMP toggle command
            ledState = DataClassify.data;
            digitalWrite(relay_pin, ledState); // Set the LED state
            Serial.print("LED state changed to: ");
            Serial.println(ledState ? "ON" : "OFF");
            break;
        default:
            Serial.println("Unknown command received.");
            break;
    }
}


// AC FUNCTIONS
void ACSetTemp() {
    if (server.hasArg("temperature")) {  // check if the route contains a temperature argument
        userTemperature = server.arg("temperature").toInt();  // put the argument from route to an int variable
        ac.next.degrees = userTemperature;  // put variable into ac parameter function

        // Toggle A/C state
        ac.sendAc();  // send IR data
        server.send(200, "text/html", "Temperature Set to " + String(userTemperature) + " degrees");  // Send success message
    } else {  // No parameters given
        server.send(400, "text/html", "Bad Request: No temperature parameter provided");  // send error
    }
}

void ACUp() {
    userTemperature++;
    ac.next.degrees = userTemperature;
    ac.sendAc();
    server.send(200, "text/html", String(userTemperature));

    DataClassify.command = 2;
    DataClassify.data = userTemperature;
    esp_now_send(broadcastAddress, (uint8_t*)&DataClassify, sizeof(DataClassify));
    unsigned long sendTime = millis();  // Capture the current time
    Serial.println("ESPNOW MESSAGE SUCCESFULLY SENT | ");
    Serial.println(sendTime);  // Print the send time
}

void ACDown() {
    userTemperature--;
    ac.next.degrees = userTemperature;
    ac.sendAc();
    server.send(200, "text/html", String(userTemperature));

    DataClassify.command = 3;
    DataClassify.data = userTemperature;
    esp_now_send(broadcastAddress, (uint8_t*)&DataClassify, sizeof(DataClassify));
    unsigned long sendTime = millis();  // Capture the current time
    Serial.println("ESPNOW MESSAGE SUCCESFULLY SENT | ");
    Serial.println(sendTime);  // Print the send time
}

void ACTurnOn1() {
    Serial.println("WiFi MESSAGE RECIEVED");
    int32_t rssi = WiFi.RSSI();
    rssi = -1 * rssi;  // Multiply by -1 to reverse the logic
    Serial.print("RSSI: -");
    Serial.print(rssi);
    Serial.println("dBm");
    // Set temperature based on user input
    ac.next.degrees = userTemperature;

    // Toggle A/C state
    acState = !acState;           // Local Variable
    ac.next.power = acState;  // library variable

    // Send Data to AC via IR
    ac.sendAc();
    server.send(200, "text/html", acState ? "AC Is ON!" : "AC Is OFF!");

    // DataClassify.command = 1;
    // DataClassify.data = acState;
    // esp_now_send(broadcastAddress, (uint8_t*)&DataClassify, sizeof(DataClassify));
    // unsigned long sendTime = millis();  // Capture the current time
    // Serial.println("ESPNOW MESSAGE SUCCESFULLY SENT | ");
    // Serial.println(sendTime);  // Print the send time
}


void ACTurnOn2() {
    // // Set temperature based on user input
    // ac.next.degrees = userTemperature;

    // // Toggle A/C state
    // acState = !acState;           // Local Variable
    // ac.next.power = acState;  // library variable

    // // Send Data to AC via IR
    // ac.sendAc();
    server.send(200, "text/html", acState ? "AC Is ON!" : "AC Is OFF!");

    DataClassify.command = 1;
    DataClassify.data = acState;
    esp_now_send(broadcastAddress, (uint8_t*)&DataClassify, sizeof(DataClassify));
    unsigned long sendTime = millis();  // Capture the current time
    Serial.println("ESPNOW MESSAGE SUCCESFULLY SENT | ");
    Serial.println(sendTime);  // Print the send time
}


void ACTurnOn3() {
    // Set temperature based on user input
    ac.next.degrees = userTemperature;

    // Toggle A/C state
    acState = !acState;           // Local Variable
    ac.next.power = acState;  // library variable

    // Send Data to AC via IR
    ac.sendAc();
    server.send(200, "text/html", acState ? "AC Is ON!" : "AC Is OFF!");

    DataClassify.command = 1;
    DataClassify.data = acState;
    esp_now_send(broadcastAddress, (uint8_t*)&DataClassify, sizeof(DataClassify));
    unsigned long sendTime = millis();  // Capture the current time
    Serial.println("ESPNOW MESSAGE SUCCESFULLY SENT | ");
    Serial.println(sendTime);  // Print the send time
}


void getACStatus() {
    server.send(200, "text/plain", (acState1 == 1) ? "On" : "Off");
}

void getACTemp() {
    server.send(200, "text/plain", String(userTemperature));
}

// LAMP FUNCTIONS
void toggleLED1() {
    ledState = 1 - ledState;  // Toggle the LED state (0 to 1 or 1 to 0)
    Serial.println("WiFi MESSAGE RECIEVED");
    int32_t rssi = WiFi.RSSI();
    rssi = -1 * rssi;  // Multiply by -1 to reverse the logic
    Serial.print("RSSI: -");
    Serial.print(rssi);
    Serial.println("dBm");
    digitalWrite(relay_pin, ledState);  // Set the LED state
    server.send(200, "text/plain", ledState ? "LED turned on" : "LED turned off");

    
}

void toggleLED2() {
    ledState = 1 - ledState;
    server.send(200, "text/plain", ledState ? "LED turned on" : "LED turned off");

    DataClassify.command = 4;
    DataClassify.data = ledState;
    esp_now_send(broadcastAddress, (uint8_t*)&DataClassify, sizeof(DataClassify));
    unsigned long sendTime = millis();  // Capture the current time
    Serial.println("ESPNOW MESSAGE SUCCESFULLY SENT | ");
    Serial.println(sendTime);  // Print the send time
}

void toggleLED3() {
    ledState = 1 - ledState;  // Toggle the LED state (0 to 1 or 1 to 0)

    digitalWrite(relay_pin, ledState);  // Set the LED state
    server.send(200, "text/plain", ledState ? "LED turned on" : "LED turned off");

    DataClassify.command = 4;
    DataClassify.data = ledState;
    esp_now_send(broadcastAddress, (uint8_t*)&DataClassify, sizeof(DataClassify));
    unsigned long sendTime = millis();  // Capture the current time
    Serial.println("ESPNOW MESSAGE SUCCESFULLY SENT | ");
    Serial.println(sendTime);  // Print the send time
}

void getLEDStatus() {
    server.send(200, "text/plain", (ledState == 1) ? "On" : "Off");
}

// WEB SERVER FUNCTIONS
void handleRoot() {
    String buttonText = acState ? "Turn Off A/C" : "Turn On A/C";
    String html = "<html><body>"
                  "<button onclick=\"window.location.href='/acControl1'\">" + buttonText + "</button>"
                  "<br><br>"
                  "Set Temperature: <input type='number' id='temperatureInput' value='" + String(userTemperature) + "'>"
                  "<br>"
                  "Current Temperature: <span id='currentTemp'>" + String(userTemperature) + "</span>"
                  "<br>"
                  "AC Status: <span id='acStatus'>" + String(acState ? "ON" : "OFF") + "</span>"
                  "<script>"
                  "function setTemperature() {"
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

// 404
void handleNotFound() {
    server.send(404, "text/html", "<html><body><p>404 Error</p></body></html>");
}

// SETUP
void setup() {
    #pragma region // DEFAULT AC PARAMETERS
    ac.next.protocol = decode_type_t::PANASONIC_AC;  // Set the protocol.
    ac.next.model = 2;                               // Some A/Cs have different models. Try just the first.
    ac.next.degrees = 25;                            // 25 Degrees is a good start point.
    ac.next.mode = stdAc::opmode_t::kCool;           // Run in cool mode initially.
    ac.next.celsius = true;                          // Use Celsius for temp units. False = Fahrenheit.
    ac.next.fanspeed = stdAc::fanspeed_t::kMedium;   // Start the fan at medium speed.
    ac.next.swingv = stdAc::swingv_t::kAuto;         // Don't swing the fan up or down.
    ac.next.swingh = stdAc::swingh_t::kAuto;         // Don't swing the fan left or right.
    ac.next.light = false;                           // Turn off any LED/Lights/Display that we can.
    ac.next.beep = false;                            // Turn off any beep from the A/C if we can.
    ac.next.econo = false;                           // Turn off any economy modes if we can.
    ac.next.filter = false;                          // Turn off any Ion/Mold/Health filters if we can.
    ac.next.turbo = false;                           // Don't use any turbo/powerful/etc modes.
    ac.next.quiet = false;                           // Don't use any quiet/silent/etc modes.
    ac.next.clean = false;                           // Turn off any Cleaning options if we can.
    ac.next.clock = false;                           // Don't set any current time if we can avoid it.
    #pragma endregion

    // Setup Serial Monitor
    Serial.begin(115200);
    Serial.print("Walawe wak");


    // Initialize WiFi
    WiFiMulti.addAP("snsv", "nataniel");
    while (WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }

    Serial.print("Masuk wak");
    Serial.print(WiFi.localIP());

    // WEB SERVER SETUP
    server.on("/", handleRoot);
    server.on("/acControl1", ACTurnOn1);
    server.on("/acControl2", ACTurnOn2);
    server.on("/acControl3", ACTurnOn3);
    server.on("/acControl4", ACTurnOn4);
    server.on("/setTemperature", ACSetTemp);
    server.on("/ACUp", ACUp);
    server.on("/ACDown", ACDown);
    server.on("/acStatus", getACStatus);
    server.on("/acTemp", getACTemp);
    server.on("/DHT", DHTSend);

    // LED ROUTE SETUP
    server.on("/turn_on_led1", toggleLED1);
    server.on("/turn_on_led2", toggleLED2);
    server.on("/turn_on_led3", toggleLED3);
    server.on("/ledStatus", getLEDStatus);

    server.onNotFound(handleNotFound);
    server.begin();

    // LED SETUP
    pinMode(relay_pin, OUTPUT);
    digitalWrite(relay_pin, LOW);



    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return; 
    }

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_register_recv_cb(OnDataRecv);

    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);


}

// LOOP
void loop() {
    webSocket.loop();
    server.handleClient();

    // ACDown();
    // delay(100);
}=