#include "Individual.hpp"
#include "TimetableConfig.hpp"
#include <chrono>
#include <random>

using namespace std;

namespace yaohui {

double Individual::score() const { return score_; }
double &Individual::score() { return score_; }
const TimetableConfig &Individual::timetable_config() const {
  return timetable_config_;
}
TimetableConfig &Individual::timetable_config() { return timetable_config_; }
void Individual::update_score() {
  score_ = Timetable(timetable_config_).total_reuse_ratio();
}

Individual::Individual(TimetableConfig tb_config)
    : timetable_config_(std::move(tb_config)) {

  auto find_T = [](second_t t, const departure_T_t &dT) -> second_t {
    // 寻找追踪间隔
    second_t departure_T;
    for (const auto &dt : dT) {
      if (t >= dt.first.first && t < dt.first.second) {
        departure_T = dt.second;
        break;
      }
    }
    return departure_T;
  };

  TimetableConfig &config = timetable_config_;

  // down departure
  // 获取下行首站发车时刻序列
  auto &down_de_vec = config.down_departure_time_vec();
  for (size_t i = 0; i != down_de_vec.size(); ++i) {
    if (i == 0) {
      continue;
    }
    second_t di_1 = down_de_vec.at(i - 1);
    second_t di = down_de_vec.at(i);
    // 寻找i-1时刻的最小追踪间隔
    second_t departure_T_min = find_T(di_1, config.departure_T_min());
    second_t oft = di - di_1 - departure_T_min; // 更新oft
    // 寻找i-1时刻的最大追踪间隔
    second_t departure_T_max = find_T(di_1, config.departure_T_max());
    second_t oyt = departure_T_max - (di - di_1); // 更新oyt

    // 随机偏移量
    auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine down_departure_mutate_offset_e(tp_epoch);
    static uniform_int_distribution<second_t> down_departure_mutate_offset_u(
        -oft, oyt);
    second_t r2 =
        down_departure_mutate_offset_u(down_departure_mutate_offset_e);
    // 更新di
    down_de_vec.at(i) += r2;
  }
  // up departure
  // 获取上行首站发车时刻序列
  auto &up_de_vec = config.up_departure_time_vec();
  for (size_t i = 0; i <= up_de_vec.size() - 1; ++i) {
    if (i == 0) {
      continue;
    }
    second_t di_1 = up_de_vec.at(i - 1);
    second_t di = up_de_vec.at(i);
    // 寻找i-1时刻的最小追踪间隔
    second_t departure_T_min = find_T(di_1, config.departure_T_min());
    second_t oft = di - di_1 - departure_T_min; // 更新oft
    // 寻找i-1时刻的最大追踪间隔
    second_t departure_T_max = find_T(di_1, config.departure_T_max());
    second_t oyt = departure_T_max - (di - di_1); // 更新oyt

    // 随机偏移量
    auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine up_departure_mutate_offset_e(tp_epoch);
    static uniform_int_distribution<second_t> up_departure_mutate_offset_u(-oft,
                                                                           oyt);
    second_t r2 = up_departure_mutate_offset_u(up_departure_mutate_offset_e);
    // 更新di
    down_de_vec.at(i) += r2;
  }

  // down stop
  for (auto &l : config.down_stop_duration_vec()) {
    for (station_id_t r1 = config.stations().front();
         r1 <= config.stations().back(); ++r1) {
      if (r1 == config.stations().front() || r1 == config.stations().back()) {
        continue;
      }
      second_t LB = config.stop_duration_min().at(r1);
      second_t UB = config.stop_duration_max().at(r1);

      auto tp_epoch =
          std::chrono::system_clock::now().time_since_epoch().count();
      static std::default_random_engine down_stop_mutate_offset_e(tp_epoch);
      static uniform_int_distribution<second_t> down_stop_mutate_offset_u(LB,
                                                                          UB);
      second_t r2 = down_stop_mutate_offset_u(down_stop_mutate_offset_e);
      l.at(r1) = r2;
    }
  }

  // up stop
  for (auto &l : config.up_stop_duration_vec()) {
    for (station_id_t r1 = config.stations().front();
         r1 <= config.stations().back(); ++r1) {
      if (r1 == config.stations().front() || r1 == config.stations().back()) {
        continue;
      }

      second_t LB = config.stop_duration_min().at(r1);
      second_t UB = config.stop_duration_max().at(r1);

      auto tp_epoch =
          std::chrono::system_clock::now().time_since_epoch().count();
      static std::default_random_engine up_stop_mutate_offset_e(tp_epoch);
      static uniform_int_distribution<second_t> up_stop_mutate_offset_u(LB, UB);
      second_t r2 = up_stop_mutate_offset_u(up_stop_mutate_offset_e);
      l.at(r1) = r2;
    }
  }

  // 更新score
  this->update_score();
}

} // namespace yaohui