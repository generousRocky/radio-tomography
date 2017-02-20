"""

This simple python codes make weight matrix and covariance matrix for image reconstruction in radio tomography

The environment for radio tomography and image reconstruction is considered square-based (same width and height)

16 Jan 2017


Heerak Lim (rocky).

rocky92.lim@gmail.com
Rocky.Lim@data61.csiro.au

"""


import numpy as np
import pandas as pd
import math


# Node Configuration
NUM_OF_NODE = 20
NUM_OF_LINK = NUM_OF_NODE*(NUM_OF_NODE-1)
NUM_OF_ONESIDE = NUM_OF_NODE/4 + 1

# Reconstruction Image Configuration
RAW = 20  # RAW means the number of voxel consist one line in the reconstructed image
NUM_OF_VOXEL = RAW*RAW
NORMALIZED_PIXEL_WITH = 0.984  # (m)
WIDTH_OF_WEIGHTING_ELLIPSE = 0.02  # parameter describing the with of the ellipse
PIXEL_CORRELATION_CONSTANT = 2
PIXEL_VARIANCE = 0.4

# number of raw recommended to exceed with the NODE_IN_LINE
if (RAW<NUM_OF_ONESIDE):
    exit(0)


def nodeDistance(x1, y1, x2, y2):
    return math.sqrt(pow(x1-x2,2)+pow(y1-y2,2))


# the followings are to define coordinate of nodes
node_coordinate = pd.DataFrame(data=np.zeros( (NUM_OF_ONESIDE*2+(NUM_OF_ONESIDE-2)*2 , 2) , dtype=np.float), columns=['x','y'] )

for i in range(NUM_OF_ONESIDE):
    node_coordinate.iloc[i] = [i,0]

k, j = 1, 1
for i in range(NUM_OF_ONESIDE, NUM_OF_ONESIDE+(NUM_OF_ONESIDE-2)*2 ):
    left = 0
    right = NUM_OF_NODE/4

    if k % 2 == 1:
        node_coordinate.iloc[i] = [left, j]
    else :
        node_coordinate.iloc[i] = [right, j]
        j = j + 1

    k = k + 1

k = 0
for i in range(NUM_OF_ONESIDE+(NUM_OF_ONESIDE-2)*2, NUM_OF_ONESIDE+(NUM_OF_ONESIDE-2)*2 + NUM_OF_ONESIDE):

    node_coordinate.iloc[i] = [k, NUM_OF_ONESIDE-1]
    k = k + 1

node_coordinate = RAW/(NUM_OF_NODE/4) * node_coordinate
print node_coordinate
# coordinate of nodes is defined


# the followings are to define weight matrix
W = pd.DataFrame(data=np.zeros((NUM_OF_LINK, NUM_OF_VOXEL), dtype=np.float),)
linkCount = 0

for i in range(NUM_OF_NODE):
    for j in range(NUM_OF_NODE):
        # Link of i to j
        print "making weight matrix, link: %s - %s" % (i, j,)
        if i != j:
            for k in range(NUM_OF_VOXEL):
                # left below point of each voxel
                p1 = np.array([k % RAW, k/RAW], dtype=np.float)

                voxel_x = p1[0]+0.5
                voxel_y = p1[1]+0.5

                d1 = nodeDistance(node_coordinate.iloc[i]['x'],
                     node_coordinate.iloc[i]['y'],
                     voxel_x,
                     voxel_y)

                d2 = nodeDistance(node_coordinate.iloc[j]['x'],
                     node_coordinate.iloc[j]['y'],
                     voxel_x,
                     voxel_y)

                dis = nodeDistance(node_coordinate.iloc[i]['x'],
                      node_coordinate.iloc[i]['y'],
                      node_coordinate.iloc[j]['x'],
                      node_coordinate.iloc[j]['y'])

                if d1+d2 < dis + WIDTH_OF_WEIGHTING_ELLIPSE:
                    realDistance = dis * NORMALIZED_PIXEL_WITH
                    W.loc[linkCount, k] = 1 / math.sqrt(realDistance)

            linkCount = linkCount + 1

W.to_csv("weightMatrix.txt",index=False, header=False)



# the followings are to define covariance matrix
Cx = pd.DataFrame(data=np.zeros((NUM_OF_VOXEL,NUM_OF_VOXEL), dtype=np.float))

for i in range(NUM_OF_VOXEL):
    print "making covariance matrix %s/%s" %(i, NUM_OF_VOXEL,)
    for j in range(NUM_OF_VOXEL):
        p1 = np.array([i % RAW, i / RAW], dtype=np.float)
        p2 = np.array([j % RAW, j / RAW], dtype=np.float)

        voxel_x1 = p1[0] + 0.5
        voxel_y1 = p1[1] + 0.5
        voxel_x2 = p2[0] + 0.5
        voxel_y2 = p2[1] + 0.5

        dis = nodeDistance(voxel_x1, voxel_y1, voxel_x2, voxel_y2)
        realDistance = dis * NORMALIZED_PIXEL_WITH

        Cx.iloc[i][j] = PIXEL_VARIANCE * math.exp(-1.0*realDistance/PIXEL_CORRELATION_CONSTANT)

Cx.to_csv("covarianceMatrix.txt", index=False, header=False)

