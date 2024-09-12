#ifndef FRI_EMULATOR__MONITORING_MESSAGE_ENCODER_HPP_
#define FRI_EMULATOR__MONITORING_MESSAGE_ENCODER_HPP_

#include "FRIMessages.pb.h"
#include "pb_decode.h"
#include "pb_frimessages_callbacks.h"

namespace fri_emulator {
class MonitoringMessageEncoder {
protected:
  FRIMonitoringMessage _pb_state;

public:
  MonitoringMessageEncoder();
  ~MonitoringMessageEncoder();

  inline const FRIMonitoringMessage &get_state() const { return _pb_state; };

  size_t encode(char *const buffer) const;
};
} // namespace fri_emulator
#endif // FRI_EMULATOR__MONITORING_MESSAGE_ENCODER_HPP_
