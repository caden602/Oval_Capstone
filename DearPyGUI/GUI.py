import dearpygui.dearpygui as dpg

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
                        dpg.add_text("data")
        

dpg.create_viewport(title='Custom Title', width=800, height=600)
dpg.setup_dearpygui()
dpg.show_viewport()
dpg.start_dearpygui()
dpg.destroy_context()
