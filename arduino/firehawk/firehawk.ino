#include <MIDI.h>

#define CONTROL_CHANNEL 1
#define SWITCH_DELAY 1
#define DEBOUNCE_REPEATS 1
#define ANALOG_DEBOUNCE_REPEATS 100

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

#define ANALOGUE_BASE 14

MIDI_CREATE_DEFAULT_INSTANCE();



#define red1 3
unsigned char green1 = 4, blue1 = 5;
unsigned char red2 = 6, green2 = 7, blue2 = 8; 
unsigned char p0 = 11, p1 = 12, p2 = 13;

unsigned char s0 = 11, s1 = 12, s2 = 13;

unsigned char gpio3 = 9, gpio4 = 10;

int acuator_count = 0;

char analogueState[] = {0,0,0,0,0,0,0};
float switchState[] = {0,0,0,0,0,0};
float ccState[] = {0,0,0,0,0,0};

char debounceTimes[] = {0,0,0,0,0,0};
char analogDebounceTimes[] = {0,0,0,0,0,0,0};

void readAndSendAnalogue(){
  for (int i = 0; i < 7; i++){
    float readIn = analogRead(ANALOGUE_BASE + i);
    float newState = readIn;

    char newValue = newState/1024 * 127;
    char currentValue = analogueState[i];
    if (newValue != currentValue){
      analogDebounceTimes[i]++;
      if (analogDebounceTimes[i] > ANALOG_DEBOUNCE_REPEATS){
        analogueState[i] = newValue;
        MIDI.sendControlChange(i + 10, newValue , CONTROL_CHANNEL);
//        Serial.print(i);
//        Serial.print(": ");
//        Serial.print(newValue, DEC);
//        Serial.print("\n");
      }
    }else{
      analogDebounceTimes[i] = 0;
    }
  } 
}


float readSwitchState(unsigned char gpio_pin, int switch_index){
    float newState = 1 - digitalRead(gpio_pin);
    float currentState = switchState[switch_index];
    if (newState != currentState){
        debounceTimes[switch_index]++;
    }else{
        debounceTimes[switch_index] = 0;
    }
    if (debounceTimes[switch_index] >= DEBOUNCE_REPEATS){
        return newState;
    }
    return currentState;
}

void readSwitchStates(){
    turnOffAll();

    digitalWrite(s0, LOW);
    digitalWrite(s1, HIGH);
    digitalWrite(s2, HIGH);
    delay(1);

    readSwitchAndSend(gpio3, 5);
    readSwitchAndSend(gpio4, 2);

    digitalWrite(s0, HIGH);
    digitalWrite(s1, LOW);
    delay(1);

    readSwitchAndSend(gpio3, 0);
    readSwitchAndSend(gpio4, 3);
    
    digitalWrite(s1, HIGH);
    digitalWrite(s2, LOW);
    delay(1);

    readSwitchAndSend(gpio3, 1);
    readSwitchAndSend(gpio4, 4);
}

void readSwitchAndSend(unsigned char gpio_pin, int switch_index){
    float currentState = switchState[switch_index];
    float newState = readSwitchState(gpio_pin, switch_index);
    if (newState != currentState){
        switchState[switch_index] = newState;
        byte value = (1- ccState[switch_index]) * 127;
        if (newState == 1){
          MIDI.sendControlChange(switch_index + 1, value , CONTROL_CHANNEL);
          ccState[switch_index] = 1 - ccState[switch_index];
        }
    }
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

void setup() {
    MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming messages
//    Serial.begin(9600);

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

//    pinMode(ANALOGUE_BASE, INPUT);
//    pinMode(ANALOGUE_BASE+1, INPUT);
//    pinMode(ANALOGUE_BASE+2, INPUT);
//    pinMode(ANALOGUE_BASE+3, INPUT);
//    pinMode(ANALOGUE_BASE+4, INPUT);
//    pinMode(ANALOGUE_BASE+5, INPUT);

  
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
    // MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming messages
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
    readAndSendAnalogue();
    setLEDS();
}
