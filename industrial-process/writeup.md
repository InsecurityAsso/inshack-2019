# Industrial secret
Thanks to the string you should find on google that it is linked to Cloakify. Then you should be able to get the original pcap file by using decloakify and the good cipher. 

Then you have to extact info from all the modbus frames. This can be done with the following snippet : 
```
import pyshark


capture = pyshark.FileCapture('test.pcapng')
pos = [[] for _ in range(25)]
for i in range(1,425,2):
    pos[int(capture[i].modbus.regval_uint16.all_fields[2].get_default_value())].append(
        (int(capture[i].modbus.regval_uint16.all_fields[0].get_default_value()) / 1000, 
        int(capture[i].modbus.regval_uint16.all_fields[1].get_default_value()) / 1000))

print(pos)
```
And finally you can simulate the cut with different techniques,for example using turtle (python2 only..) : 
```
import turtle
import time
pos = "insert here the result from previous script"
myPen = turtle.Turtle()
myPen.hideturtle()
myPen.speed(0)
window = turtle.Screen()
window.bgcolor("#000000")
myPen.pensize(2)
myPen.color("#FF00FF")
fontSize = 30
characterSpacing = 5
x = -450
y = 0
for letter in pos:
    myPen.penup()
    for dot in letter:
        myPen.goto(x + dot[0] * fontSize, y + dot[1] * fontSize)
        myPen.pendown()

    x += fontSize
    x += characterSpacing
time.sleep(100)
```