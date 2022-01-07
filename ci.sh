#!/bin/bash

set -e

export DEBIAN_FRONTEND="noninteractive"

apt update
apt upgrade -y
apt install -y git make fakeroot dpkg-dev build-essential libgtk-3-dev libjson-glib-dev valac
apt --fix-broken install

make
make pack
make clean
