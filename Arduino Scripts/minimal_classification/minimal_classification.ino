#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "BmpImage.h"
#include <Camera.h>
#include "model.h"
#define IMG_SIZE 128

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
constexpr int kTensorArenaSize = 900000;
uint8_t tensor_arena[kTensorArenaSize];


// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CamCB(CamImage img){
static uint32_t count=0;
  img.convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY);
  uint8_t* image = img.getImgBuff();
  for (int i = 0; i <img.getWidth()*img.getHeight(); ++i) {
    input->data.f[i] = (float)(image[i])/255.0;
  }

  interpreter->Invoke();

  Serial.println("--------------------------------------------------------");
  Serial.println("("+String(count)+")");
  Serial.println("(1)" + String(output->data.f[0]));
  Serial.println("(2)" + String(output->data.f[1]));
  Serial.println("(3)" + String(output->data.f[2]));
  Serial.println("(4)" + String(output->data.f[3]));
  Serial.println("(5)" + String(output->data.f[4]));
  Serial.println("(6)" + String(output->data.f[5]));
  Serial.println("(7)" + String(output->data.f[6]));
  Serial.println("(8)" + String(output->data.f[7]));
  Serial.println("(9)" + String(output->data.f[8]));
  Serial.println("(10)" + String(output->data.f[9]));
  Serial.println("--------------------------------------------------------");
  count++;
}

void setup() {
  Serial.begin(115200);
  tflite::InitializeTarget();
  memset(tensor_arena, 0, kTensorArenaSize*sizeof(uint8_t));
  
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter= &micro_error_reporter;
  
  model = tflite::GetModel(model_tflite);
  static tflite::AllOpsResolver resolver;
  static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;
  interpreter->AllocateTensors();

  size_t used_size= interpreter->arena_used_bytes();
  Serial.println("Area used bytes: " + String(used_size));
  input = interpreter->input(0);
  output = interpreter->output(0);

  theCamera.begin(1,CAM_VIDEO_FPS_30, IMG_SIZE, IMG_SIZE);
  theCamera.startStreaming(true, CamCB);
  Serial.println("Ready to capture");
}

void loop() {
}
