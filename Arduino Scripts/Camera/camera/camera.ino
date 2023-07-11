
#include <SDHCI.h>
#include <stdio.h>  /* for sprintf */
#include <Camera.h>
#define TOTAL_PICTURE_COUNT     (100)

SDClass  theSD;
int take_picture_count = 0;


void printError(enum CamErr err)
{
  Serial.print("Error: ");
  switch (err)
    {
      case CAM_ERR_NO_DEVICE:
        Serial.println("No Device");
        break;
      case CAM_ERR_ILLEGAL_DEVERR:
        Serial.println("Illegal device error");
        break;
      case CAM_ERR_ALREADY_INITIALIZED:
        Serial.println("Already initialized");
        break;
      case CAM_ERR_NOT_INITIALIZED:
        Serial.println("Not initialized");
        break;
      case CAM_ERR_NOT_STILL_INITIALIZED:
        Serial.println("Still picture not initialized");
        break;
      case CAM_ERR_CANT_CREATE_THREAD:
        Serial.println("Failed to create thread");
        break;
      case CAM_ERR_INVALID_PARAM:
        Serial.println("Invalid parameter");
        break;
      case CAM_ERR_NO_MEMORY:
        Serial.println("No memory");
        break;
      case CAM_ERR_USR_INUSED:
        Serial.println("Buffer already in use");
        break;
      case CAM_ERR_NOT_PERMITTED:
        Serial.println("Operation not permitted");
        break;
      default:
        break;
    }
}


void CamCB(CamImage img)
{
  if (img.isAvailable()){;/* If you want RGB565 data, convert image data format to RGB565 */ }
  else{Serial.println("Failed to get video stream image");}
}


void setup()
{
  CamErr err;
  Serial.begin(115200);
  while (!Serial){;}
  while (!theSD.begin()){Serial.println("Insert SD card.");}
  Serial.println("Prepare camera");
  err = theCamera.begin();
  if (err != CAM_ERR_SUCCESS){printError(err);}
  Serial.println("Start streaming");
  err = theCamera.startStreaming(true, CamCB);
  if (err != CAM_ERR_SUCCESS){printError(err);}

  /* Auto white balance configuration */
  // Serial.println("Set Auto white balance parameter");
  // err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  // if (err != CAM_ERR_SUCCESS)
  //   {
  //     printError(err);
  //   }
 
  Serial.println("Set still picture format");
  err = theCamera.setStillPictureImageFormat(224,224,CAM_IMAGE_PIX_FMT_JPG);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }
}

void loop()
{
  // sleep(1); /* wait for one second to take still picture. */
  if (take_picture_count < TOTAL_PICTURE_COUNT)
    {
      theCamera.setColorEffect(CAM_COLOR_FX_BW);
      CamImage image = theCamera.takePicture();
      image.convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY);

      if (image.isAvailable())
        { 
          char filename[16] = {0};
          sprintf(filename, "PICT%03d.JPG", take_picture_count);
          Serial.print("Save taken picture as "+ String(filename) +"\n");
          theSD.remove(filename);
          File myFile = theSD.open(filename, FILE_WRITE);
          myFile.write(image.getImgBuff(), image.getImgSize());
          myFile.close();
        }
      else{Serial.println("Failed to take picture");}
}
  else if (take_picture_count == TOTAL_PICTURE_COUNT){Serial.println("End.");theCamera.end();}
  
  take_picture_count++;
}
