'''
Software Project - CPU scheduler simulator
This file include main windows, and this fiel is main
Create by Yedarm Seong
2015-12-04
'''
import sys
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from PyQt4 import uic
import Visualize
import Processing

# 전역으로 사용할 변수입니다.
# 클래스 함수 내부에서 사용할 경우 global로 표기해주어야 합니다.
form_class = uic.loadUiType("ui.ui")[0]
successedList = []
failedList = []
mode = ""
f = open("result.txt", 'w')
taskCount = 0
periodicCount = 0
aperiodicCount = 0
duration = 0

# 주 화면 클래스입니다.
class MyWindow(QMainWindow, form_class):
    # super를 이용하여 생성자를 초기화합니다.
    def __init__(self):
        super().__init__()
        self.setupUi(self)

    # 각각의 버튼에 대한 동작입니다.
    def set_click_RM(self):
        global mode
        mode = "RM"
        self.label.setText("RM")

    def set_click_EDF(self):
        global mode
        mode = "EDF"
        self.label.setText("EDF")

    def set_click_USER(self):
        global mode
        mode = "USER"
        self.label.setText("USER")

    def set_click_git(self):
        return

    def set_click_SE(self):
        return

    def set_click_InputFile(self):
        return

    def set_click_Scheduling(self):
        global successedList
        global failedList
        global periodicCount
        global aperiodicCount
        global taskCount
        global mode
        global duration
        successedList, failedList, periodicCount, aperiodicCount, duration = Processing.Scheduling(mode)

        if mode == "RM": taskCount = periodicCount
        else: taskCount = periodicCount + aperiodicCount

    def set_click_Show(self):
        global successedList
        global failedList
        global taskCount
        global duration
        Visualize.Show(successedList, failedList, taskCount, duration)

    def set_click_OutputFile(self):
        global successedList
        global failedList

        Processing.OutputFile(successedList, failedList)

    def set_click_Help(self):
        if self.pushButton_10.text() == "도움말이 필요하다면 이 곳을 누르세요.":
            self.pushButton_10.setText(
            "도움말을 종료하시려면 다시 한번 클릭해주세요.\n\n\n\n" +
            "1. input.txt 파일을 생성하고, 형식에 맞게 작성하여 \n" +
            ".py 파일들과 같은 경로에 위치시킵니다.\n\n" +
            "2. 원하는 스케줄링 알고리즘을 선택합니다.\n 단 유저 알고리즘의 경우 프로그램 제작을\n 요청한 사람만을 유저라고 가정합니다.\n (해당 프로젝트의 경우 개발 요청자와 프로그램 제작 인물이 같습니다.)\n\n" +
            "3. 스케줄링 버튼을 클릭합니다.\n 이제 스케줄링이 되었습니다.\n\n" +
            "4. 그림으로 확인하고 싶으시다면 결과 보기를, \n, 파일에 출력하고 싶으시면 파일 출력을 클릭하시면 됩니다.\n 결과를 보신 후 파일에 출력할 수\n 있고, 파일을 다 보신 후 결과를 보실 수도 있습니다.\n\n" +
            "해당 프로그램의 소스코드는 \nbitbucket.org/mybirth0407에 공유되어 있습니다.")

        else:
            self.pushButton_10.setText("도움말이 필요하다면 이 곳을 누르세요.")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    myWindow = MyWindow()
    myWindow.show()
    app.exec_()
