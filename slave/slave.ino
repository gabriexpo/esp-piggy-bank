#include <Wire.h>
#include <Simple5641AS.h>

#define T 15

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

void receiveEvent(size_t howMany) {
  String data = "";
  while (Wire.available() > 0) {
    data += (char) Wire.read();
  }
  Serial.println(data);

  // Chiamata alla funzione di display della moneta
  displayCoin(data);
}

// Funzione per fare il display della moneta
void displayCoin(String c) {
  uint8_t x[] = {B00000000, B00000000, B00000000, B00000000};

  if (c == "0,10") {
    //x = [B11111100, B11111101, B01100000, B11111100];   // 00.10
    x[0] = 252;   // 00.10
    x[1] = 253;
    x[2] = 96;
    x[3] = 252;
  } else if (c == "0,20") {
    //x = [B11111100, B11111101, B11011010, B11111100];   // 00.20
    x[0] = 252;   // 00.20
    x[1] = 253;
    x[2] = 218;
    x[3] = 252;
  } else if (c == "0,50") {
    //x = [B11111100, B11111101, B10110110, B11111100];   // 00.50
    x[0] = 252;   // 00.50
    x[1] = 253;
    x[2] = 182;
    x[3] = 252;
  } else if (c == "1,00") {
    //x = [B11111100, B01100001, B11111100, B11111100];   // 01.00
    x[0] = 252;   // 01.00
    x[1] = 97;
    x[2] = 252;
    x[3] = 252;
  } else if (c == "2,00") {
    //x = [B11111100, B11011011, B11111100, B11111100];   // 02.00
    x[0] = 252;   // 02.00
    x[1] = 219;
    x[2] = 252;
    x[3] = 252;
  } else {
    x[0] = 158;   // EEEE (Errore)
    x[1] = 158;
    x[2] = 158;
    x[3] = 158; 
  }

  component.displayCode(T, x, 0);
}
