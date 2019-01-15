
#include <ControlChain.h>

ControlChain cc;

int red1 = 0, green1 = 1, blue1 = 2;
int red2 = 3, green2 = 4, blue2 = 5; 
int p0 = 6, p1 = 7, p2 = 8;

int s0 = 9, s1 = 10, s2 = 11;

int gpio3 = 12, gpio4 = 13;

int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;  // the previous reading from the input pin

// the following variables are unsigned long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 20;    // the debounce time; increase if the output flickers

cc_actuator_t *getActuatorConfig(){

    // configure actuator
    cc_actuator_config_t actuator_config;
    actuator_config.type = CC_ACTUATOR_MOMENTARY;
    actuator_config.name = "PressMe";
//    actuator_config.value = &buttonValue;
    actuator_config.min = 0.0;
    actuator_config.max = 1.0;
    actuator_config.supported_modes = CC_MODE_TOGGLE | CC_MODE_TRIGGER;
    actuator_config.max_assignments = 1;

    // create and add actuator to device
    cc_actuator_t *actuator;
    actuator = cc.newActuator(&actuator_config);
    return actuator;
}

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

    //digitalWrite(ledPin, LOW);

    // configure button pin as input and enable internal pullup
    pinMode(gpio3, INPUT);
    pinMode(gpio4, INPUT);

    //digitalWrite(buttonPin, HIGH);

    // initialize control chain
    // note that control chain requires the Serial 0 and pin 2, which means
    // these peripherals won't be available to be used in your program
    cc.begin();

    // define device name (1st parameter) and its URI (2nd parameter)
    // the URI must be an unique identifier for your device. A good practice
    // is to use a URL pointing to your project's code or documentation
    const char *uri = "https://github.com/moddevices/cc-arduino-lib/tree/master/examples/Button";
    cc_device_t *device = cc.newDevice("Firehawk", uri);

    cc_actuator_t* actuator = getActuatorConfig();
    cc.addActuator(device, actuator);

    // set a callback function for the update event
    // this means that the updateLED function will be called by the
    // library every time there is an assignment update on this device
    cc.setEventCallback(CC_EV_UPDATE, updateLED);

    // the currently possible event callbacks are:
    // CC_EV_ASSIGNMENT, CC_EV_UNASSIGNMENT and CC_EV_UPDATE
}

void updateLED(cc_assignment_t *assignment) {
    // check if assignment mode is toggle
    // turn led on/off according the assignment value
    if (assignment->mode & CC_MODE_TOGGLE) {
       
    }
}

int readButton(void) {
    
    return 0;
}

void loop() {
    // read button state
    // state =  1 -> button pressed
    // state = -1 -> button released
    // state =  0 -> same state as before (no change)
    int state = readButton();
    if (state == 1) {
    //    buttonValue = 1.0;
    } else if (state == -1) {
    //  buttonValue = 0.0;
    }

    // this function always must be placed in your program loop
    // it's responsible for the control chain processing
    cc.run();
}
