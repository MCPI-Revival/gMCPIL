#
#  Makefile
#  
#  Copyright 2021 Alvarito050506 <donfrutosgomez@gmail.com>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  

CC:=gcc
ARCH:=$(shell uname -m | grep -Eo "arm|aarch|86")

OBJS:=$(patsubst %,build/%.o,mcpil helpers callbacks tabs)
MODS:=$(patsubst %,build/lib%.so,multiplayer)

CFLAGS:=-I./src/include
GTK_CONFIG:=`pkg-config --libs --cflags gtk+-3.0`
MOD_CONFIG:=--shared -ldl -I./lib/libreborn

VERSION:=0.9.0-rc4

.PHONY: mkdir

mcpil: mkdir $(MODS) $(OBJS)
	$(CC) -fPIC -fpie $(OBJS) -o ./build/mcpil $(GTK_CONFIG) $(CFLAGS)

./build/%.o: ./src/%.c ./src/include/*.h
	$(CC) -c $< -o $@ $(GTK_CONFIG) $(CFLAGS)

./build/lib%.so: ./src/mods/%.c
	$(CC) $< -o $@ $(MOD_CONFIG) $(CFLAGS)

mkdir:
	mkdir -p ./build/

pack: mcpil
	mkdir -p ./deb/DEBIAN/
	mkdir -p ./deb/usr/bin/
	mkdir -p ./deb/usr/share/mcpil/
	mkdir -p ./deb/usr/share/doc/mcpil/
	mkdir -p ./deb/usr/lib/mcpil/
	cp ./build/mcpil ./deb/usr/bin/
	cp ./build/libmultiplayer.so ./deb/usr/lib/mcpil/
	cp ./CHANGELOG.txt ./deb/usr/share/doc/mcpil/
	cp ./lib/Adwaita-dark.css ./deb/usr/share/mcpil/
	cp -r ./res/. ./deb/usr/share/
	sudo chmod a+x ./deb/usr/bin/mcpil
	@echo "Package: gmcpil" > ./deb/DEBIAN/control
	@echo "Version: $(VERSION)" >> ./deb/DEBIAN/control
	@echo "Priority: optional" >> ./deb/DEBIAN/control
ifeq ($(ARCH),86)
	@echo "Architecture: i386" >> ./deb/DEBIAN/control
else
	@echo "Architecture: armhf" >> ./deb/DEBIAN/control
endif
	@echo "Depends: minecraft-pi-reborn-native | minecraft-pi-reborn-virgl | mcpirdl, libgtk-3-0" >> ./deb/DEBIAN/control
	@echo "Maintainer: Alvarito050506 <donfrutosgomez@gmail.com>" >> ./deb/DEBIAN/control
	@echo "Homepage: https://mcpirevival.tk" >> ./deb/DEBIAN/control
	@echo "Vcs-Browser: https://github.com/MCPI-Revival/gMCPIL" >> ./deb/DEBIAN/control
	@echo "Vcs-Git: https://github.com/MCPI-Revival/gMCPIL.git" >> ./deb/DEBIAN/control
	@echo "Description: Simple launcher for Minecraft: Pi Edition - GTK+ Edition.\n" >> ./deb/DEBIAN/control
	dpkg-deb -b ./deb/ ./gmcpil_$(VERSION).deb

clean:
	rm -rf ./deb/
	rm -rf ./build/
	rm ./gmcpil_*-*.deb
