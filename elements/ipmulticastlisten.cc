#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "ipmulticastlisten.hh"

CLICK_DECLS
IPMulticastListen::IPMulticastListen()
{}

IPMulticastListen::~ IPMulticastListen()
{}

int IPMulticastListen::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (cp_va_kparse(conf, this, errh, "MAXPACKETSIZE", cpkM, cpInteger, &maxSize, cpEnd) < 0) return -1;
	if (maxSize <= 0) return errh->error("maxsize should be larger than 0");
	return 0;
}

void IPMulticastListen::push(int, Packet *p){
	click_chatter("Got a packet of size %d",p->length());
	if (p->length() > maxSize)  p->kill();
	else output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IPMulticastListen)
