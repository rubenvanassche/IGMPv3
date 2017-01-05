all: build test

build: copy
	cd click; make elemlist; make -j2

configure:
	cd click; ./configure --disable-linuxmodule --enable-local --enable-etherswitch

copy:
	cp -R elements/. click/elements/local/igmp

app:
	cd click/apps/ClickController; java ClickController localhost 1981

test:
	cd click; userlevel/click ../scripts/test/handler.click -p 1981
