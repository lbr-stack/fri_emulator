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

class Sender {
public:
  Sender(const std::string &remote_ip, int remote_port) {
    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
      std::cerr << "Failed to create socket" << std::endl;
      exit(EXIT_FAILURE);
    }

    // Setup the remote address
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(remote_port);

    // Convert the IP address string to binary form
    if (inet_pton(AF_INET, remote_ip.c_str(), &remote_addr.sin_addr) <= 0) {
      std::cerr << "Invalid address or address not supported: " << remote_ip << std::endl;
      close(sockfd);
      exit(EXIT_FAILURE);
    }

    std::cout << "UDP socket ready to send data to " << remote_ip << ":" << remote_port
              << std::endl;
  }

  ~Sender() { close(sockfd); }

  // Method to send data
  void sendData(char *data, const size_t &size) {
    ssize_t sentBytes =
        sendto(sockfd, data, size, 0, (const struct sockaddr *)&remote_addr, sizeof(remote_addr));
    printf("sentBytes: %ld\n", sentBytes);
    if (sentBytes < 0) {
      std::cerr << "Failed to send data" << std::endl;
    } else {
      std::cout << "Sent: " << data << std::endl;
    }
  }

  void receiveData(char *data, const size_t &size) {
    ssize_t receivedBytes = recvfrom(sockfd, data, size, 0, NULL, NULL);
    printf("receivedBytes: %ld\n", receivedBytes);
    if (receivedBytes < 0) {
      std::cerr << "Failed to receive data" << std::endl;
    } else {
      std::cout << "Received: " << data << std::endl;
    }
  }

private:
  int sockfd;
  struct sockaddr_in remote_addr;
};

int main() {
  using namespace std;

  // Send data to the broadcast address (assuming remote_host is listening on INADDR_ANY)
  string remote_ip = "127.0.0.1"; // or a specific IP address
  int remote_port = 30200;

  // Create the sender instance
  Sender sender(remote_ip, remote_port);

  // encoder
  MonitoringMessageEncoder encoder;
  encoder.init_message();

  // Send some test data
  char data[KUKA::FRI::FRI_MONITOR_MSG_MAX_SIZE];
  // get size of data
  int cnt = 0;
  while (true) {
    auto bytes_written = encoder.to_buffer(data);
    sender.sendData(data, bytes_written);
    sleep(0.1);
    ++cnt;
    printf("cnt: %d\n", cnt);
    printf("receiving response...\n");
    sender.receiveData(data, KUKA::FRI::FRI_MONITOR_MSG_MAX_SIZE);
  }

  return 0;
}
