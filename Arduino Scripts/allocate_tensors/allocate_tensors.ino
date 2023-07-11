// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ()Including All Libraries

#include <Camera.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "model_0.h"

tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
constexpr int kTensorArenaSize = 980000;
uint8_t tensor_arena[kTensorArenaSize];
int inference_count=0;

void setup() {
  Serial.begin(115200);
  tflite::InitializeTarget();
  memset(tensor_arena, 0, kTensorArenaSize*sizeof(uint8_t));

  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter= &micro_error_reporter;

  model = tflite::GetModel(model_tflite);
  if(model->version() != TFLITE_SCHEMA_VERSION) {Serial.println("Model provided is schema version " + String(model->version()) + " not equal "+ "to supported version "+ String(TFLITE_SCHEMA_VERSION));return;} 
  else{Serial.println("Model Version: " + String(model->version()));}

  static tflite::AllOpsResolver resolver;


  static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  TfLiteStatus allocate_status= interpreter->AllocateTensors();
  input = interpreter->input(0);
  output = interpreter->output(0);

  if (allocate_status != kTfLiteOk) {Serial.println("AllocateTensors() failed");return;} 
  else {Serial.println("AllocateTensor() Success");}
  
  size_t used_size = interpreter->arena_used_bytes();
  Serial.println("Area used bytes: " + String(used_size));
  input = interpreter->input(0);
  output = interpreter->output(0);

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
}
void loop() {
}