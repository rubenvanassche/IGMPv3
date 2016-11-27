#ifndef CLICK_ROUTERELEMENT_HH
#define CLICK_ROUTERELEMENT_HH
#include <click/element.hh>
CLICK_DECLS

class RouterElement : public Element { 
	public:
		RouterElement();
		~RouterElement();
		
		const char *class_name() const	{ return "RouterElement"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);
	private:

		uint32_t maxSize;
		bool client1listening;
		bool client2listening;
};

CLICK_ENDDECLS
#endif
