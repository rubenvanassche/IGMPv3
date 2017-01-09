#include <click/config.h>
// #include <click/TODO.hh>
#include "ProcessReport.hh"

CLICK_DECLS

ProcessReport::ProcessReport() { };
ProcessReport::~ProcessReport() { };

Packet *ProcessReport::simple_action(Packet *p) {
	// TODO: fill

	return p;
};

bool ProcessReport::verifyChecksum(Packet* p){
	WritablePacket *q = p->uniqueify();
	unsigned char * igmpbegin = (unsigned char *)p->data()+sizeof(click_ip);
	igmpv3report* format = (igmpv3report*)igmpbegin;

	uint16_t checksum = format->checksum;
	format->checksum = 0x00;

	return (click_in_cksum((unsigned char *)format, q->length()-sizeof(click_ip)) == checksum);
}

int ProcessReport::process(Packet* packet){
	unsigned char * igmpbegin = (unsigned char *)packet->data()+sizeof(click_ip);
	igmpv3report* format = (igmpv3report*)igmpbegin;


	this->verifiedChecksum = this->verifyChecksum(packet);
	this->checksum = format->checksum;
	if(this->verifiedChecksum == false){
		click_chatter("Incorrect checksum for report");
		return -1;
	}

	this->type = format->type;
	if(this->type != 0x22){
		click_chatter("Incorrect type for report");
		return -1;
	}

	this->no_of_grouprecords = ntohs(format->no_of_grouprecords);


	// Get the records
	int offset = 8; // Size of the header of the report
	unsigned char* recordPointer = reinterpret_cast<unsigned char*>(format);
	for(int i = 0;i < this->no_of_grouprecords;i++){
		igmpv3grouprecord* record = (igmpv3grouprecord*)(recordPointer+offset);
		grouprecord* r = new grouprecord();

		int no_of_sources = ntohs(record->no_of_sources);
		r->type = record->type;
		r->multicast_address = IPAddress(record->multicast_address);

		for(int j = 0;j < no_of_sources;j++){
			r->sources.push_back(IPAddress(record->sources[j]));
		}

		this->records.push_back(r);

		offset += sizeof(igmpv3grouprecord) + no_of_sources * sizeof(in_addr);
	}

	return 0;
}

void ProcessReport::print(){
	click_chatter("Type %x", this->type);
	click_chatter("Checksum %x", this->checksum);
	click_chatter("Number of Group Records %i :", this->no_of_grouprecords);


	for(int i = 0;i < this->records.size();i++){
		click_chatter("    Record %x", this->records.at(i)->type);
		click_chatter("    - Multicast Address %s", this->records.at(i)->multicast_address.s().c_str());
		click_chatter("    - Number Of Sources %i :", this->records.at(i)->sources.size());
		for(int j = 0;j < this->records.at(i)->sources.size();j++){
			click_chatter("    - %s", this->records.at(i)->sources.at(j).s().c_str());
		}
	}

	if(this->verifiedChecksum){
		click_chatter("Checksum correct");
	}else{
		click_chatter("Wrong checksum!!");
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(ProcessReport)
