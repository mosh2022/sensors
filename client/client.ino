#include "painlessMesh.h"
#include <ESPAsyncWebServer.h> 

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555
#define   SERVER_PORT     80   // Порт веб-сервера

Scheduler userScheduler; // для контроля
painlessMesh  mesh;
AsyncWebServer server(SERVER_PORT); // Инициализация веб-сервера

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

// Обработчик URL-адреса, возвращающий JSON-объект
void handleGetRequest(AsyncWebServerRequest *request) {
  String message = "Hello, world!"; // Текстовое сообщение для включения в JSON
  StaticJsonDocument<256> doc; // Создание JSON-объекта
  doc["message"] = message; // Добавление сообщения в JSON
  String jsonString; // Строка, содержащая JSON
  serializeJson(doc, jsonString); // Конвертация JSON в строку
  request->send(200, "application/json", jsonString); // Отправка JSON
}

void setup() {
  Serial.begin(115200);
  mesh.setDebugMsgTypes( ERROR | STARTUP );
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  // Определение обработчика URL-адреса
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    handleGetRequest(request);
  });

  server.begin(); // Запуск веб-сервера
}

void loop() {
  mesh.update();
}