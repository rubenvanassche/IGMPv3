// Output configuration:
//
// Packets for the network are put on output 0
// Packets for the host are put on output 1

elementclass IGMP {
    $db |
    source::IGMPClient(DB $db);

    source
        -> IPEncap(4, 192.168.1.254	, 224.0.0.1, TTL 1, PROTO 2)
    	-> EtherEncap(0x0800, 1A:7C:3E:90:78:41, 1A:7C:3E:90:78:42)
        -> [0]output;

	input[0] -> source
}

elementclass Client {
	$address, $gateway |

	igmpDB::IGMPClientDB;
	igmpClient::IGMP(igmpDB);
	igmpClassifier::IGMPClassifier(DB igmpDB);

	ip :: Strip(14)
		-> CheckIPHeader()
		-> rt :: StaticIPLookup(
					$address:ip/32 0,
					$address:ipnet 0,
					0.0.0.0/0.0.0.0 $gateway 1)
		-> [1]output;

	rt[1]
		-> igmpClassifier // Unicast continue
		-> DropBroadcasts
		-> ipgw :: IPGWOptions($address)
		-> FixIPSrc($address)
		-> ttl :: DecIPTTL
		-> frag :: IPFragmenter(1500)
		-> arpq :: ARPQuerier($address)
		-> output;

	ipgw[1]
		-> ICMPError($address, parameterproblem)
		-> output;

	ttl[1]
		-> ICMPError($address, timeexceeded)
		-> output;

	frag[1]
		-> ICMPError($address, unreachable, needfrag)
		-> output;

	// IGMP multicast
	igmpClassifier[1] -> [1]output;

	// IGMP Message
	igmpClassifier[2] -> igmpClient;

	igmpClient -> Discard;

	// Incoming Packets
	input
		-> HostEtherFilter($address)
		-> in_cl :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800)
		-> arp_res :: ARPResponder($address)
		-> output;

	in_cl[1]
		-> [1]arpq;

	in_cl[2]
		-> ip;
}
