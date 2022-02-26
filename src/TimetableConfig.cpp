#include "TimetableConfig.hpp"
#include <chrono>
#include <map>
#include <random>
#include <utility>
#include <vector>

using namespace std;

namespace yaohui {

TimetableConfig::second_t TimetableConfig::first_train_time() const { return first_train_time_; }
TimetableConfig::second_t TimetableConfig::last_train_time() const { return last_train_time_; }
TimetableConfig::second_t TimetableConfig::produce_duration() const { return produce_duration_; }
TimetableConfig::second_t TimetableConfig::consume_duration() const { return consume_duration_; }

const std::vector<TimetableConfig::station_id_t> &TimetableConfig::stations() const {
  return stations_;
}
const std::map<TimetableConfig::station_id_t, TimetableConfig::supply_arm_id_t> &
TimetableConfig::supply_arm() const {
  return supply_arm_;
}
const std::map<TimetableConfig::interval_id_t, TimetableConfig::second_t> &
TimetableConfig::travel_duration() const {
  return travel_duration_;
}
const std::map<TimetableConfig::station_id_t, TimetableConfig::second_t> &TimetableConfig::stop_duration() const {
  return stop_duration_;
}
const std::map<std::pair<TimetableConfig::second_t, TimetableConfig::second_t>, TimetableConfig::second_t> &
TimetableConfig::departure_T() const {
  return departure_T_;
}

TimetableConfig::first_departure_time_t &
TimetableConfig::make_basic_departure_time_sequence(
    TimetableConfig::first_departure_time_t &basic_departure_time_seq) {
  //  basic_departure_time_seq.clear(); // 清理数据成员
  // 保存结果
  for (second_t curr_time = first_train_time_; curr_time < last_train_time_;) {
    // 将当前遍历到的时刻加入基本发车时刻序列
    basic_departure_time_seq.push_back(curr_time);
    // 遍历departure_T寻找当前循环的发车间隔
    second_t curr_departure_T;
    for (const auto &dt : departure_T_) {
      if (curr_time >= dt.first.first && curr_time < dt.first.second) {
        curr_departure_T = dt.second;
        break;
      }
    }
    curr_time += curr_departure_T;
  }
  return basic_departure_time_seq;
}

TimetableConfig::each_stop_duration_t &
TimetableConfig::make_basic_stop_duration(
    TimetableConfig::each_stop_duration_t &stop_duration_vec,
    size_t missions_cnt) {
  stop_duration_vec.reserve(missions_cnt); // 预先分配内存
  for (auto i = 0; i != missions_cnt; ++i) {
    stop_duration_vec.emplace_back(stop_duration_); // 用map构造map
  }
  return stop_duration_vec;
}

void TimetableConfig::show() const {
  cout << ">>>> Down Standard First Departure Time <<<<" << endl;
  for (size_t i = 0; i != down_basic_departure_time_sequence_.size(); ++i) {
    cout << "[down] [index=" << i << "] [first departure time=["
         << down_basic_departure_time_sequence_.at(i) << "]" << endl;
  }
  cout << "<<<< Down Standard First Departure Time >>>>" << endl;

  cout << ">>>> Up Standard First Departure Time <<<<" << endl;
  for (size_t i = 0; i != up_basic_departure_time_sequence_.size(); ++i) {
    cout << "[up] [index=" << i << "] [first departure time=["
         << up_basic_departure_time_sequence_.at(i) << "]" << endl;
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

void TimetableConfig::random_offset_down_first_departure_time_vec(
    second_t departure_time_offset_range) {
  auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static default_random_engine e(tp_epoch);
  static uniform_int_distribution<second_t> u(-departure_time_offset_range,
                                              departure_time_offset_range);
  for (auto &item : down_basic_departure_time_sequence_) {
    item += u(e); // 调整
  }
}

void TimetableConfig::random_offset_up_first_departure_time_vec(
    second_t departure_time_offset_range) {
  auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static default_random_engine e(tp_epoch);
  static uniform_int_distribution<second_t> u(-departure_time_offset_range,
                                              departure_time_offset_range);
  for (auto &item : up_basic_departure_time_sequence_) {
    item += u(e); // 调整
  }
}

// 随机调整各条下行运行线的停站时长
void TimetableConfig::random_offset_down_stop_duration_vec(
    second_t stop_duration_offset_range) {
  auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static default_random_engine e(tp_epoch);
  static uniform_int_distribution<second_t> u(-stop_duration_offset_range,
                                              stop_duration_offset_range);

  for (auto &item : down_stop_duration_vec_) {
    for (auto &p : item) {
      if (!(p.first == 0 || p.first == 15)) {
        (p.second) += u(e); // 调整
      }
    }
  }
}

// 随机调整各条上行运行线的停站时长
void TimetableConfig::random_offset_up_stop_duration_vec(
    second_t stop_duration_offset_range) {
  auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static default_random_engine e(tp_epoch);
  static uniform_int_distribution<second_t> u(-stop_duration_offset_range,
                                              stop_duration_offset_range);

  for (auto &item : up_stop_duration_vec_) {
    for (auto &p : item) {
      if (!(p.first == 0 || p.first == 15)) {
        (p.second) += u(e); // 调整
      }
    }
  }
}

} // namespace yaohui