#ifndef CLICK_IGMPClient_HH
#define CLICK_IGMPClient_HH
#include <click/element.hh>
#include <click/vector.cc>
#include <click/args.hh>
#include <assert.h>
#include "packets.hh"
#include "reportigmp.hh"
#include "IGMPClientDB.hh"

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
#endif

CLICK_DECLS

class IGMPClient : public Element {
	public:
		IGMPClient();
		~IGMPClient();

		int configure(Vector<String>&, ErrorHandler*);

		void run_timer(Timer*);

		const char *class_name() const { return "IGMPClient"; }
		const char *port_count() const { return "0/1"; }
		const char *processing() const { return PUSH; }

		void push(Packet *p);

		// Handlers
		// Use: M {IPAdresss}, S {IPADRESS}
		// Use: M {IPAdresss}, S {IPADRESS}+{IPADRESS}
		static int includeSourcesHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh);
		static int excludeSourcesHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh);
		void add_handlers();
	private:
		Timer timer;
		ReportIGMPElement reporter;
		IGMPClientDB db;
};

class ConfigParse{
public:
    Vector<IPAddress> sources(const String &conf, ErrorHandler * errh);
    IPAddress multicastAddress(const String &conf, ErrorHandler * errh);
};

CLICK_ENDDECLS

#endif
