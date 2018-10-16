#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


const char* ssid = "BYU-WiFi";     // SSID of the test Network in my Office
const char* password = "";              // PSK of the test network in my office
const char* server = "10.37.212.212";   // Raspberry Pi running mosquitto mqtt

String macAddr = WiFi.macAddress();      // Store arduino MAC address as a string
String host = "arduino-" + macAddr.substring(15) ;  // Set a client ID for this device (should not match other MQTT devices)


String message;                         // Used for formatting message
long timer;                             // Used to track how often to send messages

WiFiClient wifiClient;                  // Instantiate wifi
PubSubClient mqttClient(wifiClient);    // Instantiate mqtt client

void setup() {
  Serial.begin(9600);                   // Start serial connection for debugging
  Serial.println("\nMQTT Example");     // Serial Intro
  Serial.print("Connecting to '");      // Connect debug info
  Serial.print(ssid);
  Serial.println("' network");

  WiFi.hostname(host);                  // Set the client hostname for wifi
  WiFi.begin(ssid, password);           // Connect to Wifi

  while (WiFi.status() != WL_CONNECTED) { //Wait for connection
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");     // Wifi connection debug
  delay (1000);                         // Let everything settle

  mqttClient.setServer(server, 1883);   // Set up MQTT Connection Info
  mqttClient.setCallback(callback);     // Set callback action for receiving messages

  Serial.println("Connecting to MQTT Broker"); // Serial debug
  char hostChar[host.length()];
  host.toCharArray(hostChar, host.length());
  if (mqttClient.connect(hostChar)) {
    Serial.println("MQTT Connected");
    Serial.println(mqttClient.state());

  } else {
    Serial.println("MQTT Connection Failure");
    Serial.println(mqttClient.state());
  }
  mqttClient.subscribe("test");
  mqttClient.subscribe("clients");

}

void loop() {
  mqttClient.loop();                    // Loop to check for new messages

  if ((millis() - timer) > 5000) {      // If it has been more than 5 seconds since posting an update
    message = "Host: " + host + ",    MAC Address: " + macAddr + ",    Current RSSI: " + String(WiFi.RSSI()); // Capture the MAC address and RSSI
    char messageChar[message.length() + 1];                                                           // Create a char
    message.toCharArray(messageChar, message.length() + 1);                                           // Convert message to char
    mqttClient.publish("clients", messageChar);                                                       // Publish the message to the MQTT bus
    timer = millis();                                                                                 // update the timer ot the current time
  } 
}

void callback(char* topic, byte* payload, unsigned int length) {                                      // Callback function to read messages for subscribed topics
  Serial.print("Message arrived [");    // Serial Debug
  Serial.print(topic);                  //    Print the topic name [in brackets]
  Serial.print("] ");                   //
  for (int i = 0; i < length; i++) {    // Iterate through the characters in the payload array
    Serial.print((char)payload[i]);     //    and display each character
  }
  Serial.println();                     // add a blank line
}
