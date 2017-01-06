#ifndef CLICK_IGMPDB_HH
#define CLICK_IGMPDB_HH

#include <click/element.hh>

CLICK_DECLS

/*
=c

IGMPDB(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class IGMPDB : public Element {
	//TODO: Add private attributes

	public:
		IGMPDB();
		~IGMPDB();

		const char *class_name() const { return "IGMPDB"; }
		const char *port_count() const { return "0/0"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);
};

CLICK_ENDDECLS

#endif
