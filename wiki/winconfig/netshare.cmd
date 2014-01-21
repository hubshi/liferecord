rem 共享上网命令,运行完命令后将网络共享给无线2就可以了。
Netsh wlan set hostednetwork mode=allow ssid=LETSGO-WiFi key=1a2b3c4d5e6f
Netsh wlan start hostednetwork