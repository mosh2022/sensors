#include <painlessMesh.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define MESH_PREFIX "Greenhouse by Niumandzi"
#define MESH_PASSWORD "Password"
#define MESH_PORT 5555

#define DHTTYPE DHT11 
#define DHTPIN 16 
#define SOILPIN A0

Scheduler userScheduler;
painlessMesh mesh;

struct Node1DataToSend {
  int soilMoisture;
  float airTemperature;
  float airHumidity;
};
Node1DataToSend data;

DHT dht(DHTPIN, DHTTYPE);

void sendMessage() {
  DynamicJsonDocument node1(1024);
  node1["soilMoisture1"] = data.soilMoisture;
  node1["airTemperature1"] = data.airTemperature;
  node1["airHumidity1"] = data.airHumidity;

  String json;
  serializeJson(node1, json);
  mesh.sendBroadcast(json);

  Serial.println("Sent message");
}


void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
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
  pinMode(DHTPIN, INPUT);
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
