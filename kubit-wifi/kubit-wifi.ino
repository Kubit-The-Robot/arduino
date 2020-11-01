#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include<SoftwareSerial.h>

const char *ssid = "Vinna";
const char *password = "senhapraque";
const int ledPort = 2;

using namespace websockets;

WebsocketsClient client;
SoftwareSerial nmcuSerial(3,1);

void setup() {
  pinMode(ledPort, OUTPUT);
  Serial.begin(9600);
  nmcuSerial.begin(9600);
  
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
  
  client.onMessage([&](WebsocketsMessage message) {
      Serial.print("Mensagem Recebida: ");
      Serial.println(message.data());
      nmcuSerial.write(message.data());
  });
}

void loop() {
  if (client.available()) {
    client.poll();
  }
}
