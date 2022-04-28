#ifndef YAOHUI_MASTER_THESIS_TIMETABLECONFIG_HPP
#define YAOHUI_MASTER_THESIS_TIMETABLECONFIG_HPP

#include <cassert>
#include <iostream>

#include "BaseDef.hpp"

namespace yaohui {

class TimetableConfig {

private:
  // 下行方向的列车经过的车站的默认id(参数)
  // 脚标i为下行方向列车经过的第i个车站
  // 脚标i对应的值stations_.at(i)为该车站的默认id
  down_stations_id_seq_t stations_ = {0, 1, 2,  3,  4,  5,  6,  7,
                                      8, 9, 10, 11, 12, 13, 14, 15};

  // 车站所属供电臂id(参数)
  supply_arm_map_t supply_arm_ = {
      {0, 0}, {1, 0}, {2, 0},  {3, 0},  {4, 1},  {5, 1},  {6, 1},  {7, 1},
      {8, 2}, {9, 2}, {10, 2}, {11, 2}, {12, 3}, {13, 3}, {14, 3}, {15, 3}};

  // 区间标准行程时长(参数)
  travel_duration_t travel_duration_ = {
      {{0, 1}, 185},   {{1, 0}, 185},   {{1, 2}, 136},   {{2, 1}, 136},
      {{2, 3}, 127},   {{3, 2}, 127},   {{3, 4}, 145},   {{4, 3}, 145},
      {{4, 5}, 150},   {{5, 4}, 150},   {{5, 6}, 119},   {{6, 5}, 119},
      {{6, 7}, 105},   {{7, 6}, 105},   {{7, 8}, 134},   {{8, 7}, 134},
      {{8, 9}, 143},   {{9, 8}, 143},   {{9, 10}, 136},  {{10, 9}, 136},
      {{10, 11}, 167}, {{11, 10}, 167}, {{11, 12}, 157}, {{12, 11}, 157},
      {{12, 13}, 172}, {{13, 12}, 172}, {{13, 14}, 181}, {{14, 13}, 181},
      {{14, 15}, 185}, {{15, 14}, 185}};

  second_t produce_duration_ = 15; // 标准产能时长(参数)
  second_t consume_duration_ = 30; // 标准用能时长(参数)
                                   // 车站标准停站时长(决策变量)
  stop_duration_t stop_duration_ = {{0, 0},   {1, 30},  {2, 30},  {3, 30},
                                    {4, 45},  {5, 45},  {6, 45},  {7, 45},
                                    {8, 45},  {9, 45},  {10, 30}, {11, 30},
                                    {12, 30}, {13, 30}, {14, 30}, {15, 0}};

  stop_duration_t stop_duration_min_ = {{0, 0},   {1, 25},  {2, 25},  {3, 25},
                                        {4, 40},  {5, 40},  {6, 40},  {7, 40},
                                        {8, 40},  {9, 40},  {10, 25}, {11, 25},
                                        {12, 25}, {13, 25}, {14, 25}, {15, 0}};

  stop_duration_t stop_duration_max_ = {{0, 0},   {1, 35},  {2, 35},  {3, 35},
                                        {4, 50},  {5, 50},  {6, 50},  {7, 50},
                                        {8, 50},  {9, 50},  {10, 35}, {11, 35},
                                        {12, 35}, {13, 35}, {14, 35}, {15, 0}};
  // 各个时段的标准追踪间隔(参数)
  departure_T_t departure_T_ = {
      {{19800, 25200}, 600}, // [5:30-7:00) 10min
      {{25200, 28800}, 240}, // [7:00,8:00) 4min
      {{28800, 36000}, 120}, // [8:00,10:00) 2min
      {{36000, 39600}, 240}, // [10:00,11:00) 4min
      {{39600, 57600}, 600}, // [11:00,16:00) 10min
      {{57600, 61200}, 240}, // [16:00,17:00) 4min
      {{61200, 68400}, 120}, // [17:00,19:00) 2min
      {{68400, 72000}, 240}, // [19:00,20:00) 4min
      {{72000, 84600}, 600}  // [20:00,23:30) 10min
  };
  // 各个时段的最小追踪间隔(参数)
  departure_T_t departure_T_min_ = {
      {{19800, 25200}, 570}, // [5:30-7:00) 10min
      {{25200, 28800}, 210}, // [7:00,8:00) 4min
      {{28800, 36000}, 90},  // [8:00,10:00) 2min
      {{36000, 39600}, 210}, // [10:00,11:00) 4min
      {{39600, 57600}, 570}, // [11:00,16:00) 10min
      {{57600, 61200}, 210}, // [16:00,17:00) 4min
      {{61200, 68400}, 90},  // [17:00,19:00) 2min
      {{68400, 72000}, 210}, // [19:00,20:00) 4min
      {{72000, 84600}, 570}  // [20:00,23:30) 10min
  };
  // 各个时段的最大追踪间隔(参数)
  departure_T_t departure_T_max_ = {
      {{19800, 25200}, 630}, // [5:30-7:00) 10min
      {{25200, 28800}, 270}, // [7:00,8:00) 4min
      {{28800, 36000}, 150}, // [8:00,10:00) 2min
      {{36000, 39600}, 270}, // [10:00,11:00) 4min
      {{39600, 57600}, 630}, // [11:00,16:00) 10min
      {{57600, 61200}, 270}, // [16:00,17:00) 4min
      {{61200, 68400}, 150}, // [17:00,19:00) 2min
      {{68400, 72000}, 270}, // [19:00,20:00) 4min
      {{72000, 84600}, 630}  // [20:00,23:30) 10min
  };

  // 首班车发车时刻(5:30)(包含)(参数)
  // 末班车发车时刻(23:30)(不包含)(参数)
  second_t first_train_time_ = 61200;
  second_t last_train_time_ = 68400;

  // 用能功率曲线
  P_curve_t consume_vec_ = {
      202.544, 607.53,  1012.21, 1416.8, 1820.87, 2224.65, 2627.32, 3029.41,
      3430.61, 3676.98, 3680.0,  3680.0, 3680.0,  3680.0,  3680.0,  3680.0,
      3680.0,  3680.0,  3680.0,  3680.0, 3680.0,  3680.0,  3680.0,  3680.0,
      3680.0,  3680.0,  3680.0,  3680.0, 3680.0,  3680.0}; // 启动阶段的用能关系
  // 产能功率曲线
  P_curve_t produce_vec_ = {4499.74, 4189.41, 3879.09, 3568.76,
                            3258.44, 2948.11, 2637.79, 2327.47,
                            2017.14, 1706.97, 1396.46, 1086.14,
                            671.127, 0.0,     0.0}; // 再生制动阶段的产能关系

  // 下行首站发车时刻序列
  first_departure_time_t down_departure_time_vec_ = {};
  // 上行首站发车时刻序列
  first_departure_time_t up_departure_time_vec_ = {};
  // 各条下行运行线的停站时长
  each_stop_duration_t down_stop_duration_vec_ = {};
  // 各条上行运行线的停站时长
  each_stop_duration_t up_stop_duration_vec_ = {};

public:
  TimetableConfig &operator=(const TimetableConfig &) = default; // 拷贝赋值
  TimetableConfig &operator=(TimetableConfig &&) = default;      // 移动赋值
  TimetableConfig(const TimetableConfig &) = default;            // 拷贝构造
  TimetableConfig(TimetableConfig &&) = default;                 // 移动构造
  ~TimetableConfig() = default;                                  // 默认析构
  // 零参数构造函数
  TimetableConfig();

private:
  void init_basic_departure_time_sequence();
  void init_basic_stop_duration(size_t missions_cnt);

public:
  const departure_T_t &departure_T() const;
  const departure_T_t &departure_T_min() const;
  const departure_T_t &departure_T_max() const;
  const stop_duration_t &stop_duration() const;
  const stop_duration_t &stop_duration_min() const;
  const stop_duration_t &stop_duration_max() const;
  second_t first_train_time() const;
  second_t last_train_time() const;
  second_t produce_duration() const;
  second_t consume_duration() const;
  const std::vector<station_id_t> &stations() const;
  const std::map<station_id_t, supply_arm_id_t> &supply_arm() const;
  const std::map<interval_id_t, second_t> &travel_duration() const;
  const std::vector<kilojoule_t> &consume_vec() const;
  const std::vector<kilojoule_t> &produce_vec() const;
  size_t down_missions_cnt() const; // 运行图中下行运行线的数目
  size_t up_missions_cnt() const;   // 运行图中上行运行线的数目
  size_t missions_cnt() const;      // 运行图中运行线的数目
  // 下行发车时刻序列
  const first_departure_time_t &down_departure_time_vec() const;
  first_departure_time_t &down_departure_time_vec();
  // 上行发车时刻序列
  const first_departure_time_t &up_departure_time_vec() const;
  first_departure_time_t &up_departure_time_vec();
  // 各条下行运行线的停站时长
  const std::vector<std::map<station_id_t, second_t>> &
  down_stop_duration_vec() const;
  std::vector<std::map<station_id_t, second_t>> &down_stop_duration_vec();
  // 各条上行运行线的停站时长
  const std::vector<std::map<station_id_t, second_t>> &
  up_stop_duration_vec() const;
  std::vector<std::map<station_id_t, second_t>> &up_stop_duration_vec();
  void show() const;
};

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_TIMETABLECONFIG_HPP
