#include <click/config.h>
// #include <click/TODO.hh>
#include "IGMPClassifier.hh"

CLICK_DECLS

IGMPClassifier::IGMPClassifier() { };
IGMPClassifier::~IGMPClassifier() { };

int IGMPClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
    // Set the database
    IGMPClientDB* tempDb;
    int res = cp_va_kparse(conf, this, errh,
         "DB", 0, cpElementCast, "IGMPClientDB", &tempDb, cpEnd);

    if(res < 0){
        return res; // parsing failed
    }

    this->db = tempDb;

    return 0;
}


Packet* IGMPClassifier::simple_action(Packet *p) {
	const click_ip* ipHeader = p->ip_header();
	IPAddress destination = IPAddress(ipHeader->ip_dst);
	IPAddress source = IPAddress(ipHeader->ip_src);

	if(ipHeader->ip_p == 2){
		// IGMP
		output(2).push(p);
		click_chatter("IGMP");
        return 0;
	}

	if(this->db->acceptFromSource(destination, source)){
        click_chatter("MULTICAST");
		output(1).push(p);
	}else{
        click_chatter("UNICAST");
		output(0).push(p);
	}

    return 0;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClassifier)
