#include <LunaSat.h>

#define RFM95_CS    4
#define RFM95_INT   3
#define RFM95_RST   2  

#define ATTINY_CS   6

#define ADXL_SAMPLE_RATE  1
#define LIS_SAMPLE_RATE   1

// Define instance of LoRa (Pass in 'true' for use with LunaSat interrupts)
RH_RF95 rf95(RFM95_CS, RFM95_INT, true);

// Define instances of all Sensors
Adafruit_BME680 bme; 
ADXL313 adxl;
Adafruit_LIS3MDL lis;

void setup() {

  Watchdog.enable(8000);
  Serial.begin(115200); 
  while (!Serial) delay(1);
  delay(50);



  // Setup ATTiny and disable CS
  pinMode(ATTINY_CS, OUTPUT);
  digitalWrite(ATTINY_CS, HIGH);
  pinMode(8, OUTPUT);

  /* Sensor Setup */
  // Serial.println("Begin Sensor Setup");

  /* For LIS3MDL, you only need to set it up once. If you try to set it up again 
     without fulling cutting power to the sensor, you will receive a setup failure. */


  lis_setup(&lis);
  lis_set_data_rate(&lis, LIS_SAMPLE_RATE);

  Watchdog.reset();
  
  bme_setup(&bme);

  Watchdog.reset();
  
  adxl_setup(&adxl);
  adxl_set_data_rate(&adxl, ADXL_SAMPLE_RATE);

  Watchdog.reset();


  /* LoRa Setup */
  lora_setup(&rf95, RFM95_RST, true);

  SPI.setClockDivider(SPI_CLOCK_DIV8); 

  /* Scheduler Setup */
  scheduler_open();

  eeprom_map_pages(1, ADXL_SAMPLE_RATE, LIS_SAMPLE_RATE);

  Serial.print("Setup Good for #");
  Serial.println(LUNA_SAT_ID);

  Watchdog.disable();

  delay(5000);
}

void loop() {

  Watchdog.enable(8000);
  
  // Sample all sensors
  bme_sample_data(&bme);
  adxl_sample_data(&adxl);
  lis_sample_data(&lis);

  Watchdog.disable();

  // Check if we recieved a data request event
  if(get_scheduled_events() & EVENT_DATA_REQUEST){
    int countdownMS = Watchdog.enable(15000);
    Serial.print("Enabled the watchdog with max countdown of ");
    Serial.print(countdownMS, DEC);

    Serial.println("Rmv Event");
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
      // while(1); 
    }
   
    Serial.println("=== Started collection ==");
    
    // Disable Slave select for ATTiny
    digitalWrite(ATTINY_CS, HIGH);

    // Turn on LoRa CS
    digitalWrite(RFM95_CS, LOW);
    delay(50);



    // send_data(&rf95);

    /*
  
    //Serial.println("Send Header");
    d_header_pack_t header;
    header.bme_n = get_and_set_bme_count();
    // Serial.print("BME Count: ");
    // Serial.println(header.bme_n);
    header.lis_n = get_and_set_lis_count();
    header.adxl_n = get_and_set_adxl_count();
    header.local_time_stamp = millis();

    char* header_bytes = reinterpret_cast<char*>(&header);
    rf95.send((uint8_t *)header_bytes, sizeof(d_header_pack_t));
    rf95.waitPacketSent();

    delay(50);

    uint8_t samples_per_page = 128 / sizeof(d_bme_pack_t);
    uint8_t pages = header.bme_n / samples_per_page;
    uint8_t remainder = header.bme_n % samples_per_page;

    for(int i=0; i < pages; i++){
      
      // array size of 20 * 6 = 120
      uint8_t num_bytes = sizeof(d_bme_pack_t) * samples_per_page;
      uint8_t bytes[num_bytes];

      // Only start at i for BME
      uint16_t address = eeprom_get_address(i, 0);

      // Get bytes from address in eeprom and store in bytes array
      eeprom_get_bytes(bytes, num_bytes, address);

      delay(100);

      // send the bytes via LoRa
      rf95.send((uint8_t *)bytes, num_bytes);
      rf95.waitPacketSent();

    }

    uint8_t size = remainder*sizeof(d_bme_pack_t);

    uint8_t bytes[size];
    uint16_t address = eeprom_get_address(pages, 0);

    Serial.print("size: ");
    Serial.println(size);

    // Get bytes from address in eeprom and store in bytes array
    eeprom_get_bytes(bytes, size, address);

    eeprom_reset();

    // send the bytes via LoRa
    rf95.send((uint8_t *)bytes, size);
    rf95.waitPacketSent();

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
    //*/

    send_data(&rf95);

    Serial.println("HERE");

    Watchdog.disable();

    Serial.println("done disbale");
    /*

    // Turn off LoRa CS
    digitalWrite(RFM95_CS, HIGH);
    delay(50);
    // Enable Slave select for ATTiny
    digitalWrite(ATTINY_CS, LOW);
    delay(50);

    // End ATTiny sample power
    SPI.transfer(0x02);
    delay(50);
    SPI.transfer(0xFF);
    delay(50);
    uint8_t resp = SPDR;
    if(resp != 0x04){
      Serial.println(resp);
      Serial.println("STUCK HERE 2");
      // while(1);

      SPI.transfer(0x02);
      delay(50);
      SPI.transfer(0xFF);
      delay(50);
      uint8_t resp2 = SPDR;
      if(resp2 != 0x04){
        Serial.println(resp2);
        Serial.println("STUCK HERE 3");
        // while(1);
      }
    }
    
    int iter = 0;
    int time_step = 0;
    int prev_time = 0;
    uint16_t sum = 0;

    Serial.println("=== Started data retreival ===");
    while (true){
      iter ++;
      SPI.transfer(0x03);
      delay(50);
      SPI.transfer(0xFF);
      delay(50);
      uint8_t highByte = SPDR;
      // delay(50);
      SPI.transfer(0x03);
      delay(50);
      SPI.transfer(0xFF);
      delay(50);
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

      sum += analogValue;
      
      if (iter > 100)
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
    uint16_t avg_analogVal = sum / iter;

    float avg = 1000* (3.3 * avg_analogVal)/(1.5 * 20 * 1024 + 1.5 * avg_analogVal);

    Serial.print("Collected ");
    Serial.print(iter);
    Serial.print(" data points, average: ");
    Serial.println(avg);
    Serial.println("=== Finished ==");
    
    delay(500);
    // Disable Slave select for ATTiny
    digitalWrite(ATTINY_CS, HIGH);

    // Turn on LoRa CS
    digitalWrite(RFM95_CS, LOW);
    delay(500);

    //*/
    rf95.setModeRx();
    delay(500);
  }
}