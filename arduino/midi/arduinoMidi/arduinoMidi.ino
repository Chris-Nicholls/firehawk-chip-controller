#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  // put your setup code here, to run once:
  MIDI.begin();
}


byte v = 0;
void loop() {
  delay(10);

  v = sin(float(millis()*2) / 1000.0)*64 + 64 ;
  MIDI.sendControlChange(1, v, 1);
  MIDI.read();
}
