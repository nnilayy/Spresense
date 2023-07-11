#include<SoftwareSerial.h> 
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>
SoftwareSerial mySerial(13,12);
WebServer server(80);

int data;
int i=0;
const char *ssid = "Redmi";
const char *password = "30023002";


void setup(void) {
  // Start Serial and SoftWare Serial
  Serial.begin(115200);
  mySerial.begin(115200);
  
  // Start The Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Server Routes
  server.on("/", handleRoot);
  server.on("/json", handleJSON);
  // server.on("/prediction", predictionPage);
  server.on("/test.svg", drawGraph);
  // server.on("/inline", []() {server.send(200, "text/plain",i);});

  // Start The Server
  server.begin();
}

void loop() {
  data=mySerial.read();
  i=i+1;
  server.handleClient();
  delay(10);
  // Serial.println(mySerial.read());
}
// -----------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// void predictionPage() {
//     char temp[400];
//     // int data;
//     // data=mySerial.read(); 
//     int i=0;

// snprintf(temp, 400,

//            "<html>\
//   <head>\
//     <meta http-equiv='refresh' content='0.000000000001'/>\
//     <title>ESP32 Demo</title>\
//     <style>\
//       body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
//     </style>\
//   </head>\
//   <body>\
//     <h1>This is The Prediction Page</h1>\
//     <p>Prediction=: %d</p>\
//   </body>\
// </html>",
//            data);
//   server.send(200, "text/html", temp);
// }

// -----------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
void handleRoot() {
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 400,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='0.000000000001'/>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP32!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

           hr, min % 60, sec % 60
          );
  server.send(200, "text/html", temp);
}

// -----------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}
// -----------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
void handleJSON(){

  char temp[400];
  snprintf(temp, 400,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='0.000000000001'/>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\    
    <p>Prediction: %d</p>\
    <p>Camera String: %d</p>\
    <p>IMU: %d</p>\
  </body>\
</html>",prediction,camString,coordinates);
  server.send(200, "text/html", temp);
}
