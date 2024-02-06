#include "LunaSat.h"

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

Adafruit_TMP117 tmp117;
Adafruit_BME680 bme; // I2C
ADXL313 adxl;
Adafruit_LIS3MDL lis3mdl;

int count = 0;
package_t package;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  //attachInterrupt(digitalPinToInterrupt(RFM95_INT), isr, CHANGE);

  Serial.begin(115200);
  while (!Serial) delay(1);
  delay(100);

  Serial.println("Feather LoRa RX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
  Wire.begin();


  //tmp117_setup(&tmp117);
  Serial.println("TEST1");
  //bme_setup(&bme);
  //adxl_setup(&adxl);
  lis3mdl_setup(&lis3mdl);

}

void loop() {
  count ++;
  if(count == 1000){

    // Get sensor data
    //package.temp_data = tmp117_get_temp(&tmp117);
    //package.adxl_data = adxl_get_data(&adxl);
    //package.bme_data = bme_get_data(&bme);
    
    Serial.println("TEST");

  }

  if (rf95.available()) {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {

      char* RX = (char*)buf;  // get the data received
      char *ID = "LunaSat # ";

      itoa(LUNA_SAT_ID, ID+9, 3);

      if(!strcmp(RX,ID)){
        // Send a reply
      
        // Convert package to bytes
        char* package_bytes = reinterpret_cast<char*>(&package);
        Serial.println(package_bytes);
        rf95.send((uint8_t *)package_bytes, 28);

        RH_RF95::printBuffer("Sent: ", (uint8_t *)package_bytes, 4);

        // 2048 bits total to send
        //rf95.send((uint8_t *)datapacket, 20);
        rf95.waitPacketSent();
        Serial.print("TX Data: ");
        Serial.println(package.temp_data);
      }
    }
    else{
      Serial.println("Receive failed");
    }
    Serial.println();
  }

}


void isr(){
  Serial.println("TES");
}