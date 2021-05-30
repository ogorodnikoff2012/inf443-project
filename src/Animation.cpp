//
// Created by xenon on 30.05.2021.
//

#include "Animation.h"

void Animation::StartLoop() {
  epoch_start_ = std::chrono::system_clock::now();
  for (auto it = timelines_.begin(); it != timelines_.end(); ++it) {
    it->second.Reset();
  }
}

void Animation::BreakLoop() {
  for (auto it = timelines_.begin(); it != timelines_.end(); ++it) {
    it->second.Bound();
  }
}

void Animation::Update(const Animation::TimePoint& now) {
  std::chrono::duration<double> diff = now - epoch_start_;
  double delta                       = diff.count();
  for (auto it = timelines_.begin(); it != timelines_.end(); ++it) {
    it->second.Update(delta);
  }
}

Timeline& Animation::NewTimeline(const std::string& name) {
  return timelines_[name];
}
const Timeline& Animation::operator[](const std::string& name) const {
  return timelines_.at(name);
}
