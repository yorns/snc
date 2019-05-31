#ifndef SNC_SYSTEMDIFACE_H
#define SNC_SYSTEMDIFACE_H

#include <cstdint>

class SystemdIface {

public:
    void notifyWatchdog();
    void notifyReady();

    uint64_t getInterval();
};

#endif // SNC_SYSTEMDIFACE_H

