#include "motors.h"
#include <AccelStepper.h>
#include "Oper.h"
#include "log.h"

Motor::Motor(const AccelStepper &accel, Manager *_man, char _axis) : AccelStepper(accel) {
    man = _man;
    axis = _axis;
}
void Motor::init() {
    enableOutputs();
    setMaxSpeed(1000);
}
RunToPos* Motor::runToPos(const long steps, int speed = 500) {
    return new RunToPos(man, this, axis, steps, speed);
}
RunToPos* Motor::runToPos(Manager *_man, const long steps, int speed = 500) {
    return new RunToPos(_man, this, axis, steps, speed);
}
RunWhile* Motor::runWhile(bool (*fn)(), int speed = 500) {
    return new RunWhile(man, this, axis, fn, speed);
}
RunWhile* Motor::runWhile(Manager *_man, bool (*fn)(), int speed = 500) {
    return new RunWhile(_man, this, axis, fn, speed);
}

RunToPos::RunToPos(Manager *manager, AccelStepper *_m, char _axis, const long _steps, int _speed = 500) : Oper(manager) {
    claimed = false;
    steps = _steps;
    speed = _speed;
    m = _m;
    axis = _axis;
    const char *_str = posLogStr(speed, axis);
    strncpy(str, _str, 6);
}
void RunToPos::end() {
    Oper::end();
    free(str);
}

void RunToPos::loop() {
    if (!claimed) {
        if (man->claim(axis, this)) {
            claimed = true;
            long cur = m->currentPosition();
            if (speed == 0 || steps == cur) {
                end();
                return;
            }
            speed = abs(speed);
            if (steps < cur) speed = 0 - speed;
            m->setSpeed(speed);
            m->moveTo(steps);
        } else return;
    }
    if (m->run()) {
        _log(str, m->currentPosition());
    } else end();
}

RunWhile::RunWhile(Manager *manager, AccelStepper *_m, char _axis, bool (*_fn)(), int _speed = 500) : Oper(manager) {
    claimed = false;
    speed = _speed;
    m = _m;
    fn = _fn;
    axis = _axis;
    const char *_str = posLogStr(speed, axis);
    strncpy(str, _str, 6);
    if (speed == 0 || fn == nullptr) end();
}
void RunWhile::loop() {
    if (!claimed) {
        if (man->claim(axis, this)) {
            claimed = true;
            m->setSpeed(speed);
        } else return;
    }
    if (fn()) {
        m->runSpeed();
        _log(str, m->currentPosition());
    } else {
        end();
    }
}
void RunWhile::end() {
    Oper::end();
    free(str);
}
