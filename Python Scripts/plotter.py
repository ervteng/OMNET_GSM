import numpy as np
import matplotlib
# matplotlib.use("Agg")
from matplotlib import pyplot as plt
from matplotlib import animation
from matplotlib import cm, ticker
import sys
import Tkinter, tkFileDialog
import os.path 
from scipy.interpolate import griddata
from mpl_toolkits.mplot3d import Axes3D

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
	#print datacolumnname
	MSindex = int(datacolumnname[8])
	if datacolumnname.find("RSSI") != -1 :
		allTheData[MSindex] = np.zeros((currentData['time'].size,4))
		allTheData[MSindex][:,3] = currentData[datacolumnname] 
		allTheData[MSindex][:,0] = currentData['time'] 
		#print allTheData[MSindex]

for i in range(1,numPhones*3):
	filename = rundir +"/" + runname + "/" + runname + "-" + str(i) + ".csv"
	currentData = np.genfromtxt(filename, dtype=float, names=True, delimiter=",")
	
	datacolumnname = currentData.dtype.names[1]
	MSindex = int(datacolumnname[8])
	if datacolumnname.find("RSSI") == -1 :
		# interpolate each point to the RSSI value
		#print MSindex
		#print allTheData[MSindex]
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
		#print allTheData[MSindex]
	
	#print allTheData
	#indivs = np.genfromtxt(filename, dtype=None, names=True, delimiter=",")

# First set up the figure, the axis, and the plot element we want to animate
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
points = []
#for _ in range(0,numPhones):
#	points += ax.plot([], [], [], 'bo')
ax.set_xlim((0, 400))
ax.set_ylim((0, 300))
ax.set_zlim((0, 0.2))
ax.zaxis.set_scale('log')
#print points
# initialization function: plot the background of each frame
print allTheData[0]

graphableData = allTheData[0]
for MS in range(1,numPhones):
	currentMSData = allTheData[MS]
	print MS
	print currentMSData
	print np.concatenate([graphableData,currentMSData])
	#points[MS].set_data(currentMSData[i,1], currentMSData[i,2])
	#points[MS].set_3d_properties(currentMSData[i,3])
X = [3,2,3]
Y = [3,2,3]
Z = [3,2,3]
grid_x, grid_y = np.mgrid[0:400:100j, 0:300:100j]
grid_z2 = griddata((graphableData[:,1],graphableData[:,2]), graphableData[:,3], (grid_x, grid_y), method='linear')
ax.plot_surface(grid_x, grid_y, grid_z2, rstride=5, cstride=5, cmap=cm.coolwarm, vmin=0, vmax = 0.004)

plt.figure()
plt.contourf(grid_x,grid_y,grid_z2,50, locator=ticker.LogLocator())
plt.show()
