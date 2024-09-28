#include <ezButton.h>
#define Y_DIST 250

ezButton ySw1(21);
ezButton ySw2(22);

void setup() {
  // motor 1
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  // limit switches
  ySw1.setDebounceTime(50);
  ySw2.setDebounceTime(50);
  // serial
  Serial.begin(9600);
}

// states
bool homed = false;
bool dir1 = true;
bool dir2 = true;
bool go1 = true;
bool go2 = true;
int st1 = 0;
int st2 = 0;
int i1 = 0;
int i2 = 0;
int i1top = 0;
int i2top = 0;

// input reading
int cur = 1;
bool rCur = false;

// stepper utils
int *getSt(int motor = cur) {
  switch (motor) {
  case 2:
    return &st2;
  default:
    return &st1;
  }
}
void pins(unsigned int motor, bool one, bool two, bool three, bool four) {
  int p1 = one ? HIGH : LOW;
  int p2 =  two ? HIGH : LOW;
  int p3 = three ? HIGH : LOW;
  int p4 = four ? HIGH : LOW;
  switch (motor) {
  case 1:
    digitalWrite(12, p1);
    digitalWrite(13, p2);
    digitalWrite(14, p3);
    digitalWrite(15, p4);
    break;
  case 2:
    digitalWrite(8, p1);
    digitalWrite(9, p2);
    digitalWrite(10, p3);
    digitalWrite(11, p4);
  }
}
void _step(unsigned int motor, bool dir = true) {
  if (motor > 2 || motor < 1) Serial.println("err: invalid motor");
  int *st = getSt(motor);
  
  switch (*st) {
  case 0:
    pins(motor, 0, 0, 0, 1);
    break;
  case 1:
    pins(motor, 0, 0, 1, 1);
    break;
  case 2:
    pins(motor, 0, 0, 1, 0);
    break;
  case 3:
    pins(motor, 0, 1, 1, 0);
    break;
  case 4:
    pins(motor, 0, 1, 0, 0);
    break;
  case 5:
    pins(motor, 1, 1, 0, 0);
    break;
  case 6:
    pins(motor, 1, 0, 0, 0);
    break;
  case 7:
    pins(motor, 1, 0, 0, 1);
    break;
  default:
    pins(motor, 0, 0, 0, 0);
    break;
  }
  if (dir) (*st)++;
  else (*st)--;
  if (*st > 7) *st = 0;
  if (*st < 0) *st = 7;
}

void step(int steps, unsigned int motor = 1, int del = 2) {
  int total = abs(steps);
  bool dir = steps < 0 ? false : true;
  for (int i = 0; i < total;  i++) {
    _step(motor, dir);
    delay(del);
  }
}

void homeY() {
  // pull up circuit, all values are inverted
  // go to bottom
  while (ySw1.getState() == HIGH) {
    step(1, 1);
    Serial.println(ySw1.getState());
    ySw1.loop();
  }
  while (ySw1.getState() == LOW) {
    step(-5, 1);
    Serial.println(ySw1.getState());
    ySw1.loop();
  }
  while (ySw1.getState() == HIGH) {
    step(1, 1, 6);
    Serial.println(ySw1.getState());
    ySw1.loop();
  }
  // go back up
  i1 = 0;
  while (ySw2.getState() == HIGH) {
    step(-1, 1);
    i1++;
    //Serial.println(ySw2.getState());
    ySw2.loop();
  }
  while (ySw2.getState() == LOW) {
    step(5, 1);
    i1 -= 5;
    Serial.println(ySw2.getState());
    ySw2.loop();
  }
  while (ySw2.getState() == HIGH) {
    step(-1, 1, 6);
    i1++;
    Serial.println(ySw2.getState());
    ySw2.loop();
  }
  i1top = i1;
  i1 = 0;
  Serial.println(i1top);
}

void loop() {
  ySw1.loop();
  ySw2.loop();
  if (!homed) {
    homeY();
    homed = true;
  }
  Serial.println(ySw1.getState());
  //step(-1,1, 3);
  bool ok = true;
  if (Serial.available()) {
    char ch = Serial.read();
    switch (ch) {
      case 'h':
        homeY();
        break;
    }
    if (ok) Serial.println("ok");
  }
}