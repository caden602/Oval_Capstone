import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Define the coordinates of the objects
object1 = (1, 2, 3)
object2 = (4, 5, 6)
object3 = (7, 8, 9)

# Create a new figure
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Scatter plot for each object
ax.scatter(*object1, color='r', label='Object 1')
ax.scatter(*object2, color='g', label='Object 2')
ax.scatter(*object3, color='b', label='Object 3')

# Set labels and title
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
ax.set_title('3D Scatter Plot')

# Add a legend
ax.legend()

# Show plot
plt.show()
