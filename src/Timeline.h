//
// Created by xenon on 30.05.2021.
//

// #ifndef INF443_TIMELINE_H
// #define INF443_TIMELINE_H

#pragma once

#include <limits>
#include <vector>

class Timeline {
 public:
  enum EInterpolationMode {
    EInterpolationModeLeft,
    EInterpolationModeRight,
    EInterpolationModeLinear,
    EInterpolationModeInQuad,
    EInterpolationModeOutQuad,
  };

  void Reset();
  void Bound();
  void Update(double time);

  Timeline& AddKeyPoint(double time, double value, EInterpolationMode mode);
  Timeline& SortKeyPoints();

  double Value() const;

 private:
  struct KeyPoint {
    double time;
    double value;
    EInterpolationMode mode;

    bool operator<(const KeyPoint& other) const { return time < other.time; }
  };

  std::vector<KeyPoint> key_points_{};
  double current_time_{0};
  double upper_bound_time_{std::numeric_limits<double>::infinity()};

  mutable double value_{0};
  mutable bool dirty_{true};

  static inline double Lerp(double a, double b, double t);
};

// #endif //INF443_TIMELINE_H
