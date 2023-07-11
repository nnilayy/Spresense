#include <Arduino.h>
#include <SDHCI.h>
#include <File.h>

SDClass SD;
File myFile;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  int i =0;
  while (!SD.begin()) { 
  Serial.println("Insert SD card."+String(i++));
  }

  SD.mkdir("data/");
  myFile = SD.open("data/test.txt", FILE_WRITE);

  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening test.txt");
  }

/* Read from the file until there's nothing else in it */
  myFile = SD.open("data/test.txt");
  if (myFile) {
    Serial.println("test.txt:");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("error opening test.txt");
  }
  
}

void loop() {

}