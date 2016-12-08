elementclass QueryIGMP {
    source::QueryIGMPElement;

    source
        -> IPEncap(4, 192.168.1.254	, 224.0.0.1, TTL 1, PROTO 2)
    	-> EtherEncap(0x0800, 1A:7C:3E:90:78:41, 1A:7C:3E:90:78:42)
        -> [0]output;
}

elementclass ReportIGMP {
    source::ReportIGMPElement;

    source
        -> IPEncap(4, 192.168.1.254	, 224.0.0.22, TTL 1, PROTO 2)
    	-> EtherEncap(0x0800, 1A:7C:3E:90:78:41, 1A:7C:3E:90:78:42)
        -> [0]output;
}

qi::ReportIGMP;
qi->ToDump("qi.dump");
