STM32F7-USB-host-ME906-lwIP-MQTT [![Build Status](https://travis-ci.org/DISTORTEC/STM32F7-USB-host-ME906-lwIP-MQTT.svg)](https://travis-ci.org/DISTORTEC/STM32F7-USB-host-ME906-lwIP-MQTT)
================================

MQTT demo project for [32F746GDISCOVERY](http://www.st.com/en/evaluation-tools/32f746gdiscovery.html) (also known as
*STM32F746G-DISCO*) and [NUCLEO-F767ZI](https://www.st.com/en/evaluation-tools/nucleo-f767zi.html) boards. It features
following components:
- *distortos*,
- *ST's* *STM32F7 HAL* and *USB host* libraries, with numerous fixes to make them actually usable,
- USB class driver for *Huawei ME906* modem,
- *lwIP*.

Configuration & building
------------------------

When you want to use *32F746GDISCOVERY* board:

    $ mkdir output
    $ cd output
    $ cmake -C../configurations/ST_32F746GDISCOVERY/distortosConfiguration.cmake .. -GNinja
    $ ninja

When you want to use *NUCLEO-F767ZI* board:

    $ mkdir output
    $ cd output
    $ cmake -C../configurations/ST_NUCLEO-F767ZI/distortosConfiguration.cmake .. -GNinja
    $ ninja

For more in-depth instructions see `distortos/README.md`.

MQTT
----

Once the modem connects to the network, it tries to connect to MQTT broker at `broker.hivemq.com`. When you subscribe to
the topic `distortos/#`, you should see the messages with device's online status (`1` is sent when the device connects,
`0` as a MQTT client's will once the connection is lost) and changes of button state.

With *32F746GDISCOVERY* board:

```
$ mosquitto_sub -h broker.hivemq.com -t "distortos/#" -v
distortos/0.7.0/ST,32F746GDISCOVERY/online 1
distortos/0.7.0/ST,32F746GDISCOVERY/buttons/0/state 0
distortos/0.7.0/ST,32F746GDISCOVERY/buttons/0/state 1
distortos/0.7.0/ST,32F746GDISCOVERY/buttons/0/state 0
```

With *NUCLEO-F767ZI* board:

```
$ mosquitto_sub -h broker.hivemq.com -t "distortos/#" -v
distortos/0.7.0/ST,NUCLEO-F767ZI/online 1
distortos/0.7.0/ST,NUCLEO-F767ZI/buttons/0/state 0
distortos/0.7.0/ST,NUCLEO-F767ZI/buttons/0/state 1
distortos/0.7.0/ST,NUCLEO-F767ZI/buttons/0/state 0
```

The application also allows you to control on-board LEDs via MQTT.

*32F746GDISCOVERY* board has just one LED available:

```
# set the LED on
$ mosquitto_pub -h broker.hivemq.com -t "distortos/0.7.0/ST,32F746GDISCOVERY/leds/0/state" -m "1"
# set the LED off
$ mosquitto_pub -h broker.hivemq.com -t "distortos/0.7.0/ST,32F746GDISCOVERY/leds/0/state" -m "0"
```

On *NUCLEO-F767ZI* board there are 3 LEDs:

```
# set green LED on
$ mosquitto_pub -h broker.hivemq.com -t "distortos/0.7.0/ST,NUCLEO-F767ZI/leds/0/state" -m "1"
# set blue LED on
$ mosquitto_pub -h broker.hivemq.com -t "distortos/0.7.0/ST,NUCLEO-F767ZI/leds/1/state" -m "1"
# set red LED on
$ mosquitto_pub -h broker.hivemq.com -t "distortos/0.7.0/ST,NUCLEO-F767ZI/leds/2/state" -m "1"
# set green LED off
$ mosquitto_pub -h broker.hivemq.com -t "distortos/0.7.0/ST,NUCLEO-F767ZI/leds/0/state" -m "0"
# set blue LED off
$ mosquitto_pub -h broker.hivemq.com -t "distortos/0.7.0/ST,NUCLEO-F767ZI/leds/1/state" -m "0"
# set red LED off
$ mosquitto_pub -h broker.hivemq.com -t "distortos/0.7.0/ST,NUCLEO-F767ZI/leds/2/state" -m "0"
```

Debug output
------------

ST-Link V2-1 has a virtual COM port which is used for debug output from the application. The stream uses typical
parameters: 115200 bps, 8N1. Below you will find the example of the output from *32F746GDISCOVERY* board.

```
[2020-04-18 23:37:26] Started ST,32F746GDISCOVERY board
[2020-04-18 23:37:29] dns_init: initializing
[2020-04-18 23:37:29] ppp phase changed[0]: phase=0
[2020-04-18 23:37:29] lwip_getaddrinfo() failed, ret = 202
[2020-04-18 23:37:29] PCUI port, read failed, errno = 128
[2020-04-18 23:37:29] GPS port, read failed, errno = 128
[2020-04-18 23:37:29] PpposManager::executeCommand: write failed, errno = 128
[2020-04-18 23:37:30] dns_tmr: dns_check_entries
...
[2020-04-18 23:37:33] dns_tmr: dns_check_entries
[2020-04-18 23:37:34] lwip_getaddrinfo() failed, ret = 202
[2020-04-18 23:37:34] dns_tmr: dns_check_entries
[2020-04-18 23:37:34] PCUI port, read failed, errno = 128
[2020-04-18 23:37:34] GPS port, read failed, errno = 128
[2020-04-18 23:37:34] PpposManager::executeCommand: write failed, errno = 128
[2020-04-18 23:37:35] dns_tmr: dns_check_entries
[2020-04-18 23:37:36] dns_tmr: dns_check_entries
[2020-04-18 23:37:37] dns_tmr: dns_check_entries
[2020-04-18 23:37:38] usbHostEventCallback: HOST_USER_CONNECTION
[2020-04-18 23:37:38] USB Device Attached
[2020-04-18 23:37:38] PID: 1573h
[2020-04-18 23:37:38] VID: 12d1h
[2020-04-18 23:37:38] Address (#1) assigned.
[2020-04-18 23:37:38] Manufacturer : HUAWEI Technology
[2020-04-18 23:37:38] Product : HUAWEI Mobile
[2020-04-18 23:37:38] Serial Number : ...
[2020-04-18 23:37:38] Enumeration done.
[2020-04-18 23:37:38] usbHostEventCallback: HOST_USER_SELECT_CONFIGURATION
[2020-04-18 23:37:38] dns_tmr: dns_check_entries
[2020-04-18 23:37:38] Default configuration set.
[2020-04-18 23:37:38] Device remote wakeup enabled
[2020-04-18 23:37:38] HuaweiMe906::SerialPort(0x2004fb20)::interfaceInitialize: interface 0, read
                      endpoint address 0x81, read endpoint size 64, read pipe 2, write endpoint address
                      0x1, write endpoint size 64, write pipe 3
[2020-04-18 23:37:38] HuaweiMe906::SerialPort(0x2004fb7c)::interfaceInitialize: interface 2, read
                      endpoint address 0x83, read endpoint size 64, read pipe 4, write endpoint address
                      0x3, write endpoint size 64, write pipe 5
[2020-04-18 23:37:38] HuaweiMe906::SerialPort(0x2004fbd8)::interfaceInitialize: interface 5, read
                      endpoint address 0x89, read endpoint size 64, read pipe 6, write endpoint address
                      0x6, write endpoint size 64, write pipe 7
[2020-04-18 23:37:38] Huawei ME906 class started.
[2020-04-18 23:37:38] usbHostEventCallback: HOST_USER_CLASS_SELECTED
[2020-04-18 23:37:38] usbHostEventCallback: HOST_USER_CLASS_ACTIVE
[2020-04-18 23:37:38] HuaweiMe906::SerialPort(0x2004fb20)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:38] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:38] HuaweiMe906::SerialPort(0x2004fbd8)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 29 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb20)::backgroundProcess: received 29 bytes
[2020-04-18 23:37:39] dns_tmr: dns_check_entries
[2020-04-18 23:37:39] lwip_getaddrinfo() failed, ret = 202
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb20)::backgroundProcess: asynchronous reader
                      consumed 29 bytes, 0 bytes left
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb20)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:39] PCUI port, read 25 bytes: "^HCSQ: "LTE",57,56,113,24"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 5 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 5 bytes
[2020-04-18 23:37:39] PpposManager::executeCommand: wrote 5 bytes: "ATI"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 29 bytes, 0 bytes left
[2020-04-18 23:37:39] PpposManager::executeCommand: read 27 bytes: "^HCSQ: "LTE",57,56,113,24"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 64 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 64 bytes, 0 bytes left
[2020-04-18 23:37:39] PpposManager::executeCommand: read 45 bytes:
                      "Manufacturer: Huawei Technologies Co., Ltd."
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:39] PpposManager::executeCommand: read 15 bytes: "Model: ME906E"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 64 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 64 bytes, 0 bytes left
[2020-04-18 23:37:39] PpposManager::executeCommand: read 27 bytes: "Revision: 12.430.19.08.00"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:39] PpposManager::executeCommand: read 23 bytes: "IMEI: ..."
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 4 bytes
[2020-04-18 23:37:39] PpposManager::executeCommand: read 14 bytes: "+GCAP: +CGSM"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 4 bytes, 0 bytes left
[2020-04-18 23:37:39] PpposManager::executeCommand: read 4 bytes: "OK"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 54 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 54 bytes
[2020-04-18 23:37:39] PpposManager::executeCommand: wrote 54 bytes:
                      "AT^SYSCFGEX="030201",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF,,"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 6 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 6 bytes, 0 bytes left
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:39] PpposManager::executeCommand: read 4 bytes: "OK"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 11 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 11 bytes
[2020-04-18 23:37:39] PpposManager::executeCommand: wrote 11 bytes: "AT+COPS=0"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 6 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 6 bytes, 0 bytes left
[2020-04-18 23:37:39] PpposManager::executeCommand: read 4 bytes: "OK"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 44 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 44 bytes
[2020-04-18 23:37:39] PpposManager::executeCommand: wrote 44 bytes:
                      "AT+CGDCONT=1,"IP","internet","0.0.0.0",0,0"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 6 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 6 bytes, 0 bytes left
[2020-04-18 23:37:39] PpposManager::executeCommand: read 4 bytes: "OK"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 37 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 37 bytes
[2020-04-18 23:37:39] PpposManager::executeCommand: wrote 37 bytes:
                      "AT+CGDCONT=16,"IP","","0.0.0.0",0,0"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 6 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 6 bytes, 0 bytes left
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:39] PpposManager::executeCommand: read 4 bytes: "OK"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 12 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 12 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 6 bytes
[2020-04-18 23:37:39] PpposManager::executeCommand: wrote 12 bytes: "AT+CGATT=1"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 6 bytes, 0 bytes left
[2020-04-18 23:37:39] PpposManager::executeCommand: read 4 bytes: "OK"
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 9 bytes
[2020-04-18 23:37:39] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 9 bytes
[2020-04-18 23:37:39] PpposManager::executeCommand: wrote 9 bytes: "ATD*99#"
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 21 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 21 bytes, 0 bytes left
[2020-04-18 23:37:40] PpposManager::executeCommand: read 19 bytes: "CONNECT 100000000"
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:40] ppp_connect[0]: holdoff=0
[2020-04-18 23:37:40] ppp phase changed[0]: phase=3
[2020-04-18 23:37:40] pppos_connect: unit 0: connecting
[2020-04-18 23:37:40] ppp_start[0]
[2020-04-18 23:37:40] ppp phase changed[0]: phase=6
[2020-04-18 23:37:40] pppos_send_config[0]: out_accm=FF FF FF FF
[2020-04-18 23:37:40] ppp_send_config[0]
[2020-04-18 23:37:40] pppos_recv_config[0]: in_accm=FF FF FF FF
[2020-04-18 23:37:40] ppp_recv_config[0]
[2020-04-18 23:37:40] sent [LCP ConfReq id=0x1 <asyncmap 0x0> <magic 0x58522087> <pcomp> <accomp>]
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 45 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 45 bytes
[2020-04-18 23:37:40] pppos_write[0]: len=24
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 55 bytes
[2020-04-18 23:37:40] ppp_start[0]: finished
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 55 bytes, 0 bytes left
[2020-04-18 23:37:40] pppos_input[0]: got 55 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:40] rcvd [LCP ConfReq id=0x0 <asyncmap 0x0> <auth 0xc223 05> <magic 0x3fe01224>
                      <pcomp> <accomp>]
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 46 bytes
[2020-04-18 23:37:40] No auth is possible
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 46 bytes, 0 bytes left
[2020-04-18 23:37:40] sent [LCP ConfRej id=0x0 <auth 0xc223 05>]
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 24 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 24 bytes
[2020-04-18 23:37:40] pppos_write[0]: len=13
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 46 bytes
[2020-04-18 23:37:40] pppos_input[0]: got 46 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 46 bytes, 0 bytes left
[2020-04-18 23:37:40] rcvd [LCP ConfAck id=0x1 <asyncmap 0x0> <magic 0x58522087> <pcomp> <accomp>]
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:40] pppos_input[0]: got 46 bytes
[2020-04-18 23:37:40] rcvd [LCP ConfReq id=0x1 <asyncmap 0x0> <magic 0x3fe01224> <pcomp> <accomp>]
[2020-04-18 23:37:40] sent [LCP ConfAck id=0x1 <asyncmap 0x0> <magic 0x3fe01224> <pcomp> <accomp>]
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 47 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 47 bytes
[2020-04-18 23:37:40] pppos_write[0]: len=24
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 22 bytes
[2020-04-18 23:37:40] netif_set_mtu[0]: mtu=1500
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 22 bytes, 0 bytes left
[2020-04-18 23:37:40] pppos_send_config[0]: out_accm=0 0 0 0
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:40] ppp_send_config[0]
[2020-04-18 23:37:40] pppos_recv_config[0]: in_accm=0 0 0 0
[2020-04-18 23:37:40] ppp_recv_config[0]
[2020-04-18 23:37:40] ppp phase changed[0]: phase=9
[2020-04-18 23:37:40] sent [IPCP ConfReq id=0x1 <addr 0.0.0.0> <ms-dns1 0.0.0.0> <ms-dns2 0.0.0.0>]
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 29 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 29 bytes
[2020-04-18 23:37:40] pppos_write[0]: len=26
[2020-04-18 23:37:40] pppos_input[0]: got 22 bytes
[2020-04-18 23:37:40] rcvd [LCP DiscReq id=0x2 magic=0x3fe01224]
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 10 bytes
[2020-04-18 23:37:40] pppos_input[0]: got 10 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 10 bytes, 0 bytes left
[2020-04-18 23:37:40] rcvd [IPCP ConfReq id=0x0]
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:40] sent [IPCP ConfNak id=0x0 <addr 0.0.0.0>]
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 28 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 28 bytes, 0 bytes left
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 17 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 17 bytes
[2020-04-18 23:37:40] pppos_write[0]: len=14
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 10 bytes
[2020-04-18 23:37:40] pppos_input[0]: got 28 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 10 bytes, 0 bytes left
[2020-04-18 23:37:40] rcvd [IPCP ConfNak id=0x1 <addr xxx.xxx.xxx.xxx> <ms-dns1 xxx.xxx.xxx.xxx>
                      <ms-dns2 xxx.xxx.xxx.xxx>]
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:40] sent [IPCP ConfReq id=0x2 <addr xxx.xxx.xxx.xxx> <ms-dns1 xxx.xxx.xxx.xxx>
                      <ms-dns2 xxx.xxx.xxx.xxx>]
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 29 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 29 bytes
[2020-04-18 23:37:40] pppos_write[0]: len=26
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 28 bytes
[2020-04-18 23:37:40] pppos_input[0]: got 10 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 28 bytes, 0 bytes left
[2020-04-18 23:37:40] rcvd [IPCP ConfReq id=0x1]
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:40] sent [IPCP ConfAck id=0x1]
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 11 bytes
[2020-04-18 23:37:40] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 11 bytes
[2020-04-18 23:37:40] pppos_write[0]: len=8
[2020-04-18 23:37:40] pppos_input[0]: got 28 bytes
[2020-04-18 23:37:40] rcvd [IPCP ConfAck id=0x2 <addr xxx.xxx.xxx.xxx> <ms-dns1 xxx.xxx.xxx.xxx>
                      <ms-dns2 xxx.xxx.xxx.xxx>]
[2020-04-18 23:37:40] Could not determine remote IP address: defaulting to 10.64.64.64
[2020-04-18 23:37:40] sifup[0]: err_code=0
[2020-04-18 23:37:40] PpposManager::pppLinkStatus: PPPERR_NONE
[2020-04-18 23:37:40]   ip4 = xxx.xxx.xxx.xxx
[2020-04-18 23:37:40]   gateway = 10.64.64.64
[2020-04-18 23:37:40]   netmask = 255.255.255.255
[2020-04-18 23:37:40]   dns1 = xxx.xxx.xxx.xxx
[2020-04-18 23:37:40]   dns2 = xxx.xxx.xxx.xxx
[2020-04-18 23:37:40] local  IP address xxx.xxx.xxx.xxx
[2020-04-18 23:37:40] remote IP address 10.64.64.64
[2020-04-18 23:37:40] primary   DNS address xxx.xxx.xxx.xxx
[2020-04-18 23:37:40] secondary DNS address xxx.xxx.xxx.xxx
[2020-04-18 23:37:40] ppp phase changed[0]: phase=10
[2020-04-18 23:37:40] dns_tmr: dns_check_entries
...
[2020-04-18 23:37:44] dns_tmr: dns_check_entries
[2020-04-18 23:37:44] dns_enqueue: "broker.hivemq.com": use DNS entry 0
[2020-04-18 23:37:44] dns_enqueue: "broker.hivemq.com": use DNS pcb 0
[2020-04-18 23:37:44] dns_send: dns_servers[0] "broker.hivemq.com": request
[2020-04-18 23:37:44] sending DNS request ID 12585 for name "broker.hivemq.com" to server 0
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 64 bytes
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 64 bytes
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 5 bytes
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 5 bytes
[2020-04-18 23:37:44] pppos_netif_output[0]: proto=0x21, len = 63
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 64 bytes
[2020-04-18 23:37:44] pppos_input[0]: got 64 bytes
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 64 bytes, 0 bytes left
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 36 bytes
[2020-04-18 23:37:44] pppos_input[0]: got 36 bytes
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 36 bytes, 0 bytes left
[2020-04-18 23:37:44] ppp_input[0]: ip in pbuf len=95
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:44] dns_recv: "broker.hivemq.com": response = 3.123.170.233
[2020-04-18 23:37:44] broker.hivemq.com is 3.123.170.233
[2020-04-18 23:37:44] MQTT client ID is "ST,32F746GDISCOVERY-004400203035510239363937"
[2020-04-18 23:37:44] mqtt_client_connect: Connecting to host: 3.123.170.233 at port:1883
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 49 bytes
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 49 bytes
[2020-04-18 23:37:44] pppos_netif_output[0]: proto=0x21, len = 44
[2020-04-18 23:37:44] Connecting to MQTT broker...
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 49 bytes
[2020-04-18 23:37:44] pppos_input[0]: got 49 bytes
[2020-04-18 23:37:44] ppp_input[0]: ip in pbuf len=44
[2020-04-18 23:37:44] mqtt_tcp_connect_cb: TCP connection established to server
[2020-04-18 23:37:44] mqtt_output_send: tcp_sndbuf: 1072 bytes, ringbuf_linear_available: 105, get 0,
                      put 105
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 49 bytes, 0 bytes left
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 64 bytes
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 64 bytes
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 64 bytes
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 64 bytes
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 21 bytes
[2020-04-18 23:37:44] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 21 bytes
[2020-04-18 23:37:45] pppos_netif_output[0]: proto=0x21, len = 145
[2020-04-18 23:37:45] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 45 bytes
[2020-04-18 23:37:45] pppos_input[0]: got 45 bytes
[2020-04-18 23:37:45] ppp_input[0]: ip in pbuf len=40
[2020-04-18 23:37:45] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 45 bytes, 0 bytes left
[2020-04-18 23:37:45] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:45] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 49 bytes
[2020-04-18 23:37:45] pppos_input[0]: got 49 bytes
[2020-04-18 23:37:45] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 49 bytes, 0 bytes left
[2020-04-18 23:37:45] ppp_input[0]: ip in pbuf len=44
[2020-04-18 23:37:45] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:45] mqtt_parse_incoming: Remaining length after fixed header: 2
[2020-04-18 23:37:45] mqtt_parse_incoming: msg_idx: 4, cpy_len: 2, remaining 0
[2020-04-18 23:37:45] mqtt_message_received: Connect response code 0
[2020-04-18 23:37:45] mqttClientConnectionCallback: status = 0
[2020-04-18 23:37:45] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 45 bytes
[2020-04-18 23:37:45] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 45 bytes
[2020-04-18 23:37:45] pppos_netif_output[0]: proto=0x21, len = 40
[2020-04-18 23:37:45] dns_tmr: dns_check_entries
...
[2020-04-18 23:37:49] dns_tmr: dns_check_entries
[2020-04-18 23:37:49] mqtt_publish: Publish with payload length 1 to topic
                      "distortos/0.7.0/ST,32F746GDISCOVERY/online"
[2020-04-18 23:37:49] mqtt_output_send: tcp_sndbuf: 1072 bytes, ringbuf_linear_available: 47, get 105,
                      put 152
[2020-04-18 23:37:49] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 64 bytes
[2020-04-18 23:37:49] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 64 bytes
[2020-04-18 23:37:49] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 28 bytes
[2020-04-18 23:37:49] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 28 bytes
[2020-04-18 23:37:49] pppos_netif_output[0]: proto=0x21, len = 87
[2020-04-18 23:37:49] mqtt_sub_unsub: Client (un)subscribe to topic
                      "distortos/0.7.0/ST,32F746GDISCOVERY/leds/+/state", id: 1
[2020-04-18 23:37:49] mqtt_output_send: tcp_sndbuf: 1025 bytes, ringbuf_linear_available: 55, get 152,
                      put 207
[2020-04-18 23:37:49] mqtt_publish: Publish with payload length 1 to topic
                      "distortos/0.7.0/ST,32F746GDISCOVERY/buttons/0/state"
[2020-04-18 23:37:49] mqtt_output_send: tcp_sndbuf: 970 bytes, ringbuf_linear_available: 49, get 207,
                      put 7
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 46 bytes
[2020-04-18 23:37:50] pppos_input[0]: got 46 bytes
[2020-04-18 23:37:50] ppp_input[0]: ip in pbuf len=40
[2020-04-18 23:37:50] mqtt_tcp_sent_cb: Calling QoS 0 publish complete callback
[2020-04-18 23:37:50] mqttRequestCallback: error = 0
[2020-04-18 23:37:50] mqtt_tcp_sent_cb: Calling QoS 0 publish complete callback
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 46 bytes, 0 bytes left
[2020-04-18 23:37:50] mqttRequestCallback: error = 0
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 64 bytes
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 64 bytes
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 64 bytes
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 64 bytes
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 28 bytes
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 28 bytes
[2020-04-18 23:37:50] pppos_netif_output[0]: proto=0x21, len = 151
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 64 bytes
[2020-04-18 23:37:50] pppos_input[0]: got 64 bytes
[2020-04-18 23:37:50] ppp_input[0]: ip in pbuf len=40
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 64 bytes, 0 bytes left
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 31 bytes
[2020-04-18 23:37:50] pppos_input[0]: got 31 bytes
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 31 bytes, 0 bytes left
[2020-04-18 23:37:50] ppp_input[0]: ip in pbuf len=45
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:50] mqtt_parse_incoming: Remaining length after fixed header: 3
[2020-04-18 23:37:50] mqtt_parse_incoming: msg_idx: 5, cpy_len: 3, remaining 0
[2020-04-18 23:37:50] mqtt_message_received: SUBACK response with id 1
[2020-04-18 23:37:50] mqttRequestCallback: error = 0
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 45 bytes
[2020-04-18 23:37:50] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 45 bytes
[2020-04-18 23:37:50] pppos_netif_output[0]: proto=0x21, len = 40
[2020-04-18 23:37:50] dns_tmr: dns_check_entries
[2020-04-18 23:37:51] dns_tmr: dns_check_entries
[2020-04-18 23:37:52] mqtt_publish: Publish with payload length 1 to topic
                      "distortos/0.7.0/ST,32F746GDISCOVERY/buttons/0/state"
[2020-04-18 23:37:52] mqtt_output_send: tcp_sndbuf: 1072 bytes, ringbuf_linear_available: 56, get 7,
                      put 63
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 64 bytes
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 64 bytes
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 37 bytes
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 37 bytes
[2020-04-18 23:37:52] pppos_netif_output[0]: proto=0x21, len = 96
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 45 bytes
[2020-04-18 23:37:52] pppos_input[0]: got 45 bytes
[2020-04-18 23:37:52] ppp_input[0]: ip in pbuf len=40
[2020-04-18 23:37:52] mqtt_tcp_sent_cb: Calling QoS 0 publish complete callback
[2020-04-18 23:37:52] mqttRequestCallback: error = 0
[2020-04-18 23:37:52] dns_tmr: dns_check_entries
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 45 bytes, 0 bytes left
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:52] mqtt_publish: Publish with payload length 1 to topic
                      "distortos/0.7.0/ST,32F746GDISCOVERY/buttons/0/state"
[2020-04-18 23:37:52] mqtt_output_send: tcp_sndbuf: 1072 bytes, ringbuf_linear_available: 56, get 63,
                      put 119
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 64 bytes
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 64 bytes
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 37 bytes
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 37 bytes
[2020-04-18 23:37:52] pppos_netif_output[0]: proto=0x21, len = 96
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 45 bytes
[2020-04-18 23:37:52] pppos_input[0]: got 45 bytes
[2020-04-18 23:37:52] ppp_input[0]: ip in pbuf len=40
[2020-04-18 23:37:52] mqtt_tcp_sent_cb: Calling QoS 0 publish complete callback
[2020-04-18 23:37:52] mqttRequestCallback: error = 0
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 45 bytes, 0 bytes left
[2020-04-18 23:37:52] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:53] dns_tmr: dns_check_entries
...
[2020-04-18 23:37:56] dns_tmr: dns_check_entries
[2020-04-18 23:37:56] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 64 bytes
[2020-04-18 23:37:56] pppos_input[0]: got 64 bytes
[2020-04-18 23:37:56] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 64 bytes, 0 bytes left
[2020-04-18 23:37:56] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:56] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 34 bytes
[2020-04-18 23:37:56] pppos_input[0]: got 34 bytes
[2020-04-18 23:37:56] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 34 bytes, 0 bytes left
[2020-04-18 23:37:56] ppp_input[0]: ip in pbuf len=93
[2020-04-18 23:37:56] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:56] mqtt_parse_incoming: Remaining length after fixed header: 51
[2020-04-18 23:37:56] mqtt_parse_incoming: msg_idx: 53, cpy_len: 51, remaining 0
[2020-04-18 23:37:56] mqtt_incomming_publish: Received message with QoS 0 at topic:
                      distortos/0.7.0/ST,32F746GDISCOVERY/leds/0/state, payload length 1
[2020-04-18 23:37:56] mqttIncomingPublishCallback: topic =
                      "distortos/0.7.0/ST,32F746GDISCOVERY/leds/0/state", total length = 1
[2020-04-18 23:37:56] mqttIncomingDataCallback: length = 1, flags = 1
[2020-04-18 23:37:56] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 45 bytes
[2020-04-18 23:37:56] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 45 bytes
[2020-04-18 23:37:56] pppos_netif_output[0]: proto=0x21, len = 40
[2020-04-18 23:37:57] dns_tmr: dns_check_entries
[2020-04-18 23:37:58] dns_tmr: dns_check_entries
[2020-04-18 23:37:59] dns_tmr: dns_check_entries
[2020-04-18 23:37:59] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 64 bytes
[2020-04-18 23:37:59] pppos_input[0]: got 64 bytes
[2020-04-18 23:37:59] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 64 bytes, 0 bytes left
[2020-04-18 23:37:59] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:59] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: received 34 bytes
[2020-04-18 23:37:59] pppos_input[0]: got 34 bytes
[2020-04-18 23:37:59] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: asynchronous reader
                      consumed 34 bytes, 0 bytes left
[2020-04-18 23:37:59] ppp_input[0]: ip in pbuf len=93
[2020-04-18 23:37:59] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: requesting 64 bytes
[2020-04-18 23:37:59] mqtt_parse_incoming: Remaining length after fixed header: 51
[2020-04-18 23:37:59] mqtt_parse_incoming: msg_idx: 53, cpy_len: 51, remaining 0
[2020-04-18 23:37:59] mqtt_incomming_publish: Received message with QoS 0 at topic:
                      distortos/0.7.0/ST,32F746GDISCOVERY/leds/0/state, payload length 1
[2020-04-18 23:37:59] mqttIncomingPublishCallback: topic =
                      "distortos/0.7.0/ST,32F746GDISCOVERY/leds/0/state", total length = 1
[2020-04-18 23:37:59] mqttIncomingDataCallback: length = 1, flags = 1
[2020-04-18 23:37:59] HuaweiMe906::SerialPort(0x2004fb7c)::requestWrite: requesting 45 bytes
[2020-04-18 23:37:59] HuaweiMe906::SerialPort(0x2004fb7c)::backgroundProcess: transmitted 45 bytes
[2020-04-18 23:37:59] pppos_netif_output[0]: proto=0x21, len = 40
[2020-04-18 23:38:00] dns_tmr: dns_check_entries
...
```
