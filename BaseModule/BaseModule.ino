#include <LunaSat.h>

// Unique identifiers for Luna Sats
#define NUM_OF_LUNASATS 4

// Pins for Feather M0
#define RFM95_CS    8
#define RFM95_INT   3
#define RFM95_RST   4


// Pins for LunaSat
// #define RFM95_CS    4
// #define RFM95_INT   3
// #define RFM95_RST   2  

// Define instance of LoRa (Pass in 'false' for use with BaseStation interrupts)
RH_RF95 rf95(RFM95_CS, RFM95_INT, false);

uint8_t lunaSatNum = 1;  // Keeps track of Luna Sat Num to request data from


void setup() {
  Serial.begin(115200);
  while (!Serial) delay(1);
  delay(100);

  // Setup LoRa
  lora_setup(&rf95, RFM95_RST, false);
}


void tx_confirmation(RH_RF95* rf95){
  char confirm_packet[18] = "CONFIMRATION #   ";
  
  itoa(lunaSatNum, confirm_packet+14, 10);

  // Serial.println(confirm_packet);

  delay(10);
  rf95->send((uint8_t *)confirm_packet, 20);
  rf95->waitPacketSent();

  rf95->setModeRx();
}

void rx_data(RH_RF95* rf95, d_header_pack_t header, sensor_choice_t sens){

  // Get number of samples and size of packages
  uint8_t size;
  uint16_t samples;

  switch(sens){
    case bme_t:
      size = BME_PACK_SIZE;
      samples = header.bme_n;
      break;
    case adxl_t:
      size = ADXL_PACK_SIZE;
      samples = header.adxl_n;
      break;
    case lis_t:
      size = LIS_PACK_SIZE;
      samples = header.lis_n;
      break;
  }

  // If no samples, return
  if(samples == 0){
    return;
  }

  // Get time offset
  unsigned long time_offset = millis() - header.local_time_stamp;
  if(time_offset < 0)
  {
    time_offset = 0;
  }

  uint8_t samples_per_page = 128 / size;
  uint8_t pages = samples / samples_per_page;
  uint8_t remainder = samples % samples_per_page;

  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = 250;

  bool done = false;
  unsigned long curr_time = millis();

  int tx_count = 0;

  while(!done){
    if (rf95->recv(buf, &len)) {

      // Set current time of rx a transmission
      curr_time = millis();

      // Check signal strength
      if (rf95->lastRssi() < -800){
        //Serial.println("Data Invalid");
      }
      else{
        tx_count ++;

        // Check if we are at the end of pages
        if(tx_count == pages + 1){

          // Get the remaining bytes specified by remainder
          d_bme_pack_t bme_pack;
          d_adxl_pack_t adxl_pack;
          d_lis_pack_t lis_pack;
          for(int i=0; i < remainder; i++){
          
            // get bytes and adjust time stamp
            switch(sens){
              case bme_t:
                bytes_to_bme_data(buf + (i * size), &bme_pack);
                bme_pack.time_stamp += time_offset;
                print_bme_for_serial(&bme_pack, lunaSatNum);
                break;
              case adxl_t:
                bytes_to_adxl_data(buf + (i * size), &adxl_pack);
                adxl_pack.time_stamp += time_offset;
                print_adxl_for_serial(&adxl_pack, lunaSatNum);
                break;
              case lis_t:
                bytes_to_lis_data(buf + (i * size), &lis_pack);
                lis_pack.time_stamp += time_offset;
                print_lis_for_serial(&lis_pack, lunaSatNum);
                break;
            }

          }
          // rf95->printBuffer("buf: ", buf, 120);
          // Serial.println("DONE!");
          done = true;
        }

        else
        {

          d_bme_pack_t bme_pack;
          d_adxl_pack_t adxl_pack;
          d_lis_pack_t lis_pack;
          
          // iterate through samples in page
          for(int j=0; j < samples_per_page; j++)
          {

            // get bytes and adjust time stamp
            switch(sens){
              case bme_t:
                bytes_to_bme_data(buf + (j * size), &bme_pack);
                bme_pack.time_stamp += time_offset;
                print_bme_for_serial(&bme_pack, lunaSatNum);
                break;
              case adxl_t:
                bytes_to_adxl_data(buf + (j * size), &adxl_pack);
                adxl_pack.time_stamp += time_offset;
                print_adxl_for_serial(&adxl_pack, lunaSatNum);
                break;
              case lis_t:
                bytes_to_lis_data(buf + (j * size), &lis_pack);
                lis_pack.time_stamp += time_offset;
                print_lis_for_serial(&lis_pack, lunaSatNum);
                break;
              }
          }
          // rf95->printBuffer("buf: ", buf, 120);
          // Send all data recieved confirmation
          tx_confirmation(rf95);
        }
      }
    }
    else{
      // Get current time
      unsigned long time = millis();

      // Check if its been over 2 seconds since last transmission
      if(time - curr_time > 2000){
        // Serial.println("Max Time");
        break;
      }
    }
  }

  // Send all data recieved confirmation
  tx_confirmation(rf95);

}

void loop() {
  delay(5000); // Wait 1 second between transmits, could also 'sleep' here!

  // Print header
  // Serial.println();
  // Serial.println("--------------------------------");

  // Serial.print("Requesting Data From LunaSat #");
  // Serial.println(lunaSatNum); // Luna Sat #1

  char request_packet[13] = "LunaSat #   ";
  itoa(lunaSatNum, request_packet+9, 10);
  //Serial.print("Sending "); Serial.println(radiopacket);

  delay(10);
  rf95.send((uint8_t *)request_packet, 20);

  //Serial.println("Waiting for packet to complete...");
  delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; // = {NULL};
  uint8_t len = sizeof(buf);

  // Wait 4 seconds for data reply
  if (rf95.waitAvailableTimeout(5000)) {
    // Should be a reply message for us now

    int time_offset = 0;
    d_header_pack_t header;

    // Get header
    if (rf95.recv(buf, &len))
    {
      bytes_to_header(buf, &header);
      time_offset = millis() - header.local_time_stamp;
      if(time_offset < 0)
      {
        time_offset = 0;
      }

      // Serial.print("Header data: "); Serial.print(header.bme_n);
      // Serial.print(' '); Serial.print(header.adxl_n); 
      // Serial.print(' '); Serial.println(header.lis_n );
    }

    rx_data(&rf95, header, bme_t);
    rx_data(&rf95, header, adxl_t);
    rx_data(&rf95, header, lis_t);

    rf95.setModeIdle();

    /*
    uint8_t samples_per_page = 128 / sizeof(d_bme_pack_t);
    uint8_t pages = header.bme_n / samples_per_page;
    uint8_t remainder = header.bme_n % samples_per_page;

    len = 250;

    bool done = false;
    unsigned long curr_time = millis();

    int tx_count = 0;

    while(!done){
      if (rf95.recv(buf, &len)) {
        if (rf95.lastRssi() < -800){
          //Serial.println("Data Invalid");
        }
        else{
          //RH_RF95::printBuffer("Received: ", buf, 33);
          // char* end_data = "END OF DATA";
          // char* RX = (char*)buf;
          // if(!strcmp(RX, end_data)){
          //   done = true;
          //   //Serial.println("TEST");
          //   // Get current time
          //   // We use this time to normalize the time sent in the final 'END OF DATA' package
          // }
          tx_count ++;

          // Check if we are at the end of pages
          if(tx_count == pages + 1){

            // Get the remaining bytes specified by offset
            d_bme_pack_t bme_pack;
            for(int i=0; i < remainder; i++){
              // get bytes (+ offest using j) and store in bme_pack
              bytes_to_bme_data(buf + (i * sizeof(d_bme_pack_t)), &bme_pack);

              // Adjust timestamp
              bme_pack.time_stamp += time_offset;

              // Print data
              Serial.print("BME Data: Time = ");
              Serial.print(bme_pack.time_stamp);
              Serial.print(" , Temp = : ");
              Serial.println(bme_pack.bme_data.temperature);
            }
            rf95.printBuffer("buf: ", buf, 120);
            Serial.println("DONE!");
            done = true;
          }

          else{

            d_bme_pack_t bme_pack;
            
            // iterate through samples in page
            for(int j=0; j < samples_per_page; j++){

              // get bytes (+ offest using j) and store in bme_pack
              bytes_to_bme_data(buf + (j * sizeof(d_bme_pack_t)), &bme_pack);

              // Adjust timestamp
              bme_pack.time_stamp += time_offset;

              // Print data
              Serial.print("BME Data: Time = ");
              Serial.print(bme_pack.time_stamp);
              Serial.print(" , Temp = : ");
              Serial.println(bme_pack.bme_data.temperature);
            }
          rf95.printBuffer("buf: ", buf, 120);
          }
        }
      }

      else{
        unsigned long time = millis();
        if(time - curr_time > 4000){
          break;
        }
      }
    }

    */
    rf95.setModeIdle();
  }
  else {
    // Serial.println("No data received");
  }
  // Serial.println("--------------------------------");

  // Iterate LunaSat to request from
  if(lunaSatNum < NUM_OF_LUNASATS){
    lunaSatNum ++;
  }
  else{
    lunaSatNum = 1;
  }

  // Serial.println(lunaSatNum);

}
