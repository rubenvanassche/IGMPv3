#include <click/config.h>
// #include <click/TODO.hh>
#include "IGMPClient.hh"

CLICK_DECLS

IGMPClient::IGMPClient() { };
IGMPClient::~IGMPClient() { };

Packet *IGMPClient::simple_action(Packet *p) {
	// TODO: fill

	return p;
};

interfacerecord* IGMPClient::getInterfacerecord(IPAddress multicast_address){
	for(int i = 0; i < this->interfacerecords.size();i++){
		interfacerecord* record = this->interfacerecords.at(i);
		if(record->multicast_address == multicast_address){
			return record;
		}
	}

	return nullptr;
}

void IGMPClient::includeSources(IPAddress multicast_address, Vector<IPAddress> sources){
	interfacerecord* record = this->getInterfacerecord(multicast_address);

	if(record == nullptr){
		// New multicast address
		record = new interfacerecord();
		record->multicast_address = multicast_address;
		record->sources = sources;
		record->filter_mode = INCLUDE;
	}else{
		if(record->filter_mode == INCLUDE){
			// Union
			for(int i = 0;i < sources.size();i++){
				bool inRecord = false;

				for(int j = 0;j < record->sources.size();j++){
					if(record->sources.at(j) == sources.at(i)){
						// Source is already in record
						inRecord = true;
						break;
					}
				}

				if(inRecord == false){
					// Add source to record
					record->sources.push_back(sources.at(i));
				}
			}
		}else if(record->filter_mode == EXCLUDE){
			// TODO: sent TO_EX event
			record->filter_mode = EXCLUDE;

			record->sources.clear();

			for(int i = 0;i < sources.size();i++){
				record->sources.push_back(sources.at(i));
			}
		}else{
			click_chatter("Wrong include sources mode");
		}
	}
}
void IGMPClient::excludeSources(IPAddress multicast_address, Vector<IPAddress> sources){
	interfacerecord* record = this->getInterfacerecord(multicast_address);

	if(record == nullptr){
		// New multicast address
		record = new interfacerecord();
		record->multicast_address = multicast_address;
		record->sources = sources;
		record->filter_mode = EXCLUDE;
	}else{
		if(record->filter_mode == INCLUDE){
			// if sources in record, remove them
			Vector<IPAddress>::iterator it;
			for(it = record->sources.begin();it != record->sources.end();it++){
				for(int j = 0;j < sources.size();j++){
					if(*it == sources.at(j)){
						// Source is already in record, so remove it
						// TODO: does this work????
						record->sources.erase(it);
						it++;
					}
				}
			}
		}else if(record->filter_mode == EXCLUDE){

		}else{
			click_chatter("Wrong exclude sources mode");
		}
	}
}

int IGMPClient::includeSourcesHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh){
	IGMPClient* me = (IGMPClient *) e;
	ConfigParse p;

	Vector<IPAddress> sources = p.sources(conf, errh);
	IPAddress multicast_address = p.multicastAddress(conf, errh);

	me->includeSources(multicast_address, sources);

	return 0;
}

int IGMPClient::excludeSources(const String &conf, Element *e, void * thunk, ErrorHandler * errh){
	IGMPClient* me = (IGMPClient *) e;
	ConfigParse p;

	Vector<IPAddress> sources = p.sources(conf, errh);
	IPAddress multicast_address = p.multicastAddress(conf, errh);

	me->excludeSources(multicast_address, sources);

	return 0;
}

void IGMPClient::add_handlers(){
	add_write_handler("include_sources", &includeSourcesHandler, (void *)0);
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
	.read_m("S", sources_string)
	.complete() < 0){
		click_chatter("Failed reading include sources");

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
	.read_m("S", sources_string)
	.complete() < 0){
		click_chatter("Failed reading include sources");

		return IPAddress("0.0.0.0");
	}

    return IPAddress(multicast_address);


}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClient)
