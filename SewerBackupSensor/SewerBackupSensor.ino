/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 *
 * DESCRIPTION
 * Motion Sensor example using HC-SR501
 * http://www.mysensors.org/build/motion
 *
 */

// Enable debug prints
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_RFM69
#define MY_RFM69_FREQUENCY RFM69_433MHZ
#define MY_IS_RFM69HW

#include <MySensors.h>

#define SKETCH_NAME "Sewer Backup Sensor"
#define SKETCH_MAJOR_VER "1"
#define SKETCH_MINOR_VER "0"

#define PRIMARY_CHILD_ID 1

#define PRIMARY_BUTTON_PIN 3   // Arduino Digital I/O pin for Water Sensing


int BATTERY_SENSE_PIN = A0;  // select the input pin for the battery sense point
int oldBatteryPcnt = 0;

MyMessage msg(PRIMARY_CHILD_ID, V_TRIPPED);




void setup() {
// Setup the buttons
  pinMode(PRIMARY_BUTTON_PIN, INPUT_PULLUP); // Activate Internal Pullup Resisitor
  #if defined(__AVR_ATmega2560__)
    analogReference(INTERNAL1V1);
  #else
    analogReference(INTERNAL);
  #endif
}



void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);

// Registor Sensor as a Water Leak Sensor
  present(PRIMARY_CHILD_ID, S_WATER_LEAK);
}


void loop() {
  // put your main code here, to run repeatedly:
  
  //Measure Battery PCT
  int sensorValue = analogRead(BATTERY_SENSE_PIN);
  Serial.println(sensorValue);

  int batteryPcnt = sensorValue / 10;

  if (batteryPcnt >= 100) {
    batteryPcnt = 99;
  }
 #ifdef MY_DEBUG
  float batteryV  = sensorValue * 0.003363075;
    Serial.print("Battery Voltage: ");
    Serial.print(batteryV);
    Serial.println(" V");

    Serial.print("Battery percent: ");
    Serial.print(batteryPcnt);
    Serial.println(" %");
   #endif

  if (oldBatteryPcnt != batteryPcnt) {
    // Power up radio after sleep
    sendBatteryLevel(batteryPcnt);
    wait(500);
    oldBatteryPcnt = batteryPcnt;
  
  }
//End Battery Code

// Start Water Sensor

  uint8_t value;
  static uint8_t sentValue=2;

  // Short delay to allow buttons to properly settle
  sleep(5000);
  value = digitalRead(PRIMARY_BUTTON_PIN);

  if (value != sentValue) {
    // Value has changed from last transmission, send the updated value
    send(msg.set(value==LOW));
    sentValue = value;
  }

  // Sleep until something happens with the sensor
  sleep(PRIMARY_BUTTON_PIN-2, CHANGE, 7200000);  //Sleep Until change on Intrupt PIN2(GPIO 3) or 2 hours(Battery Check in).

}
