#include <LunaSat.h>

#define RFM95_CS    4
#define RFM95_INT   3
#define RFM95_RST   2  

#define ATTINY_CS   6

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

int bme_page_start;
int adxl_page_start;
int lis_page_start;

void setup() {
  Serial.begin(115200); 
  while (!Serial) delay(1);
  delay(100);


  // Setup ATTiny and disable CS
  pinMode(ATTINY_CS, OUTPUT);
  digitalWrite(ATTINY_CS, HIGH);

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

  SPI.setClockDivider(SPI_CLOCK_DIV8);//divide the clock by 8

  /* Scheduler Setup */
  scheduler_open();

  delay(5000);
}

void loop() {

  // Sample all sensors
  sample_data(&package, &bme, &adxl, &lis3mdl);

  // TODO:
  // Add in timestamp here and apply to package.
  package.time_stamp = millis();

  // Print contents of pacakge
  // print_package_for_serial(&package);

  // Store Package in EEPORM if we can fit it
  if(page_num < 512){
    store_package(&package, page_num);
    Serial.print("Storing package on page ");
    Serial.println(page_num);
    print_package_for_serial(&package);
    page_num++;
  }
  else{
    // reset page_num and start overwriting existing measurments
    // page_num = 0;

    // For now, let's not overwrite existing data.
  }


  // Check if we recieved a data request event
  if(get_scheduled_events() & EVENT_DATA_REQUEST){
    // Serial.println("RX Data Req");
    remove_scheduled_event(EVENT_DATA_REQUEST);
    // Serial.print("Sending "); Serial.print(num_pack); Serial.println(" packages");

    /*

    // Turn off LoRa CS
    digitalWrite(RFM95_CS, HIGH);
    delay(500);
    
    // Enable Slave select for ATTiny
    digitalWrite(ATTINY_CS, LOW);
    delay(500);

    // Begin ATTiny sample power
    SPI.transfer(0x01);
    delay(50);
    SPI.transfer(0xFF);
    delay(50);
    if(SPDR != 0x03){
      Serial.println("STUCK HERE 1");
      while(1); 
    }
   
    Serial.println("========= Started collection =========");
    
    // Disable Slave select for ATTiny
    digitalWrite(ATTINY_CS, HIGH);

    // Turn on LoRa CS
    digitalWrite(RFM95_CS, LOW);
    delay(50);

    //*/


    

    Serial.println("Send Header");
    package_header_t header;
    header.num_packages = page_num;
    header.current_time = millis();

    char* header_bytes = reinterpret_cast<char*>(&header);
    rf95.send((uint8_t *)header_bytes, 6);
    rf95.waitPacketSent();

    delay(50);

    Serial.println("Send Data");
    package_t package_new;

    // Send all data from EEPROM (TODO: Condense packages into minimal # of TX)
    for(int i=0; i < page_num; i++){    
        // delay(300);
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

    /*

    // Turn off LoRa CS
    digitalWrite(RFM95_CS, HIGH);
    
    // Enable Slave select for ATTiny
    digitalWrite(ATTINY_CS, LOW);
    delay(50);

    // End ATTiny sample power
    SPI.transfer(0x02);
    delay(50);
    SPI.transfer(0xFF);
    delay(50);
    if(SPDR != 0x04){
      Serial.println("STUCK HERE 2");
      while(1);
    }
    
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

      if(lowByte == 0)
        break;

      float current = 1000* (3.3 * analogValue)/(1.5 * 22 * 1024 + 1.5 * analogValue);
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

    // Disable Slave select for ATTiny
    digitalWrite(ATTINY_CS, HIGH);

    // Turn on LoRa CS
    digitalWrite(RFM95_CS, LOW);

    //*/
  }
  delay(3000);
}