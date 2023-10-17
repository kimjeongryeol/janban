#include <Wire.h>
#include <Adafruit_PN532.h>
#include "HX711.h"
#include <SoftwareSerial.h>

SoftwareSerial bluetooth(2, 3); // RX, TX

#define SDA_PIN A4
#define SCL_PIN A5

#define calibration_factor -7050.0

const int DOUT = 4;
const int CLK = 5;

HX711 scale;
float sum_kg = 0.0; // Variable to store cumulative weight

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

void setup(void) {
  // Initialize the scale
  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor);
  scale.tare();

  // Initialize the NFC module
  nfc.begin();

  // Initialize Bluetooth communication
  bluetooth.begin(9600);
}

void loop(void) {
  nfc.SAMConfig();

  // Read the weight from the scale
  scale.set_scale(calibration_factor);
  float weight_kg = scale.get_units(1); // Convert weight to kg

  if (weight_kg >= 0) {
    Serial.print("Weight: ");
    Serial.print(weight_kg, 1); // Print weight with 1 decimal place
    Serial.println(" kg");

    // Update cumulative weight
    sum_kg += weight_kg;

    // Print cumulative weight
    Serial.print("Sum: ");
    Serial.print(sum_kg, 1); // Print cumulative weight with 1 decimal place
    Serial.println(" kg");
  } else {
    weight_kg = 0.0;
    Serial.print("Weight: 0.0 kg\n");
  }

  // Check for an NFC card
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Found an NFC card!");

    Serial.print("UID Value (Decimal): ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(uid[i]);
      if (i < uidLength - 1) {
        Serial.print(", ");
      }
    }
    Serial.println();

    // Send weight and UID over Bluetooth
    
    bluetooth.print("UID: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      bluetooth.print(uid[i]);
      if (i < uidLength - 1) {
        bluetooth.print(" ");
      }
    }
    bluetooth.println();
  }
    bluetooth.print(" Weight: ");
    bluetooth.print(weight_kg, 1); // Send weight with 1 decimal place
    bluetooth.print(" kg, Sum: ");
    bluetooth.print(sum_kg, 1); // Send cumulative weight with 1 decimal place
    bluetooth.print(" kg");
    bluetooth.println();

  delay(1000);
}