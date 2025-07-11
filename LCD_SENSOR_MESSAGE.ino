#include "display.h"
#include "sensor.h"
#include "server.h"

void setup() {
  Serial.begin(115200);
  setupDisplay();
  setupSensor();
  setupWiFiAndServer();
}

void loop() {
  handleServer();
  handleMessageDisplay();
}