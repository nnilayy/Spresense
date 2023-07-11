#include<SoftwareSerial.h> 
SoftwareSerial mySerial(13,12);
void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
}

void loop() {
Serial.println(mySerial.read());
delay(10);
}

