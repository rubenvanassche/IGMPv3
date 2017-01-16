#ifndef CLICK_SETIPSRC_HH
#define CLICK_SETIPSRC_HH

#include <click/element.hh>

CLICK_DECLS

/*
=c

SetIPSrc(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class SetIPSrc : public Element {
	//TODO: Add private attributes

	public:
		SetIPSrc();
		~SetIPSrc();

		int configure(Vector<String>&, ErrorHandler*);

		const char *class_name() const { return "SetIPSrc"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return PUSH; }

		void push(int, Packet *p);
	private:
		IPAddress ipAddress;
};

CLICK_ENDDECLS

#endif
