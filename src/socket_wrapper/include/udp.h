#ifndef UDP_END_POINT_H
#define UDP_END_POINT_H

#include "protocol.h"

namespace socket_wrapper::udp {

template <ProtocolFamily ProtoFamily>
struct UdpProtocol : Protocol {
  UdpProtocol() : Protocol(SocketType::kDatagram, ProtoFamily, ProtocolName::kUdp) {
  }
};

/// Конечная точка, используемая для UDP-протокола.
template <ProtocolFamily ProtoFamily>
using UdpEndPoint = EndPoint<UdpProtocol<ProtoFamily>>;

}  // namespace socket_wrapper::udp

#endif  // UDP_END_POINT_H
