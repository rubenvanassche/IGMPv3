#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "reportigmp.hh"

CLICK_DECLS

ReportIGMPElement::ReportIGMPElement(){}
ReportIGMPElement::~ReportIGMPElement(){}

grouprecord* ReportIGMPElement::generateRecord(uint8_t type, IPAddress multicast_address, Vector<IPAddress>& sources){
   grouprecord* record = new grouprecord;
   record->type = type;
   record->multicast_address = multicast_address;
   record->sources = sources;

   return record;
}

void ReportIGMPElement::toEX(IPAddress  multicast_address, Vector<IPAddress>& sources, int robustnessVariable = 2){
    grouprecord* record = this->generateRecord(0x04, multicast_address, sources);

    this->recordsTable.set(record, robustnessVariable);
}


void ReportIGMPElement::toIN(IPAddress multicast_address, Vector<IPAddress>& sources, int robustnessVariable = 2){
    grouprecord* record = this->generateRecord(0x03, multicast_address, sources);

    this->recordsTable.set(record, robustnessVariable);
}


int ReportIGMPElement::calculateGrouprecordSize(grouprecord* record){
    int totalSize = 0;

    totalSize += sizeof(igmpv3grouprecord);
    totalSize += record->sources.size() * sizeof(in_addr);

    return totalSize;
}

Packet* ReportIGMPElement::generatePacket(Vector<grouprecord*> records){
    int packetsize = sizeof(igmpv3report);
    int sourcesSize = 0; // Allocate space for IP Adresses

    for(int i = 0;i < records.size();i++){
        sourcesSize += this->calculateGrouprecordSize(records.at(i));
    }

    WritablePacket *packet = Packet::make(0,0,packetsize + sourcesSize, 0);
    if(packet == 0){
        click_chatter( "cannot make igmpv3report packet!");

        return nullptr;
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
    format->no_of_grouprecords = htons(records.size());

    // Set the records
    int offset = 8; // Size of the header of the report
    unsigned char* recordPointer = reinterpret_cast<unsigned char*>(format);

    for(int i = 0;i < records.size();i++){
        igmpv3grouprecord* record = (igmpv3grouprecord*)(recordPointer+offset);
        grouprecord* r = records.at(i);

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
Packet* ReportIGMPElement::simple_action(int, Packet *r){
    HashTable<grouprecord*, int>::iterator it;
    Vector<grouprecord*> recordsToGenerate;
    Vector<grouprecord*> recordsToRemove;
    Vector<grouprecord*>::iterator it2;
    for(it = this->recordsTable.begin();it != this->recordsTable.end();it++){
        // Remove record if robustness variable is 0
        if(it->second == 0){
            recordsToRemove.push_back(it->first);
            continue;
        }

        recordsToGenerate.push_back(it->first);
        it->second -= 1;
    }

    // Remove records
    for(it2 = recordsToRemove.begin();it2 != recordsToRemove.end();it2++){
        this->recordsTable.erase(*it2);
    }

    // check if there are reports
    if(this->recordsTable.size() == 0){
        return nullptr;
    }


    return this->generatePacket(recordsToGenerate);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(ReportIGMPElement)
