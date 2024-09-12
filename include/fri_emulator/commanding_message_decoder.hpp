#ifndef FRI_EMULATOR__COMMANDING_MESSAGE_DECODER_HPP_
#define FRI_EMULATOR__COMMANDING_MESSAGE_DECODER_HPP_

#include "FRIMessages.pb.h"
#include "friLBRClient.h"
#include "pb_decode.h"
#include "pb_frimessages_callbacks.h"

namespace fri_emulator {
class CommandingMessageDecoder {
protected:
  FRICommandMessage _pb_command;

public:
  CommandingMessageDecoder();
  ~CommandingMessageDecoder();

  void init();
  inline const FRICommandMessage &get_command() const { return _pb_command; }
  void decode(const char *const buffer);
};
} // namespace fri_emulator
#endif // FRI_EMULATOR__COMMANDING_MESSAGE_DECODER_HPP_
