#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include "reportigmp.hh"

CLICK_DECLS

ReportIGMPElement::ReportIGMPElement(): timer(this){}
ReportIGMPElement::~ReportIGMPElement(){}

int ReportIGMPElement::configure(Vector<String> &conf, ErrorHandler *errh) {
    // Generate some basic reports for testing
    Vector<IPAddress> addresses1;
    addresses1.push_back(IPAddress("86.78.34.24"));
    addresses1.push_back(IPAddress("102.78.34.24"));
    addresses1.push_back(IPAddress("105.78.34.24"));

    Vector<IPAddress> addresses2;
    addresses2.push_back(IPAddress("86.78.34.24"));
    addresses2.push_back(IPAddress("102.78.34.24"));

    Vector<IPAddress> addresses3;
    addresses3.push_back(IPAddress("86.78.34.24"));

    grouprecord* r1 = this->generateRecord(0x02, IPAddress("192.168.1.1"), addresses1);
    grouprecord* r2 = this->generateRecord(0x02, IPAddress("192.168.1.6"), addresses2);
    grouprecord* r3 = this->generateRecord(0x02, IPAddress("192.168.1.10"), addresses3);
    this->grouprecordVector.push_back(r1);
    this->grouprecordVector.push_back(r2);
    this->grouprecordVector.push_back(r3);

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

grouprecord* ReportIGMPElement::generateRecord(uint8_t type, IPAddress multicast_address, Vector<IPAddress>& sources){
   grouprecord* record = new grouprecord;
   record->type = type;
   record->multicast_address = multicast_address;
   record->sources = sources;

   return record;
}


int ReportIGMPElement::calculateGrouprecordsSize(){
    int totalSize = 0;

    for(int i = 0;i < this->grouprecordVector.size();i++){
        totalSize += this->calculateGrouprecordSize(this->grouprecordVector.at(i));
    }

    return totalSize;
}

int ReportIGMPElement::calculateGrouprecordSize(grouprecord* record){
    int totalSize = 0;

    totalSize += sizeof(igmpv3grouprecord);
    totalSize += record->sources.size() * sizeof(in_addr);

    return totalSize;
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

    // Set the records
    int offset = 8; // Size of the header of the report
    unsigned char* recordPointer = reinterpret_cast<unsigned char*>(format);

    for(int i = 0;i < grouprecordVector.size();i++){
        igmpv3grouprecord* record = (igmpv3grouprecord*)(recordPointer+offset);
        grouprecord* r = grouprecordVector.at(i);

        record->type = r->type;
        record->aux_data_len = 0x00;
        record->no_of_sources = htons(r->sources.size());
        record->multicast_address = r->multicast_address.addr();

        for(int j = 0;j < r->sources.size();j++){
            record->sources[j] = r->sources.at(j).addr();
        }

        // Now calculate the offset
        offset += this->calculateGrouprecordSize(r); // offset for this record
    }

    // Let's calculate the checksum
    format->checksum = click_in_cksum((unsigned char *)format, packet->length());

    output(0).push(packet);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(ReportIGMPElement)
