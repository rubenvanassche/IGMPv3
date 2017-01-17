#include <click/config.h>
// #include <click/TODO.hh>
#include "IGMPClientDB.hh"

CLICK_DECLS

IGMPClientDB::IGMPClientDB() { };
IGMPClientDB::~IGMPClientDB() { };


String IGMPClientDB::toString(){
	String out;
	out += "IGMPRouterDB\n";
	out += "------------\n";

	for(int i = 0;i < this->records.size();i++){
		Record* record = this->records.at(i);

		String ie;
		if(record->filter_mode == INCLUDE){
			ie = "(I)";
		}else{
			ie = "(E)";
		}

		out += ie + " - " + record->multicast_address.s().c_str() + "\n";
		for(int j = 0;j < record->sources.size();j++){
			IPAddress ip = record->sources.at(j);
			out += "    ";
			out +=  ip.s().c_str();
			out += "\n";
		}
	}

	return out;
}

void IGMPClientDB::includeSources(IPAddress multicast_address, Vector<IPAddress> sources){
	Record* record = this->getRecord(multicast_address);

	if(record == NULL){
		click_chatter("tes");
		// New multicast address
		record = new Record();
		record->multicast_address = multicast_address;
		record->sources = sources;
		record->filter_mode = INCLUDE;

		this->records.push_back(record);
	}else{
		if(record->filter_mode == INCLUDE){
			/*
			// Shouldn't be implemented

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
			*/
			click_chatter("INCLUDE + INCLUDE shouldn't be implemented");
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
void IGMPClientDB::excludeSources(IPAddress multicast_address, Vector<IPAddress> sources){
	Record* record = this->getRecord(multicast_address);

	if(record == NULL){
		// New multicast address
		record = new Record();
		record->multicast_address = multicast_address;
		//record->sources = sources;
		record->filter_mode = INCLUDE; // See RFC

		this->records.push_back(record);
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
			// Shouldn't be implemented
			click_chatter("EXCLUDE + EXLCUDE shouldn't be implemented");
		}else{
			click_chatter("Wrong exclude sources mode");
		}
	}
}

int IGMPClientDB::addSources(IPAddress multicast_address, Vector<IPAddress> sources){
	Record* record = this->getRecord(multicast_address);

	if(record == NULL){
		return -1;
	}

	for(int i = 0;i < sources.size();i++){
		record->sources.push_back(sources.at(i));
	}

	return 0;
}
int IGMPClientDB::addSource(IPAddress multicast_address, IPAddress source){
	Record* record = this->getRecord(multicast_address);

	if(record == NULL){
		return -1;
	}

	record->sources.push_back(source);

	return 0;
}
int IGMPClientDB::clearSources(IPAddress multicast_address){
	Record* record = this->getRecord(multicast_address);

	if(record == NULL){
		return -1;
	}

	record->sources.clear();

	return 0;
}
int IGMPClientDB::removeSources(IPAddress multicast_address, Vector<IPAddress> sources){
	Record* record = this->getRecord(multicast_address);

	if(record == NULL){
		return -1;
	}

	Vector<IPAddress>::iterator it;
	Vector<Vector<IPAddress>::iterator> removeIterators;
	for(it = record->sources.begin();it != record->sources.end();it++){
		for(int i = 0;i < sources.size();i++){
			if((*it) == sources.at(i)){
				removeIterators.push_back(it);
				break;
			}
		}
	}

	for(int i = 0;i < removeIterators.size();i++){
		record->sources.erase(removeIterators.at(i));
	}

	return 0;
}
int IGMPClientDB::removeSource(IPAddress multicast_address, IPAddress source){
	Record* record = this->getRecord(multicast_address);

	if(record == NULL){
		return -1;
	}

	Vector<IPAddress>::iterator it;
	Vector<Vector<IPAddress>::iterator> removeIterators;
	for(it = record->sources.begin();it != record->sources.end();it++){
		if((*it) == source){
			removeIterators.push_back(it);
		}
	}

	for(int i = 0;i < removeIterators.size();i++){
		record->sources.erase(removeIterators.at(i));
	}

	return 0;
}
bool IGMPClientDB::recordExists(IPAddress multicast_address){
	for(int i = 0;i < this->records.size();i++){
		if(this->records.at(i)->multicast_address == multicast_address){
			return true;
		}
	}

	return false;
}
Record* IGMPClientDB::getRecord(IPAddress multicast_address){
	for(int i = 0;i < this->records.size();i++){
		if(this->records.at(i)->multicast_address == multicast_address){
			return this->records.at(i);
		}
	}

	return NULL;
}
int IGMPClientDB::addRecord(IPAddress multicast_address, Vector<IPAddress> sources, filtermode mode){
	Record* record = new Record;
	record->multicast_address = multicast_address;
	record->sources = sources;
	record->filter_mode = mode;

	this->records.push_back(record);
}
bool IGMPClientDB::acceptFromSource(IPAddress multicast_address, IPAddress source){
	Record* record = this->getRecord(multicast_address);

	if(record == NULL){
		return false;
	}

	bool inSourceList = false;
	for(int i = 0;i < record->sources.size();i++){
		if(record->sources.at(i) == source){
			inSourceList = true;
			break;
		}
	}

	if(record->filter_mode == INCLUDE and inSourceList == true){
		return true;
	}else if(record->filter_mode == INCLUDE and inSourceList == false){
		return false;
	}else if(record->filter_mode == EXCLUDE and inSourceList == true){
		return false;
	}else if(record->filter_mode == EXCLUDE and inSourceList == false){
		return true;
	}else{
		return false;
	}
}
int IGMPClientDB::setMode(IPAddress multicast_address, filtermode mode){
	Record* record = this->getRecord(multicast_address);

	if(record == NULL){
		return -1;
	}

	record->filter_mode = mode;

	return 0;
}
Vector<IPAddress> IGMPClientDB::getSources(IPAddress multicast_address){
	Record* record = this->getRecord(multicast_address);

	return record->sources;
}
filtermode IGMPClientDB::getMode(IPAddress multicast_address){
	Record* record = this->getRecord(multicast_address);

	return record->filter_mode;
}

HashTable<IPAddress, filtermode> IGMPClientDB::getMulticastFiltermodeTable(){
	HashTable<IPAddress, filtermode> output;

	for(int i = 0;i < this->records.size();i++){
		output.set(this->records.at(i)->multicast_address, this->records.at(i)->filter_mode);
	}

	return output;
}

Packet *IGMPClientDB::simple_action(Packet *p) {
	// TODO: fill

	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPClientDB)
