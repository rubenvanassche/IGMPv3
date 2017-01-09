#include <click/config.h>
// #include <click/TODO.hh>
#include "IGMPRouterDB.hh"

CLICK_DECLS

IGMPRouterDB::IGMPRouterDB() { };
IGMPRouterDB::~IGMPRouterDB() { };

String IGMPRouterDB::toString(){
	HashTable<IPAddress, IGMPClientDB*>::iterator it;
	String out;

	for(it = this->dbs.begin();it != this->dbs.end();it++){
		out += "============================\n";
		out += "Client ";
		out += it->first.s().c_str();
		out += "\n";
		out += "============================\n";
		out += it->second->toString();
	}

	return out;
}

Packet *IGMPRouterDB::simple_action(Packet *p) {
	// TODO: fill

	return p;
};

bool IGMPRouterDB::dbExists(IPAddress client_address){
	if(this->dbs.find(client_address) == this->dbs.end()){
		return false;
	}else{
		return true;
	}
}

IGMPClientDB* IGMPRouterDB::getDb(IPAddress client_address){
	return this->dbs.get(client_address);
}

int IGMPRouterDB::addDb(IPAddress client_address){
	if(this->dbExists(client_address)){
		return -1;
	}

	IGMPClientDB* newDb = new IGMPClientDB();
	this->dbs.set(client_address, newDb);

	return 0;
}

Vector<IPAddress> IGMPRouterDB::acceptFromSource(IPAddress multicast_address, IPAddress source){
	HashTable<IPAddress, IGMPClientDB*>::iterator it;
	Vector<IPAddress> accepted;

	for(it = this->dbs.begin();it != this->dbs.end();it++){
		IPAddress client = it->first;
		IGMPClientDB* clientDb = it->second;

		if(clientDb->acceptFromSource(multicast_address, source)){
			accepted.push_back(client);
		}
	}

	return accepted;
}

bool IGMPRouterDB::acceptFromSourceAsClient(IPAddress multicast_address, IPAddress source, IPAddress client_address, IPAddress client_address_mask){
	HashTable<IPAddress, IGMPClientDB*>::iterator it;
	Vector<IPAddress> accepted;

	for(it = this->dbs.begin();it != this->dbs.end();it++){
		IPAddress client = it->first;
		IGMPClientDB* clientDb = it->second;

		if(client.matches_prefix(client_address, client_address_mask) == false){
			continue;
		}



		if(clientDb->acceptFromSource(multicast_address, source)){
			return true;
		}
	}

	return false;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouterDB)
