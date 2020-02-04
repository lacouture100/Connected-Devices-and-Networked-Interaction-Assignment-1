
#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"
// your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the Wifi radio's status

WiFiClient client;
char server[] = "10.23.11.80";

const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;

//Buttons
int pinUp = 2;
int buttonStateUp = HIGH;
int pinDown = 4;
int buttonStateDown = HIGH;
int pinLeft = 5;
int buttonStateLeft = HIGH;
int pinRight = 3;
int buttonStateRight = HIGH;
int pinConnectButton = 6;
bool connect = false;

int pinLED = 7;
int pinLEDPower = 8;

char message = 0;
int blinkDelay = 500;

const int sendInterval = 100;
const int debounceInterval = 15;
int prevButtonState = 0;
long lastTimeSent = 0;


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  Serial.print("Hello");

  pinMode(pinLED, OUTPUT);
  pinMode(pinLEDPower, OUTPUT);
  pinMode(pinConnectButton, INPUT);

  pinMode(pinUp, INPUT_PULLUP);
  pinMode(pinDown, INPUT_PULLUP);
  pinMode(pinLeft, INPUT_PULLUP);
  pinMode(pinRight, INPUT_PULLUP);

  int wifiButton = digitalRead(pinConnectButton);

  digitalWrite(pinLED, HIGH);
    

  while (status != WL_CONNECTED) {
    connect = false;
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // wait 1 seconds for connection:
    digitalWrite(pinLED, LOW);
    delay(500);
    digitalWrite(pinLED, HIGH);
    delay(500);
  }
  Serial.println("You're connected to the network!");
  connect = true;
  printWifiStatus();
  printWifiData();
}


void loop() {
  char message = 0;
  long now = millis();
  boolean buttonPushed = buttonRead(pinConnectButton);
  if (buttonPushed) {
    if (client.connected()) {
      Serial.println("disconnecting");
      client.print("x");
      client.stop();
    }
    else {
      Serial.println("connecting");
      client.connect(server, 8080);
    }
  }

  buttonStateUp = digitalRead(pinUp);
  buttonStateDown = digitalRead(pinDown);
  buttonStateLeft = digitalRead(pinLeft);
  buttonStateRight = digitalRead(pinRight);

  switch (buttonStateUp) {
    case LOW:
      message = 'u';
      break;
  }
  switch (buttonStateDown) {
    case LOW:
      message = 'd';
      break;
  }
  switch (buttonStateLeft) {
    case LOW:
      message = 'l';
      break;
  }
  switch (buttonStateRight) {
    case LOW:
      message = 'r';
      break;
  }

  //LED CONNECTION STATE
  if (client.connected()
      && now - lastTimeSent > sendInterval
      && message != 0) {
    
    client.print(message);
    Serial.print(message);
    lastTimeSent = now;
  }

  digitalWrite(pinLED, client.connected());
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
}


boolean buttonRead(int thisButton) {
  boolean result = false;
  int currentState = digitalRead (thisButton);
  if (currentState != prevButtonState
      && currentState == LOW) {
    result = true;
  }
  delay(debounceInterval);
  prevButtonState = currentState;
  return result;
}


void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("Local device IP Address: ");
  Serial.println(ip);
  Serial.print("Target device IP Address: ");
  Serial.println(server);


  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("Local device MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("Router BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("Local IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
