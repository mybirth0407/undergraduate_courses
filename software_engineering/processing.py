'''
Software Project - CPU scheduler simulator
This file include scheduling processing, file input output
Create by Yedarm Seong
2015-12-04
'''

import Scheduler

# 직접적으로 모드를 받아 스케줄링 하는 함수입니다.
def Scheduling(mode):
    f = open("input.txt", 'r')
    duration = Scheduler.DurationParser(f.readline())
    taskList, periodicCount, aperiodicCount = Scheduler.TaskParser(f)
    periodList, nonPeriodList  = Scheduler.TaskGenerator(taskList, duration)
    list = periodList + nonPeriodList
    result1, result2 = Scheduler.Scheduling(list, duration, mode)

    return result1, result2, periodicCount, aperiodicCount, duration

# 파일 출력 입력이 들어로면, 결과를 result.txt 파일에 출력합니다.
def OutputFile(result1, result2):
    f = open("result.txt", 'w')

    # 스케줄링에 실패한 작업들입니다.
    if len(result2) > 1:
        f.write("Scheduling Failed List is\n")
        for i in range(0, len(result2)):
            if result2[i][0] == 1:
                f.write("periodic Task (" + str(result2[i][5]) + ", " +
                str(result2[i][6]) + "): " + str(result2[i][1]) + ", " +
                str(result2[i][2]) + ", " + str(result2[i][3]) + ", " +
                str(result2[i][4]) + '\n')
            else:
                f.write("Aperiodic Task " + str(result2[i][4]) + ": " + str(result2[i][1]) + ", " + str(result2[i][2]) + ", " +
                str(result2[i][3]) + '\n')
    else:
        f.write("Scheduling Failed List is Empty\n")

    # 스케줄링에 성공한 작업들 입니다.
    f.write("Scheduling Succesed List is\n")
    for i in range(0, len(result1)):
        if result1[i][0] == 1:
            f.write("periodic Task (" + str(result1[i][5]) + ", " +
            str(result1[i][6]) + "): " + str(result1[i][1]) + ", " +
            str(result1[i][2]) + ", " + str(result1[i][3]) + ", " +
            str(result1[i][4]) + '\n')
        else:
            f.write("Aperiodic Task" + str(result1[i][4]) + ": " + str(result1[i][1]) + ", " + str(result1[i][2]) + ", " +
            str(result1[i][3]) + '\n')

    f.close()
