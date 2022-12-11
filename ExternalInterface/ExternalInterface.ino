#define DECODE_RC6
#define IR_RECEIVE_PIN 5
#include <IRremote.hpp>

//#define DEBUG

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial1.begin(115200);
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("Start...");
#endif
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
}

void loop() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.address == 0 && (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) == 0 ) {
      if ( digitalRead(LED_BUILTIN) == HIGH) digitalWrite(LED_BUILTIN, LOW);
      else digitalWrite(LED_BUILTIN, HIGH);
      if (IrReceiver.decodedIRData.command == 0x22) { // RakutenTV??
        ESP.reset();  // Somtimes the IRremote library gets out of sync :-(
      }
      Serial1.write(IrReceiver.decodedIRData.command);
#ifdef DEBUG
      Serial.print(IrReceiver.decodedIRData.command, HEX);
      Serial.print('/');
      Serial.print(IrReceiver.decodedIRData.decodedRawData, HEX);
      Serial.print('/');
      Serial.println(IrReceiver.repeatCount);
      if (IrReceiver.decodedIRData.command == 0x10) {
        Serial.println("VOLUME-UP");
      } else if (IrReceiver.decodedIRData.command == 0x11) {
        Serial.println("VOLUME-DOWN");
      } else if (IrReceiver.decodedIRData.command == 0x2B) {
        Serial.println("BACK");
      } else if (IrReceiver.decodedIRData.command == 0x28) {
        Serial.println("FWD");
      } else if (IrReceiver.decodedIRData.command == 0x2B) {
        Serial.println("BACK");
      } else if (IrReceiver.decodedIRData.command == 0x28) {
        Serial.println("FWD");
      } else if (IrReceiver.decodedIRData.command == 0x58) {
        Serial.println("UP");
      } else if (IrReceiver.decodedIRData.command == 0x59) {
        Serial.println("DOWN");
      } else if (IrReceiver.decodedIRData.command == 0x5A) {
        Serial.println("LEFT");
      } else if (IrReceiver.decodedIRData.command == 0x5B) {
        Serial.println("RIGHT");
      } else if (IrReceiver.decodedIRData.command == 0x5C) {
        Serial.println("OK");
      } else if (IrReceiver.decodedIRData.command == 0x0C) {
        Serial.println("POWER");
      }
#endif
    }
    IrReceiver.resume();
  }
  delay(5);
}
