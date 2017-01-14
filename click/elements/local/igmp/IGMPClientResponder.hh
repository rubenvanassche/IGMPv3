#ifndef CLICK_IGMPCLIENTRESPONDER_HH
#define CLICK_IGMPCLIENTRESPONDER_HH
#include <click/element.hh>
#include <click/timer.hh>
#include "IGMPClientDB.hh"

CLICK_DECLS

/*
=c

IGMPClientResponder(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class IGMPClientResponder : public Element {
	//TODO: Add private attributes

	public:
		IGMPClientResponder();
		~IGMPClientResponder();

		int configure(Vector<String>&, ErrorHandler*);

		void run_timer(Timer*);

		const char *class_name() const { return "IGMPClientResponder"; }
		const char *port_count() const { return "1/0"; }
		const char *processing() const { return PUSH; }

		void push(int port, Packet* p);

	private:
		Timer timer;
		IGMPClientDB* db;
		IPAddress ipAddress;
};

CLICK_ENDDECLS

#endif
