#include <click/config.h>
// #include <click/TODO.hh>
#include "IGMPRouterClassifier.hh"

CLICK_DECLS

IGMPRouterClassifier::IGMPRouterClassifier() { };
IGMPRouterClassifier::~IGMPRouterClassifier() { };

int IGMPRouterClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
    // Set the database
    IGMPRouterDB* tempDb;
    int res = cp_va_kparse(conf, this, errh,
         "DB", 0, cpElementCast, "IGMPRouterDB", &tempDb, cpEnd);

    if(res < 0){
        return res; // parsing failed
    }

    this->db = tempDb;

    return 0;
}


Packet* IGMPRouterClassifier::simple_action(Packet *p) {
	const click_ip* ipHeader = p->ip_header();
	IPAddress destination = IPAddress(ipHeader->ip_dst);
	IPAddress source = IPAddress(ipHeader->ip_src);

	if(ipHeader->ip_p == 2){
		// IGMP
		output(2).push(p);
		//click_chatter("IGMP");
        return 0;
	}

    Vector<IPAddress> multicastAcceptors = this->db->acceptFromSource(destination, source);
    if(multicastAcceptors.size() > 0){
        //click_chatter("MULTICAST TRAFIC");
        output(1).push(p);
    }else{
        //click_chatter("OTHER TRAFIC");
        output(0).push(p);
    }

    return 0;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouterClassifier)
