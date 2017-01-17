#include <click/config.h>
// #include <click/TODO.hh>
#include "ProcessQuery.hh"

CLICK_DECLS

ProcessQuery::ProcessQuery() { };
ProcessQuery::~ProcessQuery() { };

bool ProcessQuery::verifyChecksum(Packet* p){
	WritablePacket *q = p->uniqueify();
	unsigned char * igmpbegin = (unsigned char *)p->data()+sizeof(click_ip);
	igmpv3query* format = (igmpv3query*)igmpbegin;

	uint16_t checksum = format->checksum;
	format->checksum = 0x00;

	return (click_in_cksum((unsigned char *)format, q->length()-sizeof(click_ip)) == checksum);
}

int ProcessQuery::process(Packet* packet){
	unsigned char * igmpbegin = (unsigned char *)packet->data()+sizeof(click_ip);
	igmpv3query* format = (igmpv3query*)igmpbegin;

	this->verifiedChecksum = this->verifyChecksum(packet);
	this->checksum = format->checksum;
	if(this->verifiedChecksum == false){
		click_chatter("Incorrect checksum for query");
		return -1;
	}

	this->type = format->type;
	if(this->type != 0x11){
		click_chatter("Incorrect type for query");
		return -1;
	}


	this->max_response_code = format->max_response_code;
	this->group_address = IPAddress(format->group_address);
	this->resv_and_s_and_qrv = format->resv_and_s_and_qrv;
	this->qqic = format->qqic;

	int no_of_sources = ntohs(format->no_of_sources);

	for(int i = 0;i < no_of_sources;i++){
		this->sources.push_back(IPAddress(format->sources[i]));
	}
}

bool ProcessQuery::isGeneralQuery(){
	return this->group_address == IPAddress("0.0.0.0") and this->sources.size() == 0;
}

bool ProcessQuery::isGroupQuery(){
	return this->group_address != IPAddress("0.0.0.0") and this->sources.size() == 0;
}

bool ProcessQuery::isGroupAndSourceQuery(){
	this->group_address != IPAddress("0.0.0.0") and this->sources.size() != 0;
}

void ProcessQuery::print(){
	click_chatter("Type %x", this->type);
	click_chatter("Checkum %x", this->checksum);
	click_chatter("Max Response Code %x", this->max_response_code);
	click_chatter("S and qrv %x", this->resv_and_s_and_qrv);
	click_chatter("QQIC %x", this->qqic);
	click_chatter("Group Address %s", this->group_address.s().c_str());

	for(int i = 0; i < this->sources.size();i++){
		click_chatter("  Source %s", this->sources.at(i).s().c_str());
	}

	if(this->verifiedChecksum){
		click_chatter("Checksum correct");
	}else{
		click_chatter("Wrong checksum!!");
	}
}

Packet *ProcessQuery::simple_action(Packet *p) {
	// TODO: fill

	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(ProcessQuery)
