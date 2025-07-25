#!/usr/bin/env bash

case "$BUILD_TARGET" in
"vita")
	docker exec vitasdk /bin/bash -c "git config --global --add safe.directory /build/git && mkdir build && cd build && cmake -DAV1_VIDEO_SUPPORT=ON -DCMAKE_BUILD_TYPE=Release -DTARGET_PLATFORM=vita .."
	;;
"switch")
	docker exec switchdev /bin/bash -c "git config --global --add safe.directory /build/git && /opt/devkitpro/portlibs/switch/bin/aarch64-none-elf-cmake -DAV1_VIDEO_SUPPORT=ON -DCMAKE_BUILD_TYPE=Release -DTARGET_PLATFORM=switch -B build -S ."
	;;
"mac")
	mkdir build && cd build && cmake -DAV1_VIDEO_SUPPORT=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" ..
	;;
"ios")
	mkdir build && cd build && cmake -DAV1_VIDEO_SUPPORT=ON -DTARGET_PLATFORM=ios -G Xcode ..
	;;
"flatpak")
	;;
"appimage")
	mkdir build && cd build && cmake -DAV1_VIDEO_SUPPORT=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
	;;
"linux")
	mkdir build && cd build && cmake -DAV1_VIDEO_SUPPORT=ON -DCMAKE_BUILD_TYPE=Release ..
	;;
"android")
	mkdir build
	;;
"emscripten")
	export EMSDK=${PWD}/emsdk
	mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DTARGET_PLATFORM=emscripten ..
	;;
*)
	mkdir build && cd build && cmake -DAV1_VIDEO_SUPPORT=ON ..
	;;
esac
