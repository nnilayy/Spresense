#include <Camera.h>
#include <SDHCI.h>
#include "BmpImage.h"

SDClass SD;
BmpImage bmp;

void CamCB(CamImage img) {

  static int photo_count = 0;

  if (img.isAvailable()) {
    img.convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY);
    bmp.begin(BmpImage::BMP_IMAGE_GRAY8, 480, 480, img.getImgBuff());

    
    char filename[16] = {};
    sprintf(filename, "gray0%03d.bmp", photo_count);
    if (SD.exists(filename)) SD.remove(filename);
    File myFile = SD.open(filename, FILE_WRITE);
    myFile.write(bmp.getBmpBuff(), bmp.getBmpSize());
    myFile.close();
    bmp.end();
    Serial.println("Saved as " + String(filename));
    ++photo_count;
  }
}

void setup() {
  Serial.begin(115200);
  while (!SD.begin()) { Serial.println("Insert SD Card"); }
  theCamera.begin(1,CAM_VIDEO_FPS_30,224,224);
  // theCamera.setColorEffect(CAM_COLOR_FX_BW)
  // theCamera.setStillPictureImageFormat(224,224,CAM_IMAGE_PIX_FMT_JPG);
  theCamera.startStreaming(true, CamCB);
  Serial.println("Ready to capture");
}

void loop() {
}
