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

    // Timer test
    TimerData* timerdata = new TimerData();
    timerdata->x = 11;
    timerdata->me = this;
    Timer* t = new Timer(&IGMPClient::handleExpiry,timerdata);
    t->initialize(this);
    t->schedule_after_msec(2500);

    return 0;
}


void IGMPClient::handleExpiry(Timer*, void * data){
    TimerData* timerdata = (TimerData*) data;
    assert(timerdata); // the cast must be good
    timerdata->me->expire(timerdata);
}

void IGMPClient::expire(TimerData* data){
    TimerData* timerdata = (TimerData*) data;
    click_chatter("TIMERDATA %i", timerdata->x);
}

void IGMPClient::run_timer(Timer* t){
    timer.schedule_after_msec(1000);
}

void IGMPClient::push(int port, Packet *p) {
    if(p == nullptr){
        return;
    }

    // Let's find out which packet p is
    unsigned char * igmpbegin = 0;
    igmpv3query* queryFormat = 0;
    igmpv3report* reportFormat = 0;

    // A report?
    igmpbegin = (unsigned char *)p->data();
    reportFormat = (igmpv3report*)igmpbegin;

    if(reportFormat->type == 0x22){
        // It is!
        output(0).push(p);
        return;
    }

    // A Query?
    igmpbegin = (unsigned char *)p->data()+sizeof(click_ip);
    queryFormat = (igmpv3query*)igmpbegin;

    if(queryFormat->type == 0x11){
        // It is!
        this->proccessQuery(p);
        p->kill();

        return;
    }

    click_chatter("Recieved an unkown packet");
    return;
}

void IGMPClient::proccessQuery(Packet *p){
    const click_ip* ipHeader = p->ip_header();

    if(ipHeader->ip_p != 2){
        click_chatter("Recieved a packet which is not IGMP Protocol");
        return;
    }

    ProcessQuery pq;
    pq.process(p);

    pq.print();
}

void IGMPClient::includeWithExclude(IPAddress multicast_address, Vector<IPAddress> sources){
    this->db->setMode(multicast_address, EXCLUDE);

    for(int i = 0;i < this->robustness_variable;i++){
        int reschedule = (int) (((double)((this->unsolicited_report_interval*1000)+1)/RAND_MAX) * rand() + 0);
        click_chatter("RE %i", reschedule);

        SendReportTimerData* data = new SendReportTimerData();
        data->report = this->reporter.toEX(multicast_address, sources);
        data->me = this;

        click_chatter("REPORT POINTER %p", data->report);

        Timer* t = new Timer(&IGMPClient::handleSendReportTimer,data);
        t->initialize(this);
        t->schedule_after_msec(reschedule);
    }

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

    for(int i = 0;i < this->robustness_variable;i++){
        int reschedule = (int) (((double)((this->unsolicited_report_interval*1000)+1)/RAND_MAX) * rand() + 0);
        click_chatter("RE %i", reschedule);

        SendReportTimerData* data = new SendReportTimerData();
        data->report = this->reporter.toIN(multicast_address, sources);
        data->me = this;

        click_chatter("REPORT POINTER %p", data->report);

        Timer* t = new Timer(&IGMPClient::handleSendReportTimer,data);
        t->initialize(this);
        t->schedule_after_msec(reschedule);
    }
}

void IGMPClient::handleSendReportTimer(Timer*, void * data){
    SendReportTimerData* timerdata = (SendReportTimerData*) data;
    assert(timerdata); // the cast must be good
    timerdata->me->push(0, timerdata->report);
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
