#include <CMRI.h>

CMRI cmri;

void setup() {
  Serial.begin(9600, SERIAL_8N2); // SERIAL_8N2 to match what JMRI expects CMRI hardware to use
  pinMode(13, OUTPUT);
}

void loop() {
  cmri.process();
  digitalWrite(13, cmri.get_bit(0));
}