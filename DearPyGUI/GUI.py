import dearpygui.dearpygui as dpg
import serial.tools.list_ports
import serial

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

# Global defs
temp_data = None

try:
    with serial.Serial(serial_port, baud_rate) as ser:
        with open(output_file_path, 'w') as out_file:
            while True:
                line = ser.readline().decode('utf-8').rstrip()  # Read a line and strip newline
                print(line)  # Optional: echo to console

                # Extract the first data point
                data_points = line.split(',')  # Split the line by commas
                temp_data = data_points[0]  # Get the first element

                out_file.write(line + '\n')  # Write to the file
                
except KeyboardInterrupt:
    # Close the serial connection when Ctrl+C is pressed
    ser.close()

# Define an empty dictionary to hold enum-like values
row_dict = {}
col_dict = {}

# TEMP
col_labels = "col1 col2 col3" #temporary col element names
row_labels = "row1 row2 row3 row4" #temporary row element names

dpg.create_context()

# # Add labes for each column
# def table_column_labels(labels):
#     # Split the input string into individual tokens
#     tokens = labels.split()

#     for token in tokens:
#         if token.upper() not in col_dict:
#             col_dict[token.upper()] = token.upper()

#     list = [col_dict[token.upper()] for token in tokens]

#     for i in range(len(list)):
#         dpg.add_table_column(label=list[i])

#     return len(list)

# Add labels for each row
def table_row_labels(labels):
    # Split the input string into individual tokens
    tokens = labels.split()

    for token in tokens:
        if token.upper() not in row_dict:
            row_dict[token.upper()] = token.upper()

    list = [row_dict[token.upper()] for token in tokens]
    # for i in range(len(list)):
    #     with dpg.table_row():
    #         dpg.add_text(list[i])

    return list, len(list)

###########################################################
with dpg.window(label="Display Data"):
    with dpg.table(header_row=True):
        dpg.add_table_column() # Column for row labels
    
        # cols_len = table_column_labels(col_labels)

        dpg.add_table_column(label="Accelerometer")
        dpg.add_table_column(label="Magnetometer")
        dpg.add_table_column(label="Pressure Sensor")
        dpg.add_table_column(label="Temp Sensor")
        dpg.add_table_column(label="Gas Sensor")

        list, rows_len = table_row_labels(row_labels)
        print(rows_len)

        for i in range(rows_len): # Rows
            with dpg.table_row():
                for j in range(0,6): # Columns
                    with dpg.table_cell():
                        dpg.add_text(temp_data)
        

dpg.create_viewport(title='Custom Title', width=800, height=600)
dpg.setup_dearpygui()
dpg.show_viewport()
dpg.start_dearpygui()
dpg.destroy_context()

