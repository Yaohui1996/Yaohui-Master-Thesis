//
// Created by Yaohui Li on 2022/1/24.
//

#include "Individual.hpp"
#include "Timetable.hpp"
#include "TimetableConfig.hpp"
#include <random>
#include <chrono>

using namespace std;

namespace yaohui {

Individual::Individual(yaohui::Individual::second_t departure_time_offset,
                       yaohui::Individual::second_t stop_duration_offset)
    : timetable_config_() {
  //  TimetableConfig default_config; // 默认参数构造一个config
  //  down_first_chromosome_ = config.down_basic_departure_time_sequence();
  //  up_first_chromosome_ = config.up_basic_departure_time_sequence();
  //  down_stop_chromosome_ = config.down_stop_duration_vec();
  //  up_stop_chromosome_ = config.up_stop_duration_vec();

  // 按照随机参数生成个体
  auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static default_random_engine e1(tp_epoch);
  static uniform_int_distribution<int32_t> u1(-departure_time_offset,
                                              departure_time_offset);
  tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static default_random_engine e2(tp_epoch);
  static uniform_int_distribution<int32_t> u2(-stop_duration_offset,
                                              stop_duration_offset);
  // 修改各个参数的值
  // 随机修改 down_first_chromosome_
  for (auto &item : timetable_config_.down_basic_departure_time_sequence()) {
    item += u1(e1);
  }

  // 随机修改 up_first_chromosome_
  for (auto &item : timetable_config_.up_basic_departure_time_sequence()) {
    item += u1(e1);
  }

  // 随机修改 down_stop_chromosome_
  for (auto &item : timetable_config_.down_stop_duration_vec()) {
    for (auto &p : item) {
      if (!(p.first == 0 || p.first == 15)) {
        (p.second) += u2(e2);
      }
    }
  }

  // 随机修改 up_stop_chromosome_
  for (auto &item : timetable_config_.up_stop_duration_vec()) {
    for (auto &p : item) {
      if (!(p.first == 0 || p.first == 15)) {
        (p.second) += u2(e2);
      }
    }
  }

  // 接下来对个体评分
  score_ = Timetable(timetable_config_).total_reuse_ratio();
  // 至此, 个体构造完毕
}

} // namespace yaohui