#!/bin/sh

sudo sysctl -w net.inet.tcp.always_keepalive=1
sudo sysctl -w net.inet.tcp.keepinit=3000
sudo sysctl -w net.inet.tcp.keepintvl=3000
sudo sysctl -w net.inet.tcp.keepidle=3000
