from controller import Robot
from controller import Motor
from controller import DistanceSensor
from controller import Camera
from controller import LED
from controller import Supervisor
import math
robot = Robot()

timestep = 32
robot.step(timestep)
        
cam = robot.getDevice("camera")
cam.enable(64)
    
lm = robot.getDevice("left wheel motor")
lm.setPosition(float("inf"))
lm.setVelocity(0)

rm = robot.getDevice("right wheel motor")
rm.setPosition(float("inf"))
rm.setVelocity(0)

# Sensors
NB_GROUND_SENS = 8
gs = []
gsNames = [
    'gs0', 'gs1', 'gs2', 'gs3',
    'gs4', 'gs5', 'gs6', 'gs7'
]
for i in range(NB_GROUND_SENS):
    gs.append(robot.getDevice(gsNames[i]))
    gs[i].enable(timestep)

# LEDs
NB_LEDS = 5
leds = []
led_Names = [
    'led0', 'led1', 'led2', 'led3', 'led4'
]
for i in range(NB_LEDS):
    leds.append(robot.getDevice(led_Names[i]))

### Private Functions ###
# Function to control LEDs
def LED_Alert():
    if (robot.getTime() - initTime)*1000 % 3000 >= 2000:
        #leds[1].set(not(leds[1].get()))
        leds[1].set(1)
        #for i in range(NB_LEDS):
            #leds[i].set(not(leds[i].get()))
    return

# Waiting for completing initialization
initTime = robot.getTime()
while robot.step(timestep) != -1:
    if (robot.getTime() - initTime) * 1000.0 > 200:
        break


### Phần code được phép chỉnh sửa cho phù hợp ##

# Định nghĩa các tín hiệu của xe
NOP = -1
MID = 0
LEFT_1 = 1
LEFT_2 = 2
LEFT_3 = 3
LEFT_4 = 4
LEFT_5 = 5
LEFT_6 = 6
RIGHT_1 = -1
RIGHT_2 = -2
RIGHT_3 = -3
RIGHT_4 = -4
RIGHT_5 = -5
RIGHT_6 = -6
FULL_SIGNAL  = 8
BLANK_SIGNAL = -8
LEFT = 9
RIGHT = -9
LEFT_inter = 10
RIGHT_inter = -10
O = 11
MAX_SPEED = 20

#threshold = [300, 300, 300, 300, 300, 300, 300, 300]
threshold = [330, 330, 330, 330, 330, 330, 330, 330]
#threshold = [350, 350, 350, 350, 350, 350, 350, 350]
#threshold = [370, 370, 370, 370, 370, 370, 370, 350]

# Biến lưu giá trị tỉ lệ tốc độ của động cơ
left_ratio = 0.0
right_ratio = 0.0

# Hàm đọc giá trị của sensors
def ReadSensors():
    gsValues = []
    filted = ""
    for i in range(NB_GROUND_SENS):
        gsValues.append(gs[i].getValue())
        if gsValues[i] > threshold[i]:
            filted +="1"
        else:
            filted += "0"
    #print(*gsValues, sep = '\t')
    return filted

# Phần code điều khiển xe
def DeterminePosition(filted):
    if (filted == "11101111" or filted == "11110111" or filted == "11100111"):
        return MID

    elif (filted == "11001111" or filted == "11011111"):
        return RIGHT_1
    elif (filted == "11110011" or filted == "11111011"):
        return LEFT_2
    elif (filted == "10111111" or filted == "10011111"):
        return RIGHT_2
    elif (filted == "11111101" or filted == "11111001"):
        return LEFT_2
    elif (filted == "01111111" or filted == "00111111"):
        return RIGHT_3
    elif (filted == "11111110" or filted == "11111100"):
        return LEFT_3
    elif (filted == "11000111" or filted == "11000011"):
        #return RIGHT_4
        return MID
    elif (filted == "11100011"):
        #return LEFT_4
        return MID
    elif (filted == "00011111" or filted == "00001111" or filted == "10000111" or filted == "10001111"):
        return RIGHT_5
    elif (filted == "11111000" or filted == "11110000" or filted == "11100001" or filted == "11110001"):
        return LEFT_5
    elif (filted == "00000111"):
        return RIGHT_6
    elif (filted == "11100000"):
        return LEFT_6   
    elif (filted == "00000000" or filted == "00000011" or filted == "10000000" or filted == "00000001" or filted == "10000001"):
        return FULL_SIGNAL    
    elif (filted == "00011000" or filted == "11000000" or filted == "00001000" or filted == "00010000" or filted == "00100100"):
        return FULL_SIGNAL
    
    elif filted == "11111111":
        return BLANK_SIGNAL
    else:
        return NOP
    
def GoStraight(filted):
    #print("Go straight\n")
    pos = DeterminePosition(filted)
    if pos == MID:
        return 0.9, 0.9

def TurnLeft(filted):
    #print("turn left\n")
    pos = DeterminePosition(filted)
    if pos == RIGHT_1:
        #print("1")
        return 0.4, 0.7
    elif pos == RIGHT_2:
        #print("2")
        return 0.2, 0.7
    elif pos == RIGHT_3:
        #print("3")
        return 0.1, 0.9
    elif (pos == RIGHT_5 or pos == RIGHT_6):
        #print("56")
        return 0.1, 0.7
    else:
        #print("...")
        return 0.7, 0.7
    
    
def TurnRight(filted):
    #print("turn right\n")
    pos = DeterminePosition(filted)
    if pos == LEFT_1:
        #print("1")
        return 0.7, 0.4
    elif pos == LEFT_2:
        #print("2")
        return 0.7, 0.2
    elif pos == LEFT_3:
        #print("3")
        return 0.9, 0.1
    elif (pos == LEFT_5 or pos == LEFT_6):
        #print("56")
        return 0.7, 0.1
    else:
        return 0.7, 0.7
    
#####################################
        
timeCounter = 0
lastPos = 0  
currentTime = robot.getTime()
interSignal = 0    
intersectionMode = 0
preFilted = ReadSensors()
circleMode = 0
cornerMode = 0
# Main loop:

while robot.step(timestep) != -1:
    filted = ReadSensors()
    pos = DeterminePosition(filted)
    if (robot.getTime()*1000.0 < 1000):
        pos == MID
        filted == "11100111"
        #print("aaaa")
    if lastPos == FULL_SIGNAL:
        if pos != FULL_SIGNAL:
            #print("intersection ")
            if interSignal == LEFT_inter:
                currentTime = robot.getTime()
                intersectionMode = 1
            elif interSignal == RIGHT_inter:
                currentTime = robot.getTime()
                intersectionMode = -1
            elif interSignal == 0:
                circleMode = 1
                
    if cornerMode == 1:
        intersectionMode = 1
        currentTime = robot.getTime()
        cornerMode = 0
    elif cornerMode == -1:
        intersectionMode = -1
        currentTime = robot.getTime()
        cornerMode = 0
    
    if circleMode == 1:
        if pos == FULL_SIGNAL:
            currentTime = robot.getTime() 
            intersectionMode = 1
        elif intersectionMode == 0:
            if (pos == 4 or pos == -4 or pos == -5 or pos == -6 or pos == O):
                currentTime = robot.getTime() 
                intersectionMode = 1
                circleMode = 0
              
                    
    #intersection signal setup
    if (pos == RIGHT_5 or pos == RIGHT_6):
        interSignal = LEFT_inter
    elif (pos == LEFT_5 or pos == LEFT_6):
        interSignal = RIGHT_inter
       
    #to go straight
    if pos == MID:
        left_ratio, right_ratio = GoStraight(filted)
        timeCounter = 0
    #to turn right
    elif (pos == LEFT_1 or pos == LEFT_2 or pos == LEFT_3):
        left_ratio, right_ratio = TurnRight(filted)
        timeCounter = 0
    #to turn left
    elif (pos == RIGHT_1 or pos == RIGHT_2 or pos == RIGHT_3):
        left_ratio, right_ratio = TurnLeft(filted)
        timeCounter = 0
    
    elif pos == BLANK_SIGNAL:
        
        if (lastPos == LEFT_1 or lastPos == LEFT_2 or lastPos == LEFT_3):
            left_ratio, right_ratio = TurnRight(preFilted)
        elif (lastPos == RIGHT_1 or lastPos == RIGHT_2 or lastPos == RIGHT_3):
            left_ratio, right_ratio = TurnLeft(preFilted)
        elif lastPos == MID:
            left_ratio,right_ratio = GoStraight(preFilted)
        if interSignal == LEFT_inter:
            cornerMode = 1
        elif interSignal == RIGHT_inter:
            cornerMode = -1
        pos = lastPos
        filted = preFilted
        
    elif pos == FULL_SIGNAL:
        left_ratio, right_ratio = GoStraight("11100111")
        #print("FULL")
        timeCounter += 1
            
    elif pos == NOP:
        #print("nop")
        left_ratio, right_ratio = 0.2, 0.2
        """
        pos = lastPos
        filted = preFilted
        """
    if (intersectionMode != 0):
        timer = robot.getTime() - currentTime
        if timer*1000.0 < 640:
            #print(timer)
            if intersectionMode == 1:
                left_ratio, right_ratio = 0.0, 0.5
            elif intersectionMode == -1:
                left_ratio, right_ratio = 0.5, 0.0
        else:
            intersectionMode = 0
            interSignal = 0
    
    #print('Position: ' + filted)
    #print("intersignal:" +str(interSignal))
    #print(intersectionMode)
    #print("time counter =" +str(timeCounter))   
    #print("circle Mode:")
    #print(circleMode)             
    #print("cornerMode:" +str(cornerMode))   
     
    if timeCounter >= 9:
        left_ratio, right_ratio = 0.0, 0.0   

    lm.setVelocity(left_ratio * MAX_SPEED)
    rm.setVelocity(right_ratio * MAX_SPEED)
    
    preFilted = filted

#lastpos setup  
    
    if pos == MID:
        lastPos= MID
    elif pos == FULL_SIGNAL:
        lastPos = FULL_SIGNAL
        
    elif pos == LEFT_1:
        lastPos = LEFT_1
    elif pos == LEFT_2:
        lastPos = LEFT_2
    elif pos == LEFT_3:
        lastPos = LEFT_3
    elif pos == LEFT_4:
        lastPos = LEFT_4
    elif pos == LEFT_5:
        lastPos = LEFT_5
    elif pos == LEFT_6:
        lastPos = LEFT_6
        
    elif pos == RIGHT_1:
        lastPos = RIGHT_1
    elif pos == RIGHT_2:
        lastPos = RIGHT_2
    elif pos == RIGHT_3:
        lastPos = RIGHT_3
    elif pos == RIGHT_4:
        lastPos = RIGHT_4
    elif pos == RIGHT_5:
        lastPos = RIGHT_5
    elif pos == RIGHT_6:
        lastPos = RIGHT_6    
      
    pass

# Enter here exit cleanup code.