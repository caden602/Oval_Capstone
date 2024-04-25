import pyqtgraph as pg
from pyqtgraph.Qt import QtCore
# import pyqtgraph.opengl as gl
import serial_helper as sh
from pyqtgraph.dockarea.Dock import Dock
from pyqtgraph.dockarea.DockArea import DockArea
from pyqtgraph.Qt import QtWidgets


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


def create_plots(num_tabs):
    app = pg.mkQApp("DockArea Example")
    win = QtWidgets.QMainWindow()
    area = DockArea()
    win.setCentralWidget(area)
    win.resize(1000, 500)
    win.setWindowTitle('pyqtgraph example: dockarea')

    docks = []
    widgets = []
    timers = []  # List to store QTimer instances for each tab

    for i in range(num_tabs):
        global c1, c2
        dock = Dock(f"LunaSat {i+1}", size=(500, 200))
        docks.append(dock)
        if i == 0:
            area.addDock(dock, 'top')
        else:
            area.addDock(dock, 'above', docks[0])

        p1 = pg.PlotWidget(title="Temperature (C°) vs Time (s)")
        c1 = p1.plot(pen='y', symbolPen='r',symbolBrush='r')

        p2 = pg.PlotWidget(title="Humidity (%) vs Time (s)")
        c2 = p2.plot(pen='y', symbolPen='r',symbolBrush='r')

        p3   = pg.PlotWidget(title="Accelerometer (mG) vs Time (s)")
        c3_x = p3.plot(pen='r', symbolPen='r',symbolBrush='r')
        c3_y = p3.plot(pen='g', symbolPen='r',symbolBrush='r')
        c3_z = p3.plot(pen='b', symbolPen='r',symbolBrush='r')

        p4   = pg.PlotWidget(title="Magnetometer (Gaus) vs Time (s)")
        c4_x = p4.plot(pen='r', symbolPen='r',symbolBrush='r')
        c4_y = p4.plot(pen='g', symbolPen='r',symbolBrush='r')
        c4_z = p4.plot(pen='b', symbolPen='r',symbolBrush='r')
        
        widgets.append(p1)
        widgets.append(p2)
        widgets.append(p3)
        widgets.append(p4)

        dock.addWidget(p1)
        dock.addWidget(p2)     
        dock.addWidget(p3)       
        dock.addWidget(p4)

        timer = QtCore.QTimer()
        timer.timeout.connect(lambda c1=c1, c2=c2, c3_x=c3_x, c3_y=c3_y, c3_z=c3_z, 
                              c4_x=c4_x, c4_y=c4_y, c4_z=c4_z, i=i: 
                              update_plot(c1, c2, c3_x, c3_y, c3_z, c4_x, c4_y, c4_z, i))
        timer.start(50)
        timers.append(timer)

    # Enable antialiasing for prettier plots
    pg.setConfigOptions(antialias=True)
    win.show()
    pg.exec()

def update_plot(c1, c2, c3_x, c3_y, c3_z, c4_x, c4_y, c4_z, tab_index):
    sh.read_serial_data()

    if tab_index == 0:
        satellite_data = SatelliteData(sh.satellites_data['luna_sat1'])
        # BME
        y1 = satellite_data.time_BME_data
        x1 = satellite_data.temp_data
        c1.setData(y1, x1)
        y2 = y1
        x2 = satellite_data.humidity_data
        c2.setData(y2, x2)
        # ADXL
        y3   = satellite_data.time_ADXL_data
        x3_x = satellite_data.accelerometer_x_data
        x3_y = satellite_data.accelerometer_y_data
        x3_z = satellite_data.accelerometer_z_data
        c3_x.setData(y3,x3_x)
        c3_y.setData(y3,x3_y)
        c3_z.setData(y3,x3_z)
        # LIS
        y4  = satellite_data.time_LIS_data
        x4_x = satellite_data.magnetometer_x_data
        x4_y = satellite_data.magnetometer_y_data
        x4_z = satellite_data.magnetometer_z_data
        c4_x.setData(y4,x4_x)
        c4_y.setData(y4,x4_y)
        c4_z.setData(y4,x4_z)
    elif tab_index == 1:
        satellite_data = SatelliteData(sh.satellites_data['luna_sat2'])
        # BME
        y1 = satellite_data.time_BME_data
        x1 = satellite_data.temp_data
        c1.setData(y1, x1)
        y2 = y1
        x2 = satellite_data.humidity_data
        c2.setData(y2, x2)
        # ADXL
        y3   = satellite_data.time_ADXL_data
        x3_x = satellite_data.accelerometer_x_data
        x3_y = satellite_data.accelerometer_y_data
        x3_z = satellite_data.accelerometer_z_data
        c3_x.setData(y3,x3_x)
        c3_y.setData(y3,x3_y)
        c3_z.setData(y3,x3_z)
        # LIS
        y4  = satellite_data.time_LIS_data
        x4_x = satellite_data.magnetometer_x_data
        x4_y = satellite_data.magnetometer_y_data
        x4_z = satellite_data.magnetometer_z_data
        c4_x.setData(y4,x4_x)
        c4_y.setData(y4,x4_y)
        c4_z.setData(y4,x4_z)
    elif tab_index == 2:
        satellite_data = SatelliteData(sh.satellites_data['luna_sat3'])
        # BME
        y1 = satellite_data.time_BME_data
        x1 = satellite_data.temp_data
        c1.setData(y1, x1)
        y2 = y1
        x2 = satellite_data.humidity_data
        c2.setData(y2, x2)
        # ADXL
        y3   = satellite_data.time_ADXL_data
        x3_x = satellite_data.accelerometer_x_data
        x3_y = satellite_data.accelerometer_y_data
        x3_z = satellite_data.accelerometer_z_data
        c3_x.setData(y3,x3_x)
        c3_y.setData(y3,x3_y)
        c3_z.setData(y3,x3_z)
        # LIS
        y4  = satellite_data.time_LIS_data
        x4_x = satellite_data.magnetometer_x_data
        x4_y = satellite_data.magnetometer_y_data
        x4_z = satellite_data.magnetometer_z_data
        c4_x.setData(y4,x4_x)
        c4_y.setData(y4,x4_y)
        c4_z.setData(y4,x4_z)
        
    elif tab_index == 3:
        satellite_data = SatelliteData(sh.satellites_data['luna_sat4'])
        # BME
        y1 = satellite_data.time_BME_data
        x1 = satellite_data.temp_data
        c1.setData(y1, x1)
        y2 = y1
        x2 = satellite_data.humidity_data
        c2.setData(y2, x2)
        # ADXL
        y3   = satellite_data.time_ADXL_data
        x3_x = satellite_data.accelerometer_x_data
        x3_y = satellite_data.accelerometer_y_data
        x3_z = satellite_data.accelerometer_z_data
        c3_x.setData(y3,x3_x)
        c3_y.setData(y3,x3_y)
        c3_z.setData(y3,x3_z)
        # LIS
        y4  = satellite_data.time_LIS_data
        x4_x = satellite_data.magnetometer_x_data
        x4_y = satellite_data.magnetometer_y_data
        x4_z = satellite_data.magnetometer_z_data
        c4_x.setData(y4,x4_x)
        c4_y.setData(y4,x4_y)
        c4_z.setData(y4,x4_z)
    elif tab_index == 4:
        satellite_data = SatelliteData(sh.satellites_data['luna_sat5'])
        # BME
        y1 = satellite_data.time_BME_data
        x1 = satellite_data.temp_data
        c1.setData(y1, x1)
        y2 = y1
        x2 = satellite_data.humidity_data
        c2.setData(y2, x2)
        # ADXL
        y3   = satellite_data.time_ADXL_data
        x3_x = satellite_data.accelerometer_x_data
        x3_y = satellite_data.accelerometer_y_data
        x3_z = satellite_data.accelerometer_z_data
        c3_x.setData(y3,x3_x)
        c3_y.setData(y3,x3_y)
        c3_z.setData(y3,x3_z)
        # LIS
        y4  = satellite_data.time_LIS_data
        x4_x = satellite_data.magnetometer_x_data
        x4_y = satellite_data.magnetometer_y_data
        x4_z = satellite_data.magnetometer_z_data
        c4_x.setData(y4,x4_x)
        c4_y.setData(y4,x4_y)
        c4_z.setData(y4,x4_z)
    else:
        print("Tab index out of range")



if __name__ == '__main__':
    user_in = input("Enter desired number of Lunasats: ")
    create_plots(user_in)
    
