#include <painlessMesh.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define MESH_PREFIX "Greenhouse by Niumandzi"
#define MESH_PASSWORD "Password"
#define MESH_PORT 5555

#define DHTTYPE DHT11 
#define DHTPIN 1 
#define SOILPIN A0
#define WINDOWPIN 7

Scheduler userScheduler;
painlessMesh mesh;

struct ReceivedData {
  float airTemperature1;
};
ReceivedData receivedData;

struct Node2DataToSend {
  int soilMoisture;
  float airTemperature;
  float airHumidity;
};
Node2DataToSend data;

struct Node2LogsToSend {
  
};
Node2LogsToSend logs;

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

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());

  DynamicJsonDocument jsonDoc(1024);
  DeserializationError error = deserializeJson(jsonDoc, msg);

  if (!error) {
    if (jsonDoc.containsKey("airTemperature1")) {
      receivedData.airTemperature1 = jsonDoc["airTemperature1"];
    }
  }  
  Serial.print("Node1 air temperature: ");
  Serial.println(receivedData.airTemperature1);
}

void readSoilMoisture() {
  int soilMoistureRaw = analogRead(SOILPIN);
  data.soilMoisture = map(soilMoistureRaw, 0, 680, 0, 100);
  Serial.print(data.soilMoisture);
  Serial.print(", "); 
}

void readAirIndicators() {
  data.airTemperature = dht.readTemperature(); 
  data.airHumidity = dht.readHumidity(); 
  Serial.print(dht.readTemperature()); 
  Serial.print(", "); 
  Serial.println(dht.readHumidity());
}

void writeWindowControl() {

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
  mesh.update();
  readSoilMoisture();
  readAirIndicators();
  sendMessage();

  Serial.println("------------------------------");
  delay(500);
}