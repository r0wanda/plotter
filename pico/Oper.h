#ifndef OPER_H
#define OPER_H

#ifndef OPER_MAX_INSTANCES
#define OPER_MAX_INSTANCES 20
#endif

#include <Array.h>

class Manager;
class Oper;
class Series;

class Oper {
public:
    Oper(Manager *manager);
    Oper(Manager *manager, int minDel);
    Oper(Oper &oper);
    /**
     * Loop function (virtual)
     */
    virtual void loop() = 0;
    /**
     * End function (call to stop operation)
     */
    virtual void end();
    void wait();
    bool isRunning();
    bool canRun();
    void updTime();
    /**
     * Custom user-defined ending function
     */
    void (*cend)();
    friend bool claimXMotor(Oper *oper);
    friend bool claimYMotor(Oper *oper);
    friend bool claimZMotor(Oper *oper);
protected:
    Manager *man;
private:
    bool running;
    int del;
    unsigned long lastRun;
};

struct RemBufItem {
    Oper *oper;
    size_t i;
};

class Manager {
public:
    Manager();
    Manager(Array<Oper*, OPER_MAX_INSTANCES> operations);
    Manager(Manager &man);
    void add(Oper *oper);
    virtual void all();
    void remove(Oper *oper, bool now);
    void waitForAll();
    virtual bool claim(char axis, Oper *oper);
    friend class Series;
protected:
    void removeBuf();
    Array<RemBufItem, OPER_MAX_INSTANCES> remBuf;
    Array<Oper*, OPER_MAX_INSTANCES> opers;
    Oper *x;
    Oper *y;
    Oper *z;
};

class Series : public Manager, public Oper {
public:
    Series(Manager *_man);
    Series(Manager *_man, Array<Oper*, OPER_MAX_INSTANCES> operations);
    virtual void loop();
    virtual void all();
    virtual bool claim(char axis, Oper *oper);
};

#endif
