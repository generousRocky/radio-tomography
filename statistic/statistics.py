import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import os

TOTAL_NODE_NUM = 8
ITERATION = 1
FILE_NAME = "donga.txt"
START =0
END =99999999

zeroDF = pd.DataFrame(data=np.zeros((ITERATION, TOTAL_NODE_NUM), dtype=np.int16))
linkDF = {}

for i in range(TOTAL_NODE_NUM):
    linkDF[i] = zeroDF


def csvToLinkDF(fileName):
    dirName = fileName.replace(".txt", "")
    for i in range(TOTAL_NODE_NUM):
        temp = pd.read_csv("%s/linkDF[%s].csv" % (dirName, i,), header=None, index_col=False)
        linkDF[i]= temp.replace(0, np.nan)


def makeLinkDF(fileName):
    raw = pd.read_table(fileName, sep="\s+", header=None)

    time = raw.ix[0:][0]
    print time

    table = raw.iloc[0:, 1:]
    transmitter = table.ix[0:][2]
    table.columns = [range(len(table.columns))]

    lenth = len(table)
    dirName = fileName.replace(".txt", "")

    if not os.path.exists(dirName):
        os.makedirs(dirName)

    for i in range(lenth):
        print i
        rssi = table.loc[i][2:]
        tempDF = zeroDF
        rssi2 = rssi.reset_index(level=0, drop=True)

        for j in range(TOTAL_NODE_NUM):
            k = j * ITERATION
            tempDF.loc[:, j] = pd.Series(rssi2[k:k +ITERATION].reset_index(level=0, drop=True), index=None)
        # end for j

        if i != 0:
            lossToMaster = ((TOTAL_NODE_NUM - 1) - transmitter[i - 1] + transmitter[i]) % TOTAL_NODE_NUM
            if lossToMaster > 0:
                for t in range(lossToMaster):
                    nowLinkIndex = transmitter[i]
                    temp = linkDF[(nowLinkIndex + (TOTAL_NODE_NUM - 1) - t) %
                                  TOTAL_NODE_NUM].append(zeroDF,ignore_index=True)  # fill 0 DF for loss
                    linkDF[(nowLinkIndex + (TOTAL_NODE_NUM - 1) - t) % TOTAL_NODE_NUM] = temp


        temp = linkDF[transmitter[i]].append(tempDF, ignore_index=True)
        linkDF[transmitter[i]] = temp

    for i in range(TOTAL_NODE_NUM):
        linkDF[i].to_csv("%s/linkDF[%s].csv" % (dirName, i,),index=False, header=False)


def showLinkDF(start, end):
    k=0
    for i in range(TOTAL_NODE_NUM):
        #plt.figure(i)
        for j in range(TOTAL_NODE_NUM):

            if i != j:
                plt.figure(k)
                k = k+1


                linkDF[i][j][start:end].plot(ylim=(-90,-30))


                title = "RSSI that Node %s Received" % (i,)
                plt.title(title)
                plt.legend()


    plt.show()


def showLinkDiff():
    for i in range(TOTAL_NODE_NUM):
        for j in range(TOTAL_NODE_NUM):
            if i < j:
                plt.figure(i)
                colText = "%s to %s - %s to %s" % (j, i, i, j,)
                diff = pd.DataFrame(data=linkDF[i][j] - linkDF[j][i], columns=[colText])

                Mean = diff.mean()
                std = diff.std()
                print ("Mean: %s\nSTD: %s" % (Mean, std,))
                #print (diff)

                diff.plot()
                plt.legend()
                plt.title(colText)
                plt.draw()
    plt.show()


def showPacketLoss():
    lossDF = pd.DataFrame(data=np.zeros((1, 1)), dtype=np.float32)
    k = 0
    for i in range(TOTAL_NODE_NUM):
        df = linkDF[i]
        for j in range(TOTAL_NODE_NUM):
            if i != j:
                lossCount = df[j].isnull().sum()
                print "loss form %s to %s :%s of %s" % (j, i, lossCount, len(df[j]),)
                lossDF.loc[:, k] = pd.Series([float(lossCount) / len(df) * 100.0])

                colText = "%d to %d" % (j, i,)
                lossDF = lossDF.rename(columns={k: colText})
                k = k + 1

    x = lossDF.columns.values
    y = lossDF.iloc[0]
    s = pd.Series(y, index=x)
    s.plot.bar()


    plt.xlabel("LINK")
    plt.ylabel("Fialure Rate(%)")
    #plt.legend()
    plt.grid()
    plt.show()


# bar graph
def meanStd():
    for i in range(TOTAL_NODE_NUM):
        plt.figure(i)

        link = np.array(range(TOTAL_NODE_NUM))
        y_pos = np.arange(len(link))
        data = np.array(linkDF[i].mean())
        error = np.array(linkDF[i].std())

        plt.barh(y_pos, data, xerr=error, align='center', alpha=0.4)
        plt.yticks(y_pos, link)
        plt.xlabel('RSSI')
        title = "Mean and STD of RSSI that Node %s Received" % (i,)
        plt.title(title)

        for j in range(TOTAL_NODE_NUM):
            if i != j:
                text = "Mean: %s, STD: %s" % (data[j], error[j],)
                print (text)
                plt.text(-40, j, text)

        plt.draw()
    plt.show()


def showBoxPlot(EVERY):
    for i in range(TOTAL_NODE_NUM):
        for j in range(TOTAL_NODE_NUM):
            targetNode = 1
            if i == targetNode & i !=j:
                df = linkDF[i]
                df.loc[:, TOTAL_NODE_NUM] = pd.Series((df.index / EVERY))

                new = pd.DataFrame(data=df[j], index=range(EVERY * ((len(df[j]) / EVERY) + 1)))
                new.loc[:, 'group'] = pd.Series(new.index / EVERY)

                k = ((len(df[j]) / EVERY) + 1)
                new.columns = [0, 'group']

                sub = pd.DataFrame()
                for k in range(k):
                    temp = pd.DataFrame(data=new[0][EVERY * k:EVERY * k + EVERY], columns=[k])
                    temp = temp.reset_index(drop=True)
                    sub = pd.concat([sub, temp], axis=1)

                print sub
                sub.plot.box()
                plt.legend()
                colText = "%d to %d" % (j, i,)
                plt.title(colText)
                plt.draw()

    plt.show()


#makeLinkDF(FILE_NAME)
csvToLinkDF(FILE_NAME)

showLinkDF(START, END)
showLinkDiff()
#showPacketLoss()
# meanStd()
# showBoxPlot(3)
