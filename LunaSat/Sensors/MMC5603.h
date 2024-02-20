#include <Adafruit_MMC56x3.h>


typedef struct{
    float x;
    float y;
    float z;
} mmc_data_t;

void mmc_setup(Adafruit_MMC5603* mmc) {
  /* Initialise the sensor */
  if (!mmc->begin(MMC56X3_DEFAULT_ADDRESS, &Wire)) {  // I2C mode
    /* There was a problem detecting the MMC5603 ... check your connections */
    Serial.println("Failed to find MMC5603");
    while (1) delay(10);
  }


  Serial.println("Found MMC5603");

  /* Display some basic information on this sensor */
  //mmc->printSensorDetails();
}

void mmc_get_data(Adafruit_MMC5603* mmc) {
  // Get a new sensor event 
  sensors_event_t event;
  mmc->getEvent(&event);

  // Display the results (magnetic vector values are in micro-Tesla (uT))
  Serial.print("X: ");
  Serial.print(event.magnetic.x);
  Serial.print("  ");
  Serial.print("Y: ");
  Serial.print(event.magnetic.y);
  Serial.print("  ");
  Serial.print("Z: ");
  Serial.print(event.magnetic.z);
  Serial.print("  ");
  Serial.println("uT");


  // TODO: FIGURE OUT WHAT MAG SENSOR DATA WE WANT
  // the event provided is 36 bytes (might be overkill size wise)

  // Delay before the next sample
  delay(100);
}