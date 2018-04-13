#include <unordered_set>

#include <aruco/aruco.h>
#include <opencv2/opencv.hpp>

#include <phil/common/args.h>
#include <phil/common/common.h>

int detectMarkers(cv::VideoCapture capture,
                  const std::vector<unsigned long> &timestamps,
                  aruco::CameraParameters cam_params,
                  bool step,
                  bool show,
                  bool no_timestamps) {
  cv::Mat frame;
  cv::Mat annotated_frame;

  float marker_size = 0.0892; // meters

  //Create the detector
  aruco::MarkerDetector MDetector;
  MDetector.setDetectionMode(aruco::DetectionMode::DM_VIDEO_FAST);
  std::map<uint32_t, aruco::MarkerPoseTracker> tracker; //use a map so that for each id, we use a different pose tracker

  if (!capture.isOpened()) {
    std::cout << "Can not load video";
  } else {
    capture >> frame;

    cam_params.resize(frame.size());

    size_t frame_idx = 0;
    while (capture.isOpened()) {
      capture >> frame;

      if (frame.empty()) {
        break;
      }

      if (frame_idx >= timestamps.size() && !no_timestamps) {
        std::cerr << phil::red
                  << "There are more frames in the video than timestamps. This is very suspicious! Exiting now."
                  << phil::reset << "\n";
        return EXIT_FAILURE;
      }

      frame.copyTo(annotated_frame);

      // detect markers in frame
      std::vector<aruco::Marker> markers = MDetector.detect(frame);

      for (auto &marker : markers) {

        tracker[marker.id].estimatePose(marker, cam_params, marker_size);

        // draw the tags that were detected
        marker.draw(annotated_frame, cv::Scalar(0, 0, 255), 2);
        aruco::CvDrawingUtils::draw3dCube(annotated_frame, marker, cam_params);
        aruco::CvDrawingUtils::draw3dAxis(annotated_frame, marker, cam_params);

        // output to std out so one can redirect to any file they want
        if (no_timestamps) {
          std::cout << 0 << ","
                    << marker.id << ","
                    << marker.Tvec.at<float>(0) << ","
                    << marker.Tvec.at<float>(1) << ","
                    << marker.Tvec.at<float>(2) << ","
                    << marker.Rvec.at<float>(0) << ","
                    << marker.Rvec.at<float>(1) << ","
                    << marker.Rvec.at<float>(2) << std::endl;
        } else {
          std::cout << timestamps[frame_idx] << ","
                    << marker.id << ","
                    << marker.Tvec.at<float>(0) << ","
                    << marker.Tvec.at<float>(1) << ","
                    << marker.Tvec.at<float>(2) << ","
                    << marker.Rvec.at<float>(0) << ","
                    << marker.Rvec.at<float>(1) << ","
                    << marker.Rvec.at<float>(2) << std::endl;
        }
      }

      if (show) {
        cv::imshow("annotated", annotated_frame);
        cv::waitKey(10);
        if (step) {
          std::cin.get();
        }
      }

      std::string dt_str;
      ++frame_idx;
    }
  }

  return EXIT_SUCCESS;
}

int main(int argc, const char **argv) {
  args::ArgumentParser parser("Prints time-stamps of detected tags and their poses to standard out."
                              "It is recommended you redirect this to a file called detected_markers.csv.\n"
                              "You can analyze the results by feeding that file into analyze_marker_detection_analysis.py\n"
                              "-v will show the video, -s will show and step through each frame (press enter)\n\n"
                              "you can use -s to step through frame by frame\n");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::Positional<std::string> video_param(parser, "video_filename", "video file to process", args::Options::Required);
  args::Positional<std::string>
      params_param(parser, "params_filename", "camera parameters yaml file", args::Options::Required);
  args::Positional<std::string> timestamps_param(parser, "timestamps_filename", "timestamps.csv file");
  args::Flag show_flag(parser, "show", "show the video", {'s', "show"});
  args::Flag step_flag(parser, "step", "step the video frame-by-frame", {'p', "step"});

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

  std::string video_filename = args::get(video_param);
  std::string timestamps_filename = args::get(timestamps_param);
  bool no_timestamps = false;
  if (timestamps_filename.empty()) {
    no_timestamps = true;
  }
  std::cout << timestamps_filename << " <<\n";

  std::string params_filename = args::get(params_param);

  cv::VideoCapture cap(video_filename);
  auto w = static_cast<const int>(cap.get(CV_CAP_PROP_FRAME_WIDTH));
  auto h = static_cast<const int>(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
  const double fps = cap.get(CV_CAP_PROP_FPS);
  cv::Size input_size(w, h);

  aruco::CameraParameters params;
  try {
    params.readFromXMLFile(params_filename);
    if (!params.isValid()) {
      std::cout << "Camera parameters are invalid." << std::endl;
      return EXIT_FAILURE;
    }
  }
  catch (cv::Exception &e) {
    std::cout << "Camera parameters are invalid." << std::endl;
    return EXIT_FAILURE;
  }

  // Read and process the time stamps
  std::vector<unsigned long> timestamps;
  if (!no_timestamps) {
    std::ifstream timestamps_file(timestamps_filename);

    if (!timestamps_file.good()) {
      std::cout << "Bad file: [" << timestamps_filename << "]. " << std::strerror(errno) << std::endl;
      return EXIT_FAILURE;
    }

    while (!timestamps_file.eof()) {
      std::string line;
      timestamps_file >> line;

      if (line.empty()) {
        break;
      }

      size_t pos = 0;
      try {

        unsigned long time = std::stoul(line, &pos);
        if (pos != line.length()) {
          std::cout << "Error parsing whole line: [" << line << "] at " << pos << std::endl;
        } else {
          timestamps.push_back(time);
        }
      }
      catch (std::invalid_argument &e) {
        std::cout << "Error parsing: [" << line << "]" << std::endl;
        std::cout << e.what() << std::endl;
      }
    }
  }

  bool show = false;
  bool step = false;
  if (argc == 5) {
    if (args::get(step_flag)) {
      step = true;
      show = true;
      std::cout << "Stepping (press enter for next frame)" << std::endl;
    } else if (args::get(show_flag)) {
      show = true;
    }
  }

  return detectMarkers(cap, timestamps, params, step, show, no_timestamps);
}
