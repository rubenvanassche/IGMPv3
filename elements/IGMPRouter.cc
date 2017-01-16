#include <click/config.h>
// #include <click/TODO.hh>
#include "IGMPRouter.hh"

CLICK_DECLS

IGMPRouter::IGMPRouter() : timer(this){ };
IGMPRouter::~IGMPRouter() { };

int IGMPRouter::configure(Vector<String> &conf, ErrorHandler *errh) {
    timer.initialize(this);
    timer.schedule_after_sec(this->query_interval);

    // Set the database
    IGMPRouterDB* tempDb;

    int res = cp_va_kparse(conf, this, errh,
         "DB", 0, cpElementCast, "IGMPRouterDB", &tempDb,
    cpEnd);

    if(res < 0){
        return res; // parsing failed
    }

    this->db = tempDb;


    return 0;
}

void IGMPRouter::run_timer(Timer* t){
    this->push(0, queryr.generalQuery());
    timer.schedule_after_sec(this->query_interval);
}

void IGMPRouter::handleSendGroupQuery(Timer*, void * data){
    SendGroupQueryTimerData* timerdata = (SendGroupQueryTimerData*) data;
    assert(timerdata); // the cast must be good
    timerdata->me->push(0, timerdata->query);
}

void IGMPRouter::sendGroupQuery(IPAddress multicast_address){
    Vector<IPAddress>::iterator it;

    for(it = this->groupQuerysSheduled.begin();it != this->groupQuerysSheduled.end();it++){
        if((*it) == multicast_address){
            // Query is already sheduled;
            return;
        }
    }

    for(int i = 0;i < this->robustness_variable;i++){
        SendGroupQueryTimerData* data = new SendGroupQueryTimerData();
        data->query = queryr.groupQuery(multicast_address);
        data->me = this;
        data->address = multicast_address;


        Timer* t = new Timer(&IGMPRouter::handleSendGroupQuery,data);
        t->initialize(this);
        t->schedule_after_msec(10);
    }

    this->groupQuerysSheduled.push_back(multicast_address);
}

int IGMPRouter::isINCLUDE(IPAddress client_address, IPAddress multicast_address){
    return 0;
}
int IGMPRouter::isEXCLUDE(IPAddress client_address, IPAddress multicast_address){

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

    this->sendGroupQuery(multicast_address);

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
    if(p == nullptr){
        return;
    }

    // Let's find out which packet p is
    unsigned char * igmpbegin = 0;
    igmpv3query* queryFormat = 0;
    igmpv3report* reportFormat = 0;

    // A report?
    igmpbegin = (unsigned char *)p->data()+sizeof(click_ip);
    reportFormat = (igmpv3report*)igmpbegin;

    if(reportFormat->type == 0x22){
        // It is!
        this->processReport(p);
        p->kill();
        return;
    }

    // A Query?
    igmpbegin = (unsigned char *)p->data()+sizeof(click_ip);
    queryFormat = (igmpv3query*)igmpbegin;

    if(queryFormat->type == 0x11){
        // It is!
        output(0).push(p);

        return;
    }

    click_chatter("Recieved an unkown packet");
    p->kill();
    return;
}

String IGMPRouter::getDBHandler(Element *e, void * thunk){
    IGMPRouter* me = (IGMPRouter *) e;
    return me->db->toString();
}

void IGMPRouter::add_handlers(){
    add_read_handler("get_db", &getDBHandler, (void *)0);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouter)
