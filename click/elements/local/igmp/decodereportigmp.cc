#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "decodereportigmp.hh"

CLICK_DECLS
DecodeReportIGMP::DecodeReportIGMP()
{}

DecodeReportIGMP::~DecodeReportIGMP()
{}

int DecodeReportIGMP::configure(Vector<String> &conf, ErrorHandler *errh) {
	return 0;
}

bool DecodeReportIGMP::verifyChecksum(Packet* p){
	WritablePacket *q = p->uniqueify();
	igmpv3report* format = (igmpv3report*)q->data();
	uint16_t checksum = 0x00;

	checksum = format->checksum;
	format->checksum = 0x00;

	return (click_in_cksum((unsigned char *)format, q->length()) == checksum);
}

void DecodeReportIGMP::push(int, Packet *packet){
	uint8_t type;
    uint16_t checksum;
    int no_of_grouprecords;
    Vector<grouprecord*> records;
	bool verifiedChecksum;

	igmpv3report* format = (igmpv3report*)packet->data();

	type = format->type;
	checksum = format->checksum;
	no_of_grouprecords = ntohs(format->no_of_grouprecords);

	verifiedChecksum = this->verifyChecksum(packet);

	// Get the records
	int offset = 8; // Size of the header of the report
	unsigned char* recordPointer = reinterpret_cast<unsigned char*>(format);
	for(int i = 0;i < no_of_grouprecords;i++){
		igmpv3grouprecord* record = (igmpv3grouprecord*)(recordPointer+offset);
		grouprecord* r = new grouprecord();

		int no_of_sources = ntohs(record->no_of_sources);
		r->type = record->type;
		r->multicast_address = IPAddress(record->multicast_address);

		for(int j = 0;j < no_of_sources;j++){
			r->sources.push_back(IPAddress(record->sources[j]));
		}

		records.push_back(r);

		offset += sizeof(igmpv3grouprecord) + no_of_sources * sizeof(in_addr);
	}


	click_chatter("Type %x", type);
	click_chatter("Checksum %x", checksum);
	click_chatter("Number of Group Records %i :", no_of_grouprecords);


	for(int i = 0;i < records.size();i++){
		click_chatter("    Record %x", records.at(i)->type);
		click_chatter("    - Multicast Address %s", records.at(i)->multicast_address.s().c_str());
		click_chatter("    - Number Of Sources %i :", records.at(i)->sources.size());
		for(int j = 0;j < records.at(i)->sources.size();j++){
			click_chatter("    - %s", records.at(i)->sources.at(j).s().c_str());
		}
	}

	if(verifiedChecksum){
		click_chatter("Checksum correct");
	}else{
		click_chatter("Wrong checksum!!");
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(DecodeReportIGMP)
