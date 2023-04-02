#include <painlessMesh.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define MESH_PREFIX "Greenhouse by Niumandzi"
#define MESH_PASSWORD "Password"
#define MESH_PORT 5555

#define DHTPIN 3 
#define DHTTYPE DHT11 

#define SOILPIN A0

Scheduler userScheduler;
painlessMesh mesh;

struct Node2DataToSend {
  int soilMoisture;
  float airTemperature;
  float airHumidity;
};

Node2DataToSend data;
DHT dht(DHTPIN, DHTTYPE);

void sendMessage() {
  DynamicJsonDocument node2(1024);
  node2["soilMoisture2"] = data.soilMoisture;
  node2["airTemperature2"] = data.airTemperature;
  node2["airHumidity2"] = data.airHumidity;

  String json;
  serializeJson(node2, json);
  mesh.sendBroadcast(json);

  Serial.println("Sent message");
}

void readSoilMoisture() {
  data.soilMoisture = analogRead(SOILPIN);
  Serial.println(analogRead(SOILPIN));
}

void readAirIndicators() {
  data.airTemperature = dht.readTemperature(); 
  data.airHumidity = dht.readHumidity(); 
  Serial.println(dht.readTemperature(), dht.readHumidity());
}

void readTensorIndicator() {
  int tensor = analogRead(A0);
  Serial.println(tensor);
}

void writeWindowControl() {

}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | STARTUP);

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void loop() {
  readSoilMoisture();
  readAirIndicators();
  sendMessage();
  mesh.update();
  delay(1000);
}