elementclass IGMP {
    db::IGMPClientDB;
    source::IGMPClient(DB db);

    source
        -> IPEncap(4, 192.168.1.254	, 224.0.0.1, TTL 1, PROTO 2)
    	-> EtherEncap(0x0800, 1A:7C:3E:90:78:41, 1A:7C:3E:90:78:42)
        -> [0]output;
}

elementclass QueryIGMP {
    source::QueryIGMPElement;

    source
        -> IPEncap(4, 192.168.1.254	, 224.0.0.1, TTL 1, PROTO 2)
    	-> EtherEncap(0x0800, 1A:7C:3E:90:78:41, 1A:7C:3E:90:78:42)
        -> [0]output;
}

//qi::IGMP;
//qi->ToDump("qi.dump");
db::IGMPClientDB;
source::IGMPClient(DB db);
class::IGMPClassifier(DB db);
query::QueryIGMP;
query->class;

RatedSource("data", 1, -1, true)
	-> DynamicUDPIPEncap(192.168.1.1, 1234, 192.168.2.2, 1234)
	-> EtherEncap(0x0800, 00:50:BA:85:84:B1, 00:50:BA:85:84:B2) /// The MAC addresses here should be from the multicast_server to get past the HostEtherFilter. This way we can reuse the input from the network for the applications.
	-> IPPrint("multicast_server -- transmitted a UDP packet")
	-> [0]class




class[0]->ToDump("qi.dump");
class[1]->Discard;
class[2]->Discard;

source->Discard;
