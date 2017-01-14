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

Packet* ReportIGMPElement::isINCLUDE(IPAddress multicast_address){
    Vector<IPAddress> sources;
    grouprecord* record = this->generateRecord(0x01, multicast_address, sources);

    Vector<grouprecord*> records;
    records.push_back(record);

    return this->generatePacket(records);
}

Packet* ReportIGMPElement::isEXCLUDE(IPAddress multicast_address){
    Vector<IPAddress> sources;
    grouprecord* record = this->generateRecord(0x02, multicast_address, sources);

    Vector<grouprecord*> records;
    records.push_back(record);

    return this->generatePacket(records);
}

Packet* ReportIGMPElement::isINCLUDEOrEXCLUDE(HashTable<IPAddress, filtermode> multicast_addresses){
    Vector<IPAddress> sources;
    Vector<grouprecord*> records;
    HashTable<IPAddress, filtermode>::iterator it;

    for(it = multicast_addresses.begin();it != multicast_addresses.end();it++){
        grouprecord* record = 0;

        if(it->second == INCLUDE){
            record = this->generateRecord(0x01, it->first, sources);
        }else{
            record = this->generateRecord(0x02, it->first, sources);
        }

        records.push_back(record);
    }

    return this->generatePacket(records);
}


Packet* ReportIGMPElement::toIN(IPAddress multicast_address, Vector<IPAddress>& sources){
    grouprecord* record = this->generateRecord(0x03, multicast_address, sources);

    Vector<grouprecord*> records;
    records.push_back(record);

    return this->generatePacket(records);
}

Packet* ReportIGMPElement::toEX(IPAddress  multicast_address, Vector<IPAddress>& sources){
    grouprecord* record = this->generateRecord(0x04, multicast_address, sources);

    Vector<grouprecord*> records;
    records.push_back(record);

    return this->generatePacket(records);
}

Packet* ReportIGMPElement::allow(IPAddress multicast_address, Vector<IPAddress>& sources){
    grouprecord* record = this->generateRecord(0x05, multicast_address, sources);

    Vector<grouprecord*> records;
    records.push_back(record);

    return this->generatePacket(records);
}

Packet* ReportIGMPElement::block(IPAddress multicast_address, Vector<IPAddress>& sources){
    grouprecord* record = this->generateRecord(0x06, multicast_address, sources);

    Vector<grouprecord*> records;
    records.push_back(record);

    return this->generatePacket(records);
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

Packet* ReportIGMPElement::simple_action(int, Packet *r){

}


CLICK_ENDDECLS
EXPORT_ELEMENT(ReportIGMPElement)
