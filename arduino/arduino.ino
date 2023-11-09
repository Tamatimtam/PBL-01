#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

        const char* ssid = "snsv";
const char* password = "nataniel";
const int relay_pin = D2;

// Create an instance of the server
ESP8266WebServer server(80);




int ledState = 0; //led status
void setup() {
  //LED
    pinMode(relay_pin, OUTPUT);
    digitalWrite(relay_pin, LOW); // Initially turn off the LED


    Serial.begin(115200);
    delay(10);

  // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP()); // Print the assigned IP address

  // Define the routes and corresponding functions
    server.on("/", HTTP_GET, handleRoot);
    server.on("/turn_on_led", HTTP_GET, toggleLED);
    server.on("/get_led_status", HTTP_GET, getLEDStatus); // Add this route
    

    // Start the server
    server.begin();
}

void loop() {
    // Handle client requests
    server.handleClient();
}

// Function to handle requests for the root URL
void handleRoot() {
    String html = "<html><body>";
    


    html += "<br> <br>";


    html += "<h1>Control LED</h1>";
    html += "<form method='get' action='/turn_on_led'>";
    html += "<button>Toggle LED</button>";
    html += "</form>";
    html += "<p>LED Status: " + String(ledState ? "ON" : "OFF") + "</p>"; 
    html += "</body></html>";

    server.send(200, "text/html", html);
}


// Function to handle requests to toggle the LED
void toggleLED() {
    ledState = 1 - ledState; // Toggle the LED state (0 to 1 or 1 to 0)

    digitalWrite(relay_pin, ledState); // Set the LED state
    server.send(200, "text/plain", ledState ? "LED turned on" : "LED turned off");
}

// Function to handle requests to get the LED status
void getLEDStatus() {
    server.send(200, "text/plain", (ledState == 1) ? "On" : "Off");
}