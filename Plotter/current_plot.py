import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import serial

# Serial config
baud_rate = 115200  # Change this to match the baud rate of your device 115200
serial_port = ""    #"/dev/cu.usbmodem141301"

# Initialize the serial connection
ser = serial.Serial(serial_port, baud_rate)

# Initialize lists to store data
time_data = []
current_data = []

# Create figure and axis objects
fig, ax = plt.subplots()
line, = ax.plot([], [], lw=2)
ax.set_xlabel('Time')
ax.set_ylabel('Current (mA)')
ax.set_title('Current vs Time')

# Function to read serial data
def read_serial_data():
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').rstrip()  # Read a line and strip newline
        print(line)  # Optional: echo to console

        # Extract the data points
        data_points = line.split(',')  # Split the line by commas
        # Parse sensor data
        current = float(data_points[1])  # Get the first element as float

        # Append data to lists
        time_data.append(len(time_data) + 1)  # Assuming time is just the number of readings
        current_data.append(current)

# Function to update the plot
def update(frame):
    read_serial_data()  # Read serial data
    line.set_data(time_data, current_data)
    ax.relim()
    ax.autoscale_view()
    ax.set_xticklabels(['Time'])
    ax.set_ylim(0, 150)    # Current
    return line,

# Create animation
ani = FuncAnimation(fig, update, frames=None, blit=True, interval=1000)

plt.show()
