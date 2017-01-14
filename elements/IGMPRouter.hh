#ifndef CLICK_IGMPROUTER_HH
#define CLICK_IGMPROUTER_HH
#include <click/element.hh>
#include <click/timer.hh>
#include "IGMPRouterDB.hh"
#include "ProcessReport.hh"
#include "queryigmp.hh"

CLICK_DECLS

/*
=c

IGMPRouter(TODO)

=s

TODO: Summary

=d

TODO: Complete description

*/
class IGMPRouter : public Element {
	//TODO: Add private attributes

	public:
		IGMPRouter();
		~IGMPRouter();

		int configure(Vector<String>&, ErrorHandler*);

		void run_timer(Timer*);

		const char *class_name() const { return "IGMPRouter"; }
		const char *port_count() const { return "1/2"; }
		const char *processing() const { return PUSH; }

		int isINCLUDE(IPAddress client_address, IPAddress multicast_address);
		int isEXCLUDE(IPAddress client_address, IPAddress multicast_address);

		int toEX(IPAddress client_address, IPAddress multicast_address, Vector<IPAddress>& sources);
		int toIN(IPAddress client_address, IPAddress multicast_address, Vector<IPAddress>& sources);

		int block(IPAddress client_address, IPAddress multicast_address, Vector<IPAddress>& sources);
		int allow(IPAddress client_address, IPAddress multicast_address, Vector<IPAddress>& sources);

		int processReport(Packet *p);

		void push(int port, Packet *p);

		static String getDBHandler(Element *e, void * thunk);

		void add_handlers();
	private:
		IGMPRouterDB* db;
		QueryIGMPElement queryr;
		IPAddress* IPclient1;
		IPAddress* IPclient2;
		Timer timer;

		inline void update_cksum(click_ip *, int) const;
};

CLICK_ENDDECLS

#endif
