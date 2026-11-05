// IO80211WCN685x.cpp — SKELETON ONLY. Untested, does not run.
// See WIFI-BRINGUP.md for the bring-up plan and IO80211WCN685x.h for
// what each TODO needs. Do not ship this as a working driver.

#include "IO80211WCN685x.h"

OSDefineMetaClassAndStructors(IO80211WCN685x, IO80211Controller)

bool IO80211WCN685x::init(OSDictionary *properties) {
    // TODO: call super::init, allocate driver state.
    return super::init(properties);
}

bool IO80211WCN685x::start(IOService *provider) {
    if (!super::start(provider)) {
        return false;
    }
    // TODO: map registers + interrupts from the provider, then
    // loadFirmware(). If firmware fails, stop and return false.
    // TODO: register the IO80211Interface with the network stack.
    return true;
}

void IO80211WCN685x::stop(IOService *provider) {
    // TODO: tear down CE rings, detach interface, unmap registers.
    super::stop(provider);
}

void IO80211WCN685x::free() {
    // TODO: free driver state.
    super::free();
}

bool IO80211WCN685x::loadFirmware() {
    // TODO: reset the target, upload the WCN6855 firmware + board data
    // from .cache/firmware, start the target, wait for WMI ready event.
    return false;
}
