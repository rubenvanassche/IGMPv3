#include <click/config.h>
// #include <click/TODO.hh>
#include "IGMPDB.hh"

CLICK_DECLS

IGMPDB::IGMPDB() { };
IGMPDB::~IGMPDB() { };

Packet *IGMPDB::simple_action(Packet *p) {
	// TODO: fill

	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPDB)
