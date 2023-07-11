#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                
#include "SD_MMC.h"            
#include "soc/soc.h"           
#include "soc/rtc_cntl_reg.h"  
#include "driver/rtc_io.h"
#include <EEPROM.h>           
#include <base64.h>
#include <WiFi.h>                                     
#include <WebServer.h>                                
#include <WebSocketsServer.h>                         
#include <ArduinoJson.h> 
#include <WiFi.h>                                     
#include <WebServer.h>                                
#include <WebSocketsServer.h>                         
#include <ArduinoJson.h>  

#define EEPROM_SIZE 1
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

int pictureNumber = 0;

const char* ssid = "Redmi";
const char* password = "30023002";

WebServer server(80);                                 
WebSocketsServer webSocket = WebSocketsServer(81);    

int interval = 1000;                                  
unsigned long previousMillis = 0;                     

String webpage = "<!DOCTYPE html><html><head><title>Page Title</title></head><body style='background-color: #EEEEEE;'><span style='color: #003366;'><h1>Lets generate a random number</h1><p>The Image is: <span id='image'>-</span></p><p>The Prediction is: <span id='prediction'>-</span></p><p>The IMU Coordinates are : <span id='imu'>-</span></p><p><button type='button' id='BTN_SEND_BACK'>Send info to ESP32</button></p></span></body><script> var Socket; document.getElementById('BTN_SEND_BACK').addEventListener('click', button_send_back); function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) { processCommand(event); }; } function button_send_back() { var msg = {brand: 'Gibson',type: 'Les Paul Studio',year: 2010,color: 'white'};Socket.send(JSON.stringify(msg)); } function processCommand(event) {var obj = JSON.parse(event.data);document.getElementById('image').innerHTML = obj.image; document.getElementById('prediction').innerHTML = obj.prediction;document.getElementById('imu').innerHTML = obj.imu; console.log(obj.image);console.log(obj.prediction); console.log(obj.imu); console.log(window.location.hostname);} window.onload = function(event) {init();}</script></html>";


void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  Serial.begin(115200);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 6;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    return;
  }
  
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  }

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

void loop() {

  unsigned long now = millis();                       
  if ((unsigned long)(now - previousMillis) > interval) {     

    camera_fb_t * fb = NULL;
    fb = esp_camera_fb_get();
    if(!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    String path = "/picture" + String(pictureNumber) +".jpg";
    String txt_path = "/hi" + String(pictureNumber) +".txt";
    Serial.printf("Picture file name: %s\n", path.c_str());
    
    String encoded = base64::encode(fb->buf, fb->len);
    
    String jsonString = "";                           
    StaticJsonDocument<5000> doc;                     
    JsonObject object = doc.to<JsonObject>();         
    object["image"] = encoded.c_str();                    
    object["prediction"] = random(100);
    object["imu"] = random(100);
    serializeJson(doc, jsonString);                                        
    webSocket.broadcastTXT(jsonString);                                        
    esp_camera_fb_return(fb); 
    Serial.println("Going to sleep now");
    previousMillis = now;                          
    pictureNumber+=1;
  }
  server.handleClient();                              
  webSocket.loop();                           
}

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



