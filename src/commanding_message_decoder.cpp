#include "fri_emulator/commanding_message_decoder.hpp"

namespace fri_emulator {
CommandingMessageDecoder::CommandingMessageDecoder() {};
CommandingMessageDecoder::~CommandingMessageDecoder() {};


void init();
void decode(const char *const buffer);
} // namespace fri_emulator