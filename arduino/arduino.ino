#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRutils.h>

const char *ssid = "tama";
const char *password = "tamatama";

const uint16_t kIrLed = D5;
IRac ac(kIrLed);

ESP8266WebServer server(80);

bool acState = false; // Variable to store A/C state (off by default)
int userTemperature = 25; // Default temperature setting

void acControlFunction() {
  // Your A/C control logic goes here
  // This is the logic you want to execute when the button is pressed

  // For example, let's use the LG protocol
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
  // Set other A/C parameters as needed

  // Set temperature based on user input
  ac.next.degrees = userTemperature;

  // Toggle A/C state
  ac.next.power = !acState;
  ac.sendAc();

  // Update A/C state variable
  acState = !acState;

  // Add any additional delay or logic as needed
}

void handleRoot() {
  String buttonText = acState ? "Turn Off A/C" : "Turn On A/C";
  String html = "<html><body>"
                "<button onclick=\"window.location.href='/acControl'\">" + buttonText + "</button>"
                "<br><br>"
                "Set Temperature: <input type='number' id='temperatureInput' value='" + String(userTemperature) + "'>"
                "<button onclick=\"setTemperature()\">Set</button>"
                "<button onclick=\"window.location.href='/setTemperature'\">" + Ubah Suhu AC + "</button>"

                "</body></html>";
  server.send(200, "text/html", html);
}

void handleACControl() {
  acControlFunction(); // Call your A/C control function
  server.send(200, "text/html", "A/C Control Function Executed");
}

void handleSetTemperature() {
  if (server.hasArg("temperature")) {
    userTemperature = server.arg("temperature").toInt();
    ac.next.degrees = userTemperature;

  // Toggle A/C state
  ac.next.power = !acState;
  ac.sendAc();

  }
  server.send(200, "text/html", "Temperature Set to " + String(userTemperature) + " degrees");
}

void setup() {
  Serial.begin(115200);
  delay(200);

  // Set up Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print the IP address when connected
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/acControl", HTTP_GET, handleACControl);
  server.on("/setTemperature", HTTP_GET, handleSetTemperature);

  // Start server
  server.begin();

  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
  // Your other loop logic goes here
}
