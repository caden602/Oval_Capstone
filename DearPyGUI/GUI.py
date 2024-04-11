import dearpygui.dearpygui as dpg
import sys
import os

# Add the parent directory of your project to the Python path
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
sys.path.append(parent_dir)

# Now you can import your module
from Plotter import plotter



###########################################################
# Define a callback function for when a tab is clicked
def tab_callback(sender, app_data):
    print("Tab", app_data, "clicked")

def graph_callback(sender, app_data):
    print("Button", app_data, "clicked")
    plotter.plot_graph()

dpg.create_context()
dpg.create_viewport(title='LunaSat GUI', width=800, height=800)

with dpg.window(label="Example Window", height=600, width=600):
    dpg.add_button(label="Graph", callback=graph_callback)
        
dpg.setup_dearpygui()
dpg.show_viewport()
dpg.start_dearpygui()
dpg.destroy_context()
