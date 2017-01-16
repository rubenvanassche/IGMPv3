#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include "queryigmp.hh"

CLICK_DECLS

QueryIGMPElement::QueryIGMPElement(){}
QueryIGMPElement::~QueryIGMPElement(){}


Packet* QueryIGMPElement::generalQuery(){
    IPAddress zero("0.0.0.0");
    Packet* p =  this->generatePacket(zero);

    p = this->setIPHeader(p, IPAddress("224.0.0.1"));
    return p;
}

Packet* QueryIGMPElement::groupQuery(IPAddress multicast_address){
    Packet* p =  this->generatePacket(multicast_address);

    p =  this->setIPHeader(p,  multicast_address);
    return p;
}

// Doesn't do a checksum and source setting
Packet* QueryIGMPElement::setIPHeader(Packet* p, IPAddress destination){
    click_ip ipheader;

	memset(&ipheader, 0, sizeof(click_ip));

	ipheader.ip_v = 4;
	ipheader.ip_hl = sizeof(click_ip) >> 2;
	ipheader.ip_ttl = 1;
	ipheader.ip_p = 2;
	ipheader.ip_dst = destination.in_addr();
	ipheader.ip_src = IPAddress("0.0.0.0").in_addr();
	ipheader.ip_sum = click_in_cksum((unsigned char *) &ipheader, sizeof(click_ip));

    WritablePacket *ipPacket = p->push(sizeof(click_ip));

    click_ip *ip = reinterpret_cast<click_ip *>(ipPacket->data());
    memcpy(ip, &ipheader, sizeof(click_ip));


    ip->ip_len = htons(ipPacket->length());
    ip->ip_id = htons(1);


    ipPacket->set_ip_header(ip, sizeof(click_ip));

    return ipPacket;

    }

Packet* QueryIGMPElement::generatePacket(IPAddress multicast_address){
    Vector<IPAddress> sourcesVector; // Empty for now

    int packetsize = sizeof(igmpv3query);
    int sourcesSize = sourcesVector.size() * sizeof(in_addr); // Allocate space for IP Adresses

    WritablePacket *packet = Packet::make(0,0,packetsize + sourcesSize, 0);
    if(packet == 0 ){
        click_chatter( "cannot make igmpv3query packet!");
        return NULL;
    }
    memset(packet->data(), 0, packet->length());

    unsigned char * igmpbegin = (unsigned char *)packet->data();
    igmpv3query* format = (igmpv3query*)igmpbegin;

    // Type for IGMP Query Packet
    format->type = 0x11;
    // TODO: the responsecode may vary, the value 1 is ok
    format->max_response_code = 0x18;
    // Checksum will be calculated later
    format->checksum = 0x0000;
    // The address of the multicast group
    format->group_address = multicast_address.addr();
    // TODO: querier robustness value (qrv) instructs the host to send all messages qrv times
    format->resv_and_s_and_qrv = 0x02;
    // TODO: this
    format->qqic = 0x14;
    // The amount of sources
    format->no_of_sources = htons(sourcesVector.size());
    // Set the sources
    for(int i = 0;i < sourcesVector.size();i++){
        format->sources[i] = sourcesVector.at(i).addr();
    }

    // Set the checksum
    format->checksum = click_in_cksum((unsigned char *)format, packet->length());

    return packet;
}

/**
 * [QueryIGMPElement::push description]
 * @param int
 * @param r
 *
 * Generates an IGMP Query Message, should be wrapped in an IP package with destination 224.0.0.1 or 224.0.0.22(see RFC)
 *
 */
void QueryIGMPElement::push(int, Packet *r){

}


CLICK_ENDDECLS
EXPORT_ELEMENT(QueryIGMPElement)
