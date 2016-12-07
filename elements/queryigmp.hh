#ifndef CLICK_QUERYIGMPELEMENT_HH
#define CLICK_QUERYIGMPELEMENT_HH
#include <click/element.hh>
#include <click/vector.cc>
#include "packets.hh"

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
#endif
CLICK_DECLS

/**
 * Send an IGMP Query over the network on the output port
 * IP Addresses are stored in the config array
 */

class QueryIGMPElement : public Element{
public:
	QueryIGMPElement();
    ~QueryIGMPElement();

    const char *class_name() const { return "QueryIGMPElement"; }
    const char *port_count() const { return "0/1"; }
    const char *processing() const { return PUSH; }

    int configure(Vector<String>&, ErrorHandler*);

	void run_timer(Timer*);

    void push(int, Packet*);
private:
    uint32_t maxSize;
	Vector<IPAddress> sourcesVector;
	Timer timer;
};
CLICK_ENDDECLS

#endif
