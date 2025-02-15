#include "log.h"

void _log(const char *cmd, const char *str) {
    if (!Serial) return;
    Serial.print(cmd);
    Serial.print(":");
    Serial.println(str);
}
void _log(const char *cmd, int str) {
    if (!Serial) return;
    Serial.print(cmd);
    Serial.print(":");
    Serial.println(str);
}
void _log(const char *cmd, long str) {
    if (!Serial) return;
    Serial.print(cmd);
    Serial.print(":");
    Serial.println(str);
}
void _log(const char *cmd, float str) {
    if (!Serial) return;
    Serial.print(cmd);
    Serial.print(":");
    Serial.println(str);
}
void _log(const char *cmd, double str) {
    if (!Serial) return;
    Serial.print(cmd);
    Serial.print(":");
    Serial.println(str);
}
void info(const char *str) {
    _log("I", str);
}
void warn(const char *str) {
    _log("W", str);
}
void error(const char *str) {
    _log("E", str);
}

const char *posLogStr(int speed, char axis) {
  char *str = (char*)malloc(sizeof(char) * 6);
  char spd;
  char dir;

  if (speed > 0) dir = 'U';
  else dir = 'D';
  if (abs(spd) >= 500) spd = 'F';
  else spd = 'S';
  snprintf(str, 6, "P%c%c%c:", axis, spd, dir);

  return str;
}
