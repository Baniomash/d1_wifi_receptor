#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const uint16_t irPin = 4;

IRsend irsend(irPin);

// WiFi configs
const char* ssid = "NetId";
const char* password = "NetPass";

// MQTT Config
const char *mqtt_broker = "MQTTIP";
const char *topic = "MPU/Keyboard";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

// Identifying as a client
WiFiClient espClient;
PubSubClient client(espClient);

unsigned short int commandTree[21][3] = {{10, 0x0, 0x21},
                                         {11, 0x0, 0x22},
                                         {12, 0x0, 0x23},
                                         {13, 0x0, 0x24},
                                         {14, 0x0, 0x25},
                                         {15, 0x0, 0x26},
                                         {16, 0x0, 0x27},
                                         {17, 0x0, 0x28},
                                         {18, 0x0, 0x29},
                                         {19, 0x0, 0x30},
                                         {20, 0x0, 0x31},
                                         {21, 0x0, 0x32},
                                         {22, 0x0, 0x33},
                                         {23, 0x0, 0x34},
                                         {24, 0x0, 0x35},
                                         {25, 0x0, 0x36},
                                         {26, 0x0, 0x37},
                                         {27, 0x0, 0x38},
                                         {28, 0x0, 0x39},
                                         {29, 0x0, 0x40},
                                         {30, 0x0, 0x41}};

void reconnectWiFi(){
  // Starting the conection
  WiFi.begin(ssid, password);
  // Waiting conection been established
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void setupWifi(){
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // Seting the ESP8266 as a client not an AP 
  WiFi.mode(WIFI_STA);
  reconnectWiFi();
  // Visual confirmation
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
  
}

void reconnectMQTT(){
  while(!client.connected()){
    if(client.connect("ESP8266-Receptor", mqtt_username, mqtt_password)){
      Serial.println("Conected on Broker!!");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

unsigned short int convertASCINum(uint8_t utf8){
  switch(utf8){
    case 48:
      return 0;
    case 49:
      return 1;
    case 50:
      return 2;
    case 51:
      return 3;
    case 52:
      return 4;
    case 53:
      return 5;
    case 54:
      return 6;
    case 55:
      return 7;
    case 56:
      return 8;
    case 57:
      return 9;
    default:
      return 0;
  }
}

void callback(char *topic, uint8_t *payload, unsigned int length) {
  // Callback for listen what is been published
  Serial.println("");
    unsigned short int command = (convertASCINum(payload[0]) * 10) + convertASCINum(payload[1]);
  
    for (int row = 0; row < 21; row++) {
      if(command == commandTree[row][0]){
        Serial.println("Comando enviado!");
        irsend.sendNEC(irsend.encodeNEC(commandTree[row][1], commandTree[row][2]));
        break;
      }
    }
}

void setupMQTT(){
  // Seting things here
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  Serial.println("Conecting to Broker...");
  
  // Enlace until conects to the broker
  reconnectMQTT();
}

void setup() {
  irsend.begin();
  Serial.begin(9600);
  setupWifi();
  setupMQTT();
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    reconnectWiFi();
  }
  if(!client.connected()){
    reconnectMQTT();
  }
  client.subscribe(topic);  
  client.loop();
}