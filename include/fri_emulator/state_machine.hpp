#ifndef FRI_EMULATOR__STATE_MACHINE_HPP_
#define FRI_EMULATOR__STATE_MACHINE_HPP_

#include "friClientIf.h"

namespace fri_emulator {
class StateMachine {
protected:
  /* data */
  // KUKA::FRI::ESessionState::COMMANDING_ACTIVE
  // KUKA::FRI::ESessionState::COMMANDING_WAIT
  // KUKA::FRI::ESessionState::IDLE
  // KUKA::FRI::ESessionState::MONITORING_READY
  // KUKA::FRI::ESessionState::MONITORING_WAIT

  // KUKA::FRI::EConnectionQuality::EXCELLENT
  // KUKA::FRI::EConnectionQuality::FAIR
  // KUKA::FRI::EConnectionQuality::GOOD
  // KUKA::FRI::EConnectionQuality::POOR
public:
  StateMachine();
  ~StateMachine();
};
} // namespace fri_emulator
#endif // FRI_EMULATOR__STATE_MACHINE_HPP_