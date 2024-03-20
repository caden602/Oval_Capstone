#include <LunaSat.h>

#define RFM95_CS    4
#define RFM95_INT   3
#define RFM95_RST   2  

#define ATTINY_CS   7

// Define instance of LoRa (Pass in 'true' for use with LunaSat interrupts)
RH_RF95 rf95(RFM95_CS, RFM95_INT, true);

// Define instances of all Sensors
Adafruit_BME680 bme; 
ADXL313 adxl;
Adafruit_LIS3MDL lis3mdl;

unsigned long myTime;

// Define global array to store packages
static package_t package;
static int num_pack = 0;
static int page_num = 0;

void setup() {

  pinMode(7, OUTPUT);

  // disable ATTiny communication
  digitalWrite(7, HIGH);

  Serial.begin(115200); 
  while (!Serial) delay(1);
  delay(100);

  /* Sensor Setup */
  // Serial.println("Begin Sensor Setup");

  /* For LIS3MDL, you only need to set it up once. If you try to set it up again 
     without fulling cutting power to the sensor, you will receive a setup failure. */
  //lis3mdl_setup(&lis3mdl);
  //delay(1000);

  adxl_setup(&adxl);
  delay(2000);
  
  bme_setup(&bme);
  delay(1000);

  // Serial.println("All Sensors Good!");

  delay(1000);

  /* LoRa Setup */
  lora_setup(&rf95, RFM95_RST, true);
  rf95.setModeRx();

  /* Scheduler Setup */
  scheduler_open();

  delay(5000);
}

void loop() {

  // disable ATTiny communication
  //digitalWrite(7, LOW);
 // while(1);

//  digitalWrite(7, HIGH);
//  delay(2000);
//  digitalWrite(7, LOW);
//  while(1);

  bme_data_t bme_data;
  adxl_data_t adxl_data;
  lis3mdl_data_t lis3mdl_data;

  // Get sensor data
  // Serial.println("Sampling BME688");
  bool bme_error = bme_get_data(&bme, &bme_data);
  if(!bme_error){
    // Serial.println("BME Data error!");
  }
  package.bme_data = bme_data;
  package.bme_error = bme_error;

  // Serial.println("Sampling ADXL313");
  bool adxl_error = adxl_get_data(&adxl, &adxl_data);
  if(!adxl_error){
    // Serial.println("ADXL Data error!");
  }
  package.adxl_data = adxl_data;
  package.adxl_error = adxl_error;

  // Serial.println("Sampling LIS3MDL");
  bool lis3mdl_error = lis3mdl_get_data(&lis3mdl, &lis3mdl_data);
  if(!lis3mdl_error){
    // Serial.println("LIS3DML Data error!");
  }
  package.lis3mdl_data = lis3mdl_data;
  package.lis3mdl_error = lis3mdl_error;

  // TODO:
  // Add in timestamp here and apply to package.
  package.time_stamp = millis();

  // Print contents of pacakge
  // print_package_for_serial(&package);

  // Store Package in EEPORM if we can fit it
  if(page_num < 512){
    store_package(&package, page_num);
    Serial.print("Storing Package on page ");
    Serial.println(page_num);
    print_package_for_serial(&package);
    page_num++;
  }
  else{
    // reset page_num and start overwriting existing measurments
    page_num = 0;
  }


  // Check if we recieved a data request event
  if(get_scheduled_events() & EVENT_DATA_REQUEST){
    Serial.println("Recieved Data Request");
    remove_scheduled_event(EVENT_DATA_REQUEST);
    // Serial.print("Sending "); Serial.print(num_pack); Serial.println(" packages");



    //*

    // Turn off LoRa CS
    digitalWrite(RFM95_CS, HIGH);
    
    // Enable Slave select for ATTiny
    digitalWrite(7, LOW);

    // Begin ATTiny sample power
    SPI.transfer(0x01);
    SPI.transfer(0x01);
    // if(SPDR != 0x22){
    //   // Serial.println("STUCK HERE");
    //   while(1);
    // }

    Serial.println("Started Sampling ATtiny");
    
    // Disable Slave select for ATTiny
    digitalWrite(7, HIGH);

    // Turn on LoRa CS
    digitalWrite(RFM95_CS, LOW);

    //*/

    package_t package_new;

    // Send all data from EEPROM (TODO: Condense packages into minimal # of TX)
    for(int i=0; i < page_num; i++){    
        //delay(300);
        get_package(&package_new, i);

        char* package_bytes = reinterpret_cast<char*>(&package_new);
        rf95.send((uint8_t *)package_bytes, PACKAGE_SIZE);
        rf95.waitPacketSent();
    }
    // Reset number of packages in EEPROM
    page_num = 0;

    // Send stopper signifier
    uint8_t data [] = "END OF DATA";

    // Get current time and convert to bytes
    //unsigned long cur_time = millis();
    //char* time_bytes = reinterpret_cast<char*>(cur_time);

    // add to end signifier 
    //strcat(data, time_bytes);

    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();

    // Serial.println("Done Sending Packages");
    
    rf95.setModeRx();

    //*
    // Turn off LoRa CS
    digitalWrite(RFM95_CS, HIGH);
    
    // Enable Slave select for ATTiny
    digitalWrite(7, LOW);

    // End ATTiny sample power
    SPI.transfer(0x02);
    SPI.transfer(0xFF);
    if(SPDR != 0x02){
      while(1);
    }
    
    // Send data in data array
    while(true){
      // Send data read and buffer
      SPI.transfer(0x03);
      SPI.transfer(0xFF);

      // Read value
      uint8_t analogValueHigh = SPDR;

      // Send data read and buffer
      SPI.transfer(0x03);
      SPI.transfer(0xFF);

      uint8_t analogValueLow = SPDR;

      uint16_t analogValue = (analogValueHigh & 3) << 8 | analogValueLow;

      uint8_t time = analogValueHigh >> 2; 
      
      // If stop signifier from slave, break out 
      if(analogValueLow == 0x00)
        break;
      
      float current = (analogValue * 3.3) / (33792.0 + (1.5 * analogValue));
      Serial.print("Analog value: ");
      Serial.print(analogValue);
      Serial.print(", Current: ");
      Serial.print(current);
      Serial.print(" A");
      Serial.print(", Time: ");
      Serial.print(time);
      Serial.println(" ms");
    }
    Serial.println("========= Finished =========");

    // Disable Slave select for ATTiny
    digitalWrite(7, HIGH);

    // Turn on LoRa CS
    digitalWrite(RFM95_CS, LOW);

    //*/
  }
  delay(3000);
}