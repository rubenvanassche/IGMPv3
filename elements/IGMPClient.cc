#include <click/config.h>
#include <click/timer.hh>
// #include <click/TODO.hh>
#include "IGMPClient.hh"

CLICK_DECLS

IGMPClient::IGMPClient():  timer(this) { };
IGMPClient::~IGMPClient() { };

int IGMPClient::configure(Vector<String> &conf, ErrorHandler *errh) {
    // init timer
    timer.initialize(this);
    timer.schedule_after_msec(1000);

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

void IGMPClient::run_timer(Timer* t){
    /*
    Vector<IPAddress> a;
    Vector<IPAddress> b;
    a.push_back(IPAddress("3.3.3.3"));
    a.push_back(IPAddress("1.1.1.1"));
    a.push_back(IPAddress("3.3.3.3"));
    b.push_back(IPAddress("1.1.1.1"));
    b.push_back(IPAddress("3.3.3.3"));

    Vector<IPAddress> out = vectorsIntersection(a, a);
    click_chatter("TEST");
    for(int i = 0;i < out.size();i++){
        click_chatter("%s", out.at(i).s().c_str());
    }
    */

    timer.schedule_after_msec(1000);
    this->push(nullptr);
}

void IGMPClient::push(Packet *p) {
	Packet* packet = this->reporter.simple_action(0, nullptr);

	if(packet == nullptr){
		return;
	}

	click_chatter("Packet send");
	output(0).push(packet);
};

void IGMPClient::includeWithExclude(IPAddress multicast_address, Vector<IPAddress> sources){

    //Vector<IPAddress> oldSources = this->db.getRecord(multicast_address)->sources;

    //Vector<IPAddress> unions = vectorsUnion(oldSources, sources);
    click_chatter("IN -> EX");
    this->db->setMode(multicast_address, EXCLUDE);
    this->reporter.toEX(multicast_address, sources, 2);
}
void IGMPClient::includeWithInclude(IPAddress multicast_address, Vector<IPAddress> sources){
    // NEEDS NOT TE BE IMPLEMENTED
    click_chatter("IN -> IN(not impelemented)");
}
void IGMPClient::excludeWithExclude(IPAddress multicast_address, Vector<IPAddress> sources){
    // NEEDS NOT TE BE IMPLEMENTED
    click_chatter("EX -> EX(not impelemented)");
}
void IGMPClient::excludeWithInclude(IPAddress multicast_address, Vector<IPAddress> sources){
    this->db->setMode(multicast_address, INCLUDE);
    this->reporter.toIN(multicast_address, sources, 2);
    click_chatter("EX -> IN");
}

int IGMPClient::includeSourcesHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh){
    /* INCLUDE
    If multicastAddress is INCLUDE
    - Union of previous and new entries for DB
    - Send Allow(new-old) and Block(old-new)
    If multicastAddress is EXCLUDE
    - New entries - old entries for DB
    - Set mode to exclude for DB
    - Send to_ex(new)
     */
	IGMPClient* me = (IGMPClient *) e;
	ConfigParse p;

    // Because simple suystem
	//Vector<IPAddress> sources = p.sources(conf, errh);
	Vector<IPAddress> sources;
	IPAddress multicast_address = p.multicastAddress(conf, errh);

    if(!me->db->recordExists(multicast_address)){
        me->db->addRecord(multicast_address, sources, INCLUDE);
        me->excludeWithInclude(multicast_address, sources);
        return 0;
    }

    filtermode mode = me->db->getMode(multicast_address);

    if(mode == INCLUDE){
        me->includeWithInclude(multicast_address, sources);
    }else{
        me->excludeWithInclude(multicast_address, sources);
    }

	return 0;
}

int IGMPClient::excludeSourcesHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh){
    /* EXCLUDE
    If multicastAddress is EXLCUDE
    - Intersection of previous and new entries for DB
    - Send Allow(old-new) and Block(new-old)
    If multicastAddress is INCLUDE
    - old entries - new entries for DB
    - Set mode to include for DB
    - Send to_ex(new)
     */
	IGMPClient* me = (IGMPClient *) e;
	ConfigParse p;

    // Because simple suystem
	//Vector<IPAddress> sources = p.sources(conf, errh);
	Vector<IPAddress> sources;
	IPAddress multicast_address = p.multicastAddress(conf, errh);

    if(!me->db->recordExists(multicast_address)){
        me->db->addRecord(multicast_address, sources, EXCLUDE);
        me->includeWithExclude(multicast_address, sources);
        return 0;
    }

    filtermode mode = me->db->getMode(multicast_address);

    if(mode == INCLUDE){
        me->includeWithExclude(multicast_address, sources);
    }else{
        me->excludeWithExclude(multicast_address, sources);
    }

	return 0;
}

String IGMPClient::getDBHandler(Element *e, void * thunk){
    IGMPClient* me = (IGMPClient *) e;
    return me->db->toString();
}

void IGMPClient::add_handlers(){
	add_write_handler("include_sources", &includeSourcesHandler, (void *)0);
	add_write_handler("exclude_sources", &excludeSourcesHandler, (void *)0);
    add_read_handler("get_db", &getDBHandler, (void *)0);
}

Vector<IPAddress> ConfigParse::sources(const String &conf, ErrorHandler * errh){
	String sources_string;
	String source_address;
    String multicast_address;
	Vector<IPAddress> sources;

	// Parse
	if(Args(errh)
	.push_back_args(conf)
	.read_m("M", multicast_address)
	.read("S", sources_string)
	.complete() < 0){
		click_chatter("Failed reading sources");
		assert(false && "Failed reading  sources");

		return sources;
	}

	// Seperate the sources_string
	for(int i = 0;i < sources_string.length(); i++){
		if(sources_string.at(i) == '+'){
			IPAddress addr(source_address);
			source_address = "";

			sources.push_back(addr);
		}else{
			source_address += sources_string.at(i);
		}

		if(i == sources_string.length() - 1){
			IPAddress addr(source_address);
			sources.push_back(addr);
		}
	}

    return sources;
}

IPAddress ConfigParse::multicastAddress(const String &conf, ErrorHandler * errh){
    String sources_string;
    String multicast_address;

	// Parse
	if(Args(errh)
	.push_back_args(conf)
	.read_m("M", multicast_address)
	.read("S", sources_string)
	.complete() < 0){
		click_chatter("Failed reading  sources");
		assert(false && "Failed reading  sources");

		return IPAddress("0.0.0.0");
	}

    return IPAddress(multicast_address);


}

Vector<IPAddress> vectorsUnion(Vector<IPAddress> a, Vector<IPAddress> b){
    Vector<IPAddress> out = a;

    for(int i = 0;i < b.size();i++){
        bool inA = false;

        for(int j = 0;j < out.size();j++){
            if(b.at(i) == out.at(j)){
                inA = true;
                break;
            }
        }

        if(inA == false){
            out.push_back(b.at(i));
        }
    }

    return out;
}

Vector<IPAddress> vectorsIntersection(Vector<IPAddress> a, Vector<IPAddress> b){
    Vector<IPAddress> out;

    for(int i = 0;i < b.size();i++){
        bool inA = false;

        for(int j = 0;j < a.size();j++){
            if(b.at(i) == a.at(j)){
                inA = true;
            }
        }

        if(inA == true){
            out.push_back(b.at(i));
        }
    }

    return out;
}

Vector<IPAddress> vectorsMinus(Vector<IPAddress> a, Vector<IPAddress> b){
    Vector<IPAddress> out = a;
    Vector<IPAddress>::iterator it = out.begin();

    while(it != out.end()){
        bool jump = false;

        for(int j = 0;j < b.size();j++){
            if(*it == b.at(j)){
                jump = true;
                Vector<IPAddress>::iterator it2 = it;
                it++;
                out.erase(it2);
                break;
            }
        }

        if(jump == false){
            it++;
        }
    }


    return out;

}

Vector<IPAddress> vectorsUnique(Vector<IPAddress> a){
    Vector<IPAddress> out;

    for(int i = 0;i < out.size();i++){
        for(int j = 0;j < a.size();j++){
            if(out.at(i) == a.at(j)){

            }
        }
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClient)
