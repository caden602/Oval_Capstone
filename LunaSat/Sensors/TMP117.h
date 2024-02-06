/**
 * @file basic_test.ino
 * @author Bryan Siepert for Adafruit Industries
 * @brief Shows how to specify a
 * @date 2020-11-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <Wire.h>
#include <Adafruit_TMP117.h>
#include <Adafruit_Sensor.h>


void tmp117_setup(Adafruit_TMP117  *tmp117) {

  // Try to initialize!
  if (!tmp117->begin()) {
    Serial.println("Failed to find TMP117");
    while (1) { delay(10); }
  }
  Serial.println("TMP117 Found!");

}

float tmp117_get_temp(Adafruit_TMP117  *tmp117) {

  sensors_event_t temp;         // create an empty event to be filled
  tmp117->getEvent(&temp);      //fill the empty event object with the current measurements
  
  return temp.temperature;
}