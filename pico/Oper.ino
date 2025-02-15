#include "Oper.h"
#include "log.h"
#include <Array.h>

// manager
Manager::Manager() {
    y = nullptr;
    info("New manager created");
}
Manager::Manager(Array<Oper*, OPER_MAX_INSTANCES> operations) {
    opers = operations;
    y = nullptr;
    info("New manager created");
}
Manager::Manager(Manager &man) {
    opers = man.opers;
    remBuf = man.remBuf;
    y = man.y;
    info("Manager copied");
}
void Manager::add(Oper *oper) {
    opers.push_back(oper);
}
void Manager::all() {
    _log("I:oper size", (int)opers.size());
    for (size_t i = 0; i < opers.size(); i++) {
        Oper *op = opers[i];
        if (op == nullptr) continue;
        if (true){//op->canRun()) {
          _log("I:running oper", (int)i);
          op->loop();
          op->updTime();
        }
    }
    removeBuf();
}
void Manager::removeBuf() {
    for (size_t i = 0; i < remBuf.size(); i++) {
        remove(remBuf[i].oper, true);
    }
    remBuf = Array<RemBufItem, OPER_MAX_INSTANCES>();
}
void Manager::remove(Oper *oper, bool now = false) {
    size_t i = 0;
    bool found = false;
    for (;i < opers.size(); i++) {
        if (opers[i] == oper) {
            found = true;
            if (now) opers.remove(i);
            break;
        }
    }
    if (!found) return;
    if (oper == y) y = nullptr;
    if (!now) remBuf.push_back(RemBufItem{oper, i});
}
void Manager::waitForAll() {
    while (opers.size() > 0) all();
}
bool Manager::claim(char axis, Oper *oper) {
  Oper *&a = this->x;
  switch (axis) {
    case 'X':
    case 'x':
      a = this->x;
      break;
    case 'Y':
    case 'y':
      a = this->y;
      break;
    case 'Z':
    case 'z':
      a = this->z;
      break;
    default:
      error("Trying to claim invalid motor");
      return false;
  }
  if (a != nullptr) {
    if (a == oper) return true;
    if (!a->isRunning()) a = nullptr;
    else return false;
  }
  a = oper;
  return true;
}

// oper
Oper::Oper(Manager *manager) {
    man = manager;
    running = true;
    lastRun = 0;
    del = -1;
    man->add(this);
    info("New operation started");
}
Oper::Oper(Manager *manager, int minDel) {
    man = manager;
    running = true;
    del = minDel;
    lastRun = 0;
    man->add(this);
    info("New operation started");
}
void Oper::end() {
    if (cend) cend();
    man->remove(this);
    running = false;
    info("Operation ended");
}
void Oper::wait() {
    while (isRunning()) this->loop();
}
bool Oper::isRunning() {
    return running;
}
bool Oper::canRun() {
    if (del < 1) return true;
    return lastRun + del <= millis();
}
void Oper::updTime() {
    lastRun = millis();
}

Series::Series(Manager *_man) : Manager(), Oper(_man) {
    info("New series operation started");
}
Series::Series(Manager *_man, Array<Oper*, OPER_MAX_INSTANCES> operations) : Manager(operations), Oper(_man) {
    info("New series operation started");
}
void Series::loop() {
    all();
}
void Series::all() {
    if (opers.size() < 1) {
      return;
    }
    _log("I:series oper size", (int)opers.size());
    Oper *op = opers[0];
    if (op == nullptr) return;
    if (true) {
      op->loop();
      op->updTime();
    }
    removeBuf();
    if (opers.size() < 1) end();
}
bool Series::claim(char axis, Oper *oper) {
  return man->claim(axis, oper);
}
