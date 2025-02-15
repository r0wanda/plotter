#ifndef MOTORS_H
#define MOTORS_H

#include "Oper.h"
#include <AccelStepper.h>

class RunToPos;
class RunWhile;

/**
 * AccelStepper wrapper with operation support
 */
class Motor : public AccelStepper {
public:
    Motor(const AccelStepper &accel, Manager *_man, char _axis);
    /**
     * Init motor
     */
    void init();
    /**
     * Run to position (in steps) at a certain speed
     */
    RunToPos* runToPos(const long steps, int speed);
    RunToPos* runToPos(Manager *_man, const long steps, int speed);
    /**
     * Run while a function returns true at a certain speed
     */
    RunWhile* runWhile(bool (*fn)(), int speed);
    RunWhile* runWhile(Manager *_man, bool (*fn)(), int speed);
private:
    Manager *man;
    char axis;
};

/**
 * Move Y motor to a position
 */
class RunToPos : public Oper {
public:
    RunToPos(Manager *manager, AccelStepper *_m, char _axis, const long _steps, int _speed);
    virtual void loop();
    virtual void end();
private:
    bool claimed;
    long steps;
    int speed;
    AccelStepper *m;
    char axis;
    char *str;
};

/**
 * Run while condition is true
 */
class RunWhile : public Oper {
public:
    RunWhile(Manager *manager, AccelStepper *_m, char _axis, bool (*_fn)(), int _speed);
    virtual void loop();
    virtual void end();
private:
    bool claimed;
    bool (*fn)();
    int speed;
    AccelStepper *m;
    char axis;
    char *str;
};

#endif
