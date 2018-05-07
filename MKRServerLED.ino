#include <WiFi101.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiSSLClient.h>
#include <WiFiUdp.h>
/* Tony Canning 2018/5/6
 * Derivative from Dr. Charif Mahmoudi's work here:
 * https://www.hackster.io/charifmahmoudi/arduino-mkr1000-getting-started-08bb4a
 * Where he notes:
 * This example is modified from the original file 
 * https://github.com/arduino-libraries/WiFi101/blob/master/examples/SimpleWebServerWiFi/SimpleWebServerWiFi.ino
 */
#include <SPI.h>
#include <WiFi101.h>
 
char ssid[] = "YOURSSID";       //  your network SSID (name)
char pass[] = "YOURPASSWORD";   // your network password
int keyIndex = 0;               // your network key Index number (needed only for WEP)
int ledpin = 6;           //The built in LED pin on the MKR1000
int vibepin = 5;          //A pin to which an LED will be connected for  
int brightness = 0;       // how bright the LED is
int fadeAmount = 51;      // how many points to fade the LED by
bool val = true;
 
int status = WL_IDLE_STATUS;
WiFiServer server(80);
 
void setup() {
  Serial.begin(9600);      // initialize serial communication
  Serial.print("Start Serial ");
  pinMode(ledpin, OUTPUT);      // set the LED pin mode
  pinMode(vibepin, OUTPUT);      // set the LED pin mode

  // Check for the presence of the shield
  Serial.print("WiFi101 shield: ");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("NOT PRESENT");
    return; // don't continue
  }
  Serial.println("DETECTED");
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    digitalWrite(ledpin, LOW);
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);
    digitalWrite(ledpin, HIGH);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
  digitalWrite(ledpin, HIGH);

}
void loop() {
  WiFiClient client = server.available();   // listen for incoming clients
 
  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
 
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
 
            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H-btn\">here</a> turn the built in LED on<br>");
            client.print("Click <a href=\"/L-btn\">here</a> turn the built in LED off<br>");
            client.print("Repeatedly click <a href=\"/H-vibe\">here</a>  to step through fading of pin 5 LED<br>");
            client.print("Click <a href=\"/L-vibe-reset\">here</a> turn the LED on pin 5 off<br>"); 
/*          The above provides a web page hosted by the MKR1000
            You can connect to it by pointing a web browser to the IP address
            which will be displayed in serial monitor.
            If you add more sensors to the client MKR you can add more
            browser controls here - Strictly, none of these are necessary 
            for the MKRs to work with each other. VERY handy for troubleshooting.
 */
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
 
        // Check to see what the client request was:

        if (currentLine.endsWith("GET /H-btn")) {
          digitalWrite(ledpin, HIGH);               // GET /H-btn turns the built in LED on
        }
        if (currentLine.endsWith("GET /L-btn")) {
          digitalWrite(ledpin, LOW);                // GET /L-btn turns the built in LED off
        }
        if (currentLine.endsWith("GET /H-vibe")) {
          pulse();                                  // GET /H-vibe sends new brightness level to pin 5 LED (see void pulse() below)
        }
        if (currentLine.endsWith("GET /L-vibe")) {
          brightness = 0;
          analogWrite(vibepin, brightness);               // GET /L-vibe basically does nothing
                                                          // it just writes a static brightness to pin 5 LED
        }
        if (currentLine.endsWith("GET /L-vibe-reset")) {
          brightness = 0;
          analogWrite(vibepin, brightness);               // GET /L-vibe resets the pin 5 LED
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}


void pulse() {
          analogWrite(vibepin, brightness);
          brightness = brightness + fadeAmount;

          if (brightness <= 0 || brightness >= 255) {
           fadeAmount = -fadeAmount;
           }
/*          This is using the example "Fade" code to send 
 *          a new value for brightness to pin 5.
 */
           
}
           
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
 
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
 
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
