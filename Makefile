all: build test

build: configure
	cd click; make -j2

configure:
	cd click; ./configure --disable-linuxmodule --enable-local --enable-etherswitch

test:
	cd click; click conf/test.click
