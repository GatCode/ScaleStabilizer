#include "HX711.h"
#include "ScaleStabilizer.h"

// Scale
const int DOUT_PIN = 23;
const int SCK_PIN = 22;
HX711 scale;

// Scale Stabilizer
const int WINDOW_SIZE = 12;
const double WEIGHT_THRESHOLD = 0.05;
ScaleStabilizer stabilizer;

void setup() {
  Serial.begin(115200);
  
  scale.begin(DOUT_PIN, SCK_PIN);
  scale.set_scale(720);
  scale.tare();

  stabilizer.begin(WINDOW_SIZE, WEIGHT_THRESHOLD);
}

void loop() {
  double adcReading = scale.get_units();
  stabilizer.add(adcReading);
  Serial.println(stabilizer.getStablilizedReading());
}
