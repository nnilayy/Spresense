#include <Camera.h>
#include <USBSerial.h>
USBSerial UsbSerial;
#define SERIAL_OBJECT   UsbSerial
#define SERIAL_BAUDRATE 115200



int16_t width = CAM_IMGSIZE_VGA_H,  height = CAM_IMGSIZE_VGA_V;

void CamCB(CamImage img)
{
  static int toggle = 0;
  static uint64_t base_time = 0;

  uint64_t now = millis();
  printf("time= %lld [ms]\n", now - base_time);
  base_time = now;
  
  if (img.isAvailable()) {
    send_jpeg(img.getImgBuff(), img.getImgSize());
    uint64_t tdiff = millis() - now;
    printf("time= %lld [ms] %.3lf [Mbps]\n", tdiff, (float)(img.getImgSize() * 8) / tdiff / 1000);
  }
}

void setup()
{
  CamErr err;
  Serial.begin(115200);
  SERIAL_OBJECT.begin(SERIAL_BAUDRATE);
  Serial.println("Prepare camera");
  err = theCamera.begin(2,
                        CAM_VIDEO_FPS_15,
                        width,
                        height,
                        CAM_IMAGE_PIX_FMT_JPG);
  assert(err == CAM_ERR_SUCCESS);

  err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  assert(err == CAM_ERR_SUCCESS);

  err = theCamera.startStreaming(true, CamCB);
  assert(err == CAM_ERR_SUCCESS);
}

int send_jpeg(const uint8_t* buffer, size_t size)
{
  if (wait_char('>', 3000)) { 
    
    SERIAL_OBJECT.write('P'); 
    SERIAL_OBJECT.write((size >> 24) & 0xFF);
    SERIAL_OBJECT.write((size >> 16) & 0xFF);
    SERIAL_OBJECT.write((size >>  8) & 0xFF);
    SERIAL_OBJECT.write((size >>  0) & 0xFF);



    size_t sent = 0;
    do {
      size_t s = SERIAL_OBJECT.write(&buffer[sent], size - sent);
      if ((int)s < 0) {
        static int recover = 0;
        Serial.println(recover++);
        SERIAL_OBJECT.end();
        SERIAL_OBJECT.begin(SERIAL_BAUDRATE);
        return -1;
      }
      sent += s;
    } while (sent < size);

    if (wait_char('<', 1000)) {
      return 0;
    } else {
      return -1;
    }
  }
  return 0;
}
void loop(){}

int wait_char(char code, int timeout)
{
  uint64_t expire = millis() + timeout;

  while (1) {
    if (SERIAL_OBJECT.available() > 0) {
      uint8_t cmd = SERIAL_OBJECT.read();
      if (cmd == code) {
        return 1;
      }
    }
    if (timeout > 0) {
      if (millis() > expire) {
        return 0;
      }
    }
  }
}
