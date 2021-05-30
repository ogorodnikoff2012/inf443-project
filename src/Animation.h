//
// Created by xenon on 30.05.2021.
//

// #ifndef INF443_ANIMATION_H
// #define INF443_ANIMATION_H

#pragma once

#include "Timeline.h"

#include <chrono>
#include <string>
#include <unordered_map>

class Animation {
 public:
  using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
  void StartLoop();
  void BreakLoop();
  void Update(const TimePoint& now);

  Timeline& NewTimeline(const std::string& name);
  const Timeline& operator[](const std::string& name) const;

 private:
  TimePoint epoch_start_;
  std::unordered_map<std::string, Timeline> timelines_;
};

// #endif //INF443_ANIMATION_H
