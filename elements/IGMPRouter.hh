#ifndef CLICK_IGMPROUTER_HH
#define CLICK_IGMPROUTER_HH

#include <click/element.hh>

CLICK_DECLS

/*
=c

IGMPRouter(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class IGMPRouter : public Element {
	//TODO: Add private attributes

	public:
		IGMPRouter();
		~IGMPRouter();

		const char *class_name() const { return "IGMPRouter"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);
};

CLICK_ENDDECLS

#endif
