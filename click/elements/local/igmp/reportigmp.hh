#ifndef CLICK_REPORTIGMPELEMENT_HH
#define CLICK_REPORTIGMPELEMENT_HH
#include <click/element.hh>
#include <click/vector.cc>
#include <click/hashtable.hh>
#include "packets.hh"

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
template class HashTable<SomeThing, OtherThing>;
#endif
CLICK_DECLS


/**
 * Send an IGMP Report over the network on the output port
 * IP Addresses are stored in the config array
 */

class ReportIGMPElement : public Element{
public:
	ReportIGMPElement();
    ~ReportIGMPElement();

    const char *class_name() const { return "ReportIGMPElement"; }
    const char *port_count() const { return "0/0"; }
    const char *processing() const { return AGNOSTIC; }

	Packet* isINCLUDE(IPAddress multicast_address);
	Packet* isEXCLUDE(IPAddress multicast_address);

	Packet* isINCLUDEOrEXCLUDE(HashTable<IPAddress, filtermode> multicast_addresses);

	Packet* toEX(IPAddress multicast_address, Vector<IPAddress>& sources);
	Packet* toIN(IPAddress multicast_address, Vector<IPAddress>& sources);

	Packet* block(IPAddress multicast_address, Vector<IPAddress>& sources);
	Packet* allow(IPAddress multicast_address, Vector<IPAddress>& sources);

	grouprecord* generateRecord(uint8_t type, IPAddress multicast_address, Vector<IPAddress>& sources);

	Packet* generatePacket(Vector<grouprecord*> records);

    Packet* simple_action(int, Packet*);
private:
	// Calculate the amount of bytes a single group record needs
	int calculateGrouprecordSize(grouprecord* record);
};
CLICK_ENDDECLS

#endif
