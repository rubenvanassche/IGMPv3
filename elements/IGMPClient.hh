#ifndef CLICK___HH
#define CLICK___HH
#include <click/element.hh>
#include <click/vector.cc>
#include <click/args.hh>
#include "packets.hh"

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
#endif

CLICK_DECLS

class IGMPClient : public Element {
	public:
		IGMPClient();
		~IGMPClient();

		const char *class_name() const { return "IGMPClient"; }
		const char *port_count() const { return "0/1"; }
		const char *processing() const { return PUSH; }

		Packet *simple_action(Packet *p);

		// Get the interface record with specified multicast Address
		interfacerecord* getInterfacerecord(IPAddress multicast_address);

		// Change interface records
		void includeSources(IPAddress multicast_address, Vector<IPAddress> sources);
		void excludeSources(IPAddress multicast_address, Vector<IPAddress> sources);

		// Handlers
		// Use: M {IPAdresss}, S {IPADRESS}
		// Use: M {IPAdresss}, S {IPADRESS}+{IPADRESS}
		static int includeSourcesHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh);
		static int excludeSources(const String &conf, Element *e, void * thunk, ErrorHandler * errh);
		void add_handlers();
	private:
		Vector<interfacerecord*> interfacerecords;
};

class ConfigParse{
public:
    Vector<IPAddress> sources(const String &conf, ErrorHandler * errh);
    IPAddress multicastAddress(const String &conf, ErrorHandler * errh);
};

CLICK_ENDDECLS

#endif
