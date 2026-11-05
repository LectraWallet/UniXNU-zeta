# Wi-Fi bring-up for UniXNU-zeta (Surface Duo 2)

> **STATUS: EXPERIMENTAL / UNTESTED — Wi-Fi does NOT work yet.**
> This branch is a bring-up plan. Nothing here has run on hardware.
> Do not expect a network interface to appear.

## Correction: the Duo 2 needs ath11k, not ath12k

UniXNU already has a working Wi-Fi stack — but it does not cover this
phone. **Ath12kWifi** ([UniXNU/Ath12kWifi](https://github.com/UniXNU/Ath12kWifi))
runs Linux ath12k + mac80211/cfg80211 on XNU, with MHI/QRTR/QMI for startup
and wpa_supplicant 2.12 for authentication. On sheng (Xiaomi Pad 6S Pro,
SM8550, WCN7850) it joins WPA2-Personal networks, gets an address by DHCP
and loads web pages.

The Duo 2's chip is **WCN6855** (FastConnect 6900, PCI `17cb:1103`), and on
Linux it is driven by **ath11k_pci — not ath12k**. ath12k is the Wi-Fi 7
driver for WCN7850/QCN9274, forked from ath11k as a clean start for the new
generation; WCN6855 support lives in ath11k, and Ath12kWifi's hardware list
is qcc2072, qcn9274 and wcn7850 only. So the Duo 2 job is **porting ath11k
to XNU for WCN6855** — not adapting Ath12kWifi, and not a simple
device-definition add to it. (An earlier version of this file said
"adapt Ath12kWifi"; that was wrong.)

What carries over from Ath12kWifi is the integration pattern, not the
wireless driver: the `nw_abi.h` interface (`lx_platform`, `nw_board`,
`nw_wifi_events`), the WifiProbe/WifiNet shape, and wpa_supplicant for
authentication. The worker underneath must be ath11k-based (WMI/HTC/Copy
Engine) instead of ath12k's MHI/QRTR path.

Reference: the official adaptation guide at
https://unixnu.com/porting/network and the component doc
`docs/drivers/ath12k-wifi.md` in the UniXNU repo.

## Hardware

- Phone: Microsoft Surface Duo 2, codename `zeta`, Qualcomm SM8350
  (Snapdragon 888).
- Wi-Fi: Qualcomm FastConnect 6900, silicon WCN6855 — Wi-Fi 6 (802.11ax),
  dual-band 2.4/5 GHz. Bluetooth 5.1 shares the same chip; that is a
  separate job and is not covered here.

## What porting ath11k to zeta needs (in order)

1. **Identify the radio.** WCN6855 PCI ID `17cb:1103`, ath11k firmware and
   board-data variant, from the Duo 2's own firmware dump. Record them in
   `proprietary-files.txt` (same pattern as the existing EDL-dump pins).
2. **Bring up the PCIe endpoint.** Record the controller/PHY windows, lane
   count, reset sequence, WLAN enable, PERST, reference clock, regulators,
   interconnect votes, SMMU streams and MSI routing. Then: power and clock
   the endpoint, train the link, read the vendor/device ID, assign and map
   the BAR, establish the DMA range and prove buffers are reachable,
   attach MSI and confirm delivery. (Reference: WCN7850 on SM8550 PCIe0
   trains at Gen2 x1, endpoint `17cb:1107`, MSIs to SPI 141.)
3. **Port the ath11k worker.** Implement the component callbacks from
   `include/nw_abi.h`: `lx_platform` (time, sleep, polling, logging,
   allocation, DMA alloc/map/sync, `fw_get`) and `nw_board` (mapped BAR,
   PCI identity, config-space access, MSI target). Port ath11k's WMI
   command/event handling, HTC message transport and Copy Engine rings
   behind that interface; service the worker step function the way
   Ath12kWifi services `nw_wifi_step`. Checkpoints: firmware and QRTR
   services start, then scanning reports BSS entries.
4. **Connect the OS network interface.** Adapt `WifiNet.cpp` to the zeta
   XNU and wireless-framework ABI (`nw_wifi_scan`, `nw_wifi_connect`,
   `nw_wifi_tx`, receive/link events, MAC identity, channel/regulatory
   data), select the ath11k-based Wi-Fi component in the component list,
   rebuild the kernel, payload and boot image. Checkpoints: scan,
   authentication, association, address configuration, receive and
   transmit.
5. **Hardware test on a real Duo 2.** Scan, join an open network, then
   WPA2, then DHCP and sustained traffic. Nothing before this step proves
   anything. Note the zeta port still has no GUI, so there is no Settings
   UI to pick networks until the display stack lands.

## Firmware

The chip runs on signed firmware blobs. On sheng they load from
`/private/var/ngpu/firmware/ath12k/WCN7850/hw2.0/` after addon install.
For zeta, extract the WCN6855 firmware + board data from the EDL dump and
pin file sizes and SHA-256 hashes the same way. Do **not** commit firmware
blobs to this repo.

## Note on the skeleton in this branch

`drivers/wcn685x/` (IO80211WCN685x.h/.cpp, Info.plist) predates the
Ath12kWifi finding and follows the wrong architecture: it assumes a native
IO80211Controller driver, but the project runs the Linux wireless stack on
XNU instead. Ignore it — the porting plan above is the real path.

## License

Same license as the rest of this repository.
