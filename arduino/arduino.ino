#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h> // Library for controlling servo motors
#include <ArduinoJson.h> // Library for handling JSON data

const char* ssid = "tama";
const char* password = "tamatama";

// Create an instance of the server
ESP8266WebServer server(80);

Servo myservo; // Create a servo object

int servoPosition = 0; // Initial position of the servo


int ledState = 0; //led status
void setup() {
  //LED
    pinMode(D1, OUTPUT); // Set D1 (GPIO 5) as an output
    digitalWrite(D1, LOW); // Initially turn off the LED


  //Servo
    myservo.attach(D4); // Attach the servo to GPIO pin D1 (GPIO 5)
    myservo.write(servoPosition); // Set the initial position of the servo

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
    server.on("/rotate_servo", HTTP_GET, rotateServo);
    server.on("/get_servo_position", HTTP_GET, getServoPosition);

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
    html += "<h1>Control Servo</h1>";
    html += "<form method='get' action='/rotate_servo'>";
    html += "<input type='number' name='position' min='0' max='180' value='" + String(servoPosition) + "'>";
    html += "<button>Rotate Servo</button>";
    html += "</form>";
    html += "<p>Servo Position: " + String(servoPosition) + "</p>"; // Display servo position


    html += "<br> <br>";


    html += "<h1>Control LED</h1>";
    html += "<form method='get' action='/turn_on_led'>";
    html += "<button>Toggle LED</button>";
    html += "</form>";
    html += "<p>LED Status: " + String(ledState ? "ON" : "OFF") + "</p>"; 
    html += "</body></html>";

    server.send(200, "text/html", html);
}

// Function to handle requests to rotate the servo
void rotateServo() {
    if (server.hasArg("position")) {
        int newPosition = server.arg("position").toInt();
        if (newPosition >= 0 && newPosition <= 180) {
            servoPosition = newPosition;
            myservo.write(servoPosition);
            server.send(200, "text/plain", "Servo rotated to position " + String(servoPosition));
        } else {
            server.send(400, "text/plain", "Invalid servo position");
        }
    } else {
        server.send(400, "text/plain", "Missing 'position' parameter");
    }
}

// Function to handle requests to toggle the LED
void toggleLED() {
    ledState = 1 - ledState; // Toggle the LED state (0 to 1 or 1 to 0)

    digitalWrite(D1, ledState); // Set the LED state
    server.send(200, "text/plain", ledState ? "LED turned on" : "LED turned off");
}

// Function to handle requests to get the LED status
void getLEDStatus() {
    server.send(200, "text/plain", (ledState == 1) ? "On" : "Off");
}



// Function to handle requests to get the servo position
void getServoPosition() {
    server.send(200, "text/plain", String(servoPosition));
}