#ifndef FRI_EMULATOR__VIRTUAL_HARDWARE_HPP_
#define FRI_EMULATOR__VIRTUAL_HARDWARE_HPP_

#include <iostream>
#include <memory>

#include "FRIMessages.pb.h"
#include "friLBRClient.h"

#include "fri_emulator/commanding_message_decoder.hpp"
#include "fri_emulator/monitoring_message_encoder.hpp"
#include "fri_emulator/state_machine.hpp"
#include "fri_emulator/udp_server.hpp"

namespace fri_emulator {

class VirtualHardware {
public:
  VirtualHardware();
  ~VirtualHardware();

  void enable_logging();
  void run();

  // on receive command update state -> mimicing the robot
  // this class mimics a server application...
  // state:
  // monitorin wait
  // |      connection quality >= good
  // V
  // monitoring ready
  // |
  // V
  // commanding wait
  // |
  // V
  // commanding active (commands change robot state)

protected:
  std::unique_ptr<CommandingMessageDecoder> _commanding_decoder;
  std::unique_ptr<MonitoringMessageEncoder> _monitoring_encoder;
  std::unique_ptr<UDPServer> _udp_server;
  std::unique_ptr<StateMachine> _state_machine;
};
} // namespace fri_emulator
#endif // FRI_EMULATOR__VIRTUAL_HARDWARE_HPP_
