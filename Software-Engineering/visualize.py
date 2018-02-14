'''
Software Project - CPU scheduler simulator
This file include show visualization
Create by Yedarm Seong
2015-12-04
'''

import numpy as np
import matplotlib.pyplot as plt
from pandas import Series, DataFrame
import pandas as pd
from numpy import reshape

# 리스트를 numpy.array 형태로 변환하여 반환합니다.
def List2Array(list):
    x = []

    for i in range(0, len(list)):
        if list[i][0] == 1:
            x.append(int(list[i][1]) + int(list[i][3]))
        else:
            x.append(int(list[i][1]) + int(list[i][2]))
    x = np.asarray(x)

    return x

# 사용자에게 스케줄링 결과를 스택 바 형태로 보여주는 함수입니다.
def Show(successedList, failedList, taskCount, duration):
    try: successedLast = int(successedList[len(successedList) - 1][1]) + \
        int(successedList[len(successedList) - 1][3])
    except: successedLast = 0

    try: failedLast = int(failedList[len(failedList) - 1][1]) + \
        int(failedList[len(failedList) - 1][2])
    except: failedLast = 0

    last = max(successedLast, failedLast)
    index = ["Success 1-" + str(duration), "Failed 1-" + str(duration)]
    x = List2Array(successedList)
    y = List2Array(failedList)
    mat = [x, y]
    columnsIndex = []

    for i in range(0, len(successedList)):
        if successedList[i][0] == 1:
            tmp = "task" + str(successedList[i][5]) + " (" + str(successedList[i][1]) + ',' + str(successedList[i][1] + successedList[i][3]) + ')'
        else:
            tmp = "task" + str(successedList[i][4]) + " (" + str(successedList[i][1]) + ',' + str(successedList[i][1] + successedList[i][2]) + ')'

        columnsIndex.append(tmp)

    df = DataFrame(mat, index, columnsIndex)
    df.plot(kind='barh', stacked=True, alpha=0.5)

    plt.show()
