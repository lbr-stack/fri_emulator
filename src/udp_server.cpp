#include "fri_emulator/udp_server.hpp"

namespace fri_emulator {
UDPServer::UDPServer(const std::string &ip_address, const int &port) {
  _socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (_socket_fd < 0) {
    throw std::runtime_error("Failed to create socket");
  }

  _server_address.sin_family = AF_INET;
  _server_address.sin_port = htons(port);
  _server_address.sin_addr.s_addr = inet_addr(ip_address.c_str());

  std::cout << "[UDPServer]: Created socket" << std::endl;
  std::cout << "[UDPServer]: IP Address: " << ip_address << std::endl;
  std::cout << "[UDPServer]: Port: " << port << std::endl;
};
UDPServer::~UDPServer() { close(_socket_fd); };

void UDPServer::send(char const *const buffer, const int &buffer_size) const {
  ssize_t sent_bytes = sendto(_socket_fd, buffer, buffer_size, 0,
                              (struct sockaddr *)&_server_address, sizeof(_server_address));
  if (sent_bytes < 0) {
    std::cerr << "[UDPServer]: Failed to send data" << std::endl;
  }
};
ssize_t UDPServer::receive(char *const buffer, const size_t &buffer_size) const {
  // set timeout
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 100000;
  setsockopt(_socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
  auto recv_bytes = recvfrom(_socket_fd, buffer, buffer_size, 0, NULL, NULL);
  if (errno == EAGAIN) { // timeout
    return 0;
  }
  if (recv_bytes < 0) {
    std::cerr << "[UDPServer]: Failed to receive data" << std::endl;
  }
  return recv_bytes;
};
} // namespace fri_emulator
