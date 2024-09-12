#ifndef FRI_EMULATOR__VIRTUAL_HARDWARE_HPP_
#define FRI_EMULATOR__VIRTUAL_HARDWARE_HPP_

#include <memory>

#include "friLBRCommand.h"
#include "friLBRState.h"

#include "state_machine.hpp"
#include "udp_server.hpp"

namespace fri_emulator {

class VirtualHardware {
public:
  VirtualHardware();
  ~VirtualHardware();

  void enable_logging();
  void run();

protected:
  KUKA::FRI::LBRState _state;
  KUKA::FRI::LBRCommand _command;

  std::unique_ptr<UDPServer> _udp_server;
  std::unique_ptr<StateMachine> _state_machine;
};
} // namespace fri_emulator
#endif // FRI_EMULATOR__VIRTUAL_HARDWARE_HPP_