#ifndef CLICK_IGMPCLASSIFIER_HH
#define CLICK_IGMPCLASSIFIER_HH

#include <click/element.hh>
#include "IGMPClientDB.hh"

CLICK_DECLS

/*
=c
Get's IP Packets and checks if they are for this multicast client(output 1) or not(output 0).
Or if they are queries(output 3)

IGMPClassifier(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class IGMPClassifier : public Element {
	//TODO: Add private attributes

	public:
		IGMPClassifier();
		~IGMPClassifier();

		int configure(Vector<String>&, ErrorHandler*);

		const char *class_name() const { return "IGMPClassifier"; }
		const char *port_count() const { return "1/3"; }
		const char *processing() const { return PUSH; }

		Packet * simple_action(Packet *p);
	private:
		IGMPClientDB* db;
};

CLICK_ENDDECLS

#endif
