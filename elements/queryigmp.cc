#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include "queryigmp.hh"

CLICK_DECLS

QueryIGMPElement::QueryIGMPElement(): timer(this){}
QueryIGMPElement::~QueryIGMPElement(){}

int QueryIGMPElement::configure(Vector<String> &conf, ErrorHandler *errh) {
    // Let's add some IP addressess hard coded(MUST BE REMOVED)
    sourcesVector.push_back(IPAddress("192.168.1.1"));
    sourcesVector.push_back(IPAddress("192.168.1.2"));

    // init timer
    timer.initialize(this);
    timer.schedule_after_msec(1000);
    return 0;
}

void QueryIGMPElement::run_timer(Timer* t){
    click_chatter("we are now 1 second later");
    timer.schedule_after_msec(1000);
    this->push(0, nullptr);
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
    IPAddress group = IPAddress("0.0.0.0");

    int packetsize = sizeof(igmpv3query);
    int sourcesSize = sourcesVector.size() * sizeof(in_addr); // Allocate space for IP Adresses

    WritablePacket *packet = Packet::make(0,0,packetsize + sourcesSize, 0);
    if(packet == 0 ){
        return click_chatter( "cannot make igmpv3query packet!");
    }
    memset(packet->data(), 0, packet->length());

    igmpv3query* format = (igmpv3query*)packet->data();

    // Type for IGMP Query Packet
    format->type = 0x11;
    // TODO: the responsecode may vary, the value 1 is ok
    format->max_response_code = 0x18;
    // Checksum will be calculated later
    format->checksum = 0xd3ec;
    // The address of the multicast group
    format->group_address = group.addr();
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

    output(0).push(packet);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(QueryIGMPElement)
