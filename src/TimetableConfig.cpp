#include "TimetableConfig.hpp"
#include <chrono>
#include <random>
#include <vector>

using namespace std;

namespace yaohui {
const departure_T_t &TimetableConfig::departure_T() const {
  return departure_T_;
}
const departure_T_t &TimetableConfig::departure_T_min() const {
  return departure_T_min_;
}
const departure_T_t &TimetableConfig::departure_T_max() const {
  return departure_T_max_;
}
const stop_duration_t &TimetableConfig::stop_duration() const {
  return stop_duration_;
}
const stop_duration_t &TimetableConfig::stop_duration_min() const {
  return stop_duration_min_;
}
const stop_duration_t &TimetableConfig::stop_duration_max() const {
  return stop_duration_max_;
}

second_t TimetableConfig::first_train_time() const { return first_train_time_; }
second_t TimetableConfig::last_train_time() const { return last_train_time_; }

second_t TimetableConfig::produce_duration() const { return produce_duration_; }
second_t TimetableConfig::consume_duration() const { return consume_duration_; }
const std::vector<station_id_t> &TimetableConfig::stations() const {
  return stations_;
}
const std::map<station_id_t, supply_arm_id_t> &
TimetableConfig::supply_arm() const {
  return supply_arm_;
}
const std::map<interval_id_t, second_t> &
TimetableConfig::travel_duration() const {
  return travel_duration_;
}
const std::vector<kilojoule_t> &TimetableConfig::consume_vec() const {
  return consume_vec_;
}
const std::vector<kilojoule_t> &TimetableConfig::produce_vec() const {
  return produce_vec_;
}

// 运行图中下行运行线的数目
size_t TimetableConfig::down_missions_cnt() const {
  assert(down_departure_time_vec_.size() == down_stop_duration_vec_.size());
  return down_departure_time_vec_.size();
}

// 运行图中上行运行线的数目
size_t TimetableConfig::up_missions_cnt() const {
  assert(up_departure_time_vec_.size() == up_stop_duration_vec_.size());
  return up_departure_time_vec_.size();
}

// 运行图中运行线的数目
size_t TimetableConfig::missions_cnt() const {
  return down_missions_cnt() + up_missions_cnt();
}

// 下行发车时刻序列
const first_departure_time_t &TimetableConfig::down_departure_time_vec() const {
  return down_departure_time_vec_;
}
first_departure_time_t &TimetableConfig::down_departure_time_vec() {
  return down_departure_time_vec_;
}
// 上行发车时刻序列
const first_departure_time_t &TimetableConfig::up_departure_time_vec() const {
  return up_departure_time_vec_;
}
first_departure_time_t &TimetableConfig::up_departure_time_vec() {
  return up_departure_time_vec_;
}

// 各条下行运行线的停站时长
const std::vector<std::map<station_id_t, second_t>> &
TimetableConfig::down_stop_duration_vec() const {
  return down_stop_duration_vec_;
}
std::vector<std::map<station_id_t, second_t>> &
TimetableConfig::down_stop_duration_vec() {
  return down_stop_duration_vec_;
}
// 各条上行运行线的停站时长
const std::vector<std::map<station_id_t, second_t>> &
TimetableConfig::up_stop_duration_vec() const {
  return up_stop_duration_vec_;
}
std::vector<std::map<station_id_t, second_t>> &
TimetableConfig::up_stop_duration_vec() {
  return up_stop_duration_vec_;
}

TimetableConfig::TimetableConfig() {
  init_basic_departure_time_sequence();
  init_basic_stop_duration(down_departure_time_vec_.size());
}

void TimetableConfig::init_basic_departure_time_sequence() {
  //  auto tp_epoch =
  //  std::chrono::system_clock::now().time_since_epoch().count(); static
  //  std::default_random_engine d_e(tp_epoch); static
  //  uniform_int_distribution<second_t> d_u(-30, 30);
  // 下行
  for (second_t curr_time = first_train_time_; curr_time < last_train_time_;) {
    // 将当前遍历到的时刻加入基本发车时刻序列
    down_departure_time_vec_.push_back(curr_time);
    // 遍历departure_T寻找当前循环的发车间隔
    second_t curr_departure_T;
    for (const auto &dt : departure_T_) {
      if (curr_time >= dt.first.first && curr_time < dt.first.second) {
        curr_departure_T = dt.second;
        break;
      }
    }
    curr_time += curr_departure_T;
    //    second_t rrr = d_u(d_e);
    //    curr_time += rrr;
  }

  // 上下行对开
  up_departure_time_vec_ = down_departure_time_vec_;
}

void TimetableConfig::init_basic_stop_duration(size_t missions_cnt) {
  down_stop_duration_vec_.reserve(missions_cnt);
  // 用map构造map
  fill_n(back_inserter(down_stop_duration_vec_), missions_cnt, stop_duration_);

  // 上下行对开
  up_stop_duration_vec_ = down_stop_duration_vec_;
}

void TimetableConfig::show() const {
  cout << ">>>> Down Standard First Departure Time <<<<" << endl;
  for (size_t i = 0; i != down_departure_time_vec_.size(); ++i) {
    cout << "[down] [index=" << i << "] [first departure time=["
         << down_departure_time_vec_.at(i) << "]" << endl;
  }
  cout << "<<<< Down Standard First Departure Time >>>>" << endl;

  cout << ">>>> Up Standard First Departure Time <<<<" << endl;
  for (size_t i = 0; i != up_departure_time_vec_.size(); ++i) {
    cout << "[up] [index=" << i << "] [first departure time=["
         << up_departure_time_vec_.at(i) << "]" << endl;
  }
  cout << "<<<< Up Standard First Departure Time >>>>" << endl;

  cout << ">>>> Down Standard Stop Duration <<<<" << endl;
  for (size_t i = 0; i != down_stop_duration_vec_.size(); ++i) {
    for (const auto &item : down_stop_duration_vec_.at(i)) {
      cout << "[down] [index=" << i << "] [station id=" << item.first
           << "] [stop duration=" << item.second << "] " << endl;
    }
  }
  cout << "<<<< Down Standard Stop Duration >>>>" << endl;

  cout << ">>>> Up Standard Stop Duration <<<<" << endl;
  for (size_t i = 0; i != up_stop_duration_vec_.size(); ++i) {
    for (const auto &item : up_stop_duration_vec_.at(i)) {
      cout << "[up] [index=" << i << "] [station id=" << item.first
           << "] [stop duration=" << item.second << "] " << endl;
    }
  }
  cout << "<<<< Up Standard Stop Duration >>>>" << endl;

  cout << ">>>> Stations Info <<<<" << endl;
  for (const auto &item : stations_) {
    cout << item << "\t";
  }
  cout << endl;
  cout << "<<<< Stations Info >>>>" << endl;

  cout << ">>>> Supply Arm Info <<<<" << endl;
  for (const auto &item : supply_arm_) {
    cout << "[station id =" << item.first << "] [supply arm=" << item.second
         << "]" << endl;
  }
  cout << "<<<< Supply Arm Info >>>>" << endl;

  cout << ">>>> Travel Duration Info <<<<" << endl;
  for (const auto &item : travel_duration_) {
    cout << "[from=" << item.first.first << "] [to=" << item.first.second
         << "] [duration=" << item.second << "]" << endl;
  }
  cout << "<<<< Travel Duration Info >>>>" << endl;

  cout << ">>>> Other Info <<<<" << endl;
  cout << "[produce_duration=" << produce_duration_ << "]" << endl;
  cout << "[consume_duration=" << consume_duration_ << "]" << endl;
  cout << "[first_train_time=" << first_train_time_ << "]" << endl;
  cout << "[last_train_time=" << last_train_time_ << "]" << endl;
  cout << "<<<< Other Info >>>>" << endl;
}

} // namespace yaohui