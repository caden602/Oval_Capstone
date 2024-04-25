import numpy as np
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore
import pyqtgraph.opengl as gl
import serial_helper as sh
import multiprocessing
import pyqtgraph.examples
import time



##################

# ## Create a GL View widget to display data
# app2 = pg.mkQApp("GLSurfacePlot Example")
# w = gl.GLViewWidget()
# w.show()
# w.setWindowTitle('pyqtgraph example: GLSurfacePlot')
# w.setCameraPosition(distance=50)

# ## Add a grid to the view
# g = gl.GLGridItem()
# g.scale(2,2,1)
# g.setDepthValue(10)  # draw grid after surfaces since they may be translucent
# w.addItem(g)

# ## Animated example
# ## compute surface vertex data
# cols = 90
# rows = 100
# x = np.linspace(-8, 8, cols+1).reshape(cols+1,1)
# y = np.linspace(-8, 8, rows+1).reshape(1,rows+1)
# d = (x**2 + y**2) * 0.1
# d2 = d ** 0.5 + 0.1

# ## precompute height values for all frames
# phi = np.arange(0, np.pi*2, np.pi/20.)
# z = np.sin(d[np.newaxis,...] + phi.reshape(phi.shape[0], 1, 1)) / d2[np.newaxis,...]


# ## create a surface plot, tell it to use the 'heightColor' shader
# ## since this does not require normal vectors to render (thus we 
# ## can set computeNormals=False to save time when the mesh updates)
# p4 = gl.GLSurfacePlotItem(x=x[:,0], y = y[0,:], shader='heightColor', computeNormals=False, smooth=False)
# p4.shader()['colorMap'] = np.array([0.2, 2, 0.5, 0.2, 1, 1, 0.2, 0, 2])
# p4.translate(10, 10, 0)
# w.addItem(p4)

# index = 0
# def update2():
#     global p4, z, index
#     index -= 1
#     p4.setData(z=z[index%z.shape[0]])
    
# timer2 = QtCore.QTimer()
# timer2.timeout.connect(update2)
# timer2.start(30)

class SatelliteData:
    def __init__(self, data):
        self.time_BME_data = data['time_BME']
        self.time_ADXL_data = data['time_ADXL']
        self.time_LIS_data = data['time_LIS']
        self.humidity_data = data['humidity']
        self.temp_data = data['temperature']
        self.accelerometer_x_data = data['accelerometer_x']
        self.accelerometer_y_data = data['accelerometer_y']
        self.accelerometer_z_data = data['accelerometer_z']
        self.magnetometer_x_data = data['magnetometer_x']
        self.magnetometer_y_data = data['magnetometer_y']
        self.magnetometer_z_data = data['magnetometer_z']

satellite_data = SatelliteData(sh.satellites_data['luna_sat1'])


# This function represents the main process
def main_process():
    app = pg.mkQApp("2D Graphs")
    #mw = QtWidgets.QMainWindow()
    #mw.resize(800,800)

    win = pg.GraphicsLayoutWidget(show=True, title="2D Graphs Layout")
    win.resize(1000,600)
    win.setWindowTitle('LunaSat Graphs')

    # Enable antialiasing for prettier plots
    pg.setConfigOptions(antialias=True)

    p1 = win.addPlot(title="Temperature vs Time")
    c1 = p1.plot(pen='y', symbolPen='r')
    
    p2 = win.addPlot(title="Humidity vs Time")
    c2 = p2.plot(pen='g', symbolPen='r')

    win.nextRow()

    p3 = win.addPlot(title="Accelerometer vs Time")
    c3_x = p3.plot(pen='r', symbolPen='r')
    c3_y = p3.plot(pen='g', symbolPen='r')
    c3_z = p3.plot(pen='b', symbolPen='r')

    p4 = win.addPlot(title="Magnetometer vs Time")
    c4_x = p4.plot(pen='r', symbolPen='r')
    c4_y = p4.plot(pen='g', symbolPen='r')
    c4_z = p4.plot(pen='b', symbolPen='r')

    def update():
        sh.read_serial_data()

        # Set data
        # BME
        y1 = satellite_data.time_BME_data
        x1 = satellite_data.temp_data
        y2 = y1
        x2 = satellite_data.humidity_data
        c1.setData(y1,x1)
        c2.setData(y2,x2)
        # ADXL
        y3   = satellite_data.time_ADXL_data
        x3_x = satellite_data.accelerometer_x_data
        x3_y = satellite_data.accelerometer_y_data
        x3_z = satellite_data.accelerometer_z_data
        c3_x.setData(y3,x3_x)
        c3_y.setData(y3,x3_y)
        c3_z.setData(y3,x3_z)
        # LIS
        y4   = satellite_data.time_LIS_data
        x4_x = satellite_data.magnetometer_x_data
        x4_y = satellite_data.magnetometer_y_data
        x4_z = satellite_data.magnetometer_z_data
        c4_x.setData(y4,x4_x)
        c4_y.setData(y4,x4_y)
        c4_z.setData(y4,x4_z)

        
        
    timer = QtCore.QTimer()
    timer.timeout.connect(update)
    timer.start(50)


    pg.exec()

main_process()
# def background_process():
#     sh.read_serial_data()
#     # sh.tester()

# if __name__ == "__main__":
#     # Create a multiprocessing.Event object to serve as a flag
#     flag = multiprocessing.Event()

#     # Create a process for the main process
#     main_proc = multiprocessing.Process(target=main_process)

#     # Start the main process
#     main_proc.start()

#     # Wait for a few seconds
#     time.sleep(4)

#     # Set the flag to signal the process to terminate
#     flag.set()

#     # Wait for the main process to finish
#     main_proc.join()

#     print("Both processes have finished")

