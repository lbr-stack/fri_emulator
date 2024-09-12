#ifndef FRI_EMULATOR__UDP_SERVER_HPP_
#define FRI_EMULATOR__UDP_SERVER_HPP_

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace fri_emulator {
class UDPServer {
private:
  constexpr static char LOCALHOST[] = "127.0.0.1";
  constexpr static int DEFAULT_PORT = 30200;

protected:
  int _socket_fd;
  struct sockaddr_in _server_address;

public:
  UDPServer() = delete;
  UDPServer(const std::string &ip_address = LOCALHOST, const int &port = DEFAULT_PORT);
  ~UDPServer();

  void send(char const *const buffer, const int &buffer_size) const;
  ssize_t receive(char *const buffer, const size_t &buffer_size) const;
};
} // namespace fri_emulator
#endif // FRI_EMULATOR__UDP_SERVER_HPP_
