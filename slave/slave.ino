#include <Wire.h>
#include <Simple5641AS.h>

#define T 10

// 5641AS stuff

const uint8_t segmentPins[] = {2, 3, 4, 5, 6, 7, 8, 9};
const uint8_t digitSelectionPins[] = {10, 11, 12, 13};

Simple5641AS component(segmentPins, digitSelectionPins);

// Setup

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent);
  Serial.begin(115200);
}

// Loop

void loop() {
  delay(1000);
}

// Functions

void receiveEvent() {
  String data = "";
  while (Wire.available() > 0) {
    data += Wire.read();
  }
  Serial.print(data);

  // Chiamata alla funzione di display della moneta
  displayCoin(data.toFloat());
}

// Funzione per fare il display della moneta
void displayCoin(float c) {
  uint8_t x[] = {B01100000, B01100000, B01100000, B01100000};
  
  
  switch(c) {
    //case
    default:
      x[] = {};
      break;
    }
  for (int i = 0; i < T; i++) {      // ciclo per mostrare il valore della moneta per T secondi
    component.displayCode(10, x, 0);
    delay(1000);
  }
}
