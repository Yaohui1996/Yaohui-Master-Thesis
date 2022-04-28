#ifndef YAOHUI_MASTER_THESIS_BASEDEF_HPP
#define YAOHUI_MASTER_THESIS_BASEDEF_HPP

#include <cstdint>
#include <map>
#include <utility>
#include <vector>

namespace yaohui {

using station_id_t = int32_t; // 车站索引类型
using interval_id_t = std::pair<station_id_t, station_id_t>; // 行车区间索引类型
using supply_arm_id_t = int32_t; // 供电臂索引类型
using mission_id_t = int32_t;    // 运输任务(运行线)索引类型
using timetable_id_t = int32_t;  // 运行图索引类型

using second_t = int32_t;   // "秒"类型
using joule_t = double;     // 能量类型 (焦耳)
using kilojoule_t = double; // 能量类型 (千焦)

using ms_plot_data_t = std::vector<interval_id_t>; // 用于画图的运行线数据类型
using tb_plot_data_t = std::vector<ms_plot_data_t>; // 用于画图的运行图数据类型

// 下行方向的列车经过的车站的id类型
using down_stations_id_seq_t = std::vector<station_id_t>;
// 车站id和供电臂id的映射表类型
using supply_arm_map_t = std::map<station_id_t, supply_arm_id_t>;
// 运行区间标准行程时间的映射表类型
using travel_duration_t = std::map<interval_id_t, second_t>;
// 车站id和标准停站时长的映射表类型
using stop_duration_t = std::map<station_id_t, second_t>;
// 时间段和发车间隔的映射表类型
using departure_T_t = std::map<std::pair<second_t, second_t>, second_t>;
// 功率曲线类型
using P_curve_t = std::vector<kilojoule_t>;

// 发车时刻序列类型
using first_departure_time_t = std::vector<second_t>;
// 各发车时刻发出的列车对应的停站时长序列类型
using each_stop_duration_t = std::vector<std::map<station_id_t, second_t>>;

// 单供电臂能量关系表
using single_energy_map_t = std::vector<std::pair<second_t, second_t>>;
// 能量关系表
using energy_map_t = std::map<supply_arm_id_t, single_energy_map_t>;

// 单供电臂能量分布曲线
using single_energy_distribution_t = std::vector<joule_t>;
// 能量分布曲线
using energy_distribution_t =
    std::map<supply_arm_id_t, single_energy_distribution_t>;

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_BASEDEF_HPP