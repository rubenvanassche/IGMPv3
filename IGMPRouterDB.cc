#include <click/config.h>
// #include <click/TODO.hh>
#include "IGMPRouterDB.hh"
#include "DummyProto.hh"

CLICK_DECLS

IGMPRouterDB::IGMPRouterDB() { };
IGMPRouterDB::~IGMPRouterDB() { };

Packet *IGMPRouterDB::simple_action(Packet *p) {
	// TODO: fill

	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouterDB)
