#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "tama";
const char* password = "tamatama";

// Create an instance of the server
ESP8266WebServer server(80);

void setup() {
    pinMode(D1, OUTPUT); // Set D1 (GPIO 5) as an output
    digitalWrite(D1, LOW); // Initially turn off the LED

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
    server.on("/turn_on_led", HTTP_GET, turnOnLED);

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
    html += "<h1>Control LED</h1>";
    html += "<form method='get' action='/turn_on_led'>";
    html += "<button>Turn On LED</button>";
    html += "</form>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
}

// Function to handle requests to turn on the LED
void turnOnLED() {
    digitalWrite(D1, HIGH); // Turn on the LED
    server.send(200, "text/plain", "LED turned on");
}
