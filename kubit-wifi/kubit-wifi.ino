#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include<SoftwareSerial.h>

const char *ssid = "Vinna";
const char *password = "senhapraque";
const int ledPort = 2;
char *stateMessage = "IDLE";

#define rxPin 3
#define txPin 1
#define dataRate 9600

using namespace websockets;

WebsocketsClient client;
SoftwareSerial nmcuSerial(rxPin, txPin);

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    nmcuSerial.write(message.c_str());
}

void setup() {
  pinMode(ledPort, OUTPUT);
  
  Serial.begin(dataRate);
  nmcuSerial.begin(dataRate);
  
  Serial.println();
  Serial.print("Configurando Rede WiFi");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Não foi possível se conectar!");
      return;
  }
  
  Serial.println("");
  Serial.println("Você está conectado!");
  Serial.println("Conectando o servidor do Kubit...");
  
  bool connected = client.connect("wss://kubit-websockets.herokuapp.com");

  if(connected) {
    Serial.println("Conectado!");
    client.send("O Kubit está online");
  } 
  else {
    Serial.println("Não foi possível se conectar ao servidor WebSocket!");
  }
  
  client.onMessage(onMessageCallback);
}

void loop() {  
  if (client.available()) {
    client.poll();
  }
}
