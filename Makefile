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

ifdef USE_CLANG
CC:=clang
ARM_CC:=clang -target arm-linux-gnueabihf
LDFLAGS:=-fuse-ld=lld
STRIP:=llvm-strip
ARCH:=$(shell $(CC) -dumpmachine | grep -Eo "arm|aarch|86|x86_64")
else
CC:=gcc
ARM_CC:=arm-linux-gnueabihf-gcc
LDFLAGS:=
STRIP:=strip
ARCH:=$(shell $(CC) -print-multiarch | grep -Eo "arm|aarch|86|x86_64")
endif

OBJS:=$(patsubst %,build/%.o,mcpil config helpers callbacks tabs)
MODS:=$(patsubst %,build/lib%.so,multiplayer)

LDFLAGS+=-Wl,--no-undefined

CFLAGS:=-I./src/include -Wall
GTK_CFLAGS:=`pkg-config --cflags gtk+-3.0 json-glib-1.0`
GTK_LDFLAGS:=`pkg-config --libs gtk+-3.0 json-glib-1.0`
MOD_CONFIG:=--shared -ldl

ifdef DEBUG
CFLAGS+=-g -Wextra -Werror
else
CFLAGS+=-O3
endif

# Some GTK+/GDK-Pixbuf combinations may generate this warning
# for internal functions, so let's "ignore" it.
CFLAGS+=-Wno-error=deprecated-declarations

VERSION:=0.10.0-rc1

.PHONY: ./build/gmcpil

./build/gmcpil: mkdir $(MODS) $(OBJS)
	$(CC) -fPIC -fpie $(OBJS) -o $@ $(GTK_LDFLAGS) $(CFLAGS) $(LDFLAGS)
ifndef DEBUG
	$(STRIP) ./build/gmcpil
endif

./build/%.o: ./src/%.c ./src/include/*.h
	$(CC) -fPIC -c $< -o $@ $(GTK_CFLAGS) $(CFLAGS)

./build/lib%.so: ./src/mods/%.c
	$(ARM_CC) -fPIC $< -o $@ $(MOD_CONFIG) $(CFLAGS) $(LDFLAGS)
ifndef DEBUG
	$(STRIP) $@
endif

mkdir:
	mkdir -p ./build/

pack: ./build/gmcpil
	mkdir -p ./deb/DEBIAN/
	mkdir -p ./deb/usr/bin/
	mkdir -p ./deb/usr/share/
	mkdir -p ./deb/usr/lib/gmcpil/
	cp ./build/gmcpil ./deb/usr/bin/
	cp ./build/lib*.so ./deb/usr/lib/gmcpil/
	cp -r ./res/. ./deb/usr/share/
	chmod a+x ./deb/usr/bin/gmcpil
	@echo "Package: gmcpil" > ./deb/DEBIAN/control
	@echo "Version: $(VERSION)" >> ./deb/DEBIAN/control
	@echo "Priority: optional" >> ./deb/DEBIAN/control
ifeq ($(ARCH),86)
	@echo "Architecture: i386" >> ./deb/DEBIAN/control
else ifeq ($(ARCH),x86_64)
	@echo "Architecture: amd64" >> ./deb/DEBIAN/control
else ifeq ($(ARCH),aarch)
	@echo "Architecture: arm64" >> ./deb/DEBIAN/control
else
	@echo "Architecture: armhf" >> ./deb/DEBIAN/control
endif
	@echo "Section: contrib/misc" >> ./deb/DEBIAN/control
	@echo "Depends: libc6 (>= 2.28), minecraft-pi-reborn-client (>= 2.0), libgtk-3-0, libjson-glib-1.0-0" >> ./deb/DEBIAN/control
	@echo "Maintainer: Alvarito050506 <donfrutosgomez@gmail.com>" >> ./deb/DEBIAN/control
	@echo "Homepage: https://mcpirevival.tk" >> ./deb/DEBIAN/control
	@echo "Vcs-Browser: https://github.com/MCPI-Revival/gMCPIL" >> ./deb/DEBIAN/control
	@echo "Vcs-Git: https://github.com/MCPI-Revival/gMCPIL.git" >> ./deb/DEBIAN/control
	@echo "Description: Simple launcher for Minecraft: Pi Edition - GTK+ Edition." >> ./deb/DEBIAN/control
	@echo " Simple GUI launcher for Minecraft: Pi Edition and MCPI-Reborn," >> ./deb/DEBIAN/control
	@echo " rewritten in C using GTK+ 3." >> ./deb/DEBIAN/control
	fakeroot dpkg-deb -b ./deb/ ./gmcpil_$(VERSION).deb

clean:
	rm -rf ./deb/
	rm -rf ./build/
	rm -f ./gmcpil_*-*.deb
