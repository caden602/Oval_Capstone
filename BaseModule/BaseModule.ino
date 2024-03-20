#include <LunaSat.h>

// Unique identifiers for Luna Sats
#define LUNA_SAT_1  1
#define LUNA_SAT_2  2

// Pins for Feather M0
#define RFM95_CS    8
#define RFM95_INT   3
#define RFM95_RST   4

// Define instance of LoRa (Pass in 'false' for use with BaseStation interrupts)
RH_RF95 rf95(RFM95_CS, RFM95_INT, false);

uint8_t lunaSatIDs[2] = {LUNA_SAT_1, LUNA_SAT_2};

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
  // Serial.println();
  // Serial.println("--------------------------------");


  // Switch LunaSat to request from
  if (lunaSatNum == 1){
    lunaSatNum = 0;
  }
  else{
    lunaSatNum = 1;
  }

  // Serial.print("Requesting Data From LunaSat #"); // Send a message to rf95_server
  // Serial.println(lunaSatIDs[lunaSatNum]); // Luna Sat #1

  char radiopacket[13] = "LunaSat #   ";
  itoa(lunaSatIDs[lunaSatNum], radiopacket+9, 3);
  //Serial.print("Sending "); Serial.println(radiopacket);

  delay(10);
  rf95.send((uint8_t *)radiopacket, 20);

  //Serial.println("Waiting for packet to complete...");
  delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; // = {NULL};
  uint8_t len = sizeof(buf);

  //Serial.println("Waiting for data...");
  if (rf95.waitAvailableTimeout(4000)) {
    // Should be a reply message for us now
    bool done = false;
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
            print_package_for_serial(&package);
          }
        }
        // Serial.print("RSSI: ");
        // Serial.println(rf95.lastRssi(), DEC);
        for(int i=0; i < 37; i++){
          buf[i] = 0;
        }
      }

    }
    rf95.setModeIdle();
  }
  else {
    // Serial.println("No data received");
  }
  // Serial.println("--------------------------------");
  // Serial.println();
}

//  0 m / -11
// 10 m / -55
// 20 m / -70
// 30 m / -76
// 40 m / -82
// 50 m / -83
// 60 m / -86
// 70 m / -90