#ifndef YAOHUI_MASTER_THESIS_MISSION_HPP
#define YAOHUI_MASTER_THESIS_MISSION_HPP

#include <vector>

#include "Interval.hpp"
#include "Station.hpp"
#include <climits>
#include <cstdint>

namespace yaohui {

class Mission {
private:
  using station_id_t = int32_t; // 车站索引类型
  using mission_id_t = int32_t; // 运输任务(运行线)索引类型
  using second_t = int32_t;     // "秒"类型

private:
  mission_id_t mission_id_ = INT32_MIN;  // 运行线id
  bool is_down_direction_ = true;        // 运行线方向
  std::vector<Station> stations_ = {};   // 运输任务包含的车站序列
  std::vector<Interval> intervals_ = {}; // 运输任务包含的区间序列

public:
  /**
   * @brief Construct a new Mission object
   *
   * @param id mission id
   * @param stations 运行线经过的车站序列
   * @param intervals 运行线经过的区间序列
   */
  //  Mission(mission_id_t id, stations_seq_t stations,
  //          std::vector<Interval> intervals);

  /**
   *
   * @param id mission id
   * @param is_down 是否是下行运输任务
   * @param stations 运行线经过的车站序列
   * @param intervals 运行线经过的区间序列
   */
  Mission(mission_id_t id, bool is_down, std::vector<Station> stations,
          std::vector<Interval> intervals);
  Mission() = default;

public:
  // 只读权限获取数据成员
  mission_id_t id() const;
  bool is_down_direction() const;
  const std::vector<Station> &stations() const;
  const std::vector<Interval> &intervals() const;

public:
  std::vector<std::pair<station_id_t, second_t>> plot_info_mission() const;

  //    std::vector<joule_t> energy_produced_distribution() {
  //
  //    }
  //    std::vector<std::pair<joule_t, supply_arm_id_t>>
};

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_MISSION_HPP
