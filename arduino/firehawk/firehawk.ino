
#include <ControlChain.h>
 

ControlChain cc;

int red1 = 3, green1 = 4, blue1 = 5;
int red2 = 6, green2 = 7, blue2 = 8; 
int p0 = 13, p1 = 12, p2 = 11;

int s0 = 13, s1 = 12, s2 = 11;

int gpio3 = 10, gpio4 = 9;

cc_actuator_t *getActuatorConfig(){

    cc_actuator_config_t actuator_config;
    actuator_config.type = CC_ACTUATOR_MOMENTARY;
    actuator_config.name = "PressMe";

    actuator_config.min = 0.0;
    actuator_config.max = 1.0;
    actuator_config.supported_modes = CC_MODE_TOGGLE | CC_MODE_TRIGGER;
    actuator_config.max_assignments = 1;

    cc_actuator_t *actuator;
    actuator = cc.newActuator(&actuator_config);
    return actuator;
}

int switchState[] = {0,0,0,0,0,0};

void readSwitchState(){
    turnOffAll();
    digitalWrite(s0, LOW);
    digitalWrite(s1, HIGH);
    digitalWrite(s2, HIGH);
    switchState[5] = 1 - digitalRead(gpio3);
    switchState[2] = 1 - digitalRead(gpio4);
    digitalWrite(s0, HIGH);
    digitalWrite(s1, LOW);
    switchState[0] = 1 - digitalRead(gpio3);
    switchState[3] = 1 - digitalRead(gpio4);
    digitalWrite(s1, HIGH);
    digitalWrite(s2, LOW);
    switchState[1] = 1 - digitalRead(gpio3);
    switchState[4] = 1 - digitalRead(gpio4);
}

struct rgb{
    bool red;
    bool green;
    bool blue;
};

struct led{
    int *red;
    int *blue;
    int *green;
    int *power;
};

void turnOffAll(){
    digitalWrite(p0, HIGH);
    digitalWrite(p1, HIGH);
    digitalWrite(p2, HIGH);

    digitalWrite(red2, HIGH);
    digitalWrite(green2, HIGH);
    digitalWrite(blue2, HIGH);

    digitalWrite(red1, HIGH);
    digitalWrite(green1, HIGH);
    digitalWrite(blue1, HIGH);
}

void setLED(struct led led, struct rgb color){
    turnOffAll();
    digitalWrite(*led.red, 1-color.red);
    digitalWrite(*led.green, 1-color.green);
    digitalWrite(*led.blue, 1-color.blue);
    digitalWrite(*led.power, LOW);
    delay(1);
}

led led0 = {&red1, &blue1, &green1, &p0};
led led1 = {&red2, &blue2, &green2, &p1};
led led2 = {&red1, &blue1, &green1, &p1};
led led3 = {&red2, &blue2, &green2, &p2};
led led4 = {&red1, &blue1, &green1, &p2};


void setup() {
    // configure led
    pinMode(red1, OUTPUT);
    pinMode(red2, OUTPUT);
    pinMode(green1, OUTPUT);
    pinMode(green2, OUTPUT);
    pinMode(blue1, OUTPUT);
    pinMode(blue2, OUTPUT);
    
    pinMode(s0, OUTPUT);
    pinMode(s1, OUTPUT);
    pinMode(s2, OUTPUT);

    pinMode(p0, OUTPUT);
    pinMode(p1, OUTPUT);
    pinMode(p2, OUTPUT);

    pinMode(gpio3, INPUT);
    pinMode(gpio4, INPUT);

    cc.begin();

    const char *uri = "https://github.com/Chris-Nicholls/firehawk-chip-controller";
    cc_device_t *device = cc.newDevice("Firehawk", uri);

    cc_actuator_t* actuator = getActuatorConfig();
    cc.addActuator(device, actuator); 
}

rgb red = rgb{1,0,0};
rgb blue = rgb{0,0,1};
rgb green = rgb{0,1,0};

void loop() {
    // setLED(led0, red);
    // setLED(led1, blue);
    // setLED(led2, blue);
    // setLED(led3, blue);
    // setLED(led4, green);
    turnOffAll();
    setLED(led0, red);
    setLED(led1, rgb{1,1,0});
    setLED(led2, green);
    setLED(led3, blue);
    setLED(led4, rgb{1,0,1});
    cc.run();
}
