#include <AccelStepper.h>

#include "Oper.h"
#include "opers.h"
#include "limit.h"
#include "log.h"
#include "motors.h"
#include <Array.h>

// consts
#define Y_DIST 250.0
// pins
#define Y_SW_BOTTOM 21
#define Y_SW_TOP 22
#define Y_MOTOR_1 12
#define Y_MOTOR_2 13
#define Y_MOTOR_3 14
#define Y_MOTOR_4 15

/*
 * Output codes:
 * 
 * Format: key:data
 * 
 * P: Positioning info
 *   P(X?Y?Z)(F?S)(U?D):long
 *   X?Y?Z: Specifies which axis is moving (X, Y, or Z)
 *   F?S: Specifies speed of motor (fast/slow)
 *   U?D: Specifies direction of motor (up/down or left/right)
 *   Examples:
 *     PYFU:2000 (means motor on Y axis is moving upwards at high speed and is currently at step 2000)
 *     PXSD:12540 (means motor on X axis is moving right at slow speed and is currently at step 12540)
 *   All P data will be the reported position/step (signed long)
 *   Data is signed because it is possible for motor to be at a negative step value (especially during homing)
 *   
 * H: Homing info
 *  HBOT:long
 *    Reached bottom of Y axis
 *    Data: Bottom position (before zeroed out)
 *  HTOP:long
 *    Reached top of Y axis OR cached homing value
 *    Data: Top position
 *  HSPMM(X?Y):double
 *    Steps per MM on the Y or X axis
 *    Data: Cached value
 * 
 * I:
 *    Info output
 *    Data: string (the info)
 * E:
 *    Error output
 *    Type: string (the error)
 */

Manager master;

// switches
Limit ySw1(Y_SW_BOTTOM);
Limit ySw2(Y_SW_TOP);

// states
bool homed = false;
//bool dir2 = true;
bool go2 = true;
int st2 = 0;
int i2 = 0;
int yTop = 0;
double stepsPerMMY = 0.0;
double stepsPerMMX = 0.0;

// input reading
//int cur = 1;
//bool rCur = false;
Motor yMotor(AccelStepper(4, Y_MOTOR_1, Y_MOTOR_3, Y_MOTOR_2, Y_MOTOR_4), &master, 'Y');

void setup() {
  // motor 1
  //yMotor.init();
  // serial
  Serial.begin(9600);
}

/**
 * Home Y axis
 * Output codes used:
 * I
 * P***
 * HBOT
 * HTOP
 */
void homeY() {
  // pull up circuit, all values are inverted (HIGH = off, LOW = on)
  // series of operations (run in order)
  Series seq(&master);

  class BotSwUpd: public Oper {
  public:
    BotSwUpd(Manager *m) : Oper(m, 50) {};
    virtual void loop() {
	    info("Waiting for limit switch to update");
	    if (!ySw1.low()) end();
    }
  };
  BotSwUpd b(&seq);
  bool (*high1)() = [](){ return ySw1.high(); };
  bool (*low1)() = [](){ return ySw1.low(); };
  bool (*high2)() = [](){ return ySw2.high(); };
  bool (*low2)() = [](){ return ySw2.low(); };
  // go to bottom
  yMotor.runWhile(&seq, high1, -500);
  // go away from switch
  yMotor.runWhile(&seq, low1, 500);
  // slowly approach switch
  RunWhile *appBot = yMotor.runWhile(&seq, high1, -100);
  // log bottom position
  appBot->cend = []() {
    _log("HBOT", yMotor.currentPosition());
  };
  // go back up
  class TopSwUpd : public Oper {
  public:
    TopSwUpd(Manager *m) : Oper(m, 50) {};
    virtual void loop() {
      if (!ySw2.low()) this->end();
	    info("Waiting for limit switch to update");
    }
    virtual void end() {
      yMotor.setCurrentPosition(0);
      Oper::end();
    }
  };
  // go to top
  yMotor.runWhile(&seq, high2, 500);
  // go away from switch
  yMotor.runWhile(&seq, low2, -500);
  // slowly approach switch
  RunWhile *appTop = yMotor.runWhile(&seq, high2, 100);
  appTop->cend = []() {
    // log & store top position
    yTop = yMotor.currentPosition();
    _log("HTOP", yTop);
    // store steps per mm
    stepsPerMMY = (double)yTop / Y_DIST;
  };
}
/**
 * Output cached homing info
 * Output codes used:
 * HTOP
 * HSPMMY
 */
void cachedHome() {
  _log("HTOP", yTop);
  _log("HSPMMY", stepsPerMMY);
}

long mm2StepsY(const float &mm) {
  return round(stepsPerMMY * mm);
}
double steps2MMY(const long &steps) {
  return (double)steps / stepsPerMMY;
}

void mmY(const float &mm, int speed = 500) {
  int steps = mm2StepsY(mm);
}

void home(int cmd) {
  switch (cmd) {
	case 'Y':
	  homeY();
	  break;
	case 'C':
	  cachedHome();
	  break;
  }
}
void yCmd(int cmd) {
  switch (cmd) {
	case 'P':
	  Serial.println(yMotor.currentPosition());
	  break;
	/*case 'S':
	  float mm = Serial.parseFloat(SKIP_NONE);
	  
	  mmY();*/
  }
}

void loop() {
  master.all();
  //step(-1,1, 3);
  //bool ok = true;
  if (Serial.available()) {
	int ch = Serial.read();
	switch (ch) {
	  case 'H': {
		  int cmd = Serial.read();
		  home(cmd);
		  break;
	  }
	  case 'Y': {
		  int cmd = Serial.read();
		  yCmd(cmd);
		  break;
	  }
	}
	Serial.println("OK:");
  }
}
