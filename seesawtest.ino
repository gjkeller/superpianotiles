#include <Adafruit_seesaw.h>

#define TEST_PIN 0
Adafruit_seesaw ss;

void setup() {
  Serial.begin(115200);
  Serial.println(F("Waiting for seesaw to start..."));
  if(!ss.begin()){
    while(1) delay(10);
  }
  Serial.println(F("Seesaw started..."));

  ss.pinMode(TEST_PIN, PULLUP);
}

void loop() {
  int result = ss.digitalRead(TEST_PIN);
  Serial.printf("Pin result: %d", result);
}