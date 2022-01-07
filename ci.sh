#!/bin/bash

set -e

export DEBIAN_FRONTEND="noninteractive"

apt-get update
apt-get upgrade -y --with-new-pkgs
apt-get install -y git make fakeroot dpkg-dev build-essential libgtk-3-dev libjson-glib-dev valac
apt-get --fix-broken install

make
make pack
make clean
