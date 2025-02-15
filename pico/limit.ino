#include "limit.h"

Limit::Limit(byte _pin) : ezButton(_pin){
    setDebounceTime(50);
}
Limit::Limit(byte _pin, unsigned long debounce) : ezButton(_pin) {
    setDebounceTime(debounce);
}
int Limit::state() {
    loop();
    return getState();
}
bool Limit::high() {
    return state() == HIGH;
}
bool Limit::low() {
    return state() == LOW;
}
