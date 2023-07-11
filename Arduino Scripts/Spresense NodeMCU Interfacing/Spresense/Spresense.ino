#include <Camera.h>
#include <SDHCI.h>
#include "BmpImage.h"
#include<SoftwareSerial.h> 
#define SIZE 300
int i=0;
SoftwareSerial mySerial(2,3);
int num=0;

void setup() {
  mySerial.begin(115200);
  Serial.begin(115200);

}

void loop() {
  // Serial.print("0.56\n");
  for(i=0;i<100;i++){
    if(i==100){
      i=0;
    }
    else{
    mySerial.write(i);
    } 
  }
  // mySerial.write(100);
  delay(10);
}