#ifndef CLICK___HH
#define CLICK___HH

#include <click/element.hh>

CLICK_DECLS

/*
=c

IGMPRouterDB(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class IGMPRouterDB : public Element {
	//TODO: Add private attributes

	public:
		IGMPRouterDB();
		~IGMPRouterDB();

		const char *class_name() const { return "IGMPRouterDB"; }
		const char *port_count() const { return "0/0"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);
};

CLICK_ENDDECLS

#endif
