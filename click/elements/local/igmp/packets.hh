#ifndef CLICK_PACKETS_HH
#define CLICK_PACKETS_HH


// Protocol Headers
struct igmpv3query {
  uint8_t type;
  uint8_t max_response_code;
  uint16_t checksum;
  uint32_t group_address;
  uint8_t resv_and_s_and_qrv;
  uint8_t qqic;
  uint16_t no_of_sources;
  uint32_t sources[0];
};

// see RFC 3376 for details
struct igmpv3grouprecord {
  uint8_t type;
  uint8_t aux_data_len;
  uint16_t no_of_sources;
  uint32_t multicast_address;
  uint32_t sources[0];
};

struct igmpv3report {
  uint8_t type;
  uint8_t reserved;
  uint16_t checksum;
  uint16_t reserved_long;
  uint16_t no_of_grouprecords;
  uint32_t records[0];
};

#endif
