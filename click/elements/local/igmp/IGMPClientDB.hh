#ifndef CLICK_IGMPCLIENTDB_HH
#define CLICK_IGMPCLIENTDB_HH


#include <click/element.hh>
#include <click/vector.cc>
#include <click/hashtable.hh>
#include <click/args.hh>
#include "packets.hh"

#if EXPLICIT_TEMPLATE_INSTANCES
template class Vector<SomeThing>;
template class HashTable<SomeThing, OtherThing>;
#endif

CLICK_DECLS

/*
=c

IGMPClientDB(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class IGMPClientDB : public Element {
	//TODO: Add private attributes

	public:
		IGMPClientDB();
		~IGMPClientDB();

		const char *class_name() const { return "IGMPClientDB"; }
		const char *port_count() const { return "0/0"; }
		const char *processing() const { return AGNOSTIC; }


		// Print the database
		String toString();

		void includeSources(IPAddress multicast_address, Vector<IPAddress> sources);
		void excludeSources(IPAddress multicast_address, Vector<IPAddress> sources);

		int addSources(IPAddress multicast_address, Vector<IPAddress> sources);
		int addSource(IPAddress multicast_address, IPAddress source);

		int clearSources(IPAddress multicast_address);
		int removeSources(IPAddress multicast_address, Vector<IPAddress> sources);
		int removeSource(IPAddress multicast_address, IPAddress source);

		bool recordExists(IPAddress multicast_address);
		Record* getRecord(IPAddress multicast_address);
		int addRecord(IPAddress multicast_address, Vector<IPAddress> sources, filtermode mode);

		// Check if this should be accepted as source
		bool acceptFromSource(IPAddress multicast_address, IPAddress source);

		int setMode(IPAddress multicast_address, filtermode mode);

		Vector<IPAddress> getSources(IPAddress multicast_address);
		filtermode getMode(IPAddress multicast_address);

		// Get an map with each multicast_addr and if it is INCLUDE of EXCLUDE
		HashTable<IPAddress, filtermode> getMulticastFiltermodeTable();

		Packet *simple_action(Packet *p);
	private:
		Vector<Record*> records;
};

CLICK_ENDDECLS

#endif
