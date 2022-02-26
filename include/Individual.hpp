//
// Created by Yaohui Li on 2022/1/24.
//

#ifndef YH_MASTER_THESIS_INDIVIDUAL_HPP
#define YH_MASTER_THESIS_INDIVIDUAL_HPP

#include "Timetable.hpp"
#include "TimetableConfig.hpp"
#include <cstdint>
#include <map>
#include <utility>
#include <vector>

namespace yaohui {

class Individual {
private:
  using second_t = int32_t; // 时间类型(秒)

public:
  // 随机构造一个个体,
  // 参数1: 发车时刻的取值波动范围
  // 参数2: 停站时长的取值波动范围
  Individual(yaohui::Individual::second_t departure_time_offset,
             yaohui::Individual::second_t stop_duration_offset);

  // 默认构造
  Individual() = default;
  Individual(const TimetableConfig &config, double score)
      : timetable_config_(config), score_(score) {}
  // 拷贝构造
  Individual(const Individual &other) {
    timetable_config_ = other.timetable_config_;
    score_ = other.score_;
  }
  // 拷贝赋值
  Individual &operator=(const Individual &rhs) {
    this->timetable_config_ = rhs.timetable_config_;
    this->score_ = rhs.score_;
    return *this;
  }

  explicit Individual(TimetableConfig tb_config)
      : timetable_config_(std::move(tb_config)) {
    score_ = Timetable(timetable_config_).total_reuse_ratio();
  }

private:
  TimetableConfig timetable_config_; // 个体的染色体信息
  double score_ = 0.0;               // 个体的适应度评分

public:
  double score() const { return score_; }
  double &score() { return score_; }
  const TimetableConfig &timetable_config() const { return timetable_config_; }
  TimetableConfig &timetable_config() { return timetable_config_; }
};

} // namespace yaohui

#endif // YH_MASTER_THESIS_INDIVIDUAL_HPP
