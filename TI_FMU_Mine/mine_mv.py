# 最近修改日期：2-19/7/27

import sensor, image, time
import network, usocket, sys
import sensor, image, time, usocket, sys
import math
from pyb import UART
from pyb import LED

# LED灯定义
red_led   = LED(1)
green_led = LED(2)
blue_led  = LED(3)

# 宏定义区
GRAYSCALE_THRESHOLD = [(0, 90)]
MIN_PIXELS = 200
width = 160
height = 120
RectHeight = 20
MAX_PIXEL = 400

# 感兴趣区域定义
Roi_Top = (0, 0, width, RectHeight)
Roi_Botton = (0, height - RectHeight, width, RectHeight)
Roi_Left = (0, 0, RectHeight, height)
Roi_Right = (width - RectHeight, 0, RectHeight, height)
Rois = [Roi_Top,Roi_Botton,Roi_Left,Roi_Right]

# 直线阈值定义
right_angle_threshold = (70, 90)
Line_Thr = 4000
Line_Theta_margin = 25
Line_Margin = 50
Line_Min_degree = 0
Line_Max_degree = 179

#颜色阈值定义
red_threshold = (40,55,55,75,20,50)     #红色阈值
green_threshold = (45,65,-50,-30,0,40)  #绿色阈值
blue_threshold = (50,70,0,25,-55,-30)   #蓝色阈值
#总颜色阈值
thresholds_color = [red_threshold,green_threshold,blue_threshold]
#颜色代码定义
red_color_code = 1      #code = 2^0 = 1
green_color_code = 2    #code = 2^1 = 2
blue_color_code = 4     #code = 2^2 = 4

#寻找所有直线
def find_all_Line(img):
    Line_All = img.find_lines(threshold = Line_Thr, theta_margin = Line_Theta_margin, rho_margin = Line_Margin)
    return Line_All
    #`threshold`控制从霍夫变换中监测到的直线 只返回大于或等于阈值的直线
    # 	一条直线的大小是组成直线所有索贝尔滤波像素大小的总和
    # `theta_margin`和`rho_margin`控件合并相似的直线
    # 	如果两直线的theta和ρ值差异小于边际 则它们合并

#计算出直线夹角
# From:http://makermare.com:60010//example/feature/right-angle-detection.html
def calculate_angle(line1, line2):
    angle  = (180 - abs(line1.theta() - line2.theta()))
    if angle > 90:
        angle = 180 - angle
    return angle

#判断两直线之间的夹角是否为直角
# From:http://makermare.com:60010//example/feature/right-angle-detection.html
def is_right_angle(line1, line2):
    global right_angle_threshold
    angle = calculate_angle(line1, line2)
    #判断直角阈值
    if angle >= right_angle_threshold[0] and angle <=  right_angle_threshold[1]:
        return True
    return False

#计算两条线的交点(输入参数为两条线)
# From:http://makermare.com:60010//example/feature/right-angle-detection.html
def calculate_intersection(line1, line2):
    a1 = line1.y2() - line1.y1()
    b1 = line1.x1() - line1.x2()
    c1 = line1.x2()*line1.y1() - line1.x1()*line1.y2()

    a2 = line2.y2() - line2.y1()
    b2 = line2.x1() - line2.x2()
    c2 = line2.x2() * line2.y1() - line2.x1()*line2.y2()

    if (a1 * b2 - a2 * b1) != 0 and (a2 * b1 - a1 * b2) != 0:
        cross_x = int((b1*c2-b2*c1)/(a1*b2-a2*b1))
        cross_y = int((c1*a2-c2*a1)/(a1*b2-a2*b1))
        return (cross_x, cross_y)
    return (-1, -1)

#将所看到的线条整理成一个点或直线信息
def line_info_process(lines):
    Cross_X = -1
    Cross_Y = -1
    line_info_list = [0,0,0,0,0,0,0]
    line_info_list[0] = -1

    # 如果有两条直线 则返回数组中 首位为0 后两位为交点横纵坐标
    if len(lines) == 2:
        if is_right_angle(lines[0],lines[1]):
            Cross_X , Cross_Y = calculate_intersection(lines[0],lines[1])
            line_info_list[0] = 0
            line_info_list[1] = Cross_X
            line_info_list[2] = Cross_Y

    # 如果有一条直线
    if len(lines) == 1:
        if (lines[0].x1() == 0 and lines[0].x2() == 159) or (lines[0].x2() == 0 and lines[0].x1() == 159):
            # 横线 则返回数组中 首位为1 后两位为横线两端点的纵坐标
            line_info_list[0] = 1
            line_info_list[1] = lines[0].y1()
            line_info_list[2] = lines[0].y2()
            pass
        if (lines[0].y1() == 0 and lines[0].y2() == 119) or (lines[0].y2() == 0 and lines[0].y1() == 119):
            # 纵线 则返回数组中 首位为2 后两位为竖线两端点的横坐标
            line_info_list[0] = 2
            line_info_list[1] = lines[0].x1()
            line_info_list[2] = lines[0].x2()
            pass

    return line_info_list

#寻找各区域中的色块
def ProcessImg(img):
    Blobs = []

    TopBlobs = img.find_blobs(GRAYSCALE_THRESHOLD, roi = Roi_Top[0:4], merge=True)
    BottonBlobs = img.find_blobs(GRAYSCALE_THRESHOLD, roi = Roi_Botton[0:4], merge=True)
    LeftBlobs = img.find_blobs(GRAYSCALE_THRESHOLD, roi = Roi_Left[0:4], merge=True)
    RightBlobs = img.find_blobs(GRAYSCALE_THRESHOLD, roi = Roi_Right[0:4], merge=True)
    #thresholds是颜色的阈值（可以包含多个颜色）
    #（在返回的色块对象blob可以调用code方法 来判断是什么颜色的色块）
    #roi是“感兴趣区”
    #merge：合并 如果设置为True 那么合并所有重叠的blob为一个
    #注意：这会合并所有的blob 无论是什么颜色的
    #如果你想混淆多种颜色的blob 只需要分别调用不同颜色阈值的find_blobs

    Blobs.append(TopBlobs)
    Blobs.append(BottonBlobs)
    Blobs.append(LeftBlobs)
    Blobs.append(RightBlobs)

    return Blobs

#计算T字型交点坐标
def CalcVerticalCross(x0,y0,x1,y1,x2,y2):
    if y1 != y0:
        k1 = (y1 - y0)/(x1-x0)
        k2 = -1/k1
        x = (-k1*x0 + y0 +x2*k2 - y2)/(k2 - k1)
        y = k1 * x - k1*x0 + y0
        return x,y
    else:
        return x2,y1

#计算两条线的交点(输入参数为两条线上各两个点)
def CalcCross(x0,y0,x1,y1,x2,y2,x3,y3):
    a1 = y1 - y0
    b1 = x0 - x1
    c1 = x1*y0 - x0*y1

    a2 = y3 - y2
    b2 = x2 - x3
    c2 = x3 * y2 - x2*y3

    if (a1 * b2 - a2 * b1) != 0 and (a2 * b1 - a1 * b2) != 0:
        cross_x = int((b1*c2-b2*c1)/(a1*b2-a2*b1))
        cross_y = int((c1*a2-c2*a1)/(a1*b2-a2*b1))
        return (cross_x, cross_y)

    return (-1, -1)

#寻找最大的斑点
def FindMaxBlobs(BlobList):
    most_pixels = 0
    largest_blob = 0
    if BlobList:
        for i in range(len(BlobList)):
            # 比较像素数
            if BlobList[i].pixels() > most_pixels:
                most_pixels = BlobList[i].pixels()
                largest_blob = i
        return BlobList[largest_blob]
    return None

#没用
def GetRectMoreInfo_V(Rect):
    x0 = Rect.x() + Rect.w()/2
    y0 = Rect.y()
    x1 = Rect.x() + Rect.w()/2
    y1 = Rect.y() + Rect.h()
    return x0,x1,y0,y1
    pass

#没用
def GetRectMoreInfo_H(Rect):
    x0 = Rect.x() + Rect.w()/2
    y0 = Rect.y()
    x1 = Rect.x() + Rect.w()/2
    y1 = Rect.y() + Rect.h()
    return x0,x1,y0,y1
    pass

#寻找特殊图形
BlobLocation = [[0, 0], [0, 0], [0, 0], [0, 0]]
Blobs = []
def RecognitionForm(Blobs,img):
    Top = 0
    Botton = 1
    Left = 2
    Right = 3

    cx = 0
    cy = 1

    MAX_WITH = 40
    MIN_WITH = 12
    MIN_HIGH = 12
    MAX_HIGH = 40

    FormType = 0xFF
    Loaction0 = 0
    Location1 = 0

    TopValid = False
    BottonValid = False
    LeftValid = False
    RightValid = False

    TopBlob = FindMaxBlobs(Blobs[0])
    BottonBlob = FindMaxBlobs(Blobs[1])
    LeftBlob = FindMaxBlobs(Blobs[2])
    RightBlob = FindMaxBlobs(Blobs[3])

    if TopBlob:
        #上方区域有一个不太宽但是足够高的斑点
        if TopBlob.w() < MAX_WITH and TopBlob.h() > MIN_HIGH:
            BlobLocation[Top][cx] = TopBlob.cx()
            BlobLocation[Top][cy] = TopBlob.cy()
            #标记小矩形和小十字
            img.draw_rectangle(TopBlob.rect())
            img.draw_cross(BlobLocation[Top][cx],BlobLocation[Top][cy])
        else:
            TopBlob = None

    if BottonBlob:
        #下方区域有一个不太宽但是足够高的斑点
        if BottonBlob.w() < MAX_WITH and BottonBlob.h() > MIN_HIGH:
            BlobLocation[Botton][cx] = BottonBlob.cx()
            BlobLocation[Botton][cy] = BottonBlob.cy()
            #标记小矩形和小十字
            img.draw_rectangle(BottonBlob.rect())
            img.draw_cross(BlobLocation[Botton][cx],BlobLocation[Botton][cy])
        else:
            BottonBlob = None

    if LeftBlob:
        #左侧区域有一个足够宽并且高度适中的斑点
        if LeftBlob.w() > MIN_WITH and LeftBlob.h() < MAX_HIGH and LeftBlob.h() > MIN_HIGH:
            BlobLocation[Left][cx] = LeftBlob.cx()
            BlobLocation[Left][cy] = LeftBlob.cy()
            #标记小矩形和小十字
            img.draw_rectangle(LeftBlob.rect())
            img.draw_cross(BlobLocation[Left][cx],BlobLocation[Left][cy])
        else:
            LeftBlob = None

    if RightBlob:
        #左侧区域有一个足够宽并且高度适中的斑点
        if RightBlob.w() > MIN_WITH and RightBlob.h() < MAX_HIGH and RightBlob.h() > MIN_HIGH:
            BlobLocation[Right][cx] = RightBlob.cx()
            BlobLocation[Right][cy] = RightBlob.cy()
            #标记小矩形和小十字
            img.draw_rectangle(RightBlob.rect())
            img.draw_cross(BlobLocation[Right][cx],BlobLocation[Right][cy])
        else:
            RightBlob = None

    #标记上下左右区域是否检测到有效斑点
    if TopBlob:
        TopValid = True
    if BottonBlob:
        BottonValid = True
    if LeftBlob:
        LeftValid = True
    if RightBlob:
        RightValid = True

    # 竖线(类型0)：上+下+非左+非右
    if TopValid and BottonValid and (not LeftValid) and (not RightValid):
        #发送线段中点坐标
        Loaction0 = (BlobLocation[Top][cx] + BlobLocation[Botton][cx])//2
        Location1 = BlobLocation[Botton][cy]//2
        FormType = 0

    # 横线(类型1)：左+右+非上+非下
    if (not TopValid) and (not BottonValid) and LeftValid and RightValid:
        #发送线段中点坐标
        Loaction0 = BlobLocation[Right][cx]//2
        Location1 = (BlobLocation[Left][cy] + BlobLocation[Right][cy])//2
        FormType = 1

    # 十字(类型2)：
    if TopValid and BottonValid and LeftValid and RightValid:
        #发送十字交点坐标
        (Loaction0,Location1) = CalcCross(BlobLocation[Left][cx],BlobLocation[Left][cy],
                                          BlobLocation[Right][cx],BlobLocation[Right][cy],
                                          BlobLocation[Top][cx],BlobLocation[Top][cy],
                                          BlobLocation[Botton][cx],BlobLocation[Botton][cy])
        FormType = 2

    # T字型(类型3)：左+右+下+非上
    if (not TopValid) and BottonValid and LeftValid and RightValid:
        #发送T字型交点坐标
        x,y = CalcVerticalCross(BlobLocation[Left][cx],BlobLocation[Left][cy],
                                BlobLocation[Right][cx],BlobLocation[Right][cy],
                                BlobLocation[Botton][cx],BlobLocation[Botton][cy])
        Loaction0 = int(x)
        Location1 = int(y)
        FormType = 3

    # 倒T字型(类型4)：左+右+上+非下
    if (TopValid) and (not BottonValid) and LeftValid and RightValid:
        #发送T字型交点坐标
        x,y = CalcVerticalCross(BlobLocation[Left][cx],BlobLocation[Left][cy],
                                BlobLocation[Right][cx],BlobLocation[Right][cy],
                                BlobLocation[Top][cx],BlobLocation[Top][cy])
        Loaction0 = int(x)
        Location1 = int(y)
        FormType = 4

    # (粗略的检测 对YAW值要求严格)
    # L字形(类型5)：右+非左+非下+上
    if  TopValid and (not BottonValid) and (not LeftValid) and RightValid:
        #发送L字形交点坐标
        Loaction0 = BlobLocation[Top][cx]
        Location1 = BlobLocation[Right][cy]
        FormType = 5

    #    |
    #    |
    # ___|字形(类型6)：左+非右+非下+上
    if TopValid and (not BottonValid) and (LeftValid) and (not RightValid):
        #发送L字形交点坐标
        Loaction0 = BlobLocation[Top][cx]
        Location1 = BlobLocation[Left][cy]
        FormType = 6

    #  ___
    # |
    # |
    # |	  字形(类型7)：非上+非下+非左+右
    if (not TopValid) and (BottonValid) and (not LeftValid) and (RightValid):
        Loaction0 = BlobLocation[Botton][cx]
        Location1 = BlobLocation[Right][cy]
        FormType = 7

    # ___
    #    |
    #    |
    #    |字形(类型8)：非上+下+左+非右
    if (not TopValid) and (BottonValid) and (LeftValid) and (not RightValid):
        Loaction0 = BlobLocation[Botton][cx]
        Location1 = BlobLocation[Left][cy]
        FormType = 8

    #返回检测到的图形类型和特征点坐标
    return FormType,Loaction0,Location1

#寻找Apriltag(类型100)
def Find_Apriltags(img):
    X = -1
    Y = -1
    FormType = 0

    for tag in img.find_apriltags(families=image.TAG16H5):
        #标记小矩形和小十字
        img.draw_rectangle(tag.rect(), color = (255, 0, 0))
        img.draw_cross(tag.cx(), tag.cy(), color = (0, 255, 0))
        #发送Apriltag中点坐标
        X = tag.cx()
        Y = tag.cy()
        FormType = 100
    return FormType,X,Y

#判断是否寻找到Apriltag
def IS_FindApriTag(img):
    GetApriTag = False

    for tag in img.find_apriltags(families=image.TAG16H5):
        GetApriTag = True

    return GetApriTag


#寻找色块
def Find_ColorBlob(img):
    X = -1
    Y = -1
    FormType = 0

    blobs = img.find_blobs(thresholds_color,area_threshold=100)

    #如果找到了目标颜色
    if blobs:
        for blob in blobs:
            X = blob.cx()
            Y = blob.cy()
            color_code = blob[8] #读取颜色代码

            #添加颜色说明
            if color_code == red_color_code:
                img.draw_string(x,y-10,"red",color = (0xFF,0x00,0x00))
            elif color_color == green_color_code:
				img.draw_string(x,y-10,"green",color = (0x00,0xFF,0x00))
				
				书签啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊啊
				
				
				
				
				
				

    for tag in img.find_blobs([blue_threshold[0]]):
        #标记小矩形和小十字
        img.draw_rectangle(tag.rect())
        img.draw_cross(tag.cx(), tag.cy())
        #发送色块中点坐标
        X = tag.cx()
        Y = tag.cy()
        FormType = 101

    for tag in img.find_blobs([red_threshold[0]]):
        #标记小矩形和小十字
        img.draw_rectangle(tag.rect())
        img.draw_cross(tag.cx(), tag.cy())
        #发送色块中点坐标
        X = tag.cx()
        Y = tag.cy()
        FormType = 102

    return FormType,X,Y


#数据打包
def ExceptionVar(var):
    data = []
    data.append(0)
    data.append(0)

    if var == -1:
        data[0] = 0
        data[1] = 0
    else:
        data[0] = var & 0xFF
        data[1] = var >> 8
    return data

#串口发送
Frame_Cnt = 0
fCnt_tmp = [0,0]
def UART_Send(FormType, Loaction0, Location1):
    global Frame_Cnt
    global fCnt_tmp
    Frame_Head = [170,170]	#0xAA
    Frame_End = [85,85]		#0x55
    fFormType_tmp = [FormType]
    Frame_Cnt += 1

    if Frame_Cnt > 65534 :
        FrameCnt = 0

    fHead = bytes(Frame_Head)

    fCnt_tmp[0] = Frame_Cnt & 0xFF
    fCnt_tmp[1] = Frame_Cnt >> 8
    fCnt = bytes(fCnt_tmp)

    fFormType = bytes(fFormType_tmp)
    fLoaction0 = bytes(ExceptionVar(Loaction0))
    fLoaction1 = bytes(ExceptionVar(Location1))
    fEnd = bytes(Frame_End)
    FrameBuffe = fHead + fCnt + fFormType + fLoaction0 + fLoaction1 + fEnd
    return FrameBuffe

#OPENMV硬件相关配置
sensor.reset() 							#init camera
sensor.set_pixformat(sensor.GRAYSCALE) 	#灰度图像
sensor.set_framesize(sensor.QQVGA) 		#设置像素为160*120
sensor.skip_frames(30) 					#等待配置生效
sensor.set_auto_gain(False) 			#颜色识别时必须关闭
sensor.set_auto_whitebal(False) 		#颜色识别时必须关闭
uart = UART(3, 115200)					#配置与飞控交互的串口
clock = time.clock()
i = 0

#开机模式选择循环
CountDown = 100	#模式选择循环次数
Find_ApriTag_ENABLE = False
Find_Line_ENABLE = False
Find_ColorBlob_ENABLE = False
while(True):
    CountDown -= 1

    if CountDown <= 0:
        Find_ApriTag_ENABLE = False
        Find_Line_ENABLE = False
        Find_ColorBlob_ENABLE = True

        if Find_ColorBlob_ENABLE == True:
            sensor.set_pixformat(sensor.RGB565) #彩色图像

        break

    #模式选择循环中 红灯绿灯同步闪烁
    i+=1
    if i % 5 == 0:
        green_led.on()
        red_led.on()
    if i % 10 == 0:
        green_led.off()
        red_led.off()
    pass


#while(True):
#    CountDown -= 1
#    img = sensor.snapshot()	#截图
#    (Type,P0,P1) = Find_Apriltags(img) #寻找Apriltag
#
#    #如果找到Apriltag 则设置为寻找Apriltag模式
#    if Type == 100:
#        Find_ApriTag_ENABLE = True
#        Find_Line_ENABLE = False
#        break
#    #如果没有找到Apriltag 则设置为一般巡线模式
#    else:
#        Find_Line_ENABLE = True
#        Find_ApriTag_ENABLE = False
#
#    #倒计时结束 红灯绿灯关闭 结束当前while
#    if CountDown <= 0:
#        red_led.off()
#        green_led.off()
#        break
#
#    #模式选择循环中 红灯绿灯同步闪烁
#    i+=1
#    if i % 5 == 0:
#        green_led.on()
#        red_led.on()
#    if i % 10 == 0:
#        green_led.off()
#        red_led.off()
#    pass

#主循环
while(True):
    img = sensor.snapshot()	#截图

    #普通巡线模式
    if Find_Line_ENABLE:
        #寻找所有线
        All_Line = find_all_Line(img)
        #将所看到的线条整理成一个点或直线信息(目前只考虑了1条/2条线的情况)
        Line_Info = line_info_process(All_Line)

        #寻找特殊图形 并获取特殊图形的信息
        (Type,P0,P1) = RecognitionForm(ProcessImg(img),img)

        #找到两条线
        if Line_Info[0] == 0:
            #在交点处画圆
            img.draw_circle(Line_Info[1],Line_Info[2], 3, color = 200, thickness = 2, fill = False)
        #寻找到线
        if All_Line:
            #画出所有线
            for ii in All_Line:
                img.draw_line(ii.line())

    #寻找Apriltag模式
    if Find_ApriTag_ENABLE:
        #获取Apriltag的坐标
        (Type,P0,P1) = Find_Apriltags(img)
        pass

    #寻找色块模式
    if Find_ColorBlob_ENABLE:
        (Type,P0,P1) = Find_ColorBlob(img)
        pass

    #串口发送(类型+坐标)
    uart.write(UART_Send(Type,P0,P1))

    #led指示
    i+=1
    if i % 5 == 0:
        #寻找Apriltag模式红灯闪烁
        if Find_ApriTag_ENABLE:
            green_led.on()
        else:
        #普通巡线模式绿灯闪烁
            red_led.on()
    if i % 10 == 0:
        if Find_ApriTag_ENABLE:
            green_led.off()
        else:
            red_led.off()

    print(Type,P0,P1)


