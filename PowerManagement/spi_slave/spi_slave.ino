/*
* UC2.c
*
* Created: 20/03/2020 15:53:57
* Author : Onion
*/
 
//horloge de 1MHz
 
#include <avr/io.h>
#include <avr/interrupt.h>
 
#include "elapsedMillis.h"

#define USICR   _SFR_IO8(0x0D)
#define READ_PIN 3
#define LED_PIN 4
struct DataStruct {
    float time = 0;
    int analogReading = 0;
};

int readingDelay = 5;

// struct {
//   uint8_t data;
//   uint8_t time_stamp;
// } DataStruct;

// uint8_t data_size = 150;
// struct DataStruct* DataArr;

// void CreateDataStruct(struct DataStruct* Arr, uint8_t length) {
//   DataArr = malloc(sizeof(DataStruct) * length);
// }
// Data of 150 points containing 2 bytes of data to transfer of length 150 items
const uint8_t size = 200;
uint16_t Data[size];
uint8_t head = 0;
uint8_t tail = 0;

bool collecting = false;
bool lastByte = false;

elapsedMillis timeSinceStart;
elapsedMillis timeSinceRead;
int iter;

void SPI_USI_init(){
  //activation des ports
  //MOSI: PB0 input
  //MISO: PB1 output
  //SCK:  PB2 input? output selon datasheet -> marche avec: INPUT
  DDRB|=0x02; //0000 0010
  DDRB&=~0x05;//0000 0101
  /*USICR=0x52;//0 1(COV interrupt) 01(3 wires) 100(master clock ?) 0   */
 
  USICR = ((1<<USIWM0)|(1<<USICS1));
  USICR |= (1<<USIOIE);
}
 
ISR (USI_OVF_vect){
  PORTB^=0x10; //PB3 test
  uint8_t reception=USIDR;

  if(digitalRead(4) == 1)
  {
    return;
  }
  else if(reception == 0x01){
    // analogWrite(4, HIGH);
    collecting = true;
    timeSinceStart = 0;
    tail = 0;
    head = 0;
    iter = 0;
    USIDR = 0x03;
    lastByte = false;
  }
  else if(reception == 0x02) {
    collecting = false;
    USIDR = 0x04;
    lastByte = false;
  }
  else if(reception == 0x03) {

    USIDR = head;
    return;

    if(tail == head) {
      USIDR = 0x00;
    }
    else {
      if(!lastByte){
        USIDR = (uint8_t)(Data[tail] >> 8);
      }
      else {
        USIDR = (uint8_t) (Data[tail] & 0xFF);

        if((tail + 1) == size)
          tail = 0;
        else
          tail += 1;
      }

      lastByte = !lastByte;
    }
  }
 
  USISR = 1<<USIOIF;
 
  PORTB^=0x10; //PB3 test
 
}
void setup() {
  pinMode(4, INPUT);
  pinMode(3, INPUT);
  // digitalWrite(4, HIGH);
  // delay(1000);
  // digitalWrite(4, LOW);
  // delay(1000);
  // digitalWrite(4, HIGH);
  // delay(1000);
  // digitalWrite(4, LOW);

  DDRB=0x10;
  DDRB|=0x10; //PB4 test 0001 0000
  PORTB&=~0x10;
  SPI_USI_init();
 

  // digitalWrite(4, HIGH);
  // delay(1000);
  // digitalWrite(4, LOW);
  // delay(1000);
  // digitalWrite(4, HIGH);
  // delay(1000);
  // digitalWrite(4, LOW);

  sei();
}

void loop(void)
{
  uint16_t reading = uint16_t(analogRead(READ_PIN));
  if (collecting) {
    // digitalWrite(4, HIGH);

  // if(collecting && timeSinceRead > readingDelay){
    if(tail == head){
      // ARRAY EMPTY

      // Store data
      uint8_t time = uint8_t(timeSinceStart);
      // uint8_t time = iter++;
      if (reading == 0){
        reading = 1000;
      }
      Data[head] = reading & 0x3FF; // 0b11 1111 1111
      Data[head] = Data[head] | ((time & 0x3F) << 10); // 0b11 1111
      
      // head = (head+1 == size) ? 0 : head++;
      if ((head + 1) == size)
        head = 0;
      else
        head += 1;

      // digitalWrite(LED_PIN, HIGH);
      // delay(100);
      // digitalWrite(LED_PIN, LOW);
      // delay(100);
    }
    else if(((head+1) == tail) || ((head+1 == size) && !tail))
    {
      // ARRAY FULL DO nothing
    }
    else {
      // If data is within 10mV of previous range -> do nothing
      uint16_t prev_reading = 0;
      if (head) 
        prev_reading = Data[head-1] & 0x3FF;
      else
        prev_reading = Data[size-1] & 0x3FF;


      if(abs(prev_reading - reading) > 3){
        // Store data
        uint8_t time = uint8_t(timeSinceStart);
        // uint8_t time = iter++;
        Data[head] = reading & 0x3FF;
        Data[head] = Data[head] | ((time & 0x3F) << 10);

        // head = (head+1 == size) ? 0 : head++;
        if((head + 1) == size)
          head = 0;
        else
          head += 1;
      }
    }

   
  //   timeSinceRead = 0;
  // }
    delay(2);

  }
  // else
  //   digitalWrite(LED_PIN, LOW);
}
