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

/*
 * Output codes:
 * P: Positioning info
 *   P*F: fast
 *   P*SD: slow (down/left)
 *   P*SU: slow up (up/right)
 *   Above, the * represents either X or Y
 *   All P data will be the reported position (signed long)
 * I: Info
 *   Type: string (represents the info)
 * E: Error
 *   Type: string (represents the error)
 */

// switches
ezButton ySw1(Y_SW_BOTTOM);
ezButton ySw2(Y_SW_TOP);

// states
bool homed = false;
//bool dir2 = true;
bool go2 = true;
int st2 = 0;
int i2 = 0;
int yTop = 0;

// input reading
//int cur = 1;
//bool rCur = false;
AccelStepper yMotor(4, Y_MOTOR_1, Y_MOTOR_3, Y_MOTOR_2, Y_MOTOR_4);

void setup() {
  // motor 1
  yMotor.enableOutputs();
  yMotor.setMaxSpeed(1000);
  // limit switches
  ySw1.setDebounceTime(50);
  ySw2.setDebounceTime(50);
  // serial
  Serial.begin(9600);
}

void homeY() {
  // pull up circuit, all values are inverted
  // go to bottom
  int i = 0;
  while (ySw1.loop(), ySw1.getState() == LOW) {
    yMotor.setSpeed(-100);
    yMotor.runSpeed();
    Serial.print("PYSD:");
    Serial.println(yMotor.currentPosition());
    if (i % 100 == 0) Serial.println("I:Moving away from bottom limit");
    i++;
  }
  yMotor.setSpeed(-500);
  while (ySw1.loop(), ySw1.getState() == HIGH) {
    yMotor.runSpeed();
    Serial.print("PYF:");
    Serial.println(yMotor.currentPosition());
  }
  yMotor.setSpeed(500);
  while (ySw1.loop(), ySw1.getState() == LOW) {
    yMotor.runSpeed();
    Serial.print("PYF:");
    Serial.println(yMotor.currentPosition());
  }
  yMotor.setSpeed(-100);
  while (ySw1.loop(), ySw1.getState() == HIGH) {
    yMotor.runSpeed();
    Serial.print("PYSD:");
    Serial.println(yMotor.currentPosition());
  }
  Serial.print("BOT:");
  Serial.println(yMotor.currentPosition());
  // go back up
  i = 0;
  while (ySw2.loop(), ySw2.getState() == LOW) {
    delay(100);
    if (i % 10) Serial.println("I:Waiting for limit switch to update");
  }
  yMotor.setCurrentPosition(0);
  yMotor.setSpeed(500);
  while (ySw2.loop(), ySw2.getState() == HIGH) {
    yMotor.runSpeed();
    Serial.print("PYF:");
    Serial.println(yMotor.currentPosition());
  }
  yMotor.setSpeed(-500);
  while (ySw2.loop(), ySw2.getState() == LOW) {
    yMotor.runSpeed();
    Serial.print("PYF:");
    Serial.println(yMotor.currentPosition());
  }
  yMotor.setSpeed(100);
  while (ySw2.loop(), ySw2.getState() == HIGH) {
    yMotor.runSpeed();
    Serial.print("PYSU:");
    Serial.println(yMotor.currentPosition());
  }
  yTop = yMotor.currentPosition();
  Serial.print("TOP:");
  Serial.println(yTop);
}

void loop() {
  ySw1.loop();
  ySw2.loop();
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