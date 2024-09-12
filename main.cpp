#include <arpa/inet.h> // For inet_addr, htons, etc.
#include <cstring>     // For memset
#include <iostream>
#include <unistd.h> // For close on Linux

//// monitoring message encoder (using protobuf)
//// commanding message decoder
#include "FRIMessages.pb.h"
#include "friLBRState.h"
#include "friMonitoringMessageDecoder.h"
#include "pb_decode.h"
#include "pb_encode.h"

#include "fri_emulator/udp_server.hpp"

#include <memory>

class MonitoringMessageEncoder {
public:
  MonitoringMessageEncoder() {

  };

  struct LocalMonitoringDataContainer {
    tRepeatedDoubleArguments m_AxQMsrLocal;
    tRepeatedDoubleArguments m_AxTauMsrLocal;
    tRepeatedDoubleArguments m_AxQCmdT1mLocal;
    tRepeatedDoubleArguments m_AxTauCmdLocal;
    tRepeatedDoubleArguments m_AxTauExtMsrLocal;
    tRepeatedIntArguments m_AxDriveStateLocal;
    tRepeatedDoubleArguments m_AxQCmdIPO;

    LocalMonitoringDataContainer() {
      init_repeatedDouble(&m_AxQMsrLocal);
      init_repeatedDouble(&m_AxTauMsrLocal);
      init_repeatedDouble(&m_AxQCmdT1mLocal);
      init_repeatedDouble(&m_AxTauCmdLocal);
      init_repeatedDouble(&m_AxTauExtMsrLocal);
      init_repeatedDouble(&m_AxQCmdIPO);
      init_repeatedInt(&m_AxDriveStateLocal);
    }

    ~LocalMonitoringDataContainer() {
      free_repeatedDouble(&m_AxQMsrLocal);
      free_repeatedDouble(&m_AxTauMsrLocal);
      free_repeatedDouble(&m_AxQCmdT1mLocal);
      free_repeatedDouble(&m_AxTauCmdLocal);
      free_repeatedDouble(&m_AxTauExtMsrLocal);
      free_repeatedDouble(&m_AxQCmdIPO);
      free_repeatedInt(&m_AxDriveStateLocal);
    }
  };

  void init_message() {
    // set initial data
    // it is assumed that no robot information and monitoring data is available and therefore the
    // optional fields are initialized with false
    _pb_message.has_robotInfo = false;
    _pb_message.has_monitorData = false;
    _pb_message.has_connectionInfo = true;
    _pb_message.has_ipoData = false;
    _pb_message.requestedTransformations_count = 0;
    _pb_message.has_endOfMessageData = false;
    _pb_message.connectionInfo.has_receiveMultiplier = true;
    _pb_message.connectionInfo.receiveMultiplier = 0;

    _pb_message.header.messageIdentifier = 2380098;
    _pb_message.header.reflectedSequenceCounter = 0;
    _pb_message.header.sequenceCounter = 0;

    _pb_message.connectionInfo.sessionState = FRISessionState_MONITORING_WAIT;
    _pb_message.connectionInfo.quality = FRIConnectionQuality_POOR;

    _pb_message.monitorData.readIORequest_count = 0;

    // allocate and map memory for protobuf repeated structures
    map_repeatedDouble(FRI_MANAGER_NANOPB_ENCODE, 7,
                       &_pb_message.monitorData.measuredJointPosition.value,
                       &m_tSendContainer.m_AxQMsrLocal);

    map_repeatedDouble(FRI_MANAGER_NANOPB_ENCODE, 7, &_pb_message.monitorData.measuredTorque.value,
                       &m_tSendContainer.m_AxTauMsrLocal);

    map_repeatedDouble(FRI_MANAGER_NANOPB_ENCODE, 7,
                       &_pb_message.monitorData.commandedJointPosition.value,
                       &m_tSendContainer.m_AxQCmdT1mLocal);

    map_repeatedDouble(FRI_MANAGER_NANOPB_ENCODE, 7, &_pb_message.monitorData.commandedTorque.value,
                       &m_tSendContainer.m_AxTauCmdLocal);

    map_repeatedDouble(FRI_MANAGER_NANOPB_ENCODE, 7, &_pb_message.monitorData.externalTorque.value,
                       &m_tSendContainer.m_AxTauExtMsrLocal);

    map_repeatedDouble(FRI_MANAGER_NANOPB_ENCODE, 7, &_pb_message.ipoData.jointPosition.value,
                       &m_tSendContainer.m_AxQCmdIPO);

    map_repeatedInt(FRI_MANAGER_NANOPB_ENCODE, 7, &_pb_message.robotInfo.driveState,
                    &m_tSendContainer.m_AxDriveStateLocal);
  }

  size_t to_buffer(char *buffer) {
    // Initialize a protobuf output stream from the buffer
    pb_ostream_t stream =
        pb_ostream_from_buffer((uint8_t *)buffer, KUKA::FRI::FRI_MONITOR_MSG_MAX_SIZE);

    // Encode the message using the generated fields and the provided message structure
    bool status = pb_encode(&stream, FRIMonitoringMessage_fields, &_pb_message);
    auto bytes_written = stream.bytes_written;
    if (!status) {
      printf("!!encoding error: %s!!\n", PB_GET_ERROR(&stream));
    }
    return bytes_written;
  }

protected:
  LocalMonitoringDataContainer m_tSendContainer;
  FRIMonitoringMessage _pb_message;
};

int main() {
  using namespace std;

  // Send data to the broadcast address (assuming remote_host is listening on INADDR_ANY)
  string remote_ip = "127.0.0.1"; // or a specific IP address
  int remote_port = 30200;

  // Create the sender instance
  // Sender sender(remote_ip, remote_port);
  fri_emulator::UDPServer sender(remote_ip, remote_port);

  // encoder
  MonitoringMessageEncoder encoder;
  encoder.init_message();

  // Send some test data
  char data[KUKA::FRI::FRI_MONITOR_MSG_MAX_SIZE];
  // get size of data
  int cnt = 0;
  while (true) {
    auto bytes_written = encoder.to_buffer(data);
    sender.send(data, bytes_written);
    sleep(0.1);
    ++cnt;
    printf("cnt: %d\n", cnt);
    printf("receiving response...\n");
    sender.receive(data, KUKA::FRI::FRI_MONITOR_MSG_MAX_SIZE);
  }

  return 0;
}
