import numpy as np
import matplotlib
# matplotlib.use("Agg")
from matplotlib import pyplot as plt
from matplotlib import animation
import sys
import Tkinter, tkFileDialog
import os.path 
import mpl_toolkits.mplot3d.axes3d as p3

u = 20
numPhones = 10

if len(sys.argv) == 1:
    print "You can also give filename as a command line argument."
    print "Double-click on the directory of the run you want to analyze"
    root = Tkinter.Tk()
    root.withdraw()
    full_path = tkFileDialog.askdirectory(initialdir="./")
    print full_path
    runname = os.path.split(full_path)[1]
    rundir = os.path.split(full_path)[0]
    print runname
    #runname = raw_input("Enter Run Name: ")
else:
    runname = sys.argv[1]
    isMean = sys.argv[2]

# this will be a python array of numpy matrices, indexed by the MS index. 
# Each numpy matrix has 4 columns, (time, X coordinate, Y coordinate,RSSI)

allTheData = range(numPhones)
# get all the RSSI values in there
for i in range(1,numPhones*3+1):
	filename = rundir +"/" + runname + "/" + runname + "-" + str(i) + ".csv"
	currentData = np.genfromtxt(filename, dtype=float, names=True, delimiter=",")
	
	datacolumnname = currentData.dtype.names[1]
	print datacolumnname
	MSindex = int(datacolumnname[8])
	if datacolumnname.find("RSSI") != -1 :
		allTheData[MSindex] = np.zeros((currentData['time'].size,4))
		allTheData[MSindex][:,3] = currentData[datacolumnname] 
		allTheData[MSindex][:,0] = currentData['time'] 
		print allTheData[MSindex]

for i in range(1,numPhones*3):
	filename = rundir +"/" + runname + "/" + runname + "-" + str(i) + ".csv"
	currentData = np.genfromtxt(filename, dtype=float, names=True, delimiter=",")
	
	datacolumnname = currentData.dtype.names[1]
	#print datacolumnname
	MSindex = int(datacolumnname[8])
	if datacolumnname.find("RSSI") == -1 :
		# interpolate each point to the RSSI value
		print MSindex
		print allTheData[MSindex]
		for j in range(0,allTheData[MSindex][:,0].size):
			# find closest value to the time in the X and Y coordinate values
			v = allTheData[MSindex][j,0]
			idx = (np.abs(currentData['time'] - v)).argmin()
			# if it's an X coordinate
			if datacolumnname.find("coordX") != -1 :
				allTheData[MSindex][j,1] = currentData[datacolumnname][idx]
			# if it's a y coordinate
			elif datacolumnname.find("coordY") != -1 :
				allTheData[MSindex][j,2] = currentData[datacolumnname][idx]
		print allTheData[MSindex]
	
	#print allTheData
	#indivs = np.genfromtxt(filename, dtype=None, names=True, delimiter=",")

# First set up the figure, the axis, and the plot element we want to animate
fig = plt.figure()
ax = p3.Axes3D(fig)
points = []
for _ in range(0,numPhones):
	points += ax.plot([], [], [], 'bo')
ax.set_xlim((0, 1000))
ax.set_ylim((0, 1000))
ax.set_zlim((0, 0.001))
print points
# initialization function: plot the background of each frame
def init():
    for pt in points:
        pt.set_data([], [])
        pt.set_3d_properties([])
    #bestpoints.set_data([], [])
    return points

# animation function.  This is called sequentially

dummy = np.zeros((3,3))
def animate(i):
	for MS in range(0,numPhones):
		currentMSData = allTheData[MS]
		points[MS].set_data(currentMSData[i,1], currentMSData[i,2])
		points[MS].set_3d_properties(currentMSData[i,3])
	fig.canvas.draw()
	return points

# call the animator.  blit=True means only re-draw the parts that have changed.
anim = animation.FuncAnimation(fig, animate, init_func=init,
                               frames=allTheData[0][:,0].size, interval=400, blit=True)

# save the animation as an mp4.  This requires ffmpeg or mencoder to be
# installed.  The extra_args ensure that the x264 codec is used, so that
# the video can be embedded in html5.  You may need to adjust this for
# your system: for more information, see
# http://matplotlib.sourceforge.net/api/animation_api.html
anim.save(filename + '_animation.mp4', fps=7)

plt.show()
