# DNS-MITM Manager

A simple Tesla Overlay to request atmosphere to reload the `hosts` file, and to toggle on/off `hosts` file entries.

All lines starting with `#` are treated as section Headers.

If the line starts with a digit, it is expected to be an IP address and toggleable.

Toggling is performed by prepending or removing a `;`.

All other lines are simply ignored.

Hosts files are reloaded in DNS MITM each time the overlay is hidden. 

See Atmosphere [docs here](https://github.com/Atmosphere-NX/Atmosphere/blob/master/docs/features/dns_mitm.md) for details on the function DNS MITM.
See Tesla components [here](https://github.com/WerWolv/Tesla-Menu) and [here](https://github.com/WerWolv/nx-ovlloader) for install Tesla.
