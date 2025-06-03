#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <time.h>

class TimeManager {
public:
  void setupTime() {
    configTime(25200, 0, "pool.ntp.org");
  }
};

#endif