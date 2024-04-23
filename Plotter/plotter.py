import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.widgets import Button
import pandas as pd
import numpy as np
import serial

lunasats = []

# Serial config
baud_rate = 115200  # Change this to match the baud rate of your device 115200
# FOR GINN ONLY
# serial_port = "usbmodem143101"
serial_port = "/dev/tty.usbmodem14201"

# Initialize the serial connection
ser = serial.Serial(serial_port, baud_rate)

# Initialize lists to store data
lunasats_data = []
time_data = []
time_BME_data = []
time_ADXL_data = []
time_LIS_data = []
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


# Create figure and axis objects
fig1, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2)
temp_line1, = ax1.plot([], [], lw=2, label='Temperature')
humidity_line2, = ax2.plot([], [], lw=2, label='Humidity')
ax_line3, = ax3.plot([], [], lw=2, label='Accelerometer x')
ay_line3, = ax3.plot([], [], lw=2, label='Accelerometer y')
az_line3, = ax3.plot([], [], lw=2, label='Accelerometer z')
mx_line4, = ax4.plot([], [], lw=2, label='Magnetometer x')
my_line4, = ax4.plot([], [], lw=2, label='Magnetometer y')
mz_line4, = ax4.plot([], [], lw=2, label='Magnetometer z')
axes = [ax1, ax2, ax3, ax4]
lines = [temp_line1, humidity_line2, ax_line3, ay_line3, az_line3, mx_line4, my_line4, mz_line4]

for ax in axes:
    ax.set_xlabel('Time')
ax1.set_ylabel('Temperature (C)')
ax2.set_ylabel('Humidity (g/kg)')
ax3.set_ylabel('Accelerometer (G)')
ax4.set_ylabel('Magnetometer (uTesla)')

ax1.set_title('Temperature vs Time')
ax2.set_title('Humidity vs Time')
ax3.set_title('Accelerometer vs Time')
ax4.set_title('Magnetometer vs Time')


# Create figure and axis objects
fig2, ((ax1_2, ax2_2), (ax3_2, ax4_2)) = plt.subplots(2, 2)
temp_line1_2, = ax1_2.plot([], [], lw=2, label='Temperature')
humidity_line2_2, = ax2_2.plot([], [], lw=2, label='Humidity')
ax_line3_2, = ax3_2.plot([], [], lw=2, label='Accelerometer x')
ay_line3_2, = ax3_2.plot([], [], lw=2, label='Accelerometer y')
az_line3_2, = ax3_2.plot([], [], lw=2, label='Accelerometer z')
mx_line4_2, = ax4_2.plot([], [], lw=2, label='Magnetometer x')
my_line4_2, = ax4_2.plot([], [], lw=2, label='Magnetometer y')
mz_line4_2, = ax4_2.plot([], [], lw=2, label='Magnetometer z')
axes_2 = [ax1_2, ax2_2, ax3_2, ax4_2]
lines_2 = [temp_line1_2, humidity_line2_2, ax_line3_2, ay_line3_2, az_line3_2, mx_line4_2, my_line4_2, mz_line4_2]

for ax_2 in axes_2:
    ax_2.set_xlabel('Time')
ax1_2.set_ylabel('Temperature (C)')
ax2_2.set_ylabel('Humidity (g/kg)')
ax3_2.set_ylabel('Accelerometer (G)')
ax4_2.set_ylabel('Magnetometer (uTesla)')

ax1_2.set_title('Temperature vs Time')
ax2_2.set_title('Humidity vs Time')
ax3_2.set_title('Accelerometer vs Time')
ax4_2.set_title('Magnetometer vs Time')


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

    print("Lunasats: ", lunasats)
    print("Counter 1:", counter1, " Counter2: ", counter2)

    # # Initialize a dictionary to store counts of each number
    # number_counts = defaultdict(int)
    
    # # Extract the number from data_points[1]
    # number = int(data_points[1])
    
    # # Increment the count for the extracted number
    # number_counts[number] += 1
    
    # # Update lunasats based on the current number counts
    # lunasats = [num for num, count in number_counts.items() if count <= 5]
    # print(lunasats)
    

# Function to read serial data
def read_serial_data():
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').rstrip()  # Read a line and strip newline
        print("Line:",line)  # Optional: echo to console

        # Extract the data points
        data_points = line.split(',')  # Split the line by commas

        print("Data points:", data_points)

        # Parse sensor data
        if len(data_points) >= 2:
            try:
                add_unique_lunasat(data_points)

                sensor_name = data_points[0]
                luna_sat_num = data_points[1]

                if sensor_name == 'BME':
                    time = int(data_points[2])
                    temp = float(data_points[3])
                    humidity = float(data_points[4])
                    satellites_data['luna_sat'+luna_sat_num]['time_BME'].append(time)
                    satellites_data['luna_sat'+luna_sat_num]['temperature'].append(temp)
                    satellites_data['luna_sat'+luna_sat_num]['humidity'].append(humidity)
                    return sensor_name
                elif sensor_name == 'ADXL':
                    time = int(data_points[2])
                    accelerometer_x = float(data_points[3])
                    accelerometer_y = float(data_points[4])
                    accelerometer_z = float(data_points[5])
                    satellites_data['luna_sat'+luna_sat_num]['time_ADXL'].append(time)
                    satellites_data['luna_sat'+luna_sat_num]['accelerometer_x'].append(accelerometer_x)
                    satellites_data['luna_sat'+luna_sat_num]['accelerometer_y'].append(accelerometer_y)
                    satellites_data['luna_sat'+luna_sat_num]['accelerometer_z'].append(accelerometer_z)
                    return sensor_name
                elif sensor_name == 'LIS':
                    time = int(data_points[2])
                    magnetometer_x = float(data_points[3])
                    magnetometer_y = float(data_points[4])
                    magnetometer_z = float(data_points[5])
                    satellites_data['luna_sat'+luna_sat_num]['time_LIS'].append(time)
                    satellites_data['luna_sat'+luna_sat_num]['magnetometer_x'].append(magnetometer_x)
                    satellites_data['luna_sat'+luna_sat_num]['magnetometer_y'].append(magnetometer_y)
                    satellites_data['luna_sat'+luna_sat_num]['magnetometer_z'].append(magnetometer_z)
                    return sensor_name
                else:
                    print("ERROR NO DATA FOUND")
                    return None

            except ValueError:
                print("Data Empty")
                return None
        else:
            return None
    else:
        return None

class LunaSat:
    def __init__(self, value, x, y, z, color):
        self.value = value
        self.position = (x, y, z)
        self.color = color

#3D
def update_plot(frame, ax):
    objects_dict = {}
    ax.clear()
    ax.set_facecolor('black')
    for index, item in enumerate(lunasats):
        # Generate random positional data and random color for each LunaSat
        x = np.random.uniform(0, 10)
        y = np.random.uniform(0, 10)
        z = np.random.uniform(0, 3)
        color = np.random.rand(3,)  # Random RGB color
        objects_dict[f'obj_{index}'] = LunaSat(item, x, y, z, color)
    # Accessing the objects using the dynamically generated variable names
    for key, obj in objects_dict.items():
        globals()[key] = obj

    for obj in objects_dict.values():
        x, y, z = obj.position
        ax.scatter(x, y, z, s=100, c=[obj.color], marker='o')  # Larger dots (s=100)
        ax.text(x, y, z, f'LunaSat {obj.value}', color='white')  # Displaying "LunaSat" with the value near the point in white
    ax.set_zlim(0,10)
    ax.grid(True, color='white')

#2D
def update_graph(frame):
    # val = input('Enter which LunaSat would you like to read from\n')
        
    current_sensor = read_serial_data()  # Read serial data
    current_lunasat = '1'

    if current_sensor and current_lunasat != None:
        if current_sensor == 'BME':
            for line, data in zip(lines[0:2], [satellites_data['luna_sat'+current_lunasat]['temperature'], 
                                            satellites_data['luna_sat'+current_lunasat]['humidity']]):
                line.set_data(satellites_data['luna_sat'+current_lunasat]['time_BME'], data)
        elif current_sensor == 'ADXL':
            for line, data in zip(lines[2:5], [satellites_data['luna_sat'+current_lunasat]['accelerometer_x'],
                                               satellites_data['luna_sat'+current_lunasat]['accelerometer_y'],
                                               satellites_data['luna_sat'+current_lunasat]['accelerometer_z']]):
                line.set_data(satellites_data['luna_sat'+current_lunasat]['time_ADXL'], data)
        elif current_sensor == 'LIS':
            for line, data in zip(lines[5:8], [satellites_data['luna_sat'+current_lunasat]['magnetometer_x'],
                                               satellites_data['luna_sat'+current_lunasat]['magnetometer_y'],
                                               satellites_data['luna_sat'+current_lunasat]['magnetometer_z']]):
                line.set_data(satellites_data['luna_sat'+current_lunasat]['time_LIS'], data)
            
        for ax in axes:
            ax.relim()
            ax.autoscale_view()
            ax.set_xticks([])
        # Set Bounds
        ax1.set_ylim(0, 100)        # Temp
        ax2.set_ylim(0, 100)         # Humidity
        ax3.set_ylim(-550, 550)       # Accelerometer
        ax4.set_ylim(-33000, 33000) # Magnetometer 
    else:
        print("Data entry empty")

def update_graph2(frame):
    # val = input('Enter which LunaSat would you like to read from\n')
        
    current_sensor = read_serial_data()  # Read serial data
    current_lunasat = '2'

    if current_sensor and current_lunasat != None:
        if current_sensor == 'BME':
            for line, data in zip(lines_2[0:2], [satellites_data['luna_sat'+current_lunasat]['temperature'], 
                                            satellites_data['luna_sat'+current_lunasat]['humidity']]):
                line.set_data(satellites_data['luna_sat'+current_lunasat]['time_BME'], data)
        elif current_sensor == 'ADXL':
            for line, data in zip(lines_2[2:5], [satellites_data['luna_sat'+current_lunasat]['accelerometer_x'],
                                               satellites_data['luna_sat'+current_lunasat]['accelerometer_y'],
                                               satellites_data['luna_sat'+current_lunasat]['accelerometer_z']]):
                line.set_data(satellites_data['luna_sat'+current_lunasat]['time_ADXL'], data)
        elif current_sensor == 'LIS':
            for line, data in zip(lines_2[5:8], [satellites_data['luna_sat'+current_lunasat]['magnetometer_x'],
                                               satellites_data['luna_sat'+current_lunasat]['magnetometer_y'],
                                               satellites_data['luna_sat'+current_lunasat]['magnetometer_z']]):
                line.set_data(satellites_data['luna_sat'+current_lunasat]['time_LIS'], data)
            
        for ax_2 in axes_2:
            ax_2.relim()
            ax_2.autoscale_view()
            ax_2.set_xticks([])
        # Set Bounds
        ax1_2.set_ylim(0, 100)        # Temp
        ax2_2.set_ylim(0, 100)         # Humidity
        ax3_2.set_ylim(-550, 550)       # Accelerometer
        ax4_2.set_ylim(-33000, 33000) # Magnetometer 
    else:
        print("Data entry empty")

# def update_table(frame):
#     ax4.table()

# columns = ['BME_Time', 'ADXL_Time', 'LIS_Time']

# fig4, ax_4 = plt.subplots()
# ax_4.table(cellText=None, colLabels=columns, loc='center')


# def update(frame):
#     for i in range(len(data)):
#         for j in range(len(data[i])):
#             data[i][j] = randint(0, 100)
#     ax.clear()
#     ax.table(cellText=data, loc='center')

# Define a dictionary to store data for each satellite
satellites_data = {
    'luna_sat1': {'time_BME': [], 'time_ADXL': [], 'time_LIS': [], 'humidity': [], 'temperature': [], 'accelerometer_x': [], 'accelerometer_y': [], 'accelerometer_z': [],
                  'magnetometer_x': [], 'magnetometer_y': [], 'magnetometer_z': []},
    'luna_sat2': {'time_BME': [], 'time_ADXL': [], 'time_LIS': [], 'humidity': [], 'temperature': [], 'accelerometer_x': [], 'accelerometer_y': [], 'accelerometer_z': [],
                  'magnetometer_x': [], 'magnetometer_y': [], 'magnetometer_z': []},
}

fig3D = plt.figure(figsize=(12,6))
ax_3d = fig3D.add_subplot(121, projection='3d')


anim0 = FuncAnimation(fig1, update_graph, frames=20, interval=500)
anim1 = FuncAnimation(fig2, update_graph2, frames=20, interval=500)
anim2 = FuncAnimation(fig3D, update_plot, fargs=(ax_3d,), frames=20, interval=500)

plt.show()



