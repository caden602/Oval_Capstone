#include <SPI.h>
#include <RH_RF95.h>


#include "LunaSat.h"
// Current Base Module uses the Feather M0
// 1) Request Data from specific LunaSat using unique identifier
// 2) Print out the data received


// Unique identifiers for Luna Sats
#define LUNA_SAT_1  1
#define LUNA_SAT_2  2

// Pins for Feather M0
#define RFM95_CS    8
#define RFM95_INT   3
#define RFM95_RST   4

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

uint8_t lunaSatIDs[2] = {LUNA_SAT_1, LUNA_SAT_2};



void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);
  while (!Serial) delay(1);
  delay(100);

  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa initialization failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio initalized");

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
}

uint8_t lunaSatNum = 1;  // Keeps track of Luna Sat Num to request data from

void loop() {
  delay(1000); // Wait 1 second between transmits, could also 'sleep' here!

  // Print header
  Serial.println();
  Serial.println("--------------------------------");


   // Switch LunaSat to request from
  if (lunaSatNum == 1){
    lunaSatNum = 0;
  }
  else{
    lunaSatNum = 1;
  }

  Serial.print("Requesting Data From LunaSat #"); // Send a message to rf95_server
  Serial.println(lunaSatIDs[lunaSatNum]); // Luna Sat #1

  char radiopacket[13] = "LunaSat #   ";
  itoa(lunaSatIDs[lunaSatNum], radiopacket+9, 3);
  //Serial.print("Sending "); Serial.println(radiopacket);

  delay(10);
  rf95.send((uint8_t *)radiopacket, 20);

  //Serial.println("Waiting for packet to complete...");
  delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  //Serial.println("Waiting for data...");
  if (rf95.waitAvailableTimeout(1000)) {
    // Should be a reply message for us now
    if (rf95.recv(buf, &len)) {
      if (rf95.lastRssi() < -800){
        Serial.println("Data Invalid");
      }
      else{
        //Serial.print("RX data: ");
        //Serial.println((char*)buf);

        RH_RF95::printBuffer("Received: ", buf, 28);
        //Serial.println(buf[2], HEX);

        package_t package = bytes_to_package(buf);
        Serial.println(package.temp_data);
      }
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
    }
    else {
      Serial.println("Receive failed");
    }
  } else {
    Serial.println("No data received");
  }
  Serial.println("--------------------------------");
  Serial.println();
}


package_t bytes_to_package(uint8_t* buf){
    package_t package;

    uint32_t temp = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];
    float temp_data = *reinterpret_cast<float*>(&temp);
    package.temp_data = temp_data;
    
    return package;
}

//  0 m / -11
// 10 m / -55
// 20 m / -70
// 30 m / -76
// 40 m / -82
// 50 m / -83
// 60 m / -86
// 70 m / -90