// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// Importing the libraries
// #include <TensorFlowLite.h> /* comment out for SDK2.5.0 */
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "model.h"
#include <Flash.h>
#include <BmpImage.h>
#define TEST_FILE "0009.bmp"


tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
constexpr int kTensorArenaSize = 900000;
uint8_t tensor_arena[kTensorArenaSize];
int inference_count = 0;
BmpImage bmp;

void setup() {
  Serial.begin(115200);
  tflite::InitializeTarget();
  memset(tensor_arena, 0, kTensorArenaSize*sizeof(uint8_t));

  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;


  model = tflite::GetModel(model_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION) {Serial.println("Model provided is schema version " + String(model->version()) + " not equal "+ "to supported version "+ String(TFLITE_SCHEMA_VERSION));return;} 
  else {Serial.println("Model version: " + String(model->version()));}


// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// Loading All Operations
  static tflite::AllOpsResolver resolver;


// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// Building Interpreter
  static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;


// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// Allocating tensors and Setting up Input and Output
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  input = interpreter->input(0);
  output = interpreter->output(0);


// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// Model Configurations
  if (allocate_status != kTfLiteOk) {Serial.println("AllocateTensors() failed");return;} 
  else {Serial.println("AllocateTensor() Success");}

  size_t used_size = interpreter->arena_used_bytes();
  Serial.println("Area used bytes: " + String(used_size));


// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// Checking Model Input and Output 
  Serial.println("Model input:");
  Serial.println("dims->size: " + String(input->dims->size));
  for (int n = 0; n < input->dims->size; ++n) {
    Serial.println("dims->data[" + String(n) + "]: " + String(input->dims->data[n]));
  }

  Serial.println("Model output:");
  Serial.println("dims->size: " + String(output->dims->size));
  for (int n = 0; n < output->dims->size; ++n) {
    Serial.println("dims->data[" + String(n) + "]: " + String(output->dims->data[n]));
  }
  

// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// Setting Up Input
  File myFile = Flash.open(TEST_FILE);
  if (!myFile) { Serial.println(TEST_FILE " not found"); return; }

  Serial.println("Read " TEST_FILE);
  bmp.begin(myFile);
  BmpImage::BMP_IMAGE_PIX_FMT fmt = bmp.getPixFormat();
  if (fmt != BmpImage::BMP_IMAGE_GRAY8) {Serial.println("support format error");return;}

  int width = bmp.getWidth();
  int height = bmp.getHeight();

  Serial.println("width:  " + String(width));
  Serial.println("height: " + String(height));
  uint8_t* img = bmp.getImgBuff();

  for (int i = 0; i < width*height; ++i) {
    input->data.f[i] = (float)(img[i]/255.0);
  }


// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// Inferencing
  Serial.println("Do inference");
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {Serial.println("Invoke failed");return;}


// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// Getting Output
  for (int n = 0; n < 10; ++n) {
    float value = output->data.f[n];
    Serial.println("[" + String(n) + "] " + String(value)); 
  }
}


// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
// void loop()
void loop() {}


// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
