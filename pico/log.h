#ifndef LOG_H
#define LOG_H

/**
 * Basic log function (cmd:str format)
 * str is usually a string but can be a number as well
 */
void _log(const char *cmd, const char *str);
void _log(const char *cmd, int str);
void _log(const char *cmd, long str);
void _log(const char *cmd, float str);
void _log(const char *cmd, double str);
/**
 * Log info (I: format)
 */
void info(const char *str);
/**
 * Log warning (W: format)
 */
void warn(const char *str);
/**
 * Log error (E: format)
 */
void error(const char *str);

/**
 * Get positioning info code for given speed (eg: "PYFD:")
 */
const char *posLogStr(int speed, char axis);

#endif
