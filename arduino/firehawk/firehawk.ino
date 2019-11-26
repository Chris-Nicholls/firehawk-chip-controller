#include <actuator.h>
#include <assignment.h>
#include <config.h>
#include <ControlChain.h>
#include <control_chain.h>

 
#define SWITCH_DELAY 1
#define DEBOUNCE_REPEATS 1

#define RED 0b100
#define BLUE 0b001
#define GREEN 0b010
#define YELLOW 0b110
#define PINK 0b101
#define CYAN 0b011
#define OFF 0b000

#define led0 red1, blue1, green1, p0
#define led1 red2, blue2, green2, p1
#define led2 red1, blue1, green1, p1
#define led3 red2, blue2, green2, p2
#define led4 red1, blue1, green1, p2


ControlChain cc;

#define red1 3
unsigned char green1 = 4, blue1 = 5;
unsigned char red2 = 6, green2 = 7, blue2 = 8; 
unsigned char p0 = 11, p1 = 12, p2 = 13;

unsigned char s0 = 11, s1 = 12, s2 = 13;

unsigned char gpio3 = 9, gpio4 = 10;

cc_actuator_config_t actuators[6];
int acuator_count = 0;


cc_actuator_t *getActuatorConfig(char* name, float* value){
    if (acuator_count > 5) acuator_count = 0;
    cc_actuator_config_t actuator_config = actuators[acuator_count++];
    actuator_config.type = CC_ACTUATOR_MOMENTARY;
    actuator_config.name = name;

    actuator_config.min = 0.0;
    actuator_config.max = 1.0;
    actuator_config.value = value;
    actuator_config.supported_modes = CC_MODE_TOGGLE ;
    actuator_config.max_assignments = 1;

    cc_actuator_t *actuator;
    actuator = cc.newActuator(&actuator_config);
    return actuator;
}

float switchState[] = {0,0,0,0,0,0};
float ccState[] = {0,0,0,0,0,0};

char debounceTimes[] = {0,0,0,0,0,0};

void readSwitchState(float newState, float* currentState, char* debounceTime){
    if (newState != *currentState){
        (*debounceTime)++;
    }else{
        *debounceTime = 0;
    }
    if (*debounceTime >= DEBOUNCE_REPEATS){
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

#pragma inline
void setLED(char red, char blue, char green, char power, char color){
    turnOffAll();
    int r = (color & 0b100)>>2;
    int g = (color & 0b010)>>1;
    int b = color & 0b001;
    digitalWrite(red, 1-r);
    digitalWrite(green, 1-g);
    digitalWrite(blue, 1-b);
    digitalWrite(power, HIGH);
    delay(1);
}

const char *uri = "na";
const char *device_name = "FH";
int setups = 0;
void setup() {
    acuator_count = 0;
    cc.begin();

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

    if (setups++ > 0){
      setLED(led1, RED);
      return;
    }
    setLED(led1, GREEN);
    setLED(led0, YELLOW);
    
    setLED(led1, RED);
    delay(100);
    setLED(led1, GREEN);

    delay(100);
    setLED(led0, RED);
    delay(100);
    setLED(led3, GREEN);
    delay(100);
    setLED(led4, GREEN);
    delay(100);

    setLED(led0, PINK);



   cc_device_t *device = cc.newDevice(device_name, uri);

   cc.addActuator(device, getActuatorConfig("0", &switchState[0])); 
   cc.addActuator(device, getActuatorConfig("1", &switchState[1])); 
   cc.addActuator(device, getActuatorConfig("2", &switchState[2])); 
   cc.addActuator(device, getActuatorConfig("3", &switchState[3])); 
   cc.addActuator(device, getActuatorConfig("4", &switchState[4]));   
   //cc.addActuator(device, getActuatorConfig("5", &switchState[5])); 

   cc.setEventCallback(CC_EV_UPDATE, updateValues);
   cc.setEventCallback(CC_EV_ASSIGNMENT, updateValues);

   
    setLED(led4, BLUE);
    delay(100);
    setLED(led3, BLUE);
    delay(100);
    setLED(led2, BLUE);
    delay(100);
    setLED(led1, BLUE);
    delay(100);
    setLED(led0, BLUE);
}

void updateValues(cc_assignment_t *assignment){
  if (! assignment) return;
  if (! (assignment->mode & CC_MODE_TOGGLE)) return; 

  if (assignment->actuator_id >= 0 &&  assignment->actuator_id < 6){
    ccState[assignment->actuator_id] = assignment->value;
  }
}

void setLEDS(){
    setLED(led0, ccState[0]==0? BLUE: GREEN);
    setLED(led1, ccState[1]==0? BLUE: RED);
    setLED(led2, ccState[2]==0? BLUE: YELLOW);
    setLED(led3, ccState[3]==0? BLUE: PINK);
    setLED(led4, ccState[4]==0? BLUE: CYAN);
}


int loop_count = 0;
void loop() {
    turnOffAll();
    readSwitchStates();
    if (loop_count > 20){
        setLEDS();

        cc.run();  

    }else{
      loop_count++;
      setLED(led0, ccState[0]==0? BLUE: GREEN);
      delay(200);
      ccState[0] = 1 - ccState[0];
    }
    if (loop_count < 0){
      setLED(led4, RED);
      setLED(led3, RED);
      setLED(led2, RED);
      delay(1000);
    }
}
