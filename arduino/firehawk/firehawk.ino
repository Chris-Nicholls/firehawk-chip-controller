
#include <ControlChain.h>
 
#define SWITCH_DELAY 1
#define DEBOUNCE_REPEATS 3

ControlChain cc;

int red1 = 3, green1 = 4, blue1 = 5;
int red2 = 6, green2 = 7, blue2 = 8; 
int p0 = 11, p1 = 12, p2 = 13;

int s0 = 11, s1 = 12, s2 = 13;

int gpio3 = 9, gpio4 = 10;

cc_actuator_t *getActuatorConfig(char* name){

    cc_actuator_config_t actuator_config;
    actuator_config.type = CC_ACTUATOR_MOMENTARY;
    actuator_config.name = name;

    actuator_config.min = 0.0;
    actuator_config.max = 1.0;
    actuator_config.supported_modes = CC_MODE_TOGGLE | CC_MODE_TRIGGER;
    actuator_config.max_assignments = 1;

    cc_actuator_t *actuator;
    actuator = cc.newActuator(&actuator_config);
    return actuator;
}

int switchState[] = {0,0,0,0,0,0};

int debounceTimes[] = {0,0,0,0,0,0};

void readSwitchState(int newState, int* currentState, int* debounceTime){
    if (newState != *currentState){
        *debounceTime++;
    }else{
        *debounceTime = 0;
    }
    if (*debounceTime > DEBOUNCE_REPEATS){
        *currentState = newState;
    }
}

void readSwitchStates(){
    turnOffAll();

    digitalWrite(s0, LOW);
    digitalWrite(s1, HIGH);
    digitalWrite(s2, HIGH);
    delay(1);

    readSwitchState(1 - digitalRead(gpio3), &switchState[5], &debounceTimes[5]);
    readSwitchState(1 - digitalRead(gpio4), &switchState[2], &debounceTimes[2]);

    digitalWrite(s0, HIGH);
    digitalWrite(s1, LOW);
    delay(1);

    readSwitchState(1 - digitalRead(gpio3), &switchState[0], &debounceTimes[0]);
    readSwitchState(1 - digitalRead(gpio4), &switchState[3], &debounceTimes[3]);
    
    digitalWrite(s1, HIGH);
    digitalWrite(s2, LOW);
    delay(1);

    readSwitchState(1 - digitalRead(gpio3), &switchState[1], &debounceTimes[1]);
    readSwitchState(1 - digitalRead(gpio4), &switchState[4], &debounceTimes[4]);
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
    digitalWrite(p0, LOW);
    digitalWrite(p1, LOW);
    digitalWrite(p2, LOW);

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
    digitalWrite(*led.power, HIGH);
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
    analogReference(INTERNAL);
    cc.begin();

    const char *uri = "https://github.com/Chris-Nicholls/firehawk-chip-controller";
    cc_device_t *device = cc.newDevice("Firehawk", uri);

    cc_actuator_t* actuator = getActuatorConfig("Button-1");
    cc.addActuator(device, actuator); 
}

rgb red = rgb{1,0,0};
rgb blue = rgb{0,0,1};
rgb green = rgb{0,1,0};
rgb yellow = rgb{1,1,0};
rgb pink = rgb{1,0,1};
rgb cyan = rgb{0,1,1};

rgb off = rgb{0,0,0};



void setLEDS(){
    if (!switchState[5]){
        setLED(led0, switchState[0]==0? red: green);
        setLED(led1, switchState[1]==0? blue: green);
        setLED(led2, switchState[2]==0? yellow: green);
        setLED(led3, switchState[3]==0? pink: green);
        setLED(led4, switchState[4]==0? blue: green);
    }else{
        setLED(led4, blue);
    }
}


void loop() {
    readSwitchStates();
    setLEDS();
    setLEDS();
    setLEDS();
    setLEDS();
    setLEDS();
    setLEDS();
    

    cc.run();
    
}
