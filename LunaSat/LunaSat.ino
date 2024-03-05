#include "LunaSat.h"

// Define instance of LoRa
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Define instances of all Sensors
Adafruit_BME680 bme; 
ADXL313 adxl;
Adafruit_LIS3MDL lis3mdl;


package_t package;

void setup() {

  // Pins for LoRa Module
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600); //115200
  while (!Serial) delay(1);
  delay(100);


  /* Sensor Setup */
  Serial.println("Begin Sensor Setup");

  /* For LIS3MDL, you only need to set it up once. If you try to set it up again 
     without fulling cutting power to the sensor, you will receive a setup failure. */
  // lis3mdl_setup(&lis3mdl);
  // delay(1000);

  adxl_setup(&adxl);
  delay(1000);

  bme_setup(&bme);
  delay(1000);

  Serial.println("All Sensors Good!");

  
  attachInterrupt(digitalPinToInterrupt(RFM95_INT), isr, CHANGE);


  /* Initalization of LoRa */ 
  Serial.println("Begin LoRa Setup!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  while (!rf95.init()) {
    Serial.println("LoRa Setup failed!");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  // Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  Serial.println("LoRa Setup Successful!");

  // Setting TX power changes the current consumption durring tranmission (23 is max)
  rf95.setTxPower(23, false);

  Serial.println();

}

void loop() {
  // Get sensor data

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
