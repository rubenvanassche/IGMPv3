all: build test

build: copy
	cd click; make elemlist; make -j2

configure:
	cd click; ./configure --disable-linuxmodule --enable-local --enable-etherswitch

copy:
	cp -R elements/. click/elements/local/igmp

test:
	cd click; userlevel/click conf/test.click
	cd click; userlevel/click ../scripts/test/setup.click
