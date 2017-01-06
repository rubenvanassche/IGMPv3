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
    return 0;
}

void IGMPClient::run_timer(Timer* t){
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


int IGMPClient::includeSourcesHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh){
	IGMPClient* me = (IGMPClient *) e;
	ConfigParse p;

	Vector<IPAddress> sources = p.sources(conf, errh);
	IPAddress multicast_address = p.multicastAddress(conf, errh);

	me->reporter.toIN(multicast_address, sources, 2);

	return 0;
}

int IGMPClient::excludeSourcesHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh){
	IGMPClient* me = (IGMPClient *) e;
	ConfigParse p;

	Vector<IPAddress> sources = p.sources(conf, errh);
	IPAddress multicast_address = p.multicastAddress(conf, errh);

	me->reporter.toEX(multicast_address, sources, 2);

	return 0;
}

void IGMPClient::add_handlers(){
	add_write_handler("include_sources", &includeSourcesHandler, (void *)0);
	add_write_handler("exclude_sources", &excludeSourcesHandler, (void *)0);
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

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClient)
