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
ifeq ($(origin CC),default)
CC:=clang
endif
STRIP?=llvm-strip
LDFLAGS:=-fuse-ld=lld
ARCH:=$(shell $(CC) -dumpmachine | grep -Eo "arm|aarch|86|x86_64")
else
ifeq ($(origin CC),default)
CC:=gcc
endif
STRIP?=strip
LDFLAGS:=
ARCH:=$(shell $(CC) -print-multiarch | grep -Eo "arm|aarch|86|x86_64")
endif

VERSION:=0.12.0

OBJS:=$(patsubst %,build/%.o,mcpil config features play servers)

CFLAGS+=-DGMCPIL_VERSION=\"v$(VERSION)\" -I./src/include -Wall -Wno-address-of-packed-member -Wno-pointer-to-int-cast -Wno-unused-result
CFLAGS+=$(shell pkg-config --cflags gtk+-3.0 json-glib-1.0)
LDFLAGS+=-Wl,--no-undefined $(shell pkg-config --libs gtk+-3.0 json-glib-1.0)

ifdef DEBUG
CFLAGS+=-g -Wextra -Werror
else
CFLAGS+=-O3
endif

# Some GTK+/GDK-Pixbuf combinations may generate this warning
# for internal functions, so let's "ignore" it.
CFLAGS+=-Wno-error=deprecated-declarations

.PHONY: ./build/gmcpil

./build/gmcpil: mkdir $(OBJS)
	$(CC) -fPIC -fpie $(OBJS) -o $@ $(CFLAGS) $(LDFLAGS)
ifndef DEBUG
	$(STRIP) ./build/gmcpil
endif

./build/%.o: ./src/%.c ./src/include/*.h
	$(CC) -fPIC -fpie -c $< -o $@ $(CFLAGS)

mkdir:
	mkdir -p ./build/

pack: ./build/gmcpil
	mkdir -p ./deb/DEBIAN/
	mkdir -p ./deb/usr/bin/
	mkdir -p ./deb/usr/share/
	cp ./build/gmcpil ./deb/usr/bin/
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
	@echo "Depends: libc6 (>= 2.28), minecraft-pi-reborn-client (>= 2.1.0), libgtk-3-0, libjson-glib-1.0-0" >> ./deb/DEBIAN/control
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
	rm -f ./gmcpil_*.deb
