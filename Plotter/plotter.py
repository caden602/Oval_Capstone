import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import serial

# Serial config
baud_rate = 9600  # Change this to match the baud rate of your device 115200
# FOR GINN ONLY
# serial_port = "usbmodem143101"
serial_port = "/dev/cu.usbmodem141301"

# Initialize the serial connection
ser = serial.Serial(serial_port, baud_rate)

# Initialize lists to store data
time_data = []
temp_data = []
humidity_data = []

# Create figure and axis objects
fig, (ax1, ax2) = plt.subplots(2, 1)
line1, = ax1.plot([], [], lw=2, label='Temperature')
line2, = ax2.plot([], [], lw=2, label='Humidity')
ax1.set_xlabel('Time')
ax1.set_ylabel('Temperature')
ax1.set_title('Temperature vs Time')
ax2.set_xlabel('Time')
ax2.set_ylabel('Humidity')
ax2.set_title('Humidity vs Time')

# Function to read serial data
def read_serial_data():
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').rstrip()  # Read a line and strip newline
        print(line)  # Optional: echo to console

        # Extract the data points
        data_points = line.split(',')  # Split the line by commas
        # Parse sensor data
        temp = float(data_points[0])  # Get the first element as float
        humidity = float(data_points[1])  # Get the second element as float

        # Append data to lists
        time_data.append(len(time_data) + 1)  # Assuming time is just the number of readings
        temp_data.append(temp)
        humidity_data.append(humidity)

# Function to update the plot
def update(frame):
    read_serial_data()  # Read serial data
    line1.set_data(time_data, temp_data)
    line2.set_data(time_data, humidity_data)
    ax1.relim()
    ax1.autoscale_view()
    ax2.relim()
    ax2.autoscale_view()
    return line1, line2

# Create animation
ani = FuncAnimation(fig, update, frames=None, blit=True, interval=1000)

plt.tight_layout()
plt.show()
