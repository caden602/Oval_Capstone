#include "LunaSat.h"

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

Adafruit_TMP117 tmp117;
Adafruit_BME680 bme; // I2C
ADXL313 adxl;
Adafruit_LIS3MDL lis3mdl;
//Adafruit_MMC5603 mmc = Adafruit_MMC5603(12345);

package_t package;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600); //115200
  while (!Serial) delay(1);
  delay(100);

  // Serial.println("________________");

  //*
  // SENSOR Setup
  // Serial.println("Beginning setup");

  //lis3mdl_setup(&lis3mdl);

  adxl_setup(&adxl);
  delay(1000);

  bme_setup(&bme);
  delay(1000);


  Serial.println("All Sensors Good!");
  //delay(5000);
  //while(true);

  Serial.println("Setup Complete skeeyee!");

  //*/

  // Initalize the pinmode for RF
  //*
  
  //attachInterrupt(digitalPinToInterrupt(RFM95_INT), isr, CHANGE);
  


  // Initalization of LoRa
  //*
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
  rf95.setTxPower(23, false);
  //*/
  

  //delay(5000);

}

void loop() {
  // Get sensor data

  // delay(5000);

  // scratch the TMP 
  // Serial.println("TEST");
  // //*
  // Serial.println("Start of loop");
  // float temp;
  // temp = tmp117_get_temp(&tmp117);
  // Serial.println(temp);

  // delay(5000);

  Serial.println("Sampling BME688");
  bme_data_t bme_data;
  bme_get_data(&bme, &bme_data);
  package.bme_data = bme_data;
  Serial.print("BME DATA: ");
  Serial.print(bme_data.humidity);
  Serial.println(bme_data.pressure);

  delay(5000);

  Serial.println("Sampling ADXL313");
  adxl_data_t adxl_data;
  adxl_get_data(&adxl, &adxl_data);
  package.adxl_data = adxl_data;
  Serial.print("ADXL DATA: ");
  Serial.print("x: "); Serial.print(adxl_data.x); Serial.print(", ");
  Serial.print("y: "); Serial.print(adxl_data.y); Serial.print(", ");
  Serial.print("z: "); Serial.print(adxl_data.z); Serial.println();

  delay(5000);

  Serial.println("Sampling LIS3MDL");
  lis3mdl_data_t lis3mdl_data;
  lis3mdl_get_data(&lis3mdl, &lis3mdl_data);
  package.lis3mdl_data = lis3mdl_data;
  Serial.print("LIS3MDL DATA: ");
  Serial.print("x: "); Serial.print(lis3mdl_data.x); Serial.print(", ");
  Serial.print("y: "); Serial.print(lis3mdl_data.y); Serial.print(", ");
  Serial.print("z: "); Serial.print(lis3mdl_data.z); Serial.println();

  delay(5000);


  //*
  while(1){
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

        RH_RF95::printBuffer("Sent: ", (uint8_t *)package_bytes, 28);

        // 2048 bits total to send
        //rf95.send((uint8_t *)datapacket, 20);
        rf95.waitPacketSent();
      }
    }
    else{
      Serial.println("Receive failed");
    }
    Serial.println();
    break;
  }
  
  //*/
  }
  Serial.println();
}

// Function is not setup yet, but can be linked as an ISR
void isr(){
  Serial.println("TEST ISR");
}
