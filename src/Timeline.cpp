//
// Created by xenon on 30.05.2021.
//

#include "Timeline.h"

#include <algorithm>
#include <cmath>
#include <limits>

void Timeline::Reset() {
  current_time_ = 0;
  if (!key_points_.empty()) {
    current_time_ = key_points_.front().time;
  }

  upper_bound_time_ = std::numeric_limits<double>::infinity();
  dirty_            = true;
}

void Timeline::Bound() {
  if (key_points_.empty()) {
    return;
  }

  double min_time = key_points_.front().time;
  double max_time = key_points_.back().time;
  double period   = max_time - min_time;

  upper_bound_time_ =
      std::max(ceil(current_time_ - min_time / period), 1.0) * period +
      min_time;
}

void Timeline::Update(double time) {
  current_time_ = time;
  dirty_        = true;
}

Timeline& Timeline::AddKeyPoint(double time, double value,
                                Timeline::EInterpolationMode mode) {
  key_points_.push_back({time, value, mode});
  return *this;
}

Timeline& Timeline::SortKeyPoints() {
  std::sort(key_points_.begin(), key_points_.end());

  return *this;
}

double Timeline::Value() const {
  if (dirty_) {
    if (key_points_.empty()) {
      value_ = 0;
    } else if (key_points_.size() == 1) {
      value_ = key_points_.front().value;
    } else if (current_time_ >= upper_bound_time_) {
      value_ = key_points_.back().value;
    } else if (current_time_ <= key_points_.front().time) {
      value_ = key_points_.front().value;
    } else {
      double min_time = key_points_.front().time;
      double max_time = key_points_.back().time;
      double period   = max_time - min_time;

      double time = std::fmod(current_time_ - min_time, period) + min_time;
      auto it     = std::upper_bound(key_points_.begin(), key_points_.end(),
                                 KeyPoint{time, 0, EInterpolationModeLeft});

      if (it == key_points_.begin()) {
        value_ = it->value;
      } else if (it == key_points_.end()) {
        value_ = key_points_.back().value;
      } else {
        const KeyPoint* begin = (it - 1).base();
        const KeyPoint* end   = it.base();

        switch (end->mode) {
        case EInterpolationModeLeft:
          value_ = begin->value;
          break;
        case EInterpolationModeLinear: {
          double x = (time - begin->time) / (end->time - begin->time);
          value_   = Lerp(begin->value, end->value, x);
          break;
        }
        case EInterpolationModeRight:
          value_ = end->value;
          break;
        case EInterpolationModeInQuad: {
          double x = (time - begin->time) / (end->time - begin->time);
          value_   = Lerp(begin->value, end->value, x * x);
          break;
        }
        case EInterpolationModeOutQuad: {
          double x = (time - begin->time) / (end->time - begin->time);
          value_   = Lerp(begin->value, end->value, x * (2 - x));
          break;
        }
        }
      }
    }
    dirty_ = false;
  }
  return value_;
}

double Timeline::Lerp(double a, double b, double t) { return a + t * (b - a); }
