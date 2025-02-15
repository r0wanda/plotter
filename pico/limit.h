#ifndef LIMIT_H
#define LIMIT_H

#include <ezButton.h>

class Limit : public ezButton {
public:
    Limit(byte _pin);
    Limit(byte _pin, unsigned long debounce);
    /**
     * Loop then return state
     */
    int state();
    bool high();
    bool low();
};

#endif
