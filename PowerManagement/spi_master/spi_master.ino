#include <SPI.h>
#include "elapsedMillis.h"

#define ATTINY_CS   6

#define CS SS

elapsedMillis timeSinceStart;

void setup (void) {
  //  Serial.begin(9600); //set baud rate to 115200 for usart
  //  digitalWrite(7, HIGH); // disable Slave Select
  //  SPI.begin ();
  //  SPI.setClockDivider(SPI_CLOCK_DIV8);//divide the clock by 8
  //  digitalWrite(7, LOW); // enable Slave Select
   Serial.begin(115200); //set baud rate to 115200 for usart

  // Serial.println("Start setup");
  // delay(1000);
  // timeSinceStart = 0;
  // for(int i = 0; i < 1000; i++){
  //   Serial.print(timeSinceStart);
  //   Serial.print(", ");
  //   Serial.println(uint8_t(timeSinceStart/100.0));

  // }
  // delay(10000);
  
  pinMode(ATTINY_CS, OUTPUT);


  //  pinMode(CS, OUTPUT);
  //  digitalWrite(CS, HIGH); // disable Slave Select
   delay(10);
   SPI.begin ();
   SPI.setClockDivider(SPI_CLOCK_DIV8);//divide the clock by 8
  //  digitalWrite(CS, LOW); // enable Slave Select   
  //  delay(10);     
  Serial.println("Setup Complete");
}

void loop (void) {
  // SPI.transfer(0x01);
  // // Serial.println(SPDR);
  // delay(50);
  // SPI.transfer(0xFF);
  // Serial.println(SPDR);
  // delay(1000);
  // SPI.transfer(0x02);
  // // Serial.println(SPDR);
  // delay(50);
  // SPI.transfer(0xFF);
  // Serial.println(SPDR);
  // delay(1000);

  // disable ATTiny communication
  // Serial.println("Turning off CS");
  // digitalWrite(ATTINY_CS, HIGH);
  // delay(1000);


  // SPI.transfer(0x01);
  // delay(50);

  // SPI.transfer(0xFF);
  // delay(50);
  // // This recieve is just whatever we sent in the first transfer.
  // uint8_t rec = SPDR;

  // Serial.print("Recieved: ");
  // Serial.println(rec);

  // delay(500);

  // Serial.println("Turning on CS");
  // digitalWrite(ATTINY_CS, LOW);
  // delay(1000);

  // SPI.transfer(0x01);
  // delay(50);
  // SPI.transfer(0xFF);
  // delay(50);
  // rec = SPDR;

  // Serial.print("Recieved: ");
  // Serial.println(rec);

  // delay(500);

  // Serial.println();

  digitalWrite(ATTINY_CS, HIGH); // disable Slave Select
  delay(500);
  digitalWrite(ATTINY_CS, LOW); // disable Slave Select
  delay(500);
  digitalWrite(ATTINY_CS, HIGH); // disable Slave Select
  delay(500);
  SPI.transfer(0x01);
  delay(50);
  SPI.transfer(0xFF);
  if(SPDR == 0x3){
    Serial.println("========= ERROR AT85 SHOULD BE DESABLED =========");
    while(1);  
  }
  Serial.println("========= NO ERROR AT85 IS DISABLED =========");

  

  digitalWrite(ATTINY_CS, LOW); // enable Slave Select        

  Serial.println("");
  Serial.println("========= Started collection =========");
  delay(50);
  SPI.transfer(0x01);
  delay(50);
  SPI.transfer(0xFF);
  if(SPDR != 0x03)
    while(1);  
  Serial.println("Worked");
   
  delay(300);
  Serial.println("========= Stopped collection =========");
  SPI.transfer(0x02);
  delay(50);
  SPI.transfer(0xFF);
  delay(1000);

  int iter = 0;
  int time_step = 0;
  int prev_time = 0;
  Serial.println("========= Started data retreival =========");
  while (true){
      iter ++;
      delay(50);
      SPI.transfer(0x03);
      delay(50);
      SPI.transfer(0xFF);
      uint8_t highByte = SPDR;
      delay(50);
      SPI.transfer(0x03);
      delay(50);
      SPI.transfer(0xFF);
      uint8_t lowByte = SPDR;
      uint16_t analogValue = ((highByte & 0x3) << 8) | lowByte;
      uint16_t time = (highByte >> 2);
      if(time < prev_time){
        time_step++;
      }
      prev_time = time;
      time = time + 64 * time_step;
      if(lowByte == 0 && highByte == 0)
        break;
      float current = 1000* (3.3 * analogValue)/(1.5 * 20 * 1024 + 1.5 * analogValue);
      Serial.print(iter);
      Serial.print(" - ");
      Serial.print("Time: ");
      Serial.print(time);
      Serial.print(", Analog value: ");
      Serial.print(analogValue);
      Serial.print(", Current: ");
      Serial.print(current);
      Serial.print(" mA     ");
      Serial.print(highByte, HEX);
      Serial.print(", ");
      Serial.print(lowByte, HEX);
      Serial.println();
  }
  Serial.println("========= Finished =========");
  delay(500);

  digitalWrite(ATTINY_CS, HIGH); // disable Slave Select
  delay(50);
  SPI.transfer(0x01);
  delay(50);
  SPI.transfer(0xFF);
  if(SPDR == 0x3){
    Serial.println("========= ERROR AT85 SHOULD BE DESABLED =========");
    while(1);  
  }
  Serial.println("========= NO ERROR AT85 IS DISABLED =========");


  // char c;
  // for (const char * p = "Hello, world!\n\r" ; c = *p; p++) 
  // {
  //   SPI.transfer (c);

  //   // Recieve data back and print it
  //   SPI.transfer(0xff);
  //   c = SPDR;
  //   Serial.println(c);
  //   // delay(500);
  // }

  //*/
}