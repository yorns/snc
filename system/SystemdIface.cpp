#include "SystemdIface.h"
#include <iostream>

#ifdef WITH_SYSTEMD
#include <systemd/sd-daemon.h>

void SystemdIface::notifyWatchdog() {
    sd_notify(0, "WATCHDOG=1"); // spec recomments: ignore return value
}

void SystemdIface::notifyReady() {
    sd_notify(0, "READY=1"); // spec recomments: ignore return value
}

uint64_t SystemdIface::getInterval() {
    uint64_t timeout_usec;
    if (sd_watchdog_enabled(0, &timeout_usec) > 0)
        return timeout_usec;
    return 0;
}


#else // not WITH_SYSTEMD

void SystemdIface::notifyWatchdog() {
    std::cerr << "notify watchdog\n";
}

void SystemdIface::notifyReady() {
    std::cerr << "notify ready\n";
}

uint64_t SystemdIface::getInterval() {
    std::cerr << "time: 20s\n";
    return 20*1000*1000; /* 20 seconds in micro seconds */
}

#endif // WITH_SYSTEMD

