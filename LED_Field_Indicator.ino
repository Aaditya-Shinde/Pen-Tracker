#include <Adafruit_NeoPixel.h>
#define LED_PIN 38
#define NUM_LEDS 1
#define HALL_READ_PIN 4
#define CALIBRATION_SAMPLES 1000

Adafruit_NeoPixel led(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

float baseline = 0;
float threshold_pos = 0;
float threshold_neg = 0;
float min_edge = 4095;
float max_edge = 0;

void calibrate() {
  Serial.println("Calibrating Baseline");
  led.setPixelColor(0, led.Color(100, 100, 0));
  led.show();

  float min_read = 4095;
  float max_read = 0;
  float sum = 0;
  float reading = 0;
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    reading = analogRead(HALL_READ_PIN);
    sum += reading;
    min_read = min(min_read, reading);
    max_read = max(max_read, reading);
    delay(3);
  }

  baseline = sum / CALIBRATION_SAMPLES;
  threshold_pos = max_read - baseline;
  threshold_neg = baseline - min_read;

  Serial.println("Baseline Calibration Complete");
  led.setPixelColor(0, led.Color(0, 0, 100));
  led.show();
  Serial.println("Pick up magnet");
  delay(3000);

  Serial.println("Present side 1 to sensor");
  led.setPixelColor(0, led.Color(255, 0, 0));
  led.show();
  max_edge = 0;
  min_edge = 4095;
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    reading = analogRead(HALL_READ_PIN);
    max_edge = max(max_edge, reading);
    min_edge = min(min_edge, reading);
    delay(3);
  }

  Serial.println("Flip magnet");
  led.setPixelColor(0, led.Color(0, 0, 100));
  led.show();
  delay(3000);

  Serial.println("Present side 2 to sensor");
  led.setPixelColor(0, led.Color(0, 255, 0));
  led.show();
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    reading = analogRead(HALL_READ_PIN);
    max_edge = max(max_edge, reading);
    min_edge = min(min_edge, reading);
    delay(3);
  }

  Serial.println("Calibration complete");
  led.setPixelColor(0, led.Color(0, 0, 100));
  led.show();
}

void setup() {
  Serial.begin(115200);
  led.begin();
  led.setBrightness(100);
  led.setPixelColor(0, led.Color(255, 255, 255));
  led.show();
  delay(5000);
  calibrate();
}

void loop() {
  float raw_reading = analogRead(HALL_READ_PIN);
  float directional_reading = raw_reading - baseline;

  Serial.println(directional_reading);

  int r, g, b;
  if (directional_reading > threshold_pos) {
    r = (directional_reading/(max_edge - baseline))*255;
    g = 0;
    b = (1- directional_reading/(max_edge - baseline))*255;
  } else if (directional_reading < -threshold_neg) {
    r = 0;
    g = (fabs(directional_reading)/(baseline-min_edge))*255;
    b = (1-fabs(directional_reading)/(baseline-min_edge))*255;
  } else {
    r = 0;
    g = 0;
    b = 255;
  }

  r = min(max(0, r), 255);
  g = min(max(0, g), 255);
  b = min(max(0, b), 255);

  led.setPixelColor(0, led.Color(r, g, b));
  led.show();
  delay(15);
}
