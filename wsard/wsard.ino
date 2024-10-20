#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Replace with your network credentials
char ssid[] = "test";        // Your Wi-Fi SSID
char password[] = "tamatama"; // Your Wi-Fi Password

// MQTT Broker settings
const char* mqtt_server = "broker.emqx.io"; // MQTT broker address
const int mqtt_port = 1883;                    // MQTT broker port

// Initialize Wi-Fi and MQTT Client Objects
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Pin for the built-in LED
const int ledPin = LED_BUILTIN;

// Callback function for MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  
  Serial.print("Message: ");
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Turn on the built-in LED
  digitalWrite(ledPin, LOW); // LOW turns the LED on for most ESP8266 boards
  
  // Optionally turn off the LED after a delay
  delay(1000);
  digitalWrite(ledPin, HIGH); // HIGH turns the LED off
}

// Function to reconnect to the MQTT broker
void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("Connected to MQTT Broker");
      
      // Subscribe to a topic
      mqttClient.subscribe("your/AC/1");
    } else {
      Serial.print("Failed to connect, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}

void setup() {
  // Start the Serial communication
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }

  // Initialize the LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // HIGH turns the LED off

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("#");
  }
  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());

  // Set MQTT server and callback function
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);

  // Seed the random number generator
  randomSeed(micros());
}

void loop() {
  // Reconnect to MQTT if disconnected
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  
  // Check if serial data is available
  if (Serial.available() > 0) {
    String mdata = Serial.readStringUntil('\n'); // Read the serial data
    Serial.println(mdata);

    // Send data through MQTT
    mqttClient.publish("your/AC/1", mdata.c_str());
  }
}