/*  
  Tony Canning 2018/5/6
  MKR1000 client connection to MKR server to change server state variable based on status of sensors connected to MKR1000 client.
  This code sends GET requests from one MKR (client) to another MKR (server) which can perform actions based on the GET request it receives.
  Additional steps are currently required to install WiFi101 library into the Arduino IDE.  See arduino.cc for details.
  Derived from code created by Grant Baker 2016/7/29
*/

#include <SPI.h>
#include <WiFi101.h>


char ssid[] = "YOURSSID";      //  your WiFi network SSID (name)
char pass[] = "YOURPASS";       // your WiFi network key
int keyIndex = 0;               // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

const int buttonPin = 9;     // Normally open button sensor connected to pin 9 based on the arduino "Button" tutorial
const int vibePin =  8;      // the number of the vibration sensor pin on the MKR1000.  I just used this for troubleshooting the sensor status.
const int ledPin =  6;      // the number of the LED pin on the MKR1000.  I just used this for troubleshooting the sensor status.
int buttonState = 1;    // set initial button state to closed
int vibeState = 1;    // set initial button state to closed

// Initialize the Wifi client library.  I couldn't get WiFiSSLClient to work with ISY!
WiFiClient client;

IPAddress ISY(192,168,0,16);  //Your target IP address

void setup() {
  Serial.begin(9600); //Initialize Serial and wait for port to open:
  pinMode(ledPin, OUTPUT);  // Set MK1000 Pin 6 LED to output
  pinMode(buttonPin, INPUT);   // Set MKR1000 button pin to input
  pinMode(vibePin, INPUT);   // Set MKR1000 vibe sensor pin to input

  connectToAP();    // connect the board to router
  printWifiStatus();  // Check WiFi connection
}

void loop() {
  
// check for BUTTON state change

   if (digitalRead(buttonPin) == HIGH && buttonState == 0) {  
     digitalWrite(ledPin, HIGH);  // turn on mrk1000 client led
     Serial.println("MKR server LED on");  // print button status
     buttonState = 1;  // set button state closed
     sendButtonChangeH();  // send http GET request for switch state change
   } 
   if (digitalRead(buttonPin) == LOW && buttonState == 1) {
     digitalWrite(ledPin, LOW);  // turn off mrk1000 client led
     Serial.println("MKR server LED off");  // print button status
     buttonState = 0;  // set button state open
     sendButtonChangeL(); // send http GET request for switch state change
   }

// check for VIBRATION state change
   
   if (digitalRead(vibePin) == HIGH && vibeState == 0) {  
     digitalWrite(ledPin, HIGH);  // turn on mrk1000 client led
     Serial.println("MKR vibe on");  // print vibe sensor status
     vibeState = 1;  // set vibe state
     sendVibeChangeH();  // send http GET request for switch state change
   } 
   if (digitalRead(vibePin) == LOW && vibeState == 1) {
     digitalWrite(ledPin, LOW);  // turn off mrk1000 client led
     Serial.println("MKR vibe off");  // print vibe sensor status
     vibeState = 0;  // set vibe state
     sendVibeChangeL(); // send http GET request for switch state change
   }
}

// Sent http GET request when sensor state changes
void sendButtonChangeL() {

  if (client.connect(ISY, 80)) { //check if connection to MKR server is made
      
      Serial.println("Connected");
      
      //Make a HTTP request to set state variable 1
      client.println(" HTTP/1.1");
      client.println("Host: 192.168.0.16"); // you need to change this to the MKR server's IP Address
      client.print("GET /L-btn");  //This sends GET request. You can also go to your browser at 
                                   //the server's IP address and see available links to perform the same actions as 
                                   //the client MKR buttons
      client.println("Content-Type: text/html");
      client.print("Client Button State = ");
      client.println(buttonState); 

      client.println("Authorization: Basic xxxxx");  //send authorization header (encoded at base64)  
      client.println();
      
      //Print variable change
      Serial.print("Variable Changed to "); 
      Serial.println(buttonState); 
      
      listenToClient();   //Listen for MKR server response to previous request
       }
      else {
    Serial.println("Connection Failed");
  }
}
void sendButtonChangeH() {

  if (client.connect(ISY, 80)) { //check if connection to MKR server is made
      
      Serial.println("Connected");
      
      //Make a HTTP request to set state variable 1
      client.println(" HTTP/1.1");
      client.println("Host: 192.168.0.16");
      client.print("GET /H-btn");  
      client.println("Content-Type: text/html");
      client.print("Client Button State = ");
      client.println(buttonState); 

      client.println("Authorization: Basic xxxxx");  //send authorization header (encoded at base64)  
      client.println();
      
      //Print variable change
      Serial.print("Variable Changed to "); 
      Serial.println(buttonState); 
      
      listenToClient();   //Listen for MKR server response to previous request
       }
      else {
    Serial.println("Connection Failed");
  }
}

void sendVibeChangeH() {

  if (client.connect(ISY, 80)) { //check if connection to MKR server is made
      
      Serial.println("Connected");
      
      //Make a HTTP request to set state variable 1
      client.println(" HTTP/1.1");
      client.println("Host: 192.168.0.16");
      client.print("GET /H-vibe");  
      client.println("Content-Type: text/html");
      client.print("Client Vibration State = ");
      client.println(vibeState);  

      client.println("Authorization: Basic xxxxx");  //send authorization header (encoded at base64)  
      client.println();
//      delay(3000);
      //Print variable change
      Serial.print("Variable Changed to "); 
      Serial.println(vibeState); 
      
      listenToClient();   //Listen for MKR server response to previous request
       }
      else {
    Serial.println("Connection Failed");
  }
}

void sendVibeChangeL() {

  if (client.connect(ISY, 80)) { //check if connection to MKR server is made
      
      Serial.println("Connected");
      
      //Make a HTTP request to set state variable 1
      client.println(" HTTP/1.1");
      client.println("Host: 192.168.0.16");
      client.print("GET /L-vibe");  
      client.println("Content-Type: text/html");
      client.print("Client Vibration State = ");
      client.println(vibeState); 
      
      client.println("Authorization: Basic xxxxx");  //send authorization header (encoded at base64)  
      client.println();
      
      //Print variable change
      Serial.print("Variable Changed to "); 
      Serial.println(vibeState); 
      
      listenToClient();   //Listen for MKR server response to previous request
       }
      else {
    Serial.println("Connection Failed");
  }
}

// listen for and print response from MKR server
void listenToClient() {
  
  unsigned long startTime = millis();
  bool received = false;

  while ((millis() - startTime < 5000) && !received) { // listen for 5 seconds
    while (client.available()) {
      received = true;
      char c = client.read();
      Serial.write(c);
    }
  }
  client.stop();
  Serial.println();
}

// WiFi connection
void connectToAP() {
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 1 second for connection:
    delay(1000);
  }
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
}
