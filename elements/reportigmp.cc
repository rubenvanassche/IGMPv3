#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include "reportigmp.hh"

CLICK_DECLS

ReportIGMPElement::ReportIGMPElement(): timer(this){}
ReportIGMPElement::~ReportIGMPElement(){}

int ReportIGMPElement::configure(Vector<String> &conf, ErrorHandler *errh) {
    // init timer
    timer.initialize(this);
    timer.schedule_after_msec(1000);
    return 0;
}

void ReportIGMPElement::run_timer(Timer* t){
    click_chatter("we are now 1 second later");
    timer.schedule_after_msec(1000);
    this->push(0, nullptr);
}

int ReportIGMPElement::calculateGrouprecordsSize(){
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
void ReportIGMPElement::push(int, Packet *r){
    int packetsize = sizeof(igmpv3report);
    int sourcesSize = this->calculateGrouprecordsSize(); // Allocate space for IP Adresses

    WritablePacket *packet = Packet::make(0,0,packetsize + sourcesSize, 0);
    if(packet == 0 ){
        return click_chatter( "cannot make igmpv3report packet!");
    }
    memset(packet->data(), 0, packet->length());

    igmpv3report* format = (igmpv3report*)packet->data();

    // Type for IGMP Report Packet
    format->type = 0x22;
    // Set reserved space to null
    format->reserved = 0x00;
    // Checksum will be calculated later
    format->checksum = 0x0000;
    // Set reserved long to null
    format->reserved_long = 0x00;
    // The amount of group records
    format->no_of_grouprecords = htons(grouprecordVector.size());
    // Set the sources
    for(int i = 0;i < grouprecordVector.size();i++){
        //format->sources[i] = grouprecordVector.at(i).addr();
    }

    output(0).push(packet);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(ReportIGMPElement)
