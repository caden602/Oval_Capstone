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

    // Get header
    if (rf95.recv(buf, &len))
    {
      package_header_t header;
      bytes_to_header(&header, buf);
      time_offset = millis() - header.current_time;
      if(time_offset < 0)
      {
        time_offset = 0;
      }

      // Serial.println(header.num_packages);
      // Serial.println(header.current_time);
    }

    len = 250;

    bool done = false;
    unsigned long curr_time = millis();

    while(!done){
      if (rf95.recv(buf, &len)) {
        if (rf95.lastRssi() < -800){
          //Serial.println("Data Invalid");
        }
        else{
          //RH_RF95::printBuffer("Received: ", buf, 33);
          char* end_data = "END OF DATA";
          char* RX = (char*)buf;
          if(!strcmp(RX, end_data)){
            done = true;
            //Serial.println("TEST");
            // Get current time
            // We use this time to normalize the time sent in the final 'END OF DATA' package
          }
          else{
            package_t package;
            bytes_to_package(&package, buf);

            //Serial.println(package.time_stamp);
            package.time_stamp += time_offset;
            print_package_for_serial(&package);
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

//  0 m / -11
// 10 m / -55
// 20 m / -70
// 30 m / -76
// 40 m / -82
// 50 m / -83
// 60 m / -86
// 70 m / -90