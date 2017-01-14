#include <click/config.h>
// #include <click/TODO.hh>
#include "IGMPRouter.hh"

CLICK_DECLS

IGMPRouter::IGMPRouter() : timer(this){ };
IGMPRouter::~IGMPRouter() { };

int IGMPRouter::configure(Vector<String> &conf, ErrorHandler *errh) {
    timer.initialize(this);
    timer.schedule_after_msec(10000);

    // Set the database
    IGMPRouterDB* tempDb;
	IPAddress* addr1 = new IPAddress("1.1.1.1");
	IPAddress* addr2 = new IPAddress("1.1.1.1");	
    int res = cp_va_kparse(conf, this, errh,
         "DB", 0, cpElementCast, "IGMPRouterDB", &tempDb,"Client1", 0, cpIPAddress, addr1,"Client2", 0, cpIPAddress, addr2, cpEnd);

    if(res < 0){
        return res; // parsing failed
    }

    this->db = tempDb;
	this->IPclient1 = addr1;
	this->IPclient2 = addr2;

    return 0;
}

void IGMPRouter::run_timer(Timer* t){

    this->push(0, NULL);
    timer.schedule_after_msec(10000);

}


int IGMPRouter::isINCLUDE(IPAddress client_address, IPAddress multicast_address){
    click_chatter("isIN");
    return 0;
}
int IGMPRouter::isEXCLUDE(IPAddress client_address, IPAddress multicast_address){
    click_chatter("isEX");
    return 0;
}

int IGMPRouter::toEX(IPAddress client_address, IPAddress multicast_address, Vector<IPAddress>& sources){
    // If client does not exist add it!
    if(this->db->dbExists(client_address) == false){
        this->db->addDb(client_address);
    }

    IGMPClientDB* cdb = this->db->getDb(client_address);
    if(cdb->recordExists(multicast_address) == false){
        Vector<IPAddress> temp;
        cdb->addRecord(multicast_address, temp, INCLUDE);
    }

    cdb->setMode(multicast_address, EXCLUDE);

    return 0;
}
int IGMPRouter::toIN(IPAddress client_address, IPAddress multicast_address, Vector<IPAddress>& sources){
    // If client does not exist add it!
    if(this->db->dbExists(client_address) == false){
        this->db->addDb(client_address);
    }

    IGMPClientDB* cdb = this->db->getDb(client_address);
    if(cdb->recordExists(multicast_address) == false){
        Vector<IPAddress> temp;
        cdb->addRecord(multicast_address, temp, INCLUDE);
    }

    cdb->setMode(multicast_address, INCLUDE);

    return 0;
}

int IGMPRouter::block(IPAddress client_address, IPAddress multicast_address, Vector<IPAddress>& sources){
    click_chatter("block");
    return 0;
}
int IGMPRouter::allow(IPAddress client_address, IPAddress multicast_address, Vector<IPAddress>& sources){
    click_chatter("allow");
    return 0;
}

int IGMPRouter::processReport(Packet *p){
    // Checks report
    ProcessReport pr;
    pr.process(p);

    const click_ip* ipHeader = p->ip_header();
    IPAddress client(ipHeader->ip_src);


    for(int i = 0;i < pr.records.size();i++){
        grouprecord* record = pr.records.at(i);
        if(record->type == 0x01){
            // isINCLUDE
            this->isINCLUDE(client, record->multicast_address);
        }else if(record->type == 0x02){
            // isEXCLUDE
            this->isEXCLUDE(client, record->multicast_address);
        }else if(record->type == 0x03){
            // toIN
            this->toIN(client, record->multicast_address, record->sources);
        }else if(record->type == 0x04){
            // toEX
            this->toEX(client, record->multicast_address, record->sources);
        }else if(record->type == 0x05){
            // Allow - shouldn't be implemented
            this->allow(client, record->multicast_address, record->sources);
        }else if(record->type == 0x06){
            // Block - shouldn't be implemented
            this->block(client, record->multicast_address, record->sources);
        }else{
            click_chatter("Unkown record type");
            continue;
        }
    }
}



void IGMPRouter::push(int port, Packet *p) {

	click_ip iph1 , iph2 ;
	int _id = 0;
	//client1
	memset(&iph1, 0, sizeof(click_ip));
	iph1.ip_v = 4;
	iph1.ip_hl = sizeof(click_ip) >> 2;
	iph1.ip_ttl = 1;
	iph1.ip_p = 2;
	iph1.ip_dst = IPAddress("224.0.0.1").in_addr();
	iph1.ip_src = this->IPclient1->in_addr();
	iph1.ip_sum = click_in_cksum((unsigned char *) &iph1, sizeof(click_ip));
	if(p == NULL){
        // Time to send
        p = queryr.generalQuery();
	
		WritablePacket *p1 = p->push(sizeof(click_ip));
		if (!p1) return;

		click_ip *ip = reinterpret_cast<click_ip *>(p1->data());
		memcpy(ip, &iph1, sizeof(click_ip));
		if (ip->ip_len) {		// use_dst_anno
		  ip->ip_dst = p1->dst_ip_anno();
		  update_cksum(ip, 16);
		  update_cksum(ip, 18);
		} else
		  p1->set_dst_ip_anno(IPAddress(ip->ip_dst));
		ip->ip_len = htons(p1->length());
		ip->ip_id = htons(1);
		update_cksum(ip, 2);
		update_cksum(ip, 4);

		p1->set_ip_header(ip, sizeof(click_ip));

			output(0).push(p1);


		//Client2
		memset(&iph2, 0, sizeof(click_ip));
		iph2.ip_v = 4;
		iph2.ip_hl = sizeof(click_ip) >> 2;
		iph2.ip_ttl = 1;
		iph2.ip_p = 2;
		iph2.ip_dst = IPAddress("224.0.0.1").in_addr();
		iph2.ip_src = this->IPclient2->in_addr();
		iph2.ip_sum = click_in_cksum((unsigned char *) &iph2, sizeof(click_ip));
		WritablePacket *p2 = p->push(sizeof(click_ip));
		if (!p2) return;

		click_ip *ip2 = reinterpret_cast<click_ip *>(p2->data());
		memcpy(ip2, &iph2, sizeof(click_ip));
		if (ip2->ip_len) {		// use_dst_anno
		  ip2->ip_dst = p2->dst_ip_anno();
		  update_cksum(ip2, 16);
		  update_cksum(ip2, 18);
		} else
		  p2->set_dst_ip_anno(IPAddress(ip2->ip_dst));
		ip2->ip_len = htons(p2->length());
		ip2->ip_id = htons(1);
		update_cksum(ip2, 2);
		update_cksum(ip2, 4);

		p2->set_ip_header(ip2, sizeof(click_ip));

		output(1).push(p2);
    }else{
        this->processReport(p);
        p->kill();
    }
}

String IGMPRouter::getDBHandler(Element *e, void * thunk){
    IGMPRouter* me = (IGMPRouter *) e;
    return me->db->toString();
}

void IGMPRouter::add_handlers(){
    add_read_handler("get_db", &getDBHandler, (void *)0);
}

inline void
IGMPRouter::update_cksum(click_ip *ip, int off) const
{
#if HAVE_INDIFFERENT_ALIGNMENT
    click_update_in_cksum(&ip->ip_sum, 0, ((uint16_t *) ip)[off/2]);
#else
    const uint8_t *u = (const uint8_t *) ip;
# if CLICK_BYTE_ORDER == CLICK_BIG_ENDIAN
    click_update_in_cksum(&ip->ip_sum, 0, u[off]*256 + u[off+1]);
# else
    click_update_in_cksum(&ip->ip_sum, 0, u[off] + u[off+1]*256);
# endif
#endif
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouter)
