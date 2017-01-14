#ifndef CLICK_PROCESSQUERY_HH
#define CLICK_PROCESSQUERY_HH

#include <click/element.hh>
#include <click/vector.cc>
#include "packets.hh"

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
#endif

CLICK_DECLS

/*
=c

ProcessQuery(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class ProcessQuery : public Element {
	//TODO: Add private attributes

	public:
		ProcessQuery();
		~ProcessQuery();

		const char *class_name() const { return "ProcessQuery"; }
		const char *port_count() const { return "0/0"; }
		const char *processing() const { return AGNOSTIC; }

		bool verifyChecksum(Packet* p);
		// Expects an IP Header and no Ethernet header
		int process(Packet* packet);
		void print();

		Packet *simple_action(Packet *p);

		// Variables
		uint8_t type;
		uint8_t max_response_code;
		uint16_t checksum;
		IPAddress group_address;
		uint8_t resv_and_s_and_qrv;
		uint8_t qqic;
		Vector<IPAddress> sources;
		bool verifiedChecksum;
};

CLICK_ENDDECLS

#endif
