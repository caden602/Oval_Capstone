#include <LunaSat.h>

// Unique identifiers for Luna Sats
#define NUM_OF_LUNASATS 2

// Pins for Feather M0
#define RFM95_CS    8
#define RFM95_INT   3
#define RFM95_RST   4

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


void rx_data(RH_RF95* rf95, d_header_pack_t header, sensor_choice_t sens){

  uint8_t size;
  uint16_t samples;
  switch(sens){
    case bme_t:
      size = sizeof(d_bme_pack_t);
      samples = header.bme_n;
      // Serial.println("TEST HERE");
      break;
    case adxl_t:
      size = sizeof(d_adxl_pack_t);
      samples = header.adxl_n;
      break;
    case lis_t:
      size = sizeof(d_lis_pack_t);
      samples = header.lis_n;
      break;
  }

  // Serial.print("SIZE: ");
  // Serial.println(size);

  unsigned long time_offset = millis() - header.local_time_stamp;
  if(time_offset < 0)
  {
    time_offset = 0;
  }

  uint8_t samples_per_page = 128 / size;
  uint8_t pages = samples / samples_per_page;
  uint8_t remainder = samples % samples_per_page;

  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; // = {NULL};
  uint8_t len = 250;

  bool done = false;
  unsigned long curr_time = millis();

  int tx_count = 0;

  while(!done){
    if (rf95->recv(buf, &len)) {
      if (rf95->lastRssi() < -800){
        //Serial.println("Data Invalid");
      }
      else{
        tx_count ++;

        // Check if we are at the end of pages
        // pages = 1--> 1 + 1 == 2
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
                print_bme_for_serial(&bme_pack);
                break;
              case adxl_t:
                bytes_to_adxl_data(buf + (i * size), &adxl_pack);
                adxl_pack.time_stamp += time_offset;
                print_adxl_for_serial(&adxl_pack);
                break;
              case lis_t:
                bytes_to_lis_data(buf + (i * size), &lis_pack);
                lis_pack.time_stamp += time_offset;
                print_lis_for_serial(&lis_pack);
                break;
            }

            // Print data
            // Serial.print("BME Data: Time = ");
            // Serial.print(bme_pack.time_stamp);
            // Serial.print(" , Temp = : ");
            // Serial.println(bme_pack.bme_data.temperature);

          }
          // rf95->printBuffer("buf: ", buf, 120);
          Serial.println("DONE!");
          done = true;
        }

        else{

          d_bme_pack_t bme_pack;
          d_adxl_pack_t adxl_pack;
          d_lis_pack_t lis_pack;
          
          // iterate through samples in page
          for(int j=0; j < samples_per_page; j++){

            // get bytes and adjust time stamp
            switch(sens){
              case bme_t:
                bytes_to_bme_data(buf + (j * size), &bme_pack);
                bme_pack.time_stamp += time_offset;
                print_bme_for_serial(&bme_pack);
                break;
              case adxl_t:
                bytes_to_adxl_data(buf + (j * size), &adxl_pack);
                adxl_pack.time_stamp += time_offset;
                print_adxl_for_serial(&adxl_pack);
                break;
              case lis_t:
                bytes_to_lis_data(buf + (j * size), &lis_pack);
                lis_pack.time_stamp += time_offset;
                print_lis_for_serial(&lis_pack);
                break;
            }

            // Print data
            // Serial.print("BME Data: Time = ");
            // Serial.print(bme_pack.time_stamp);
            // Serial.print(" , Temp = : ");
            // Serial.println(bme_pack.bme_data.temperature);
          }
        // rf95->printBuffer("buf: ", buf, 120);
        }
      }
    }
    else{
      unsigned long time = millis();
      if(time - curr_time > 1000){
        break;
      }
    }
  }
}

void loop() {
  delay(5000); // Wait 1 second between transmits, could also 'sleep' here!

  // Print header
  Serial.println();
  Serial.println("--------------------------------");

  Serial.print("Requesting Data From LunaSat #");
  Serial.println(lunaSatNum); // Luna Sat #1

  char request_packet[13] = "LunaSat #   ";
  itoa(lunaSatNum, request_packet+9, 3);
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
  if (rf95.waitAvailableTimeout(4000)) {
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

      Serial.print("Recieved header w/ ");
      Serial.print(header.bme_n);
      Serial.println(" bme samples.");
    }

    rx_data(&rf95, header, bme_t);
    rx_data(&rf95, header, adxl_t);
    rx_data(&rf95, header, lis_t);


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
    Serial.println("No data received");
  }
  Serial.println("--------------------------------");

  // Iterate LunaSat to request from
  if(lunaSatNum < NUM_OF_LUNASATS){
    lunaSatNum ++;
  }
  else{
    lunaSatNum = 1;
  }

}
