#!/bin/bash

set -e

export DEBIAN_FRONTEND="noninteractive"

echo "deb [trusted=yes] https://deb.debian.org/debian buster-backports main contrib non-free" >> /etc/apt/sources.list

apt update
apt upgrade -y
apt install -y git make fakeroot dpkg-dev build-essential libgtk-3-dev libjson-glib-dev
apt install -y -t buster-backports valac
apt --fix-broken install

make pack
make clean
