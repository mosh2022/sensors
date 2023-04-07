#include <painlessMesh.h>
#include <ArduinoJson.h>

#define MESH_PREFIX "Greenhouse by Niumandzi"
#define MESH_PASSWORD "Password"
#define MESH_PORT 5555

#define WATERPIN 12
#define HUMIDIFIERPIN 16
#define SPRINKLERPIN 5

Scheduler userScheduler;
painlessMesh mesh;

struct ReceivedData {
  int soilMoisture1;
  float airHumidity1;
  int soilMoisture2;
  float airHumidity2;  
};
ReceivedData receivedData;

struct Node3LogsToSend {
  
};
Node3LogsToSend logs;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());

  DynamicJsonDocument jsonDoc(1024);
  DeserializationError error = deserializeJson(jsonDoc, msg);

  if (!error) {
    if (jsonDoc.containsKey("soilMoisture1") &&
        jsonDoc.containsKey("airHumidity1")) {
      receivedData.soilMoisture1 = jsonDoc["soilMoisture1"];
      receivedData.airHumidity1 = jsonDoc["airHumidity1"];
    }

    if (jsonDoc.containsKey("soilMoisture2") &&
        jsonDoc.containsKey("airHumidity2")) {
      receivedData.soilMoisture2 = jsonDoc["soilMoisture2"];
      receivedData.airHumidity2 = jsonDoc["airHumidity2"];
    }
  }
  Serial.print("Node1 soil moisture: ");
  Serial.println(receivedData.soilMoisture1);
  Serial.print("Node1 air humidity: ");
  Serial.println(receivedData.airHumidity1);

  Serial.print("Node2 soil moisture: ");
  Serial.println(receivedData.soilMoisture2);
  Serial.print("Node2 air humidity: ");
  Serial.println(receivedData.airHumidity2);
  Serial.println("------------------------------");
}

void readWaterLevel() {
  int waterLevel = digitalRead(WATERPIN);
  Serial.println(waterLevel);
}

void writeHumidifireControl() {

}

void writeSprinklerControl() {
  int averageMoisture = (receivedData.soilMoisture1 + receivedData.soilMoisture2) / 2;
  if (averageMoisture <= 80) {
    digitalWrite(SPRINKLERPIN, 1);
  } else {
    digitalWrite(SPRINKLERPIN, 0);
  }
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {
  pinMode(SPRINKLERPIN, OUTPUT);
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
  readWaterLevel();
  writeSprinklerControl();

  delay(500);
}
