#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "decodequeryigmp.hh"

CLICK_DECLS
DecodeQueryIGMP::DecodeQueryIGMP()
{}

DecodeQueryIGMP::~DecodeQueryIGMP()
{}

int DecodeQueryIGMP::configure(Vector<String> &conf, ErrorHandler *errh) {
	return 0;
}

bool DecodeQueryIGMP::verifyChecksum(Packet* p){
	WritablePacket *q = p->uniqueify();
	igmpv3query* format = (igmpv3query*)q->data();
	uint16_t checksum = 0x00;

	checksum = format->checksum;
	format->checksum = 0x00;

	return (click_in_cksum((unsigned char *)format, q->length()) == checksum);
}

void DecodeQueryIGMP::push(int, Packet *packet){
	uint8_t type;
    uint8_t max_response_code;
    uint16_t checksum;
    IPAddress group_address;
    uint8_t resv_and_s_and_qrv;
    uint8_t qqic;
    int no_of_sources;
    Vector<IPAddress> sources;
	bool verifiedChecksum;

	igmpv3query* format = (igmpv3query*)packet->data();

	type = format->type;
	max_response_code = format->max_response_code;
	checksum = format->checksum;
	group_address = IPAddress(format->group_address);
	resv_and_s_and_qrv = format->resv_and_s_and_qrv;
	qqic = format->qqic;
	no_of_sources = ntohs(format->no_of_sources);

	verifiedChecksum = this->verifyChecksum(packet);

	for(int i = 0;i < no_of_sources;i++){
		sources.push_back(IPAddress(format->sources[i]));
	}

	click_chatter("Type %x", type);
	click_chatter("Max Response Code %x", max_response_code);
	click_chatter("Checksum %x", checksum);
	click_chatter("Group Addres %s", group_address.s().c_str());
	click_chatter("Reserved and S and QRV %x", resv_and_s_and_qrv);
	click_chatter("Number of Sources %i :", no_of_sources);
	for(int i = 0;i < no_of_sources;i++){
		click_chatter("%s", sources.at(i).s().c_str());
	}
	
	if(verifiedChecksum){
		click_chatter("Checksum correct");
	}else{
		click_chatter("Wrong checksum!!");
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(DecodeQueryIGMP)
