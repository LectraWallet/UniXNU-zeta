// IO80211WCN685x.h — SKELETON ONLY. Untested, does not run, does not
// compile against the XNU tree yet. See WIFI-BRINGUP.md.
// Target: Qualcomm FastConnect 6900 (WCN6855) on Surface Duo 2 (zeta, SM8350).
// This file declares the shape of an IO80211Family driver; every real
// method is a TODO. Method signatures must be checked against the
// IO80211Family headers (Kernel Debug Kit) before any of this is trusted.

#ifndef IO80211WCN685X_H
#define IO80211WCN685X_H

#include <IOKit/IOService.h>
#include <IOKit/network/IO80211Controller.h>

class IO80211WCN685x : public IO80211Controller {
    OSDeclareDefaultStructors(IO80211WCN685x)

public:
    // IOKit lifecycle.
    virtual bool init(OSDictionary *properties) override;
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    virtual void free() override;

    // IO80211Controller: interface and link state.
    // TODO: implement getHardwareAddress, enable/disable, and the
    // scan/join/auth/assoc/key entry points against IO80211Family.
    // TODO: attach to the platform provider (PCIe node from the ADT/DT)
    // and map the chip's registers + interrupts.

    // Firmware: load WCN6855 firmware + board data (extracted from the
    // device dump, see WIFI-BRINGUP.md) and start the target.
    // TODO: implement reset -> upload -> start sequence.
    virtual bool loadFirmware();

    // WMI/HTC/CE: port the ath11k command/event, message transport and
    // Copy Engine ring logic here.
    // TODO: implement WMI handling, HTC endpoints, CE rings.

private:
    // TODO: register base, interrupt source, firmware buffers,
    // WMI/HTC/CE state.
};

#endif // IO80211WCN685X_H
