#ifndef CLICK_IPMulticastListen_HH
#define CLICK_IPMulticastListen_HH
#include <click/element.hh>
CLICK_DECLS

class IPMulticastListen : public Element { 
	public:
		IPMulticastListen();
		~IPMulticastListen();
		
		const char *class_name() const	{ return "IPMulticastListen"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:
		uint32_t maxSize;
};

CLICK_ENDDECLS
#endif
