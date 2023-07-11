#include <WiFi.h>                                     
#include <WebServer.h>                                
#include <WebSocketsServer.h>                         
#include <ArduinoJson.h>                             
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
const char* ssid = "Redmi";
const char* password = "30023002";

WebServer server(80);                                 
WebSocketsServer webSocket = WebSocketsServer(81);    

int interval = 3000;                                  
unsigned long previousMillis = 0;                     
String webpage = "<!DOCTYPE html><html><head><title>Page Title</title></head><body style='background-color: #EEEEEE;'><span style='color: #003366;'><h1>Lets generate a random number</h1><p>The Image is: <span id='image'>-</span></p><p>The Prediction is: <span id='prediction'>-</span></p><p>The IMU Coordinates are : <span id='imu'>-</span></p><p><button type='button' id='BTN_SEND_BACK'>Send info to ESP32</button></p></span></body><script> var Socket; document.getElementById('BTN_SEND_BACK').addEventListener('click', button_send_back); function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) { processCommand(event); }; } function button_send_back() { var msg = {brand: 'Gibson',type: 'Les Paul Studio',year: 2010,color: 'white'};Socket.send(JSON.stringify(msg)); } function processCommand(event) {var obj = JSON.parse(event.data);document.getElementById('image').innerHTML = obj.image; document.getElementById('prediction').innerHTML = obj.prediction;document.getElementById('imu').innerHTML = obj.imu; console.log(obj.image);console.log(obj.prediction); console.log(obj.imu); console.log(window.location.hostname);} window.onload = function(event) {init();}</script></html>";
void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length);
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
void setup() {
  Serial.begin(115200); 
  WiFi.begin(ssid, password);                               
  while (WiFi.status() != WL_CONNECTED) {             // wait until WiFi is connected
    delay(1000);
    Serial.print(".");
  }
  Serial.print("Connected to network with IP address: ");
  Serial.println(WiFi.localIP()); 
  server.on("/", []() {server.send(200, "text/html", webpage);});
  server.begin();                                       
  webSocket.begin();                                 
  webSocket.onEvent(webSocketEvent);                  
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
void loop() {
  server.handleClient();                              
  webSocket.loop();                                   

  unsigned long now = millis();                       
  if ((unsigned long)(now - previousMillis) > interval) { 
    String jsonString = "";                           
    StaticJsonDocument<5000> doc;                     
    JsonObject object = doc.to<JsonObject>();         
    object["image"] = "";
    object["prediction"] = random(100);
    object["imu"] = random(100);

    serializeJson(doc, jsonString);                  
    webSocket.broadcastTXT(jsonString);    //Sending Data to Server               
    // Serial.println(jsonString);                       
    previousMillis = now;                          
  }
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// Handling the Buttong Action
void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) {      
  switch (type) {                                     
    
    case WStype_DISCONNECTED:                         
      Serial.println("Client " + String(num) + " disconnected");
      break;
    
    case WStype_CONNECTED:                            
      Serial.println("Client " + String(num) + " connected");
      break;
    
    case WStype_TEXT:                                 
      StaticJsonDocument<200> doc;                    
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      else {
        const char* g_brand = doc["brand"];
        const char* g_type = doc["type"];
        const int g_year = doc["year"];
        const char* g_color = doc["color"];
        Serial.println("Received guitar info from user: " + String(num));
        Serial.println("Brand: " + String(g_brand));
        Serial.println("Type: " + String(g_type));
        Serial.println("Year: " + String(g_year));
        Serial.println("Color: " + String(g_color));
      }
      Serial.println(" ");
      break;
  }
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
