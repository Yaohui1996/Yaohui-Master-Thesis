#ifndef YAOHUI_MASTER_THESIS_MISSION_HPP
#define YAOHUI_MASTER_THESIS_MISSION_HPP

#include "Interval.hpp"
#include "Station.hpp"
#include <vector>

namespace yaohui {

class Mission {
private:
  mission_id_t mission_id_ = INT32_MIN;  // 运行线id
  bool is_down_direction_ = true;        // 运行线方向
  std::vector<Station> stations_ = {};   // 运输任务包含的车站序列
  std::vector<Interval> intervals_ = {}; // 运输任务包含的区间序列

public:
  /**
   *
   * @param id mission id
   * @param is_down 是否是下行运输任务
   * @param stations 运行线经过的车站序列
   * @param intervals 运行线经过的区间序列g
   */
  Mission(mission_id_t id, bool is_down, std::vector<Station> stations,
          std::vector<Interval> intervals)
      : mission_id_(id), is_down_direction_(is_down),
        stations_(std::move(stations)), intervals_(std::move(intervals)) {}
  Mission() = delete;

public:
  mission_id_t id() const { return mission_id_; }
  bool is_down_direction() const { return is_down_direction_; }
  const std::vector<Station> &stations() const { return stations_; }
  const std::vector<Interval> &intervals() const { return intervals_; }

  /**
   *
   * @return 用于绘制运行图的数据结构
   */
  ms_plot_data_t plot_info_mission() const {
    ms_plot_data_t mission_info_vec;
    for (const Station &s : this->stations_) {
      mission_info_vec.emplace_back(s.station_id(), s.arrive_time());
      mission_info_vec.emplace_back(s.station_id(), s.departure_time());
    }
    return std::move(mission_info_vec);
  }
};

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_MISSION_HPP
