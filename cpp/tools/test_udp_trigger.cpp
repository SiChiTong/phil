#include <iostream>

#include <phil/common/udp.h>
#include <phil/common/args.h>

int main(int argc, const char **argv) {
  args::ArgumentParser parser("This program send a udp test message",
                              "Meant for debugging or triggering mocap_record without the RIO."
                                  " It literally just sends the value 1, and it's a 1 byte message."
                                  "It can be built for the RoboRIO.");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::ValueFlag<std::string> server_hostname_flag(parser, "hostname", "hostname of the client to send to", {'o'});
  args::ValueFlag<int16_t> udp_port_flag(parser, "udp port", "port to send udp message to", {'p'});

  try
  {
    parser.ParseCLI(argc, argv);
  }
  catch (args::Help &e)
  {
    std::cout << parser;
    return 0;
  }

  std::string hostname = "raspberrypi.local";
  int16_t port = phil::kPort;
  if (server_hostname_flag) {
    hostname = args::get(server_hostname_flag);
  }
  else {
    std::cout << "defaulting to phil-tk1.local\n";
  }

  if (udp_port_flag) {
    port = args::get(udp_port_flag);
  }

  phil::UDPClient client(hostname, port);
  uint8_t request = 1;
  client.RawTransaction(&request, 1, nullptr, 1);

  return 0;
}
