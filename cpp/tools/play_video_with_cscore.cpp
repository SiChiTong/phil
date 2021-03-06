#include <cstdlib>
#include <iostream>

#include <cscore.h>
#include <opencv2/core/mat.hpp>

#include <phil/common/args.h>
#include <opencv2/videoio.hpp>
#include <opencv2/videoio/videoio_c.h>

int main(int argc, const char**argv) {
  args::ArgumentParser parser("Publish a given video as a cscore http stream");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::Positional<std::string> video_arg(parser, "video_file", "The video file to play", args::Options::Required);

  try {
    parser.ParseCLI(argc, argv);
  }
  catch (args::Help &e) {
    std::cout << parser;
    return 0;
  }
  catch (args::RequiredError &e) {
    std::cout << parser;
    return 0;
  }

  auto video = cv::VideoCapture(args::get(video_arg));

  const auto w = static_cast<int>(video.get(CV_CAP_PROP_FRAME_WIDTH));
  const auto h = static_cast<int>(video.get(CV_CAP_PROP_FRAME_HEIGHT));
  const auto fps = static_cast<int>(video.get(CV_CAP_PROP_FPS));

  std::cout << w << "x" << h << "p" << fps << "\n";

  cs::CvSource source("phil/source", cs::VideoMode::kMJPEG, w, h, fps);
  cs::MjpegServer mjpegServer("phil/camera", 8081);
  mjpegServer.SetSource(source);

  cv::Mat frame;
  while (true) {
    video >> frame;

    if (frame.empty()) {
      break;
    }

    struct timespec timeout{};
    timeout.tv_sec = 0;
    timeout.tv_nsec = static_cast<long>(1.0/fps*1e9);
    clock_nanosleep(CLOCK_REALTIME, 0, &timeout, nullptr);

    source.PutFrame(frame);
  }


  return EXIT_SUCCESS;
}