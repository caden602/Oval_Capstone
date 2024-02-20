#include <SPI.h>
#include <RH_RF95.h>
#include "Sensors/TMP117.h"
#include "Sensors/BME688.h"
#include "Sensors/ADXL313.h"
#include "Sensors/LIS3MDL.h"
#include "Sensors/MMC5603.h"

#define RFM95_CS    4
#define RFM95_INT   3
#define RFM95_RST   2  

#define RF95_FREQ 915.0

#define LUNA_SAT_ID 1


typedef struct{
    bme_data_t bme_data;                        // 17 bytes
    adxl_data_t adxl_data;                      // 7  bytes
    lis3mdl_data_t lis3mdl_data;
}package_t;                                     // total = 28 bytes