#include <AccelStepper.h>
#include <ezButton.h>
// consts
#define Y_DIST 250
// pins
#define Y_SW_BOTTOM 21
#define Y_SW_TOP 22
#define Y_MOTOR_1 12
#define Y_MOTOR_2 13
#define Y_MOTOR_3 14
#define Y_MOTOR_4 15

// switches
ezButton ySw1(Y_SW_BOTTOM);
ezButton ySw2(Y_SW_TOP);

// motors
struct Motor {
  unsigned short one;
  unsigned short two;
  unsigned short three;
  unsigned short four;
  bool dir;
  int go;
  int st;
  int i;
  int iTop;
};
//struct Motor yMotor;

// states
bool homed = false;
//bool dir2 = true;
bool go2 = true;
int st2 = 0;
int i2 = 0;
int i2top = 0;

// input reading
//int cur = 1;
//bool rCur = false;
AccelStepper yMotor(4, Y_MOTOR_1, Y_MOTOR_3, Y_MOTOR_2, Y_MOTOR_4);

void setup() {
  // motor 1
  yMotor.enableOutputs();
  yMotor.setMaxSpeed(1000);
  /*pinMode(Y_MOTOR_1, OUTPUT);
  pinMode(Y_MOTOR_2, OUTPUT);
  pinMode(Y_MOTOR_3, OUTPUT);
  pinMode(Y_MOTOR_4, OUTPUT);
  /*yMotor.one = Y_MOTOR_1;
  yMotor.two = Y_MOTOR_2;
  yMotor.three = Y_MOTOR_3;
  yMotor.four = Y_MOTOR_4;
  yMotor.dir = true;
  yMotor.go = false;
  yMotor.st = 0;
  yMotor.i = 0;
  yMotor.iTop = 0;*/
  // limit switches
  ySw1.setDebounceTime(50);
  ySw2.setDebounceTime(50);
  // serial
  Serial.begin(9600);
}

// stepper utils
/*void pins(unsigned int motor, bool one, bool two, bool three, bool four) {
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
void _step(struct Motor *m, bool dir = true) {
  if (m) Serial.println("err: invalid motor");
  int *st = &m->st;
  
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

void step(int steps, struct Motor *motor = &yMotor, int del = 2) {
  int total = abs(steps);
  bool dir = steps < 0 ? false : true;
  for (int i = 0; i < total;  i++) {
    _step(motor, dir);
    if (del > 0) delay(del);
  }
}*/

/*void homeY() {
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
    step(-1, 1, 0);
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
}*/

void loop() {
  ySw1.loop();
  ySw2.loop();
  yMotor.setSpeed(500);
  yMotor.runSpeed();
  if (!homed) {
    //homeY();
    homed = true;
  }
  /*Serial.println(ySw1.getState());
  //step(-1,1, 3);
  bool ok = true;
  if (Serial.available()) {
    char ch = Serial.read();
    switch (ch) {
      case 'h':
        //homeY();
        break;
    }
    if (ok) Serial.println("ok");
  }*/
}