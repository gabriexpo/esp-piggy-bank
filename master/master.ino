#include <Stepper.h>

const int steps_per_revolution = 2048;

Stepper *myStepper = new Stepper(steps_per_revolution, D6, D4, D5, D3);


// Soglie per il riconoscimento delle monete

/*  Alcune delle misurazioni (n/m -> valore che oscilla tra n e m)
  10 -> 194/195 - 195 - 195/196 - 195/196
  20 -> 217/218 - 216 - 214 - 214
  50 -> 229/230 - 229/230 - 229
  1 -> 221/222 - 220/221 - 223 - 222 - 220
  2 -> 241/242 - 241 - 238 - 238/240 - 240/241
*/

#define MIN10c 190
#define MAX10c 197
#define MIN20c 213
#define MAX20c 218
#define MIN1e 219
#define MAX1e 224
#define MIN50c 228
#define MAX50c 231
#define MIN2e 236
#define MAX2e 243

int baseValue = 0;
int sensorValue = 0;


// the setup routine runs once when you press reset:
void setup() {
  myStepper->setSpeed(5);
  
  Serial.begin(115200);

  setBaseValue();
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  
  int currSensorValue = analogRead(A0);
  if (abs(currSensorValue - sensorValue) > 5) {
    Serial.println(currSensorValue);
    // cambiamento significativo
    measurement();
    String coin = getCoin();
    Serial.println(coin);
    // update total
    // send to mqtt
    // send to slave
    // reposition lever
    repositionLever();
    Serial.println("Fine");
  }

  delay(1000);
  
}

void setBaseValue() {
  int sum = 0;
  int current = 0;
  for(int i = 0; i < 10; i++) {   // prime read per togliere dei risultati strani
    delay(100);
    current = analogRead(A0);
    Serial.println(current);
  }
  for(int i = 0; i < 10; i++) {   // read per calcolare il valore base
    delay(100);
    current = analogRead(A0);
    Serial.println(current);
    sum += current;
  }
  Serial.println(sum);
  baseValue = sum / 10;
  sensorValue = baseValue;
  Serial.println("Base value:");
  Serial.println(baseValue);
}

void measurement() {
  int diff;

  Serial.println("Measurement:");
  do {
    delay(200);             // forse gioco un po' con il delay
    int currSensorValue = analogRead(A0);
    diff = abs(sensorValue - currSensorValue);
    sensorValue = currSensorValue;
    Serial.println(sensorValue);
  } while(diff > 3);
}

String getCoin() {
  String coin;
  if (sensorValue >= MIN10c && sensorValue <= MAX10c) {
    coin = "0,10";
  } else if (sensorValue >= MIN20c && sensorValue <= MAX20c) {
    coin = "0,20";
  } else if (sensorValue >= MIN1e && sensorValue <= MAX1e) {
    coin = "1,0";
  } else if (sensorValue >= MIN50c && sensorValue <= MAX50c) {
    coin = "0,50";
  } else if (sensorValue >= MIN2e && sensorValue <= MAX2e) {
    coin = "2,00";
  } else {
    coin = "Non riconosciuta";
  }
  
  return coin;
}

void repositionLever() {
  Serial.println("Reposition lever:");
  int stepsTaken = 0;
  int current;
  while (abs(sensorValue - baseValue) > 2) {
    myStepper->step(2);
    stepsTaken += 2;
    current = analogRead(A0);
    sensorValue = current;
    Serial.println(current);
    delay(1);
  }

  //myStepper->step(-500); errore se provo con step troppo grandi

  for (int i = 0; i < stepsTaken / 2; i++) {
    myStepper->step(-2);
    delay(1);
  }
}
