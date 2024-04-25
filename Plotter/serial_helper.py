import serial

# Serial config
baud_rate = 115200  # Change this to match the baud rate of your device 115200
serial_port = "/dev/tty.usbmodem141101"
serial_port = "/dev/tty.usbserial-14210"

# Initialize the serial connection
ser = serial.Serial(serial_port, baud_rate)


# Initialize lists to store data
lunasats = []
lunasats_data = []
time_data = []
time_BME_data = []
time_ADXL_data = []
time_LIS_data = []
temp_data = []
humidity_data = []
accelerometer_x_data = []
accelerometer_y_data = []
accelerometer_z_data = []
magnetometer_x_data = []
magnetometer_y_data = []
magnetometer_z_data = []

counter1 = 0
counter2 = 0
counter3 = 0

luna_sat_num = '1'

satellites_data = {
    'luna_sat1': {'time_BME': [], 'time_ADXL': [], 'time_LIS': [], 'humidity': [], 'temperature': [], 'accelerometer_x': [], 'accelerometer_y': [], 'accelerometer_z': [],
                  'magnetometer_x': [], 'magnetometer_y': [], 'magnetometer_z': []},
    'luna_sat2': {'time_BME': [], 'time_ADXL': [], 'time_LIS': [], 'humidity': [], 'temperature': [], 'accelerometer_x': [], 'accelerometer_y': [], 'accelerometer_z': [],
                  'magnetometer_x': [], 'magnetometer_y': [], 'magnetometer_z': []},
    'luna_sat3': {'time_BME': [], 'time_ADXL': [], 'time_LIS': [], 'humidity': [], 'temperature': [], 'accelerometer_x': [], 'accelerometer_y': [], 'accelerometer_z': [],
                  'magnetometer_x': [], 'magnetometer_y': [], 'magnetometer_z': []},
    'luna_sat4': {'time_BME': [], 'time_ADXL': [], 'time_LIS': [], 'humidity': [], 'temperature': [], 'accelerometer_x': [], 'accelerometer_y': [], 'accelerometer_z': [],
                  'magnetometer_x': [], 'magnetometer_y': [], 'magnetometer_z': []},
    'luna_sat5': {'time_BME': [], 'time_ADXL': [], 'time_LIS': [], 'humidity': [], 'temperature': [], 'accelerometer_x': [], 'accelerometer_y': [], 'accelerometer_z': [],
                  'magnetometer_x': [], 'magnetometer_y': [], 'magnetometer_z': []},
}

# satellites_data['luna_sat1']['time_BME'].append(0)

class LunaSat:
    def __init__(self, value, x, y, z, color):
        self.value = value
        self.position = (x, y, z)
        self.color = color

def add_unique_lunasat(data_points):
    global counter1
    global counter2
    global counter3
    if int(data_points[1]) < 10:
        number = int(data_points[1])
        if number not in lunasats:
            lunasats.append(number)
            print("added lunasat")
    
    if int(data_points[1]) != 1:
        counter1 += 1
    elif int(data_points[1]) == 1:
        counter1 = 0
    if int(data_points[1]) != 2:
        counter2 += 1
    elif int(data_points[1]) == 2:
        counter2 = 0
    # if int(data_points[1]) != 3:
    #     counter3 += 1
    # elif int(data_points[1]) == 3:
    #     counter3 = 0

    if counter1 > 10:
        lunasats.remove(1)
    if counter2 > 10:
        lunasats.remove(2)
    # if counter3 > 10:
    #     lunasats.remove(3)

    # print("Lunasats: ", lunasats)
    # print("Counter 1:", counter1, " Counter2: ", counter2)

# Function to read serial data
def read_serial_data():
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').rstrip()  # Read a line and strip newline

        # Extract the data points
        data_points = line.split(',')  # Split the line by commas

        print("Data points:", data_points)
        # print("Sensor:", data_points[0])

        # Parse sensor data
        if len(data_points) >= 2:
            try:
                # try:
                #     add_unique_lunasat(data_points)
                # except ValueError:
                #     print("ADD UNIQUE LUNASAT ERROR")
                global luna_sat_num
                
                sensor_name = data_points[0]
                luna_sat_num = data_points[1]

                if sensor_name == 'BME':
                    try:
                        time = int(data_points[2])/1000
                        temp = float(data_points[3])
                        humidity = float(data_points[4])
                        satellites_data['luna_sat'+luna_sat_num]['time_BME'].append(time)
                        satellites_data['luna_sat'+luna_sat_num]['temperature'].append(temp)
                        satellites_data['luna_sat'+luna_sat_num]['humidity'].append(humidity)
                    except ValueError:
                        print("BME ERROR")
                    # return luna_sat_num
                elif sensor_name == 'ADXL':
                    try:
                        time = int(data_points[2])/1000
                        # time = time_fixer(time, luna_sat_num)
                        accelerometer_x = float(data_points[3])
                        accelerometer_y = float(data_points[4])
                        accelerometer_z = float(data_points[5])
                        satellites_data['luna_sat'+luna_sat_num]['time_ADXL'].append(time)
                        satellites_data['luna_sat'+luna_sat_num]['accelerometer_x'].append(accelerometer_x)
                        satellites_data['luna_sat'+luna_sat_num]['accelerometer_y'].append(accelerometer_y)
                        satellites_data['luna_sat'+luna_sat_num]['accelerometer_z'].append(accelerometer_z)
                    except ValueError:
                        print("ADXL ERROR")
                    # return luna_sat_num
                elif sensor_name == 'LIS':
                    try:
                        time = int(data_points[2])/1000
                        # time = time_fixer(time, luna_sat_num)
                        magnetometer_x = float(data_points[3])
                        magnetometer_y = float(data_points[4])
                        magnetometer_z = float(data_points[5])
                        satellites_data['luna_sat'+luna_sat_num]['time_LIS'].append(time)
                        satellites_data['luna_sat'+luna_sat_num]['magnetometer_x'].append(magnetometer_x)
                        satellites_data['luna_sat'+luna_sat_num]['magnetometer_y'].append(magnetometer_y)
                        satellites_data['luna_sat'+luna_sat_num]['magnetometer_z'].append(magnetometer_z)
                    except ValueError:
                        print("LIS ERROR")
                    # return luna_sat_num
                else:
                    print("ERROR NO SENSOR DATA FOUND")
                    return None

            except ValueError:
                print("Data Empty", data_points)
                return None
        else:
            return None
    else:
        return None
    
def tester():
    print("test")