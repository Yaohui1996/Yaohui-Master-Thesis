#ifndef YAOHUI_MASTER_THESIS_TIMETABLE_HPP
#define YAOHUI_MASTER_THESIS_TIMETABLE_HPP

#include "BaseDef.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "Interval.hpp"
#include "Mission.hpp"
#include "Station.hpp"
#include "TimetableConfig.hpp"

namespace yaohui {

class Timetable {

private:
  timetable_id_t timetable_id_ = INT32_MIN; // 运行图id
  std::vector<Mission> missions_ = {}; // 运行图包含的运输任务序列
  TimetableConfig config_;             // 运行图的基因
public:
  Timetable() = delete;                              // 默认构造
  Timetable(Timetable &&) = default;                 // 移动构造
  Timetable(const Timetable &) = default;            // 拷贝构造
  Timetable &operator=(Timetable &&) = default;      // 移动赋值
  Timetable &operator=(const Timetable &) = default; // 拷贝赋值
  ~Timetable() = default;                            // 默认析构
  explicit Timetable(const TimetableConfig &config); // 单参数构造函数
  // mission_数据成员的get方法
  const std::vector<Mission> &missions() const;
  // 各个供电臂的总能量利用率
  double total_reuse_ratio() const;
  // 输出能量分布曲线
  void output_energy_distribution(std::string pre_name) const;
  // 将运行图写至json文件
  void write_to_file(std::string pre_name) const;
  // 输出运行图画图数据
  void output_plot_data(std::string pre_name) const;

private:
  std::vector<Station> make_down_stations_vec(size_t down_id);
  std::vector<Station> make_up_stations_vec(size_t up_id);
  std::vector<Interval>
  make_down_intervals_vec(const std::vector<Station> &stations_seq);
  std::vector<Interval>
  make_up_intervals_vec(const std::vector<Station> &stations_seq);
  std::vector<Interval>
  make_intervals_vec(const std::vector<Station> &stations_seq);
  // 运行图绘图数据
  tb_plot_data_t get_plot_data() const;
  // 各个供电臂的产能区间和各个供电臂的用能区间
  std::pair<energy_map_t, energy_map_t> energy_exchange_duration() const;
  // 各个供电臂的产能分布和各个供电臂的用能分布
  std::pair<energy_distribution_t, energy_distribution_t>
  energy_distribution() const;
};

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_TIMETABLE_HPP