#!/bin/bash

set -e

export DEBIAN_FRONTEND="noninteractive"

echo "deb [trusted=yes] https://deb.debian.org/debian buster-backports main contrib non-free" >> /etc/apt/sources.list

apt update
apt upgrade -y
apt install -y git make fakeroot dpkg-dev build-essential libgtk-3-dev libjson-glib-dev patchelf
apt install -y -t buster-backports valac
apt --fix-broken install

make

# Oh my- (Debian Buster workaround)
patchelf \
	--remove-needed libgmodule-2.0.so.0 \
	--remove-needed libcairo.so.2 \
	--remove-needed libgdk_pixbuf-2.0.so.0 \
	--remove-needed libpango-1.0.so.0 \
	--remove-needed libatk-1.0.so.0 \
	--remove-needed libX11.so.6 \
	--remove-needed libpangocairo-1.0.so.0 \
	--remove-needed libharfbuzz.so.0 \
	--remove-needed libcairo-gobject.so.2 \
	--remove-needed libpthread.so.0 \
	--remove-needed libharfbuzz-gobject.so \
	build/gmcpil

make pack
make clean
