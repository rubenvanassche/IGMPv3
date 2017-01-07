#include <click/config.h>
// #include <click/TODO.hh>
#include "IGMPRouter.hh"

CLICK_DECLS

IGMPRouter::IGMPRouter() { };
IGMPRouter::~IGMPRouter() { };

Packet *IGMPRouter::simple_action(Packet *p) {
	// TODO: fill

	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouter)
