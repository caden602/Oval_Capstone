#include <LunaSat.h>

#define RFM95_CS    4
#define RFM95_INT   3
#define RFM95_RST   2  


// Define instance of LoRa (For use with LunaSat interrupts)
RH_RF95 rf95(RFM95_CS, RFM95_INT, true);

// Define instances of all Sensors
Adafruit_BME680 bme; 
ADXL313 adxl;
Adafruit_LIS3MDL lis3mdl;

static package_t packages[3];
static int num_pack = 0;

void setup() {

  Serial.begin(115200); 
  while (!Serial) delay(1);
  delay(100);

  /* Sensor Setup */
  Serial.println("Begin Sensor Setup");

  /* For LIS3MDL, you only need to set it up once. If you try to set it up again 
     without fulling cutting power to the sensor, you will receive a setup failure. */
  //lis3mdl_setup(&lis3mdl);
  //delay(1000);
  
  adxl_setup(&adxl);
  delay(2000);

  bme_setup(&bme);
  delay(1000);

  Serial.println("All Sensors Good!");

  delay(1000);
  // Initalize LoRa
  lora_setup(&rf95, RFM95_RST, true);
  rf95.setModeRx();

  // Initialize scheduler
  scheduler_open();

  delay(5000);
}

void loop() {
  package_t package;

  // Get sensor data
  Serial.println("Sampling BME688");
  bme_data_t bme_data;
  bool bme_error = bme_get_data(&bme, &bme_data);
  package.bme_data = bme_data;
  package.bme_error = bme_error;

  Serial.println("Sampling ADXL313");
  adxl_data_t adxl_data;
  bool adxl_error = adxl_get_data(&adxl, &adxl_data);
  package.adxl_data = adxl_data;
  package.adxl_error = adxl_error;

  Serial.println("Sampling LIS3MDL");
  lis3mdl_data_t lis3mdl_data;
  bool lis3mdl_error = lis3mdl_get_data(&lis3mdl, &lis3mdl_data);
  package.lis3mdl_data = lis3mdl_data;
  package.lis3mdl_error = lis3mdl_error;

  // TODO:
  // Add in timestampe here and apply to package.

  // Print contents of pacakge
  // print_package(&package);

  if(num_pack < 3){
    packages[num_pack] = package;
    num_pack ++;
  }
  else{
    // if our packages array is filled up...??
    Serial.println("Packages array full!");
  }

  // Check if we recieved a data request event
  if(get_scheduled_events() & EVENT_DATA_REQUEST){
    remove_scheduled_event(EVENT_DATA_REQUEST);

    // Send all data in packages array (TODO: Condense packages into minimal # of TX)
    for(int i=0; i < num_pack; i++){
        char* package_bytes = reinterpret_cast<char*>(&packages[i]);
        rf95.send((uint8_t *)package_bytes, PACKAGE_SIZE);
        rf95.waitPacketSent();
    }
    // Reset number of packages in array
    num_pack = 0;

    // Send stopper signifier
    uint8_t data[] = "END OF DATA";
    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();

    Serial.println("DONE TRANSMITTING");
    
    rf95.setModeRx();
  }
  // delay
  delay(4000);
}


void print_package(package_t *package){
    Serial.println("BME DATA: ");
    Serial.print("Humidity: "); Serial.println(package->bme_data.humidity);
    Serial.print("Gas Resistance: "); Serial.println(package->bme_data.gas_resistance);
    Serial.print("Temperature: "); Serial.println(package->bme_data.temperature);
    Serial.print("Pressure: "); Serial.println(package->bme_data.pressure);

    Serial.println();

    Serial.println("ADXL DATA: ");
    Serial.print("x: "); Serial.print(package->adxl_data.x); Serial.print(", ");
    Serial.print("y: "); Serial.print(package->adxl_data.y); Serial.print(", ");
    Serial.print("z: "); Serial.print(package->adxl_data.z); Serial.println();

    Serial.println();

    Serial.println("LIS3MDL DATA: ");
    Serial.print("x: "); Serial.print(package->lis3mdl_data.x); Serial.print(", ");
    Serial.print("y: "); Serial.print(package->lis3mdl_data.y); Serial.print(", ");
    Serial.print("z: "); Serial.print(package->lis3mdl_data.z); Serial.println();

    Serial.println();

}