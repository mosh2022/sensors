#include <painlessMesh.h>
#include <ArduinoJson.h>

#define MESH_PREFIX "Greenhouse by Niumandzi"
#define MESH_PASSWORD "Password"
#define MESH_PORT 5555

#define WATERPIN 0
#define HUMIDIFIERPIN 1
#define SPRINKLERPIN 3

struct RecivedData {
  int soilMoisture1;
  float airHumidity1;
  int soilMoisture2;
  float airHumidity2;  
};

struct Node3LogsToSend {
  
};

RecivedData recivedData;

Scheduler userScheduler;
painlessMesh mesh;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());

  DynamicJsonDocument jsonDoc(1024);
  DeserializationError error = deserializeJson(jsonDoc, msg);

  if (!error) {
    if (jsonDoc.containsKey("soilMoisture1") &&
        jsonDoc.containsKey("airHumidity1")) {
      recivedData.soilMoisture1 = jsonDoc["soilMoisture1"];
      recivedData.airHumidity1 = jsonDoc["airHumidity1"];
      Serial.println("Received data from Node1");
    }

    if (jsonDoc.containsKey("soilMoisture2") &&
        jsonDoc.containsKey("airHumidity2")) {
      recivedData.soilMoisture2 = jsonDoc["soilMoisture2"];
      recivedData.airHumidity2 = jsonDoc["airHumidity2"];
      Serial.println("Received data from Node2");
    }
  }
}

void humidifire() {

}

void sprinkler() {
  
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

  int waterLevel = digitalRead(WATERPIN);
  Serial.println(waterLevel);

  Serial.print("Node1 soil moisture: ");
  Serial.println(recivedData.soilMoisture1);
  Serial.print("Node1 air humidity: ");
  Serial.println(recivedData.airHumidity1);

  Serial.print("Node2 soil moisture: ");
  Serial.println(recivedData.soilMoisture2);
  Serial.print("Node2 air humidity: ");
  Serial.println(recivedData.airHumidity2);
  Serial.println("------------------------------");

  delay(1000);
}
