#ifndef YAOHUI_MASTER_THESIS_TIMETABLECONFIG_HPP
#define YAOHUI_MASTER_THESIS_TIMETABLECONFIG_HPP

#include <cassert>
#include <iostream>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

//#include "BaseDef.hpp"

namespace yaohui {

class TimetableConfig {
private:
  using station_id_t = int32_t;    // 车站索引类型
  using supply_arm_id_t = int32_t; // 供电臂索引类型
  using interval_id_t =
      std::pair<station_id_t, station_id_t>; // 行车区间索引类型
  using mission_id_t = int32_t; // 运输任务(运行线)索引类型
  using second_t = int32_t;     // "秒"类型

private:
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

private:
  // 发车时刻序列类型
  using first_departure_time_t = std::vector<second_t>;
  // 各发车时刻发出的列车对应的停站时长序列类型
  using each_stop_duration_t = std::vector<std::map<station_id_t, second_t>>;

private:
  // 下行方向的列车经过的车站的默认id(参数)
  // 脚标i为下行方向列车经过的第i个车站
  // 脚标i对应的值stations_.at(i)为该车站的默认id
  const down_stations_id_seq_t stations_ = {0, 1, 2,  3,  4,  5,  6,  7,
                                            8, 9, 10, 11, 12, 13, 14, 15};

  // 车站所属供电臂id(参数)
  const supply_arm_map_t supply_arm_ = {
      {0, 0}, {1, 0}, {2, 0},  {3, 0},  {4, 1},  {5, 1},  {6, 1},  {7, 1},
      {8, 2}, {9, 2}, {10, 2}, {11, 2}, {12, 3}, {13, 3}, {14, 3}, {15, 3}};

  // 区间标准行程时长(参数)
  const travel_duration_t travel_duration_ = {
      {{0, 1}, 185},   {{1, 0}, 185},   {{1, 2}, 136},   {{2, 1}, 136},
      {{2, 3}, 127},   {{3, 2}, 127},   {{3, 4}, 145},   {{4, 3}, 145},
      {{4, 5}, 150},   {{5, 4}, 150},   {{5, 6}, 119},   {{6, 5}, 119},
      {{6, 7}, 105},   {{7, 6}, 105},   {{7, 8}, 134},   {{8, 7}, 134},
      {{8, 9}, 143},   {{9, 8}, 143},   {{9, 10}, 136},  {{10, 9}, 136},
      {{10, 11}, 167}, {{11, 10}, 167}, {{11, 12}, 157}, {{12, 11}, 157},
      {{12, 13}, 172}, {{13, 12}, 172}, {{13, 14}, 181}, {{14, 13}, 181},
      {{14, 15}, 185}, {{15, 14}, 185}};

  const second_t produce_duration_ = 15; // 标准产能时长(参数)
  const second_t consume_duration_ = 30; // 标准用能时长(参数)
  const second_t first_train_time_ = 19800; // 首班车发车时刻(5:30)(包含)(参数)
  const second_t last_train_time_ =
      84600; // 末班车发车时刻(23:30)(不包含)(参数)

  // 车站标准停站时长(决策变量)
  const stop_duration_t stop_duration_ = {
      {0, 0},   {1, 30},  {2, 30},  {3, 30}, {4, 45},  {5, 45},
      {6, 45},  {7, 45},  {8, 45},  {9, 45}, {10, 30}, {11, 30},
      {12, 30}, {13, 30}, {14, 30}, {15, 0}};

  // 各个时段的标准发车间隔(参数)
  const departure_T_t departure_T_ = {
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

  // 下行标准发车时刻序列(由标准发车间隔决定), 序列长度为下行的运行线数目
  first_departure_time_t down_basic_departure_time_sequence_ = {};
  // 上行标准发车时刻序列(由标准发车间隔决定), 序列长度为上行的运行线数目
  first_departure_time_t up_basic_departure_time_sequence_ = {};
  // 各条下行运行线的停站时长
  each_stop_duration_t down_stop_duration_vec_ = {};
  // 各条上行运行线的停站时长
  each_stop_duration_t up_stop_duration_vec_ = {};

private:
  // 根据首末班车时刻和各个时段的发车频率, 填充标准发车时刻序列
  first_departure_time_t &make_basic_departure_time_sequence(
      first_departure_time_t &basic_departure_time_seq);

  // 根据车站标准停站时长, 填充每一次运输任务的停站时长
  each_stop_duration_t &
  make_basic_stop_duration(each_stop_duration_t &stop_duration_vec,
                           size_t missions_cnt);

public:
  // 移动构造函数
  //  TimetableConfig(TimetableConfig &&rhs) = delete;
  // 拷贝构造函数
  TimetableConfig(const TimetableConfig &rhs)
      : down_basic_departure_time_sequence_(
            rhs.down_basic_departure_time_sequence_),
        up_basic_departure_time_sequence_(
            rhs.up_basic_departure_time_sequence_),
        down_stop_duration_vec_(rhs.down_stop_duration_vec_),
        up_stop_duration_vec_(rhs.up_stop_duration_vec_) {}
  // 重载拷贝赋值
  TimetableConfig &operator=(const TimetableConfig &rhs) {
    if (this == &rhs) {
      return *this;
    }
    down_basic_departure_time_sequence_ =
        rhs.down_basic_departure_time_sequence_;
    up_basic_departure_time_sequence_ = rhs.up_basic_departure_time_sequence_;
    down_stop_duration_vec_ = rhs.down_stop_duration_vec_;
    up_stop_duration_vec_ = rhs.up_stop_duration_vec_;
    return *this;
  }

  // 零参数构造函数
  TimetableConfig() {
    down_basic_departure_time_sequence_ =
        make_basic_departure_time_sequence(down_basic_departure_time_sequence_);
    up_basic_departure_time_sequence_ =
        make_basic_departure_time_sequence(up_basic_departure_time_sequence_);

    down_stop_duration_vec_ = make_basic_stop_duration(
        down_stop_duration_vec_, down_basic_departure_time_sequence_.size());
    up_stop_duration_vec_ = make_basic_stop_duration(
        up_stop_duration_vec_, up_basic_departure_time_sequence_.size());

    assert(down_basic_departure_time_sequence_.size() ==
           down_stop_duration_vec_.size());
    assert(up_basic_departure_time_sequence_.size() ==
           up_stop_duration_vec_.size());

    //    std::cout << down_basic_departure_time_sequence_.size() << std::endl;
    //    std::cout << up_basic_departure_time_sequence_.size() << std::endl;
    //    std::cout << down_stop_duration_vec_.size() << std::endl;
    //    std::cout << up_stop_duration_vec_.size() << std::endl;
  }

  // 四参数构造函数
  TimetableConfig(
      std::vector<second_t> down_departure_time_vec,
      std::vector<second_t> up_departure_time_vec,
      std::vector<std::map<station_id_t, second_t>> down_stop_duration_vec,
      std::vector<std::map<station_id_t, second_t>> up_stop_duration_vec)
      : down_basic_departure_time_sequence_(std::move(down_departure_time_vec)),
        up_basic_departure_time_sequence_(std::move(up_departure_time_vec)),
        down_stop_duration_vec_(std::move(down_stop_duration_vec)),
        up_stop_duration_vec_(std::move(up_stop_duration_vec)) {}

  // 单参数构造函数
  //  explicit TimetableConfig(const Individual &individual) {
  //    // 其它数据成员使用默认值
  //    down_basic_departure_time_sequence_ = individual.down_first_vec();
  //    up_basic_departure_time_sequence_ = individual.up_first_vec();
  //    down_stop_duration_vec_ = individual.down_stop_vec();
  //    up_stop_duration_vec_ = individual.up_stop_vec();
  //  }

private:
  second_t first_train_time() const;
  second_t last_train_time() const;

  const std::map<station_id_t, second_t> &stop_duration() const;
  const std::map<std::pair<second_t, second_t>, second_t> &departure_T() const;

public:
  second_t produce_duration() const;
  second_t consume_duration() const;
  const std::map<interval_id_t, second_t> &travel_duration() const;
  const std::vector<station_id_t> &stations() const;
  const std::map<station_id_t, supply_arm_id_t> &supply_arm() const;

  // 运行图中下行运行线的数目
  size_t down_missions_cnt() const {
    //    std::cout << down_basic_departure_time_sequence_.size() << std::endl;
    //    std::cout << down_stop_duration_vec_.size() << std::endl;

    assert(down_basic_departure_time_sequence_.size() ==
           down_stop_duration_vec_.size());
    return down_basic_departure_time_sequence_.size();
  }

  // 运行图中上行运行线的数目
  size_t up_missions_cnt() const {
    assert(up_basic_departure_time_sequence_.size() ==
           up_stop_duration_vec_.size());
    return up_basic_departure_time_sequence_.size();
  }

  // 运行图中运行线的数目
  size_t missions_cnt() const {
    return down_missions_cnt() + up_missions_cnt();
  }

  // 下行标准发车时刻序列
  const std::vector<second_t> &down_basic_departure_time_sequence() const {
    return down_basic_departure_time_sequence_;
  }
  std::vector<second_t> &down_basic_departure_time_sequence() {
    return down_basic_departure_time_sequence_;
  }
  // 上行标准发车时刻序列
  const std::vector<second_t> &up_basic_departure_time_sequence() const {
    return up_basic_departure_time_sequence_;
  }
  std::vector<second_t> &up_basic_departure_time_sequence() {
    return up_basic_departure_time_sequence_;
  }

  // 各条下行运行线的停站时长
  const std::vector<std::map<station_id_t, second_t>> &
  down_stop_duration_vec() const {
    return down_stop_duration_vec_;
  }
  std::vector<std::map<station_id_t, second_t>> &down_stop_duration_vec() {
    return down_stop_duration_vec_;
  }
  // 各条上行运行线的停站时长
  const std::vector<std::map<station_id_t, second_t>> &
  up_stop_duration_vec() const {
    return up_stop_duration_vec_;
  }
  std::vector<std::map<station_id_t, second_t>> &up_stop_duration_vec() {
    return up_stop_duration_vec_;
  }

public:
  void show() const;

  // 随机调整下行标准发车时刻序列
  void random_offset_down_first_departure_time_vec(
      second_t departure_time_offset_range);

  // 随机调整上行标准发车时刻序列
  void random_offset_up_first_departure_time_vec(
      second_t departure_time_offset_range);

  // 随机调整各条下行运行线的停站时长
  void
  random_offset_down_stop_duration_vec(second_t stop_duration_offset_range);

  // 随机调整各条上行运行线的停站时长
  void random_offset_up_stop_duration_vec(second_t stop_duration_offset_range);
};

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_TIMETABLECONFIG_HPP
