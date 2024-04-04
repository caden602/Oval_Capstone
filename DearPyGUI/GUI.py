import dearpygui.dearpygui as dpg
import serial.tools.list_ports
import serial
import datetime
import time

# Global Variables
choice = 2

def detect_serial_port():
    # Get a list of all available serial ports
    ports = serial.tools.list_ports.comports()
    
    # Iterate over each port and check if it meets your criteria
    for port in ports:
        # Print information about each port (optional)
        print("Found port:", port.device, "-", port.description)
        
        # Check if the port meets your criteria (e.g., based on description or manufacturer)
        if "usbmodem" in port.description:  # You can adjust this condition based on your requirements
            return port.device  # Return the device name if it meets your criteria
    
    # If no suitable port is found, return None
    return None

# Detect the serial port
serial_port = detect_serial_port()

# Check if a serial port was found
if serial_port:
    print("Serial port found:", serial_port)
else:
    print("No suitable serial port found.")

# Serial config
baud_rate = 9600 # Change this to match the baud rate of your device 115200 
output_file_path = 'output.txt'
# FOR GINN ONLY
# serial_port = "usbmodem143101"
serial_port = "/dev/cu.usbmodem141301"

# Initialize the serial connection
ser = serial.Serial(serial_port, baud_rate)

###########################################################
# Define a callback function for when a tab is clicked
def tab_callback(sender, app_data):
    print("Tab", app_data, "clicked")

dpg.create_context()
dpg.create_viewport(title='LunaSat GUI', width=800, height=800)
dpg.setup_dearpygui()
dpg.show_viewport()

with serial.Serial(serial_port, baud_rate) as ser:
    with open(output_file_path, 'w') as out_file:
        if choice == 1:
            with dpg.window(label="Graph", height=600, width=600):
                while dpg.is_dearpygui_running():
                    line = ser.readline().decode('utf-8').rstrip()  # Read a line and strip newline
                    print(line)  # Optional: echo to console

                    # Extract the first data point
                    data_points = line.split(',')  # Split the line by commas
                    # Parse sensor data
                    temp_data = data_points[0]  # Get the first element
                    humidity_data = data_points[1]
                    gas_data = data_points[2]
                    pressure_data = data_points[3]
                    adxl_data_x = data_points[4]
                    adxl_data_y = data_points[5]
                    adxl_data_z = data_points[6]
                    lis3md_data_x = data_points[7]
                    lis3md_data_y = data_points[8]
                    lis3md_data_z = data_points[9]

                    out_file.write(line + '\n')  # Write to the file

                    dpg.add_simple_plot(label="Histogram", default_value=(float(temp_data), float(humidity_data)), overlay="Overlaying", height=600, width=600,
                        histogram=True)
                    dpg.render_dearpygui_frame()    
                    time.sleep(0.0001)

        if choice == 2:
            with dpg.window(label="Data Table"):
                with dpg.table(header_row=True):
                    dpg.add_table_column(label="Time")
                    dpg.add_table_column(label="Temp Sensor")
                    dpg.add_table_column(label="Humidity Sensor")
                    dpg.add_table_column(label="Gas Sensor")
                    dpg.add_table_column(label="Pressure Sensor")
                    dpg.add_table_column(label="Accelerometer")
                    dpg.add_table_column(label="Magnetometer")

                    while dpg.is_dearpygui_running():
                        line = ser.readline().decode('utf-8').rstrip()  # Read a line and strip newline
                        print(line)  # Optional: echo to console

                        # Extract the first data point
                        data_points = line.split(',')  # Split the line by commas
                        # Parse sensor data
                        temp_data = data_points[0]  # Get the first element
                        humidity_data = data_points[1]
                        gas_data = data_points[2]
                        pressure_data = data_points[3]
                        adxl_data_x = data_points[4]
                        adxl_data_y = data_points[5]
                        adxl_data_z = data_points[6]
                        lis3md_data_x = data_points[7]
                        lis3md_data_y = data_points[8]
                        lis3md_data_z = data_points[9]

                        out_file.write(line + '\n')  # Write to the file

                        with dpg.table_row():
                            for i in range(0,7): # Columns
                                with dpg.table_cell():
                                    if i == 0:
                                        # Get the current date and time
                                        timestamp = datetime.datetime.now()
                                        # Extract the time portion
                                        current_time = timestamp.strftime("%H:%M:%S")
                                        dpg.add_text(current_time)
                                    elif i == 1:
                                        dpg.add_text(temp_data)
                                    elif i == 2:
                                        dpg.add_text(humidity_data)
                                    elif i == 3:
                                        dpg.add_text(gas_data)
                                    elif i == 4:
                                        dpg.add_text(pressure_data)
                                    elif i == 5:
                                        dpg.add_text(adxl_data_x+' '+adxl_data_y+' '+adxl_data_z)
                                    elif i == 6:
                                        dpg.add_text(lis3md_data_x+' '+lis3md_data_y+' '+lis3md_data_z)
                                    else:
                                        dpg.add_text('NULL')
                        
        
                        dpg.render_dearpygui_frame()    
                        time.sleep(0.0001)
            
        

# dpg.create_viewport(title='Custom Title', width=800, height=600)
# dpg.setup_dearpygui()
# dpg.show_viewport()
# dpg.start_dearpygui()
dpg.destroy_context()

