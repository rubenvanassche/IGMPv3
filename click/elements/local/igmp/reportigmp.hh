#ifndef CLICK_REPORTIGMPELEMENT_HH
#define CLICK_REPORTIGMPELEMENT_HH
#include <click/element.hh>
#include <click/vector.cc>
#include "packets.hh"

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
#endif
CLICK_DECLS

// High order grouprecord
struct grouprecord{
	uint8_t type;
	IPAddress multicast_address;
	Vector<IPAddress> sources;
};

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
    const char *processing() const { return PUSH; }

    int configure(Vector<String>&, ErrorHandler*);

	void run_timer(Timer*);

	grouprecord* generateRecord(uint8_t type, IPAddress multicast_address, Vector<IPAddress>& sources);

    void push(int, Packet*);
private:
	// Calculate the amount of bytes the group records array needs
	int calculateGrouprecordsSize();

	// Calculate the amount of bytes a single group record needs
	int calculateGrouprecordSize(grouprecord* record);

    uint32_t maxSize;
	Vector<grouprecord*> grouprecordVector;
	Timer timer;
};
CLICK_ENDDECLS

#endif
