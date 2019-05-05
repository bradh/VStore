#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"
#include "RxManager.h"
#include "tensorflow/noscope/noscope_data.h"
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

namespace noscope {

// FIXME: should really fix this
const cv::Size NoscopeData::kYOLOResol_(608, 608);
const cv::Size NoscopeData::kDiffResol_(100, 100);
const cv::Size NoscopeData::kDistResol_(50, 50);

NoscopeData::NoscopeData(vs::Frame **frame,
                   const size_t kSkip, const size_t kNbFrames, const size_t kStart) :
    kNbFrames_(kNbFrames / kSkip),
    kSkip_(kSkip),
    yolo_data_(kYOLOFrameSize_ * kNbFrames_),
    diff_data_(kDiffFrameSize_ * kNbFrames_),
    dist_data_(kDistFrameSize_ * kNbFrames_) {
 // cv::VideoCapture cap(fname);

  ///if (kStart > 0)
 // cap.set(cv::CAP_PROP_POS_FRAMES, kStart - 1);

  //cv::Mat frame;
  cv::Mat yolo_frame(NoscopeData::kYOLOResol_, CV_8UC3);
  cv::Mat diff_frame(NoscopeData::kDiffResol_, CV_8UC3);
  cv::Mat dist_frame(NoscopeData::kDistResol_, CV_8UC3);
  cv::Mat dist_frame_f(NoscopeData::kDistResol_, CV_32FC3);

  for (size_t i = 0; i < kNbFrames; i++) {
    //cap >> frame;
    if (i % kSkip_ == 0) {
      const size_t ind = i / kSkip_;
      //cv::resize((static_cast<char *>(frame[2][i].msg_p->data()), frame[2][i].msg_p->size()), yolo_frame, NoscopeData::kYOLOResol_, 0, 0, cv::INTER_NEAREST);
      //cv::resize((static_cast<char *>(frame[0][i].msg_p->data()), frame[0][i].msg_p->size()), diff_frame, NoscopeData::kDiffResol_, 0, 0, cv::INTER_NEAREST);
      //cv::resize((static_cast<char *>(frame[1][i].msg_p->data()), frame[1][i].msg_p->size()), dist_frame, NoscopeData::kDistResol_, 0, 0, cv::INTER_NEAREST);
      cv::Mat frame_yolo;
      cv::Mat frame_diff;
      cv::Mat frame_dist;

      char *imagedata_yolo = NULL;
      imagedata_yolo = static_cast<char *>(frame[2][i].msg_p->data()), frame[2][i].msg_p->size();
      frame_yolo.create(NoscopeData::kYOLOResol_, CV_8UC3);
      memcpy(frame_yolo.data, imagedata_yolo, kYOLOFrameSize_);

      char *imagedata_diff = NULL;
      imagedata_diff = static_cast<char *>(frame[0][i].msg_p->data()), frame[0][i].msg_p->size();
      frame_diff.create(NoscopeData::kDiffResol_, CV_8UC3);
      memcpy(frame_diff.data, imagedata_diff, kDiffFrameSize_);


      char *imagedata_dist = NULL;
      imagedata_dist = static_cast<char *>(frame[1][i].msg_p->data()), frame[1][i].msg_p->size();
      frame_dist.create(NoscopeData::kDistResol_, CV_8UC3);
      memcpy(frame_dist.data, imagedata_dist, kDistFrameSize_);

      cv::resize(frame_yolo, yolo_frame, NoscopeData::kYOLOResol_, 0, 0, cv::INTER_NEAREST);
      cv::resize(frame_diff, diff_frame, NoscopeData::kDiffResol_, 0, 0, cv::INTER_NEAREST);
      cv::resize(frame_dist, dist_frame, NoscopeData::kDistResol_, 0, 0, cv::INTER_NEAREST);

      dist_frame.convertTo(dist_frame_f, CV_32FC3);

      if (!yolo_frame.isContinuous()) {
        throw std::runtime_error("yolo frame is not continuous");
      }
      if (!diff_frame.isContinuous()) {
        throw std::runtime_error("diff frame is not continuous");
      }
      if (!dist_frame.isContinuous()) {
        throw std::runtime_error("dist frame is not conintuous");
      }
      if (!dist_frame_f.isContinuous()) {
        throw std::runtime_error("dist frame f is not continuous");
      }

      memcpy(&yolo_data_[ind * kYOLOFrameSize_], yolo_frame.data, kYOLOFrameSize_);
      memcpy(&diff_data_[ind * kDiffFrameSize_], diff_frame.data, kDiffFrameSize_);
      memcpy(&dist_data_[ind * kDistFrameSize_], dist_frame_f.data, kDistFrameSize_ * sizeof(float));
    }
  }
}
/*
const cv::Size NoscopeData::kYOLOResol_(608, 608);
const cv::Size NoscopeData::kDiffResol_(100, 100);
const cv::Size NoscopeData::kDistResol_(50, 50);

NoscopeData::NoscopeData(const std::string& fname,
                   const size_t kSkip, const size_t kNbFrames, const size_t kStart) :
    kNbFrames_(kNbFrames / kSkip),
    kSkip_(kSkip),
    yolo_data_(kYOLOFrameSize_ * kNbFrames_),
    diff_data_(kDiffFrameSize_ * kNbFrames_),
    dist_data_(kDistFrameSize_ * kNbFrames_) {
  cv::VideoCapture cap(fname);

  if (kStart > 0)
    cap.set(cv::CAP_PROP_POS_FRAMES, kStart - 1);

  cv::Mat frame;
  cv::Mat yolo_frame(NoscopeData::kYOLOResol_, CV_8UC3);
  cv::Mat diff_frame(NoscopeData::kDiffResol_, CV_8UC3);
  cv::Mat dist_frame(NoscopeData::kDistResol_, CV_8UC3);
  cv::Mat dist_frame_f(NoscopeData::kDistResol_, CV_32FC3);
  for (size_t i = 0; i < kNbFrames; i++) {
    cap >> frame;
    if (i % kSkip_ == 0) {
      const size_t ind = i / kSkip_;
      cv::resize(frame, yolo_frame, NoscopeData::kYOLOResol_, 0, 0, cv::INTER_NEAREST);
      cv::resize(frame, diff_frame, NoscopeData::kDiffResol_, 0, 0, cv::INTER_NEAREST);
      cv::resize(frame, dist_frame, NoscopeData::kDistResol_, 0, 0, cv::INTER_NEAREST);
      dist_frame.convertTo(dist_frame_f, CV_32FC3);

      if (!yolo_frame.isContinuous()) {
        throw std::runtime_error("yolo frame is not continuous");
      }
      if (!diff_frame.isContinuous()) {
        throw std::runtime_error("diff frame is not continuous");
      }
      if (!dist_frame.isContinuous()) {
        throw std::runtime_error("dist frame is not conintuous");
      }
      if (!dist_frame_f.isContinuous()) {
        throw std::runtime_error("dist frame f is not continuous");
      }

      memcpy(&yolo_data_[ind * kYOLOFrameSize_], yolo_frame.data, kYOLOFrameSize_);
      memcpy(&diff_data_[ind * kDiffFrameSize_], diff_frame.data, kDiffFrameSize_);
      memcpy(&dist_data_[ind * kDistFrameSize_], dist_frame_f.data, kDistFrameSize_ * sizeof(float));
    }
  }
}
*/
static std::ifstream::pos_type filesize(const std::string& fname) {
  std::ifstream in(fname, std::ifstream::ate | std::ifstream::binary);
  return in.tellg();
}
NoscopeData::NoscopeData(const std::string& fname) :
    kNbFrames_(filesize(fname) / (kYOLOFrameSize_ + kDiffFrameSize_ + kDistFrameSize_* sizeof(float))),
    kSkip_(1),
    yolo_data_(kYOLOFrameSize_ * kNbFrames_),
    diff_data_(kDiffFrameSize_ * kNbFrames_),
    dist_data_(kDistFrameSize_ * kNbFrames_) {
  std::cerr << kNbFrames_ << "\n";
  std::ifstream in(fname, std::ifstream::binary);
  in.read((char *) &yolo_data_[0], yolo_data_.size());
  in.read((char *) &diff_data_[0], diff_data_.size());
  in.read((char *) &dist_data_[0], dist_data_.size() * sizeof(float));
}

void NoscopeData::DumpAll(const std::string& fname) {

  std::cerr << "Dumping " << kNbFrames_ << "\n";
  std::ofstream fout(fname, std::ios::binary | std::ios::out);
  fout.write((char *) &yolo_data_[0], yolo_data_.size());
  fout.write((char *) &diff_data_[0], diff_data_.size());
  fout.write((char *) &dist_data_[0], dist_data_.size() * sizeof(float));
}

} // namespace noscope
