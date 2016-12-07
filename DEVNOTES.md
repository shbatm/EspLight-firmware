# Development Notes for Shbatm's fork of ESPLight
This document is a running list of notes about upcoming features and TO-DO items.

## Send Serial Debug Log over Wifi
### Multicast notes:
224.0.0.251 - zeroconf address?
IPAddress broadcastIp;
broadcastIp = ~WiFi.subnetMask() | WiFi.gatewayIP();
beginPacketMulticast(IPAddress broadcastIp, 
                                   uint16_t port, 
                                   IPAddress interfaceAddress, 
                                   int ttl = 1);
