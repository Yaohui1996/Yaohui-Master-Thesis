#ifndef YAOHUI_MASTER_THESIS_TIMETABLE_HPP
#define YAOHUI_MASTER_THESIS_TIMETABLE_HPP

#include <climits>
#include <cstdint>
#include <iostream>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

#include "Interval.hpp"
#include "Mission.hpp"
#include "Station.hpp"
#include "TimetableConfig.hpp"

namespace yaohui {

class Timetable {
private:
  using station_id_t = int32_t;    // 车站索引类型
  using supply_arm_id_t = int32_t; // 供电臂索引类型
  using mission_id_t = int32_t;    // 运输任务(运行线)索引类型
  using timetable_id_t = int32_t;  // 运行图索引类型
  using second_t = int32_t;        // "秒"类型
  using joule_t = double;          // 能量类型 (焦耳)
  using kilojoule_t = double;      // 能量类型 (千焦耳)
  using plot_data_t =
      std::vector<std::vector<std::pair<station_id_t, second_t>>>;

private:
  std::vector<kilojoule_t> consume_vec_ = {
      202.544, 607.53,  1012.21, 1416.8, 1820.87, 2224.65, 2627.32, 3029.41,
      3430.61, 3676.98, 3680.0,  3680.0, 3680.0,  3680.0,  3680.0,  3680.0,
      3680.0,  3680.0,  3680.0,  3680.0, 3680.0,  3680.0,  3680.0,  3680.0,
      3680.0,  3680.0,  3680.0,  3680.0, 3680.0,  3680.0}; // 启动阶段的用能关系
  std::vector<kilojoule_t>
      produce_vec_ = {4499.74, 4189.41, 3879.09, 3568.76, 3258.44, 2948.11,
                     2637.79, 2327.47, 2017.14, 1706.97, 1396.46, 1086.14,
                     671.127, 0.0,     0.0}; // 再生制动阶段的产能关系

  timetable_id_t timetable_id_ = INT32_MIN; // 运行图id
  std::vector<Mission> missions_; // 运行图包含的运输任务序列
private:
  static std::vector<Station>
  make_down_stations_vec(size_t down_id, const TimetableConfig &config);

  static std::vector<Station>
  make_up_stations_vec(size_t up_id, const TimetableConfig &config);

  static std::vector<Interval>
  make_down_intervals_vec(const std::vector<Station> &stations_seq);
  static std::vector<Interval>
  make_up_intervals_vec(const std::vector<Station> &stations_seq);
  static std::vector<Interval>
  make_intervals_vec(const std::vector<Station> &stations_seq);

public:
  Timetable(timetable_id_t id, std::vector<Mission> missions);

  explicit Timetable(std::vector<Mission> missions)
      : timetable_id_(-1), missions_(std::move(missions)) {}

  explicit Timetable(const TimetableConfig &config);

  const std::vector<Mission> &missions() const;
  plot_data_t get_plot_data() const;

  // 各个供电臂的总能量利用率
  double total_reuse_ratio() const;

  // 能量利用率
  std::map<supply_arm_id_t, double> reuse_ratio() const;

  // 输出能量分布曲线
  void output_consume_distribution(const std::string &output_path) const;
  void output_produce_distribution(const std::string &output_path) const;

  // 将运行图写至json文件
  void write_to_file() const;

private:
  // 耗能分布
  std::map<supply_arm_id_t, std::vector<Timetable::joule_t>>
  energy_consume_distribution() const;

  // 产能分布
  std::map<supply_arm_id_t, std::vector<Timetable::joule_t>>
  energy_produce_distribution() const;

  std::map<supply_arm_id_t, std::vector<std::pair<second_t, second_t>>>
  energy_consume_distribution_helper() const;

  std::map<supply_arm_id_t, std::vector<std::pair<second_t, second_t>>>
  energy_produce_distribution_helper() const;
};

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_TIMETABLE_HPP