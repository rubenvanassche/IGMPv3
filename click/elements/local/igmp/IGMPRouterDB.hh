#ifndef CLICK_IGMPROUTERDB_HH
#define CLICK_IGMPROUTERDB_HH
#include <click/element.hh>
#include <click/hashtable.hh>
#include "IGMPClientDB.hh"

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
template class HashTable<SomeThing, OtherThing>;
#endif
CLICK_DECLS

/*
=c

IGMPRouterDB(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class IGMPRouterDB : public Element {
	//TODO: Add private attributes

	public:
		IGMPRouterDB();
		~IGMPRouterDB();

		const char *class_name() const { return "IGMPRouterDB"; }
		const char *port_count() const { return "0/0"; }
		const char *processing() const { return AGNOSTIC; }

		String toString();

		bool dbExists(IPAddress client_address);
		IGMPClientDB* getDb(IPAddress client_address);
		int addDb(IPAddress client_address);

		// get all the clients who want a multicast packet from this source
		Vector<IPAddress> acceptFromSource(IPAddress multicast_address, IPAddress source);

		Packet *simple_action(Packet *p);
	private:
		HashTable<IPAddress, IGMPClientDB*> dbs;
};

CLICK_ENDDECLS

#endif
