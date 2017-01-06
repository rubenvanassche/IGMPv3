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
    const char *port_count() const { return "0/1"; }
    const char *processing() const { return AGNOSTIC; }

	void isINCLUDE();
	void isEXCLUDE();

	void toEX(IPAddress multicast_address, Vector<IPAddress>& sources, int robustnessVariable);
	void toIN(IPAddress multicast_address, Vector<IPAddress>& sources, int robustnessVariable);

	grouprecord* generateRecord(uint8_t type, IPAddress multicast_address, Vector<IPAddress>& sources);

	Packet* generatePacket(Vector<grouprecord*> records);

    Packet* simple_action(int, Packet*);
private:
	// Calculate the amount of bytes a single group record needs
	int calculateGrouprecordSize(grouprecord* record);

    uint32_t maxSize;
	HashTable<grouprecord*, int> recordsTable;
};
CLICK_ENDDECLS

#endif
