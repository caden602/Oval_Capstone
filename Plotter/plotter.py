import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import serial

# Serial config
baud_rate = 115200  # Change this to match the baud rate of your device 115200
# FOR GINN ONLY
# serial_port = "usbmodem143101"
serial_port = "/dev/cu.usbmodem141301"

# Initialize the serial connection
ser = serial.Serial(serial_port, baud_rate)

# Initialize lists to store data
time_data = []
temp_data = []
humidity_data = []
gas_data = []
pressure_data = []

# Create figure and axis objects
fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2)
line1, = ax1.plot([], [], lw=2, label='Temperature')
line2, = ax2.plot([], [], lw=2, label='Humidity')
line3, = ax3.plot([], [], lw=2, label='Gas')
line4, = ax4.plot([], [], lw=2, label='Pressure')
axes = [ax1, ax2, ax3, ax4]
lines = [line1, line2, line3, line4]

for ax in axes:
    ax.set_xlabel('Time')
ax1.set_ylabel('Temperature (C)')
ax2.set_ylabel('Humidity (g/kg)')
ax3.set_ylabel('Gas (J/Kmol)')
ax4.set_ylabel('Pressure (psi)')

ax1.set_title('Temperature vs Time')
ax2.set_title('Humidity vs Time')
ax3.set_title('Gas vs Time')
ax4.set_title('Pressure vs Time')

# Function to read serial data
def read_serial_data():
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').rstrip()  # Read a line and strip newline
        print(line)  # Optional: echo to console

        # Extract the data points
        data_points = line.split(',')  # Split the line by commas
        # Parse sensor data
        temp = float(data_points[1])  # Get the first element as float
        humidity = float(data_points[2])  # Get the second element as float
        gas = float(data_points[3])  # Get the third element as float
        pressure = float(data_points[4])  # Get the fourth element as float

        # Append data to lists
        time_data.append(len(time_data) + 1)  # Assuming time is just the number of readings
        temp_data.append(temp)
        humidity_data.append(humidity)
        gas_data.append(gas)
        pressure_data.append(pressure)

# Function to update the plot
def update(frame):
    read_serial_data()  # Read serial data
    for line, data in zip(lines, [temp_data, humidity_data, gas_data, pressure_data]):
        line.set_data(time_data, data)
    for ax in axes:
        ax.relim()
        ax.autoscale_view()
        ax.set_xticklabels(['Time'])
    # Set Bounds
    ax1.set_ylim(0, 100)    # Temp
    ax2.set_ylim(0, 50)     # Humidity
    ax3.set_ylim(50, 200)   # Gas
    ax4.set_ylim(0, 20)     # Pressure
    return lines

# Create animation
ani = FuncAnimation(fig, update, frames=None, blit=True, interval=1000)

plt.tight_layout()
plt.show()
