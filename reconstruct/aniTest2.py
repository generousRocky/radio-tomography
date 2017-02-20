"""

This simple python codes reconstruct the image from raw data of RSSIs.

The RSSI values are read as a text file that written by "serial.py" python code.

There also two important matrix file to make image, weight matrix and covariance matrix.

These two matrix are made by "weightMatrix.py" python file as a text file.

16 Jan 2017


Heerak Lim (rocky).

rocky92.lim@gmail.com
Rocky.Lim@data61.csiro.au

"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.animation as animation


NUM_OF_NODE = 20
NUM_OF_LINK = NUM_OF_NODE*(NUM_OF_NODE-1)
RAW = 20
NUM_OF_VOXEL = RAW*RAW

DIFF_WITH_PREVIOUS_CYCLE = NUM_OF_NODE*1
FRAME_INTERVAL = 50
NOISE_VARIANCE = 10


# weight matrix
W = pd.read_csv("weightMatrix.txt", header=None, index_col=False)
W_inverse = pd.DataFrame(np.linalg.pinv(W.values), W.columns, W.index)
W_transpose = W.transpose()

# covariance matrix
Cx = pd.read_csv("covarianceMatrix.txt", header=None, index_col=False)
Cx_inverse = pd.DataFrame(np.linalg.pinv(Cx.values), Cx.columns, Cx.index)

# matrix operation
mat = W_transpose.dot(W) + NOISE_VARIANCE * Cx_inverse
mat_inverse = pd.DataFrame(np.linalg.pinv(mat.values), mat.columns, mat.index)
MAT = mat_inverse.dot(W_transpose)

# raw data text file from serial
fileName = "lib.txt"

raw = pd.read_table(fileName, sep="\s+", header=None)
table = raw.iloc[0:, 1:] # detatch time
table.columns = [range(len(table.columns))]


afterDF = pd.DataFrame(data=np.zeros((NUM_OF_LINK, 1), dtype=np.float))


fig = plt.figure()

x = np.linspace(0, NUM_OF_NODE, NUM_OF_NODE)
y = np.linspace(0, NUM_OF_NODE, NUM_OF_NODE).reshape(-1, 1)
im = plt.matshow(x+y, fignum=0 , vmin=0, vmax=3)

#frameCount=0

def updatefig(*args):

    try: updatefig.i
    except: updatefig.i = 0

    try: updatefig.frameCount
    except: updatefig.frameCount = 0

    try : updatefig.beforeDF
    except : updatefig.beforeDF = pd.DataFrame(data=np.zeros((NUM_OF_LINK, 1), dtype=np.float))

    try: updatefig.Y = pd.DataFrame(data=np.zeros((NUM_OF_LINK, 1), dtype=np.float))
    except: updatefig.Y = pd.DataFrame(data=np.zeros((NUM_OF_LINK, 1), dtype=np.float))

    for p in range(NUM_OF_NODE):


        transmitter = table.iloc[updatefig.i][1]
        rssi = table.loc[updatefig.i][2:]

        #rssiDF = rssi.replace(0, np.nan)
        rssiDF = rssi.replace(0, -99)

        rssiArr = rssiDF.as_matrix().reshape(NUM_OF_NODE, 1)


        linkCount =0
        for ii in range(NUM_OF_NODE):
            if transmitter !=ii:
                afterDF.iloc[transmitter * (NUM_OF_NODE - 1) + linkCount] = np.nanmean(rssiArr[ii])

                linkCount = linkCount + 1
        # end for


        if updatefig.frameCount % DIFF_WITH_PREVIOUS_CYCLE == 0:
            updatefig.Y += ((updatefig.beforeDF - afterDF).copy(deep=True)).abs()
            updatefig.beforeDF = afterDF.copy(deep=True)

        updatefig.frameCount += 1

        updatefig.i += 1
    # end for p

    X = (MAT.dot(updatefig.Y)).as_matrix().reshape(RAW, RAW)
    im.set_array(np.abs(X))
    plt.title(raw.iloc[updatefig.i][0])

    return im,

ani = animation.FuncAnimation(fig, updatefig, interval=FRAME_INTERVAL)
plt.colorbar()
plt.show()