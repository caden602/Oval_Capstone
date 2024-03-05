/******************************************************************************
  Example1_BasicReadings.ino
  Read values of x/y/z axis of the ADXL313 (via I2C), print them to terminal.
  This uses default configuration (1G range, full resolution, 100Hz datarate).

  SparkFun ADXL313 Arduino Library
  Pete Lewis @ SparkFun Electronics
  Original Creation Date: September 19, 2020
  https://github.com/sparkfun/SparkFun_ADXL313_Arduino_Library

  Do you like this library? Help support SparkFun. Buy a board!

    SparkFun 3-Axis Digital Accelerometer Breakout - ADXL313 (Qwiic)
    https://www.sparkfun.com/products/17241  

  Development environment specifics:

	IDE: Arduino 1.8.13
	Hardware Platform: SparkFun Redboard Qwiic
	SparkFun 3-Axis Digital Accelerometer Breakout - ADXL313 (Qwiic) Version: 1.0

  Hardware Connections:
  Use a qwiic cable to connect from the Redboard Qwiic to the ADXL313 breakout (QWIIC).
  You can also choose to wire up the connections using the header pins like so:

  ARDUINO --> ADXL313
  SDA (A4) --> SDA
  SCL (A5) --> SCL
  3.3V --> 3.3V
  GND --> GND

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <Wire.h>
#include <SparkFunADXL313.h> //Click here to get the library: http://librarymanager/All#SparkFun_ADXL313

// Define enum for sample rates
enum SampleRate {
    SAMPLE_RATE_1600 = ADXL313_BW_1600,
    SAMPLE_RATE_800 = ADXL313_BW_800,
    SAMPLE_RATE_400 = ADXL313_BW_400,
    SAMPLE_RATE_50 = ADXL313_BW_50,
    SAMPLE_RATE_25 = ADXL313_BW_25
};

// Change sample rate here:
SampleRate selectedSampleRate = SAMPLE_RATE_1600; // Default sample rate selected

typedef struct{
    int16_t x;          // 16 bits -> 2 bytes
    int16_t y;          // 16 bits -> 2 bytes
    int16_t z;          // 16 bits -> 2 bytes
    uint8_t error = 1;  // 8 bits  -> 1 byte
} adxl_data_t;          // total   -> 7 bytes

void adxl_setup(ADXL313 *myAdxl)
{
    //Wire.begin();

    if (!myAdxl->begin()) //Begin communication over I2C
    {
        Serial.println("Failed to find AXDL313");
        while(1); 
    }
    Serial.println("ADXL313 Found!");
    
    myAdxl->measureModeOn(); // wakes up the sensor from standby and puts it into measurement mode

    // Configure Sample Rate
    myAdxl->setRate(selectedSampleRate);
}

void adxl_get_data(ADXL313 *myAdxl, adxl_data_t*adxl_data)
{
    adxl_data->error = 1;
    if(myAdxl->dataReady()) // check data ready interrupt, note, this clears all other int bits in INT_SOURCE reg
    {
        myAdxl->readAccel(); // read all 3 axis, they are stored in class variables: myAdxl.x, myAdxl.y and myAdxl.z
        adxl_data->x = myAdxl->x;
        adxl_data->y = myAdxl->y;
        adxl_data->z = myAdxl->z;

        // set error bit to 0 (false)
        adxl_data->error = 0;
    }



}