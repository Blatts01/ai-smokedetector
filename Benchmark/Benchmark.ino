#include "neuton.h"

  float inputs[] = {
  23.990, 40.300, 131, 418, 13634, 21658, 926.686 ,1.51,1.57,10.37,1.617,0.037
  };

void print_uint64_t(uint64_t num) {
  char rev[128]; 
  char *p = rev+1;
  while (num > 0) {
    *p++ = '0' + ( num % 10);
    num/= 10;
  }
  p--;
  /*Print the number which is now in reverse*/
  while (p > rev) {
    Serial.print(*p--);
  }
}


void setup() {
  Serial.begin(9600);
  Serial.println("Neuton model benchmark Tool");
}

void loop() {

  int ramUsage = neuton_model_ram_usage();
  int flashUsage = neuton_model_size_with_meta();
  Serial.write("Model Ram Usage:");
  Serial.println((String)ramUsage+ " Byte");
  Serial.write("Model Flash Usage:");
  Serial.println((String)flashUsage+ " Byte");

  uint64_t start = micros();
  float *outputs;
  uint64_t inference_time;
  if (neuton_model_set_inputs(inputs) == 0) {
    if (neuton_model_run_inference(NULL, &outputs) == 0)
    {
      uint64_t stop = micros();
      inference_time = stop - start;
    }
  }  
  Serial.write("Model inference time: ");
  print_uint64_t(inference_time);
  Serial.write(" ms \n");
  delay(1000);
}
