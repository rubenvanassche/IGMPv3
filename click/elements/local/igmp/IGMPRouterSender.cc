#include <click/config.h>
// #include <click/TODO.hh>
#include "IGMPRouterSender.hh"

CLICK_DECLS

IGMPRouterSender::IGMPRouterSender() { };
IGMPRouterSender::~IGMPRouterSender() { };

int IGMPRouterSender::configure(Vector<String> &conf, ErrorHandler *errh){
	IGMPRouterDB* tempDb;
	IPAddress* addr = new IPAddress("1.1.1.1");
	IPAddress* mask = new IPAddress("1.1.1.1");

	int res = cp_va_kparse(conf, this, errh,
		"ADDR", 0, cpIPPrefix, addr, mask,
		 "DB", 0, cpElementCast, "IGMPRouterDB", &tempDb,
		cpEnd);

	if(res < 0){
		return -1; // parsing failed
	}

	this->subnetAddress = IPAddress(*addr);
	this->subnetMask = IPAddress(*mask);

	this->db = tempDb;

	return 0;
}

void IGMPRouterSender::push(int port, Packet *p) {
	const click_ip* ipHeader = p->ip_header();
	IPAddress destination = IPAddress(ipHeader->ip_dst);
	IPAddress source = IPAddress(ipHeader->ip_src);

	int subnetMaskLength = this->subnetMask.mask_to_prefix_len();
	if(subnetMaskLength == -1){
		click_chatter("Cant determine subnetmask");
		p->kill();
		return;
	}

	if(this->db->acceptFromSourceAsClient(destination, source, this->subnetAddress, this->subnetMask)){
		//click_chatter("Accepted");
		output(0).push(p);
	}else{
		//click_chatter("NOT Accepted");
		p->kill();
	}
};

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouterSender)
