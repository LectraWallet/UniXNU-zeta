# Wi-Fi bring-up for UniXNU-zeta (Surface Duo 2)

> **STATUS: EXPERIMENTAL / UNTESTED — Wi-Fi does NOT work yet.**
> This branch is a bring-up plan plus a driver skeleton. Nothing here has
> run on hardware. Do not expect a network interface to appear. The work
> below is the honest starting point, not a finished driver.

## Where the port stands (2026-11-05)

From the repo's own README (main branch): the XNU kernel boots on the Duo 2,
`launchd` reaches PID 1 (via the owner-root policy), AMFI loads, and the
read-only root mounts. The graphical interface still needs bring-up, and
there is **no Wi-Fi code anywhere in the tree** — no driver, no firmware
loading, no branch. Wi-Fi sits behind the GUI: there is no SpringBoard to
show a network picker until the display stack lands.

## Hardware

- Phone: Microsoft Surface Duo 2, codename `zeta`, Qualcomm SM8350
  (Snapdragon 888).
- Wi-Fi: Qualcomm FastConnect 6900, silicon WCN6855 — Wi-Fi 6 (802.11ax),
  dual-band 2.4/5 GHz. Bluetooth 5.1 shares the same chip; that is a
  separate job and is not covered here.
- The Wi-Fi block's interconnect must be confirmed from the stock device
  tree (`zeta.dtb`, built by `scripts/make-zeta-dtb.py`): WCN685x on SM8350
  is normally exposed over PCIe. Verify the node and its address before
  writing the attach path — do not assume it.

## Reference implementation: Linux ath11k

ath11k is the mainline Linux driver for Qualcomm 802.11ax parts (in the
kernel since v5.6) and the only complete open implementation of this chip.
Its layering is the map for the XNU driver:

```
mac80211 (Linux 802.11 stack)
  ath11k core (WMI command/event handling, scan, assoc, keys, TX/RX)
    WMI  — Wireless Module Interface (firmware command protocol)
    HTC  — Host-Target Communication (message transport)
    HIF  — Host Interconnect Framework
    CE   — Copy Engine (DMA rings)
    PCI  — bus attachment
```

The XNU equivalent replaces `mac80211` with Apple's `IO80211Family`
(`IO80211Controller` / `IO80211Interface`) and the PCI/CE layers with the
platform's bus attachment from the ADT. The WMI/HTC/CE logic is the part
worth porting, not the Linux bus code.

Sources: https://wireless.docs.kernel.org/en/latest/en/users/drivers/ath11k/installation.html
and the ath11k tree in mainline Linux (`drivers/net/wireless/ath/ath11k/`).

## Firmware

The chip runs on signed firmware blobs. On Linux these ship in
`linux-firmware` under `ath11k/WCN6855/hw2.0/` (`board-2.bin` plus the
`WLAN.HSP.*` firmware image). Do **not** commit firmware blobs to this
repo. Extract them from the phone's own firmware dump, the same way
`proprietary-files.txt` already pins EDL-dump partitions by SHA-256 — add
the Wi-Fi firmware partitions to that file once their names are confirmed
from the dump, and load them from `.cache/firmware` at build time.

## What a working driver needs (in order)

1. **GUI/SpringBoard working first.** No network UI exists until the
   display stack lands (see README Stage 6).
2. **Device discovery.** Confirm the WCN6855 node in the ADT/DT and its
   interrupt + memory resources. Add it to the platform description.
3. **Firmware loading.** Extract the WCN6855 firmware + board data from
   the EDL dump; implement the load sequence (reset, upload, start) in the
   driver.
4. **WMI/HTC/CE port.** Port the ath11k WMI command/event handling, HTC
   message transport and Copy Engine ring management to XNU. This is the
   bulk of the work.
5. **IO80211Controller subclass.** Implement scan, join, auth/assoc, key
   management and TX/RX against `IO80211Family`, and publish an
   `IO80211Interface` so the network stack sees `en0`.
6. **Hardware test.** Scan for networks on the real Duo 2, associate to an
   open network, then WPA2, then check throughput and power management.
   Nothing before step 6 proves anything.

## What is in this branch

- This document (`WIFI-BRINGUP.md`).
- `drivers/wcn685x/` — a minimal `IO80211Controller` skeleton
  (`IO80211WCN685x.h` / `.cpp`, `Info.plist`). It declares the class,
  matching dictionary and the firmware-load hook, and every real method is
  a marked TODO. It does not compile against the XNU tree yet and does not
  run. It exists so the next person starts from the right shape instead of
  a blank file.

## License

Same license as the rest of this repository.
