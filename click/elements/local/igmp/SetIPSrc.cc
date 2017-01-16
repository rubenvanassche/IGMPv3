#include <click/config.h>
#include <click/confparse.hh>
// #include <click/TODO.hh>
#include "SetIPSrc.hh"

CLICK_DECLS

SetIPSrc::SetIPSrc() { };
SetIPSrc::~SetIPSrc() { };

int SetIPSrc::configure(Vector<String> &conf, ErrorHandler *errh) {
    IPAddress* addr = new IPAddress("1.1.1.1");

    int res = cp_va_kparse(conf, this, errh,
        "ADDR", 0, cpIPAddress, addr,
        cpEnd);

    if(res < 0){
        return -1; // parsing failed
    }

    this->ipAddress = IPAddress(*addr);

    return 0;
}

void SetIPSrc::push(int port, Packet *p) {
    WritablePacket *ipPacket = p->uniqueify();

    click_ip *ip = reinterpret_cast<click_ip *>(ipPacket->data());

    ip->ip_src = this->ipAddress.in_addr();
	ip->ip_sum = click_in_cksum((unsigned char *) &ip, sizeof(click_ip));


    ipPacket->set_ip_header(ip, sizeof(click_ip));

    output(0).push(ipPacket);
};

CLICK_ENDDECLS
EXPORT_ELEMENT(SetIPSrc)
