import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

class LunaSat:
    def __init__(self, value, x, y, z, color):
        self.value = value
        self.position = (x, y, z)
        self.color = color

lunasats = [1, 2, 3]

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
    fig = plt.figure(facecolor='black')  # Setting the background color to black
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
    plt.show()

visual_connections(lunasats)
