'''
Software Project - CPU scheduler simulator
This file include scheduler, task parser
Create by Yedarm Seong
2015-12-04
'''

'''
# RM - Rate monotonic
# 정적 스케줄링 방식으로, 주기가 짧은 것을 우선순위로 스케줄링합니다.
# 주기적인 Task들만 고려합니다.
# 제 코드에서는 먼저 들어왔더라도, 앞의 Task가 완료된 시점에서
# 주기만 보고 판단하도록 되어 있습니다.

# EDF - Eearlist deadLine first
# 주기적, 비 주기적 Task들을 모두 고려합니다.
# 동적 스케줄링 방식으로, 마감 한계가 빠른 순서대로 스케줄링합니다.

# User - 사용자 우선순위 지정 알고리즘
# 주기적, 비 주기적 Task들을 모두 고려합니다.
# 사용자가 지정한 우선순위대로 스케줄링합니다.
'''

# Deep copy를 위한 라이브러리
import copy

# 파일로부터 Duration을 파싱하여 반환합니다.
def DurationParser(input):
    n = input.find(':')
    x = int(input[n + 2: len(input) - 1])

    return x

# 인자로 받은 파일로부터 주기적인 Task, 비 주기적인 Task를 파싱하여 반환합니다.
def TaskParser(f):
    taskList = []
    periodicCount = 0
    aperiodicCount = 0

    while True:
        line = f.readline()

        if not line: break

        if line[0] == 'P': periodicCount += 1
        else: aperiodicCount += 1

        n = line.find('(')
        m = line.find(')', n)
        x = line[n + 1: m]
        task = []

        while True:
            q = x.find(',')

            if q == -1:
                arg = x[0: m]
                task.append(int(arg))
                x = x[q +2: len(x)]
                break

            arg = x[0: q]
            task.append(int(arg))
            x = x[q + 2: len(x)]

        taskList.append(task)

    return taskList, periodicCount, aperiodicCount

# 입력받은 파일과 duration으로 Task들을 발생시켜 반환합니다.
# 각각의 Task의 뒤에 몇 번째 Instance인지 번호로 표기합니다.
# 비 주기적 Task의 경우 Instance를 0으로 표기합니다.
def TaskGenerator(taskList, duration):
    periodId = 1
    nonPeriodId = 1
    instance = 1
    periodList = []
    nonPeriodList = []
    tmpList = []
    loopValue = 0

    for i in range (0, len(taskList)):
        deadLineAlpha = 0
        list = copy.deepcopy(taskList[i])

        if len(list) == 3:
            list.append(nonPeriodId)
            nonPeriodId += 1
            list.insert(0, 0)
            list[3] += list[1]
            tmpList.append(list)
            nonPeriodList.append(tmpList)
            list = []
            tmpList = []
            continue

        else:
            deadLineAlpha = list[0]
            list.insert (0, 1)
            list.append(periodId)
            list.append(instance)
            periodId += 1
            list[4] += deadLineAlpha
            tmpList.append(list)
            deadLineAlpha = list[2]

            # 리스트 연결
            while loopValue < duration:
                list = copy.deepcopy(list)
                # ((flag of period, nonPeriod), start, pri, excuteTime, deadLine, periodId, instance)
                list[1] += list[2]
                list[4] += deadLineAlpha
                list[6] += 1
                tmpList.append(list)
                loopValue = list[1]

            del(tmpList[len(tmpList) - 1])

        loopValue = 0
        periodList.append(tmpList)
        tmpList = []

    return periodList, nonPeriodList

# 리스트에서 비어있는 항목들을 제거합니다.
def RemoveEmptyList(list):
    for i in range (0, len(list)):
        try:
            if len(list[i]) == 0: list.remove(list[i])
        except: pass

    return list

# 실질적으로 스케줄링을 진행하는 함수입니다.
# 인자로 받은 Tasks 들로부터 duration만큼 스케줄링을 사용합니다.
# 인자 attr는 코드 재사용을 위한 인자로 스케줄링할 모드를 나타냅니다.
# 속성에는 "RM", "EDF", "USER" 세 가지가 있고 각각의 모드를 지원합니다.
# 각각 스케줄링 알고리즘을 구현하기보다
# 재사용할 수 있는 코드들을 재사용하는 것이 효율적이라 생각했습니다.
def Scheduling(tasks, duration, attr):
    '''
    x = [
        [[1, 12, 20, 5, 32, 1, 1], [1, 32, 20, 5, 52, 1, 2]],
        [[1, 12, 40, 12, 47, 2, 1], [1, 52, 40, 12, 87, 2, 2]],
        [[0, 57, 5, 77, 1]],
        [[1, 40, 30, 7, 70, 3, 1], [1, 70, 30, 7, 100, 3, 2]],
        [[0, 1200, 53, 1400, 2]]
        ]
    '''

    # tasks 자체에서 직접 스케줄링을 하는 것보다, Deep copy하여 사용하는 것이
    # 안정성 면에서 우수합니다.
    scheduleSuccess = []
    scheduleFailed = []
    taskList = []

    # RM 알고리즘의 경우 주기적인 작업들만 고려합니다.
    if attr == "RM":
        for i in range(0, len(tasks)):
            if tasks[i][0][0] == 1:
                tmpList = copy.deepcopy(tasks[i])
                taskList.append(tmpList)

    else: taskList = copy.deepcopy(tasks)

    while True:
        startTime = taskList[0][0][1]
        period = taskList[0][0][2]
        excuteTime = taskList[0][0][3]
        deadLine = taskList[0][0][4]
        endTime = startTime + excuteTime
        flag = 0

        # 이전 수행한 작업의 종료시간을 저장합니다.
        try:
            if scheduleSuccess[len(scheduleSuccess) - 1][0] == 1:
                prevEndTime = scheduleSuccess[len(scheduleSuccess) - 1][1] + \
                              scheduleSuccess[len(scheduleSuccess) - 1][3]
            else:
                prevEndTime = scheduleSuccess[len(scheduleSuccess) - 1][1] + \
                              scheduleSuccess[len(scheduleSuccess) - 1][2]

        except: pass

        # RM 알고리즘은 주기를 통해 우선순위를 지정합니다.
        if attr == "RM":
            for i in range(1, len(taskList)):
                if startTime > taskList[i][0][1]:
                    startTime = taskList[i][0][1]
                    period = taskList[i][0][2]
                    excuteTime = taskList[i][0][3]
                    deadLine = taskList[i][0][4]
                    endTime = startTime + excuteTime
                    flag = i

                elif startTime == taskList[i][0][1] and \
                                period > taskList[i][0][2]:
                        startTime = taskList[i][0][1]
                        period = taskList[i][0][2]
                        excuteTime = taskList[i][0][3]
                        deadLine = taskList[i][0][4]
                        endTime = startTime + excuteTime
                        flag = i

        # EDF 알고리즘은 마감 한계를 통해 우선순위를 지정합니다.
        elif attr == "EDF":
            for i in range(1, len(taskList)):
                if taskList[i][0][0] == 0:
                    if startTime > taskList[i][0][1]:
                        startTime = taskList[i][0][1]
                        excuteTime = taskList[i][0][2]
                        deadLine = taskList[i][0][3]
                        endTime = startTime + excuteTime
                        flag = i

                    elif startTime == taskList[i][0][1] and \
                                deadLine > taskList[i][0][4]:
                        startTime = taskList[i][0][1]
                        excuteTime = taskList[i][0][2]
                        deadLine = taskList[i][0][3]
                        endTime = startTime + excuteTime
                        flag = i
                    continue

                if startTime > taskList[i][0][1]:
                    startTime = taskList[i][0][1]
                    excuteTime = taskList[i][0][3]
                    deadLine = taskList[i][0][4]
                    endTime = startTime + excuteTime
                    flag = i

                elif startTime == taskList[i][0][1] and \
                                deadLine > taskList[i][0][4]:
                        startTime = taskList[i][0][1]
                        excuteTime = taskList[i][0][3]
                        deadLine = taskList[i][0][4]
                        endTime = startTime + excuteTime
                        flag = i

        # 사용자 우선순위 알고리즘의 경우 마감 한계를 고려하고
        # 동시에 도착한 작업의 경우 하나의 작업만 수행합니다.
        if attr == "USER":
            for i in range(1, len(taskList)):
                if taskList[i][0][0] == 0:
                    if startTime > taskList[i][0][1]:
                        startTime = taskList[i][0][1]
                        excuteTime = taskList[i][0][2]
                        deadLine = taskList[i][0][3]
                        endTime = startTime + excuteTime
                        flag = i

                    elif startTime == taskList[i][0][1] and \
                                deadLine > taskList[i][0][4]:
                        startTime = taskList[i][0][1]
                        excuteTime = taskList[i][0][2]
                        deadLine = taskList[i][0][3]
                        endTime = startTime + excuteTime
                        flag = i
                    continue

                if startTime > taskList[i][0][1]:
                    startTime = taskList[i][0][1]
                    excuteTime = taskList[i][0][3]
                    deadLine = taskList[i][0][4]
                    endTime = startTime + excuteTime
                    flag = i

                elif startTime == taskList[i][0][1] and \
                                deadLine > taskList[i][0][4]:
                        startTime = taskList[i][0][1]
                        excuteTime = taskList[i][0][3]
                        deadLine = taskList[i][0][4]
                        endTime = startTime + excuteTime
                        flag = i

        RemoveEmptyList(taskList)

        try:
            if endTime > deadLine or prevEndTime > startTime:
                scheduleFailed.append(taskList[flag][0])
                taskList[flag].remove(taskList[flag][0])
                continue

        except:
            if endTime > deadLine:
                scheduleFailed.append(taskList[flag][0])
                taskList[flag].remove(taskList[flag][0])
                continue

        scheduleSuccess.append(taskList[flag][0])
        taskList[flag].remove(taskList[flag][0])

        RemoveEmptyList(taskList)

        # 수행될 수 없는 작업들을 처리합니다.
        if attr == "RM":
            for i in range(0, len(taskList)):
                for j in range(0, len((taskList[i]))):
                    if taskList[i][j][1] < endTime:
                        tmp = taskList[i][j][1]
                        taskList[i][j][1] = endTime

                        if taskList[i][j][1] + taskList[i][j][3] > \
                                taskList[i][j][4]:
                            taskList[i][j][1] = tmp
                            scheduleFailed.append(taskList[i][j])
                            taskList[i].remove(taskList[i][j])

        elif attr == "EDF":
            for i in range(0, len(taskList)):
                try:
                    for j in range(0, len(taskList[i])):
                        if taskList[i][j][0] == 0:
                            if taskList[i][j][1] < endTime:
                                tmp = taskList[i][j][1]
                                taskList[i][j][1] = endTime

                                if taskList[i][j][1] + taskList[i][j][2] > \
                                        taskList[i][j][3]:
                                    taskList[i][j][1] = tmp
                                    scheduleFailed.append(taskList[i][j])
                                    taskList[i].remove(taskList[i][j])

                        else:
                            if taskList[i][j][1] < endTime:
                                tmp = taskList[i][j][1]
                                taskList[i][j][1] = endTime

                                if taskList[i][j][1] + taskList[i][j][3] > \
                                        taskList[i][j][4]:
                                    taskList[i][j][1] = tmp
                                    scheduleFailed.append(taskList[i][j])
                                    taskList[i].remove(taskList[i][j])

                except: pass

        elif attr == "USER":
            for i in range(0, len(taskList)):
                try:
                    for j in range(0, len(taskList[i])):
                        if taskList[i][j][0] == 0:
                            if taskList[i][j][1] < endTime:
                                scheduleFailed.append(taskList[i][j])
                                taskList[i].remove(taskList[i][j])

                        else:
                            if taskList[i][j][1] < endTime:
                                scheduleFailed.append(taskList[i][j])
                                taskList[i].remove(taskList[i][j])

                except: pass
        RemoveEmptyList(taskList)

        # 종료 조건문입니다.
        if deadLine > duration: break;

    return scheduleSuccess, scheduleFailed
