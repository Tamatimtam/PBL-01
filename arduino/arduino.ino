
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRutils.h>


#pragma region Hotspot Setup
const char *ssid = "tama";
const char *password = "tamatama";
#pragma endregion



ESP8266WebServer server(80);


#pragma region //Local Variables
bool acState = false; // Variable to store A/C state (off by default)
int userTemperature = 25; // Default temperature setting

const uint16_t kIrLed = D5; //GPIO Pin
IRac ac(kIrLed); // Declare AC object
#pragma endregion

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



void ACTurnOn() {
  // Set temperature based on user input
  ac.next.degrees = userTemperature;

  // Toggle A/C state
  ac.next.power = !acState; //library variable
  acState = !acState;       //Local Variable
  
  // Send Data to AC via IR
    ac.sendAc();
    server.send(200, "text/html", "A/C Control Function Executed");
}




//HTML WEB SERVER
void handleRoot() {
  String buttonText = acState ? "Turn Off A/C" : "Turn On A/C";
  String html = "<html><body>"
                "<button onclick=\"window.location.href='/acControl'\">" + buttonText + "</button>"
                "<br><br>"
                "Set Temperature: <input type='number' id='temperatureInput' value='" + String(userTemperature) + "'>"

              "<script>"
               " function setTemperature() {"
                "var temperatureInput = document.getElementById('temperatureInput').value;"
                "window.location.href = '/setTemperature?temperature=' + temperatureInput;"
                "}"
              "</script>"

                "<button onclick=\"setTemperature()\">Set</button>"
                "<button onclick=\"window.location.href='/setTemperature'\">" + "Ubah Suhu AC" + "</button>"

                "</body></html>";
  server.send(200, "text/html", html);
}





//Wifi and server startup
void setup() {
  Serial.begin(115200);
  delay(200);

  #pragma region // Set up Wi-Fi, and server
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
  server.on("/acControl", HTTP_GET, ACTurnOn);
  server.on("/setTemperature", HTTP_GET, ACSetTemp);

  // Start server
  server.begin();

  Serial.println("Web server started");

  #pragma endregion

}






//just a handle function for server
void loop() {
  server.handleClient();
  // Your other loop logic goes here
}
