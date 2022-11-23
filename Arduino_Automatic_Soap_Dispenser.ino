//  AutomaticSoapDispenser, Automatic Soap Dispenser made with Arduino MKR1000
//  Copyright (C) 2022 BurgersMcFly and contributors

//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <Servo.h>  // SG90 servomotor driving program
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <RTCZero.h>

RTCZero rtc;

char ssid[] = "arduino";             // network SSID (name) between the " "
char pass[] = "arduino1234";      // network password between the " "
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int flag = 0;
int status = WL_IDLE_STATUS;      //connection status
const int GMT = 3; //change this to adapt it to your time zone
WiFiServer server(80);            //server socket

WiFiClient client = server.available();

#define echoPin 7   // U/S sensor Echo Pin
#define trigPin 8   // U/S sensor Trigger Pin

Servo servo1; // Dosage motor
Servo servo2; // Counting motor
int angle = 145;          // servo test
int maximumRange = 200;   // Maximum range needed U/S sensor
int minimumRange = 0;     // Minimum range needed U/S sensor
long duration, distance;  // Duration used to calculate distance -  U/S sensor

void setup() {

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  servo1.attach(9);
  servo1.write(angle);
  Serial.begin(9600);
  delay(2000);

  servo2.attach(10);
  servo2.write(angle);
  Serial.begin(9600);
  delay(2000);
  // while (!Serial);

  enable_WiFi();
  connect_WiFi();

  server.begin();
  printWifiStatus();

  rtc.begin();

  unsigned long epoch;

  int numberOfTries = 0, maxTries = 6;

  do {

    epoch = WiFi.getTime();

    numberOfTries++;

  }

  while ((epoch == 0) && (numberOfTries < maxTries));

  if (numberOfTries == maxTries) {

    Serial.print("NTP unreachable!!");

    while (1);

  }

  else {

    Serial.print("Epoch received: ");

    Serial.println(epoch);

    rtc.setEpoch(epoch);

    Serial.println();

  }


}


void loop() {


  client = server.available();

  if (client) {
    printWEB();
  }

}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void enable_WiFi() {

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printWEB() {

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

            // Soap Dispenser Code Begins
            
            angle = 150;                // servo up angle
            //Serial.println(angle);
            servo1.write(angle);         // servo/lever up
            delay(2000);

            /* The following trigPin/echoPin cycle is used to determine the
              distance of the nearest object by bouncing soundwaves off of it. */
            digitalWrite(trigPin, LOW);
            delayMicroseconds(2);

            digitalWrite(trigPin, HIGH);
            delayMicroseconds(10);

            digitalWrite(trigPin, LOW);
            duration = pulseIn(echoPin, HIGH);
            //delay(5000);
            //Calculate the distance (in cm) based on the speed of sound in air
            distance = duration / 58.2;

            if (distance >= maximumRange || distance <= minimumRange) {
              /* Send a negative number to computer
                to indicate "out of range" */
              Serial.println("-1");
            }
            else {
              /* Send the distance to the computer using Serial protocol
                to indicate successful reading. */
              //    Serial.println(distance);

              /* when hand is at distance less than 7 cm from the U/S sensor
                 the servomotor will drive 10 pump soap doses. After this sequence
                 the second motor will start working for 20 seconds.
              */
              if (distance < 7) {       // condition trigger for soap dispensing

                client.print("Soap Dispenser is occupied"); // Message to client that Soap Dispenser is used!

                client.print("<br>");

                // Soap Dispenser Last Use
                
                client.print("Soap Dispenser Last Use: ");
                client.print(rtc.getDay());
                client.print("/");
                client.print(rtc.getMonth());
                client.print("/");
                client.print(rtc.getYear());

                client.print("  ");

                client.print(rtc.getHours() + GMT);
                client.print(":");
                client.print(rtc.getMinutes());
                client.print(":");
                client.print(rtc.getSeconds());
                client.print("<br>");


                for (int i = 1; i < 11; i++) { // initiate soap dispensing for 10 doses
                  angle = 16;             // servo down angle
                  //  Serial.println(angle);
                  servo1.write(angle);     // servo/lever down
                  delay(1000);

                  angle = 150;
                  //  Serial.println(angle);
                  servo1.write(angle);     // servo/lever back to up position
                  delay(700);

                  flag = 1;

                }                         //end of soap dispensing


                for (int i = 1; i < 21; i++) { // Start second motor counting for 20 seconds

                  angle = 16;             // servo down angle
                  //  Serial.println(angle);
                  servo2.write(angle);     // servo/lever down
                  delay(1000);

                  angle = 150;
                  //  Serial.println(angle);
                  servo2.write(angle);     // servo/lever back to up position
                  delay(700);

                  client.print(20 - i); // Remaining time of handwashing
                  client.print("<br>");

                }
              }
            }
            client.print("Soap Dispenser is not used");  // Message to client that Soap Dispenser is not used!
            delay(5000); // Sensor reading every 5 seconds



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

      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
void printTime()
{

  print2digits(rtc.getHours() + GMT);

  Serial.print(":");

  print2digits(rtc.getMinutes());

  Serial.print(":");

  print2digits(rtc.getSeconds());

  Serial.println();
}

void printDate()
{

  Serial.print(rtc.getDay());

  Serial.print("/");

  Serial.print(rtc.getMonth());

  Serial.print("/");

  Serial.print(rtc.getYear());

  Serial.print(" ");
}

void print2digits(int number) {

  if (number < 10) {

    Serial.print("0");

  }

  Serial.print(number);
}
