#pragma  once

#pragma once

#include <memory>
#include <string>

#include <AHRS.h>
#include <Encoder.h>
#include <SpeedController.h>
#include <networktables/NetworkTable.h>

#include <phil/common/udp.h>
#include <phil/common/common.h>

namespace phil {

class Phil {
 private:
  static Phil *instance;

  Phil();

 public:
  frc::Encoder *left_encoder;
  frc::Encoder *right_encoder;
  frc::SpeedController *left_motor;
  frc::SpeedController *right_motor;
  AHRS *ahrs;
  std::shared_ptr<nt::NetworkTable> table;

  static Phil *GetInstance();

  void GiveSensors(frc::Encoder *left_encoder, frc::Encoder *right_encoder, AHRS *ahrs);

  /**
   * For when we have a camera being processed on a co-processor
   */
  void ReadSensorsAndProcessRemotely();

  /**
   * Make sure the UDP client is connect
   */
  void Reconnect();

  /**
   * Literally just reads the value of network tables and returns it
   */
  pose_t GetPosition();

  /**
   * Send an arbitrary UDP message to any host on the network.
   */
  void SendUDPTo(std::string hostname,
                 uint8_t *request,
                 size_t request_size,
                 uint8_t *response,
                 size_t response_size,
                 uint16_t udp_port=kPort);

  /**
   * Send an arbitrary UDP message to the TK1.
   */
  void SendUDPToPI(uint8_t *request, size_t request_size, uint8_t *response, size_t response_size);

  /**
   * For communicating with the TK1
   */
  UDPClient udp_client;
  double tk1_time_offset;
};

} // end namespace
