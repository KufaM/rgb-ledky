#include <Arduino.h>
#include <IRremote.hpp>
#define IR_USE_AVR_TIMER 1
#define IR_RECEIVE_PIN 12
#define MINUS 4161273600 // Kód tlačítka pro potvrzení zadání hodnoty pro červenou LED
#define PLUS 3927310080// Kód tlačítka pro potvrzení zadání hodnoty pro zelenou LED
#define EQ 4127850240// Kód tlačítka pro potvrzení zadání hodnoty pro modrou LED
#define redLed 9
#define greenLed 5
#define blueLed 6

unsigned long codes[] = {
    3910598400, // 0
    4077715200, // 1
    3877175040, // 2
    2707357440, // 3
    4144561920, // 4
    3810328320, // 5
    2774204160, // 6
    3175284480, // 7
    2907897600, // 8
    3041591040, // 9
};

String accumulatedNumber = "";
int activeLED = 0; // 0 = červená, 1 = zelená, 2 = modrá
const int codesArraySize = sizeof(codes) / sizeof(codes[0]); // Počet prvků v poli codes

String getNumFromCode(unsigned long irCode) {
  for (int i = 0; i < codesArraySize; i++) {
    if (irCode == codes[i]) {
      return String(i);
    }
  }
  return ""; // Pokud kód neodpovídá žádnému číslu, vrátí prázdný řetězec
}

void adjustLED(int value, int led) {
  switch (led) {
    case 0:
      analogWrite(redLed, value);
      Serial.print("Červená: ");
      Serial.println(value);
      break;
    case 1:
      analogWrite(greenLed, value);
      Serial.print("Zelená: ");
      Serial.println(value);
      break;
    case 2:
      analogWrite(blueLed, value);
      Serial.print("Modrá: ");
      Serial.println(value);
      break;
    default:
      Serial.println("Neplatný stav ");
      break;
  }
}

void setup() {
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Inicializace přijímače
  Serial.begin(9600); // Inicializace sériové komunikace

  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);

  Serial.println("Zadejte číslo pro LED:");
}

void loop() {
  if (IrReceiver.decode()) { 
    unsigned long receivedIRCode = IrReceiver.decodedIRData.decodedRawData; // Uložení přijatého kódu
    if (receivedIRCode > 0) { // Kontrola platnosti kódu
      Serial.print("Přijatý kód: ");
      Serial.println(receivedIRCode); // Výpis přijatého kódu
      if (receivedIRCode == EQ || receivedIRCode == MINUS || receivedIRCode == PLUS) {
        int intValue = accumulatedNumber.toInt(); // Převod čísla na int
        if (intValue >= 0 && intValue <= 255) { // Kontrola,rozsahu 0 - 255
          if (receivedIRCode == MINUS) {
            adjustLED(intValue, 0);
          } else if (receivedIRCode == PLUS) {
            adjustLED(intValue, 1);
          } else if (receivedIRCode == EQ) {
            adjustLED(intValue, 2);
          }
          Serial.print("Nastavená hodnota: ");
          Serial.println(intValue); // Výpis nastavené hodnoty
          accumulatedNumber = ""; // Vyčištění  čísla
        } else {
          Serial.println("Hodnota mimo rozsah");
          accumulatedNumber = ""; // Vyčištění čísla
        }
      } else {
        accumulatedNumber += getNumFromCode(receivedIRCode);
        Serial.print("Aktuální číslo: ");
        Serial.println(accumulatedNumber); // Výpis  čísla
      }
    }

    IrReceiver.resume(); // Příprava přijímače na další signál
  }
}
