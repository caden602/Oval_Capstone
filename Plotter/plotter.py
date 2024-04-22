import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from mpl_toolkits.mplot3d import Axes3D
from collections import defaultdict
import numpy as np
import serial

lunasats = []

# Serial config
baud_rate = 115200  # Change this to match the baud rate of your device 115200
# FOR GINN ONLY
# serial_port = "usbmodem143101"
serial_port = "/dev/tty.usbmodem142101"

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
def read_serial_data():
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').rstrip()  # Read a line and strip newline
        print(line)  # Optional: echo to console

        # Extract the data points
        data_points = line.split(',')  # Split the line by commas

        print(data_points)

        # Parse sensor data
        if len(data_points) >= 2:
            try:
                add_unique_lunasat(data_points)
                current_lunasat = data_points[1]
                
                current_sensor = data_points[0]
                if data_points[0] == 'BME':
                    # lunasat = int(data_points[1])
                    time = int(data_points[2])
                    time_BME_data.append(time)  
                    temp = float(data_points[3])
                    temp_data.append(temp)
                    humidity = float(data_points[4])
                    humidity_data.append(humidity)
                    return current_sensor
                elif data_points[0] == 'ADXL':
                    # lunasat = int(data_points[1])
                    time = int(data_points[2])
                    time_ADXL_data.append(time)  
                    accelerometer_x = float(data_points[3])
                    accelerometer_x_data.append(accelerometer_x)
                    accelerometer_y = float(data_points[4])
                    accelerometer_y_data.append(accelerometer_y)
                    accelerometer_z = float(data_points[5])
                    accelerometer_z_data.append(accelerometer_z)
                    return current_sensor
                elif data_points[0] == 'LIS':
                    # lunasat = int(data_points[1])
                    time = int(data_points[2])
                    time_LIS_data.append(time)  
                    magnetometer_x = float(data_points[3])
                    magnetometer_x_data.append(magnetometer_x)
                    magnetometer_y = float(data_points[4])
                    magnetometer_y_data.append(magnetometer_y)
                    magnetometer_z = float(data_points[5])
                    magnetometer_z_data.append(magnetometer_z)
                    return current_sensor
                else:
                    print("ERROR NO DATA FOUND")
            except ValueError:
                print("Data Empty")
                return

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

    if current_sensor == 'BME':
        for line, data in zip(lines[0:2], [temp_data, humidity_data]):
            line.set_data(time_BME_data, data)
    elif current_sensor == 'ADXL':
        for line, data in zip(lines[2:5], [accelerometer_x_data, accelerometer_y_data, accelerometer_z_data]):
            line.set_data(time_ADXL_data, data)
    elif current_sensor == 'LIS':
        for line, data in zip(lines[5:8], [magnetometer_x_data, magnetometer_y_data, magnetometer_z_data]):
            line.set_data(time_LIS_data, data)

        
    for ax in axes:
        ax.relim()
        ax.autoscale_view()
        ax.set_xticks([])
    # Set Bounds
    ax1.set_ylim(0, 100)        # Temp
    ax2.set_ylim(0, 100)         # Humidity
    ax3.set_ylim(-550, 550)       # Accelerometer
    ax4.set_ylim(-33000, 33000) # Magnetometer 


fig2 = plt.figure(figsize=(6, 6))
ax_3d = fig2.add_subplot(121, projection='3d')


anim1 = FuncAnimation(fig1, update_graph, frames=20, interval=500)
anim2 = FuncAnimation(fig2, update_plot, fargs=(ax_3d,), frames=20, interval=500)

plt.show()



