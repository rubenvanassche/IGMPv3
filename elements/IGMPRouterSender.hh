#ifndef CLICK_IGMPROUTERSENDER_HH
#define CLICK_IGMPROUTERSENDER_HH

#include <click/element.hh>
#include "IGMPRouterDB.hh"

CLICK_DECLS

/*
=c

Gets as configuration a subnet + IGMPRouterDB and as input multicast packets, will check if a packet needs to be send to this subnet.
If so it will send the packet through, otherwise it will kill it.

IGMPRouterSender(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class IGMPRouterSender : public Element {
	//TODO: Add private attributes

	public:
		IGMPRouterSender();
		~IGMPRouterSender();

		int configure(Vector<String>&, ErrorHandler*);

		const char *class_name() const { return "IGMPRouterSender"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return PUSH; }

		void push(int port, Packet *p);
	private:
		IGMPRouterDB* db;
		IPAddress subnetAddress;
		IPAddress subnetMask;
};

CLICK_ENDDECLS

#endif
