all: configure

configure:
	cd click; ./configure --disable-linuxmodule --enable-local --enable-etherswitch
	
