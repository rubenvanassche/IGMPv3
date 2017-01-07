#ifndef CLICK_PROCESSREPORT_HH
#define CLICK_PROCESSREPORT_HH
#include <click/element.hh>
#include <click/vector.cc>
#include "packets.hh"

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
#endif
CLICK_DECLS

/*
=c

ProcessReport(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class ProcessReport : public Element {
	//TODO: Add private attributes

	public:
		ProcessReport();
		~ProcessReport();

		const char *class_name() const { return "ProcessReport"; }
		const char *port_count() const { return "0/0"; }
		const char *processing() const { return AGNOSTIC; }

		bool verifyChecksum(Packet* p);
		// Expects an IP Header and no Ethernet header
		int process(Packet* packet);
		void print();

		Packet *simple_action(Packet *p);

		// Variables
		uint8_t type;
		uint16_t checksum;
		int no_of_grouprecords;
		Vector<grouprecord*> records;
		bool verifiedChecksum;
};

CLICK_ENDDECLS

#endif
