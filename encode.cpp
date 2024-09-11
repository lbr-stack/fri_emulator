
#include "FRIMessages.pb.h"
#include <pb_encode.h>
#include <stdio.h>

int main() {
  /* This is the buffer where we will store our message. */
  uint8_t buffer[128];
  size_t message_length;
  bool status;

  /* Initialize the structure with constants */
  Checksum message = {true, 0};

  message.has_crc32 = true;

  /* Create a stream that will write to our buffer. */
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  /* Fill in the lucky number */
  message.crc32 = 13;

  /* Now we are ready to encode the message! */
  status = pb_encode(&stream, Checksum_fields, &message);
  message_length = stream.bytes_written;

  /* Then just check for any errors.. */
  if (!status) {
    printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
    return 1;
  }
}
