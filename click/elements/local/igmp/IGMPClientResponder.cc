#include <click/config.h>
// #include <click/TODO.hh>
#include "IGMPClientResponder.hh"

CLICK_DECLS

IGMPClientResponder::IGMPClientResponder() { };
IGMPClientResponder::~IGMPClientResponder() { };

int IGMPClientResponder::configure(Vector<String> &conf, ErrorHandler *errh) {
    // init timer
    timer.initialize(this);
    timer.schedule_after_msec(1000);

    // Set the database
    IGMPClientDB* tempDb;
    IPAddress* addr = new IPAddress("1.1.1.1");

    int res = cp_va_kparse(conf, this, errh,
        "ADDR", 0, cpIPAddress, addr,
         "DB", 0, cpElementCast, "IGMPClientDB", &tempDb,
        cpEnd);

    if(res < 0){
        return -1; // parsing failed
    }

    this->ipAddress = IPAddress(*addr);
    this->db = tempDb;

    return 0;
}

void IGMPClientResponder::run_timer(Timer* t){
    timer.schedule_after_msec(1000);

	click_chatter("TILME");
}

void IGMPClientResponder::push(int port, Packet* p) {
    p->kill();
	click_chatter("WOWKES");

};

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientResponder)
