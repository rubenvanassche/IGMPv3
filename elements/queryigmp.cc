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
    return this->generatePacket(zero);
}

Packet* QueryIGMPElement::groupQuery(IPAddress multicast_address){
    return this->generatePacket(multicast_address);
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

    igmpv3query* format = (igmpv3query*)packet->data();

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
