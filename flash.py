#!/usr/bin/python
import CHIP_IO.GPIO as GPIO                
import math    
import random
import serial
import mido


def pin(n): return "XIO-P"+str(n)          
def off(pin): GPIO.output(pin, GPIO.LOW)    
def on(pin): GPIO.output(pin, GPIO.HIGH)  
                                           
           
class Pin:
    def __init__(self, name):
        self.name = name
        self.on = True
        GPIO.setup(name, GPIO.OUT)                   
        self.set(0)

    def set(self,on):
        if (on != self.on):
            self.on = on
            GPIO.output(self.name, GPIO.LOW if on else GPIO.HIGH)

    def high(self):
        self.set(False)

    def low(self):
        self.set(True)

class Pin2:
    def __init__(self, name, id):
        self.name = name if name else pin(id)
        self.on = True
        GPIO.setup(self.name, GPIO.OUT)                   
        self.file = open("/sys/class/gpio/gpio"+str(id) +"/value", "w", 0)
        self.set(0)

    def set(self,on):
        if (on != self.on):
            self.on = on
            self.file.write("0" if on else "1")

    def high(self):
        self.set(False)

    def low(self):
        self.set(True)



class LED:
    def __init__(self, power, r, g, b, *off):
        self.power = power
        self.red = r
        self.blue = b
        self.green = g
        self.off = off
        self.active = False

    def set(self, r, g, b):
        for i in self.off: i.set(0)
        self.red.set(r)
        self.green.set(g)
        self.blue.set(b)
        self.power.set(1)

    def setColor(self, c):
        self.set(c.r, c.g, c.b)

    def turnOff(self):
        self.power.set(0)


class Color: 
    def __init__(self, r,g,b):
        self.r = float(r)/(r+g+b)
        self.g = float(g)/(r+g+b)
        self.b = float(b)/(r+g+b)

    def eval(self, t):
        r = self.r > ((t*10) % 1)
        g = not r and self.g +self.r > ((t*10) % 1)
        b = not r and not g and self.b +self.g + self.r > ((t*10) % 1)
        return Color(r,g,b)

    def __mul__(self, x):
        return Color(self.r*x, self.g*x, self.b*x)

    def __add__(self, c):
        return Color(self.r+c.r, self.g+c.g, self.b+c.b)



class Cycle:
    def __init__(self, c1, c2, freq):
        self.c1 = c1
        self.c2 = c2
        self.freq = freq

    def eval(self, t):
        c1 = self.c1.eval(t)
        c2 = self.c2.eval(t)
        l = math.cos(t*math.pi*2/self.freq)/2 + 0.5
        l2 = 1-l
        return Color(c1.r*l + c2.r*l2, c1.g*l+c2.g*l2, c1.b*l+c2.b*l2).eval(t)


                                           
red1 = Pin2("LCD-D7", 32*3+7)                               
green1 = Pin2("LCD-D11", 32*3+11)  
blue1 = Pin2("LCD-D13", 32*3+13)                              

red2 = Pin2("LCD-D10", 32*3+10)                               
green2 = Pin2("LCD-D12", 32*3+12)                            
blue2 = Pin2("LCD-D14", 32*3+14)                              
                                           
p0 = Pin2("LCD-D3", 32*3+3)                                
p1 = Pin2("LCD-D4", 32*3+4)    
p2 = Pin2("LCD-D5",32*3+5)                            
                                                                 
led4 = LED(p0, red1, green1, blue1, p1, p2, red2, green2, blue2) 
led3 = LED(p0, red2, green2, blue2, p1, p2, red1, green1, blue1)                 
led2 = LED(p1, red1, green1, blue1, p0, p2, red2, green2, blue2)                                                                          
led1 = LED(p1, red2, green2, blue2, p0, p2, red1, green1, blue1) 
led0 = LED(p2, red1, green1, blue1, p0, p1, red2, green2, blue2)


p0.set(1)
p1.set(1)
p2.set(1)

red = Color(1,0,0)
green = Color(0,1,0)
blue = Color(0,0,1)
yellow = Color(0.2,1,0)
purple = Color(0.2,0,1)

rb = Cycle(red, blue, 20)
bg = Cycle(blue, green, 22)
rg = Cycle(red, green, 24)
py = Cycle(purple, yellow,21)
by = Cycle(blue, yellow, 23)

# s0 = Pin2(pin(0), 1013)
# s1 = Pin2(pin(1), 1014)
# s2 = Pin2(pin(2), 1015)

s0 = Pin2("LCD-D18", 32*3+18)                                
s1 = Pin2("LCD-D15", 32*3+15)    
s2 = Pin2("LCD-D20",32*3+20) 


GPIO.setup(pin(3), GPIO.IN)
GPIO.setup(pin(4), GPIO.IN)

def readSwitchState2():
    state = [0]*6
    s0.low()
    s1.low()
    s2.low()
    if (GPIO.input(pin(3)) == 1 and  GPIO.input(pin(4)) == 1):
        return state
    s0.low()
    s1.high()
    s2.high()
    state[5] = 1-GPIO.input(pin(3))
    state[2] = 1-GPIO.input(pin(4))
    s0.high()
    s1.low()
    state[0] = 1-GPIO.input(pin(3))
    state[3] = 1-GPIO.input(pin(4))
    s1.high()
    s2.low()
    state[1] = 1-GPIO.input(pin(3))
    state[4] = 1-GPIO.input(pin(4))
    return state
        
def light(dt):
    t = 0
    while 1: 
        led4.setColor(rb.eval(t*dt))
        led3.setColor(bg.eval(t*dt))
        led2.setColor(rg.eval(t*dt))
        led1.setColor(py.eval(t*dt))
        led0.setColor(by.eval(t*dt))
        t += 1
        if (t % 10 == 0):
            s = readSwitchState2()   
            for i in range(6):
                if (s[i]): print i

def game():
    dt = 0.011
    t = 0
    current = 0
    led = led1
    color = purple
    while 1:
        if (t % 10 == 0):
            s = readSwitchState2()
            if s[current]:
                current = (current + random.randrange(5)) % 5 
                color  = random.choice([rb, bg, rg, py, by, red, blue, green, purple, yellow])
                led = [led0, led1,led2,led3,led4][current]
        led.setColor(color.eval(t*dt))
        t += 1


def sixSwitch():
    with serial.Serial('/dev/ttyS0', 31250) as ser:
        dt = 0.011
        t = 0
        leds = [led0, led1, led2, led3, led4]
        color = purple
        states = [False]*6
        prev = [False]*6
        while 1:
            if (t % 6 == 0):
                s =readSwitchState2()
                for i in range(5):
                    if (s[i] and not prev[i]):
                        states[i] = not states[i]
                prev = s

            for i in range(5):
                if states[i]:
                    leds[i].setColor(purple)
            if not any(states):
                p0.set(0)
                p1.set(0)
                p2.set(0)
            t = t+1


        msg = mido.Message('control_change', channel=0, value=i)
        ser.write(msg.bytes())
        #print(mido.parse(ser.read(3)))
led1.setColor(green)
sixSwitch()