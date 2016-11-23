#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "queryigmp.hh"

CLICK_DECLS

QueryIGMPElement::QueryIGMPElement(){}
~QueryIGMPElement::QueryIGMPElement(){}

int QueryIGMPElement::configure(Vector<String> &conf, ErrorHandler *errh) {
    if(cp_va_kparse(conf, this, errh, "MAXPACKETSIZE", cpkM, cpInteger, &maxSize, cpEnd) < 0){
        return −1;
    }
    if(maxSize <= 0){
        return errh−>error("maxsize should be larger than 0");
    }
    return 0;
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
    /**
    click_chatter("Got a packet of size %d", p−>length());
    if(p−>length() > maxSize){
        p−>kill();
    }else{
        output(0).push(p);
    }
    **/
    IPaddress group = IPAddress("232.2.2.2");

    int tailroom = 0;
    int packetsize = sizeof(igmpv3query);
    int headroom = 0;
    WritablePacket *packet = Packet::make(headroom,0,packetsize, tailroom);
    if(packet == 0 ){
        return click_chatter( "cannot make igmpv3query packet!");
    }
    memset(packet−>data(), 0, packet−>length());
    igmpv3query* format=(igmpv3query*)packet−>data();
    // Type for IGMP Query Packet
    format>type = 0x11;
    // TODO: the responsecode may vary, the value 1 is ok
    format−>max_response_code = 1;
    // The address of the multicast group
    format−>group = group;
    // TODO: querier robustness value (qrv) instructs the host to send all messages qrv times
    format->resv_and_s_and_qrv = 0x03;
    // TODO: this
    format->qqic = 0x00;

    format->checksum = 0x0000;




}

CLICK_ENDDECLS EXPORT_ELEMENT(QueryIGMPElement)
