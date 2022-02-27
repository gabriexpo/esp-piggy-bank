#include <Stepper.h>
#include <Wire.h>
#include <ESP_EEPROM.h>
#include <EspMQTTClient.h>
#include "costanti.h"

// EspMQTTClient init

EspMQTTClient client(
  WIFI,             // da costanti.h
  PASSWORD,         // da costanti.h
  "192.168.1.54",  // MQTT Broker server ip
//  "MQTTUsername",   // Omesso dato che uso una connessione anonima
//  "MQTTPassword",   // Omesso dato che uso una connessione anonima
  "ESP8266",        // Client name
  1883              // The MQTT port, default to 1883. this line can be omitted
);

// Stepper init

const int steps_per_revolution = 2048;
Stepper *myStepper = new Stepper(steps_per_revolution, D6, D4, D5, D3);

// I2C data

#define SDA_PIN D2
#define SCL_PIN D1
const int16_t I2C_MASTER = 0x42;
const int16_t I2C_SLAVE = 0x08;


// Soglie per il riconoscimento delle monete

/*  Alcune delle misurazioni (n/m -> valore che oscilla tra n e m)
  Moneta -> [misurazioni]
  
  10  -> 194/195 - 195 - 195/196 - 195/196
  20  -> 217/218 - 216 - 214 - 214
  50  -> 229/230 - 229/230 - 229
  1   -> 221/222 - 220/221 - 223 - 222 - 220
  2   -> 241/242 - 241 - 238 - 238/240 - 240/241
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
#define MAX2e 249

int baseValue = 0;
int sensorValue = 0;

float total;

// -----
// SETUP
// -----

void setup() {
  Serial.begin(115200);
  
  // Set Stepper motor speed
  myStepper->setSpeed(5);

  // Setting up EEPROM memory space and retrieving total
  EEPROM.begin(16);
  EEPROM.get(0, total);

  // Setting up i2c connection
  Wire.begin(SDA_PIN, SCL_PIN, I2C_MASTER);

  // Setting MQTT related options
  client.enableDebuggingMessages();
  client.enableMQTTPersistence();
  //client.enableLastWillMessage("ESP8266/piggy-bank", "Piggy-bank going offline!");

  // Setting up base value for measurement
  setBaseValue();
}

// ----
// LOOP
// ----

void loop() {

  client.loop();
  
  int currSensorValue = analogRead(A0);
  if (abs(currSensorValue - baseValue) > 20) {        // 20 valore fissato per indicare un cambiamento significativo nello stato del potenziometro
    Serial.println(currSensorValue);
    
    measurement();
    String coin = getCoin();
    Serial.println(coin);
    
    // update total
    if (coin != "nan") {
      total += coin.toFloat();
      //Serial.println(coin.toFloat());
      Serial.print("Total: ");
      Serial.println(total);

      // EEPROM
      EEPROM.put(0, total);
      boolean res = EEPROM.commit();
      Serial.println((res) ? "Commit ok" : "Commit failed");

      // send to mqtt
      client.publish("ESP8266/piggy-bank", "Inserted coin: " + coin);
    }
    
    // send to slave
    i2c_Transmission(coin);

    delay(500);
    // reposition lever
    repositionLever();
    Serial.println("Fine");
  }

  delay(1000);
  
}

void onConnectionEstablished() {
  // Subscribe to ESP8266/piggy-bank to check messages arrived to MQTT server
  client.subscribe("ESP8266/piggy-bank", [](const String & payload) {
    Serial.println("MQTT server received: " + payload);
  });
  
  client.publish("ESP8266/piggy-bank", "Piggy-bank online!");
}

// Funzione per stabilire il valore "a riposo" che percepisce il potenziometro.

void setBaseValue() {
  int sum = 0;
  int current = 0;
  
  for(int i = 0; i < 10; i++) {   // prime read per togliere dei risultati strani
    delay(100);
    current = analogRead(A0);
    //Serial.println(current);
  }
  
  for(int i = 0; i < 10; i++) {   // read per calcolare il valore base
    delay(100);
    current = analogRead(A0);
    //Serial.println(current);
    sum += current;
  }
  
  baseValue = sum / 10;
  sensorValue = baseValue;
  
  Serial.println("Base value:");
  Serial.println(baseValue);
}

// Funzione per trasmettere, sotto forma di stringa, il valore della moneta
// inserita alla board slave.

void i2c_Transmission(String message) {
  Wire.beginTransmission(I2C_SLAVE);  // transmit to slave
  Wire.write(message.c_str());
  Wire.endTransmission();    // stop transmitting
}

// Funzione che aspetta che l'utente finisca di far passare la moneta
// tra la barra di misurazione e la parete di sostegno e che termina 
// ponendo SensorValue al valore raggiunto dal passaggio della moneta.

void measurement() {
  Serial.println("Measurement:");

  int diff;
  
  do {
    delay(200);             // forse gioco un po' con il delay
    int currSensorValue = analogRead(A0);
    diff = abs(sensorValue - currSensorValue);
    sensorValue = currSensorValue;
    //Serial.println(sensorValue);
  } while(diff > 3);
  
}

// Funzione che trasforma il valore del potenziometro (nella variabile sensorValue)
// in una stringa contenente il valore della moneta secondo le soglie definite 
// nelle prime righe del codice

String getCoin() {
  String coin;
  
  if (sensorValue >= MIN10c && sensorValue <= MAX10c) {
    coin = "0.10";
  } else if (sensorValue >= MIN20c && sensorValue <= MAX20c) {
    coin = "0.20";
  } else if (sensorValue >= MIN1e && sensorValue <= MAX1e) {
    coin = "1.00";
  } else if (sensorValue >= MIN50c && sensorValue <= MAX50c) {
    coin = "0.50";
  } else if (sensorValue >= MIN2e && sensorValue <= MAX2e) {
    coin = "2.00";
  } else {
    coin = "nan";
  }

  return coin;
}

// Funzione per riposizionare la leva che aziona il potenziometro dopo una misurazione

void repositionLever() {
  Serial.println("Reposition lever:");
  
  int stepsTaken = 0;
  int current;
  
  while (abs(sensorValue - baseValue) > 2) {
    myStepper->step(-2);
    stepsTaken += 2;
    current = analogRead(A0);
    sensorValue = current;
    //Serial.println(current);
    delay(1);
  }

  //myStepper->step(-500); errore se provo con step troppo grandi

  for (int i = 0; i < stepsTaken / 2; i++) {
    myStepper->step(2);
    delay(1);
  }
}
