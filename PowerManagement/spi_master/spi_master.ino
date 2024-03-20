#include <SPI.h>
void setup (void) {
   Serial.begin(115200); //set baud rate to 115200 for usart
   pinMode(7, OUTPUT);
   digitalWrite(7, HIGH); // disable Slave Select
   SPI.begin ();
   SPI.setClockDivider(SPI_CLOCK_DIV8);//divide the clock by 8
   digitalWrite(7, LOW); // enable Slave Select
}
void loop (void) {
  delay(50);
  Serial.println("========= Started collection =========");
  digitalWrite(7, HIGH); // disable Slave Select
  delay(4000);
  digitalWrite(7, LOW);

  SPI.transfer(0x01);
  while(SPDR != 0x01){
    SPI.transfer(0x01);
  }
  Serial.println("WORKED ONCE");

  SPI.transfer(0x01);
  SPI.transfer(0x01);
  if(SPDR != 0x01){
    Serial.println("STUCK HERE 1");
    while(1);
  }

   
  delay(5000);
  SPI.transfer(0x02);
  SPI.transfer(0xFF);
  if(SPDR != 0x02){
    Serial.println("STUCK HERE 2 ");
    while(1);
  }
  Serial.println("========= Stopped collection =========");
  delay(1000);
  Serial.println("========= Started data retreival =========");
  while (true){
      SPI.transfer(0x03);
      // send something as a buffer
      SPI.transfer(0xFF);
      float analogValue = SPDR;
      if(analogValue == 0x00)
        break;
      float current = (analogValue/1024) *95.6;
      Serial.print("Analog value: ");
      Serial.print(analogValue);
      Serial.print(", Current: ");
      Serial.print(current);
      Serial.println(" mA");
  }
  Serial.println("========= Finished =========");
  // delay(500);
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
}
