import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import serial
from datetime import datetime

counter = 66600
running = False

lunasats = 0

def count():
    if running:
        global counter
        
        if counter==66600:
            display="Starting..."
        else:
            tt = datetime.fromtimestamp(counter)

        counter += 1
    count()

def Start():
    global running
    running=True
    count()
    running = False

def Reset():
    global counter
    counter=66600



# Serial config
baud_rate = 115200  # Change this to match the baud rate of your device 115200
# FOR GINN ONLY
# serial_port = "usbmodem143101"
serial_port = "/dev/tty.usbmodem141301"

# Initialize the serial connection
ser = serial.Serial(serial_port, baud_rate)

# Initialize lists to store data
lunasats_data = []
time_data = []
temp_data = []
humidity_data = []
accelerometer_x_data = []
accelerometer_y_data = []
accelerometer_z_data = []
magnetometer_x_data = []
magnetometer_y_data = []
magnetometer_z_data = []


# Create figure and axis objects
fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2)
line1, = ax1.plot([], [], lw=2, label='Temperature')
line2, = ax2.plot([], [], lw=2, label='Humidity')
ax_line3, = ax3.plot([], [], lw=2, label='Accelerometer x')
ay_line3, = ax3.plot([], [], lw=2, label='Accelerometer y')
az_line3, = ax3.plot([], [], lw=2, label='Accelerometer z')
mx_line4, = ax4.plot([], [], lw=2, label='Magnetometer x')
my_line4, = ax4.plot([], [], lw=2, label='Magnetometer y')
mz_line4, = ax4.plot([], [], lw=2, label='Magnetometer z')
axes = [ax1, ax2, ax3, ax4]
lines = [line1, line2, ax_line3, ay_line3, az_line3, mx_line4, my_line4, mz_line4]

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

dataLines = []

# Function to read serial data
def read_serial_data(start, end):
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').rstrip()  # Read a line and strip newline
        print(line)  # Optional: echo to console

        dataLines.append(line)

        # Extract the data points
        data_points = line.split(',')  # Split the line by commas

        for i in dataLines[start:end]:
            # Parse sensor data
            if len(data_points) == 11:
                try:
                    for data_point in data_points:
                        # Check the prefix of the data_point
                        # if data_point.startswith('L'):
                        #     lunasats =+ int(data_point[1:])
                        #     lunasats_data.append(lunasats)
                        if data_point.startswith('T'):
                            time = int(data_point[1:])
                            time_data.append(time)
                        elif data_point.startswith('t'):
                            temp = float(data_point[1:])
                            temp_data.append(temp)
                        elif data_point.startswith('h'):
                            humidity = float(data_point[1:])
                            humidity_data.append(humidity)
                        elif data_point.startswith('ax'):
                            accelerometer_x = float(data_point[2:])
                            accelerometer_x_data.append(accelerometer_x)
                        elif data_point.startswith('ay'):
                            accelerometer_y = float(data_point[2:])
                            accelerometer_y_data.append(accelerometer_y)
                        elif data_point.startswith('az'):
                            accelerometer_z = float(data_point[2:])
                            accelerometer_z_data.append(accelerometer_z)
                        elif data_point.startswith('mx'):
                            magnetometer_x = float(data_point[2:])
                            magnetometer_x_data.append(magnetometer_x)
                        elif data_point.startswith('my'):
                            magnetometer_y = float(data_point[2:])
                            magnetometer_y_data.append(magnetometer_y)
                        elif data_point.startswith('mx'):
                            magnetometer_z = float(data_point[2:])
                            magnetometer_z_data.append(magnetometer_x)

                    # time = int(data_points[0])
                    # temp = float(data_points[1])  # Get the first element as float
                    # humidity = float(data_points[2])  # Get the second element as float
                    # accelerometer_x = float(data_points[5])
                    # accelerometer_y = float(data_points[6])  # Get the 6th element as float (Accel y-data)
                    # accelerometer_z = float(data_points[7])
                    # magnetometer_x = float(data_points[8])
                    # magnetometer_y = float(data_points[9])
                    # magnetometer_z = float(data_points[10])  # Get the 10th element as float (Mag z-data)

                    # # Append data to lists
                    # time_data.append(time)
                    # # time_data.append(len(time_data) + 1)  # Assuming time is just the number of readings
                    # temp_data.append(temp)
                    # humidity_data.append(humidity)
                    # accelerometer_x_data.append(accelerometer_x)
                    # accelerometer_y_data.append(accelerometer_y)
                    # accelerometer_z_data.append(accelerometer_z)
                    # magnetometer_x_data.append(magnetometer_x)
                    # magnetometer_y_data.append(magnetometer_y)
                    # magnetometer_z_data.append(magnetometer_z)
                except ValueError:
                    print("Data Empty")
                    return

# Function to update the plot
def update(frame):
    Start()
    start = None
    end = None
    val = input('Enter which LunaSat would you like to read from\n')
    if val == 1:
        start = 1
        end = 5
    elif val == 2:
        start = 6
        end = 10
    read_serial_data(start, end)  # Read serial data

    if counter > 5: #Adjust to fit proper package size
        # Do some stuff to read package size
        for numLines in dataLines %5: # Adjust per packet size
            lunasats =+ 1
        else:
            lunasats = 0
        Reset()

    for line, data in zip(lines, [temp_data, humidity_data, accelerometer_x_data, accelerometer_y_data, accelerometer_z_data, magnetometer_x_data, magnetometer_y_data, magnetometer_z_data]):
        line.set_data(time_data, data)
    for ax in axes:
        ax.relim()
        ax.autoscale_view()
        ax.set_xticks([])
        # if len(time_data) >= 1:
        #     ax.set_xticks([time_data])  # Set x-axis ticks with the last time point
        #     ax.set_xticklabels([f"{time_data}s"])  # Set tick label for the last time point
        #     ax.set_xlim(time_data)
        # else:
    # Set Bounds
    ax1.set_ylim(0, 100)        # Temp
    ax2.set_ylim(0, 50)         # Humidity
    ax3.set_ylim(-30, 30)       # Accelerometer
    ax4.set_ylim(-33000, 33000) # Magnetometer

    visual_connections(lunasats)

    return lines

# Create animation
ani = FuncAnimation(fig, update, frames=None, blit=True, interval=100) # Plot update interval = 100ms
plt.tight_layout()

class LunaSat:
    def __init__(self, value, x, y, z, color):
        self.value = value
        self.position = (x, y, z)
        self.color = color

def visual_connections(lunasats):
    objects_dict = {}
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

    # Visualization
    fig = plt.figure(2, facecolor='black')  # Setting the background color to black
    ax = fig.add_subplot(111, projection='3d')
    ax.set_facecolor('black')  # Setting the background color to black
    for obj in objects_dict.values():
        x, y, z = obj.position
        ax.scatter(x, y, z, s=100, c=[obj.color], marker='o')  # Larger dots (s=100)
        ax.text(x, y, z, f'LunaSat {obj.value}', color='white')  # Displaying "LunaSat" with the value near the point in white
    ax.set_xlabel('X', color='white')  # Setting the axis labels to white
    ax.set_ylabel('Y', color='white')  # Setting the axis labels to white
    ax.set_zlabel('Z', color='white')  # Setting the axis labels to white
    ax.set_zlim(0,10)
    ax.grid(True, color='white')  # Setting the grid color to white

# visual_connections(lunasats)

# initial_lunasats = lunasats
# current_lunasats = [1,2,3,4] # change to read serial data "lunasats"

# if current_lunasats != initial_lunasats:
#     plt.close(2)
#     visual_connections(current_lunasats)


plt.show()


