#include "Timetable.hpp"
#include "TimetableConfig.hpp"
#include <fstream>
#include <json.hpp>
#include <map>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace nlohmann;

namespace yaohui {

const std::vector<Mission> &Timetable::missions() const { return missions_; }
vector<Station> Timetable::make_down_stations_vec(size_t down_id) {
  // 第i条下行运行线的发车时刻
  auto arrive_time = config_.down_departure_time_vec().at(down_id);
  // 保存结果
  vector<Station> seq;
  seq.reserve(config_.stations().size());
  // 按照下行顺序遍历每一个车站
  for (auto iter = config_.stations().cbegin();
       iter != config_.stations().cend(); ++iter) {
    // 当前station id
    station_id_t curr_id = *iter;
    // 当前车站停站时长
    second_t curr_stop_dur =
        config_.down_stop_duration_vec().at(down_id).at(curr_id);
    // 当前车站离站时刻
    second_t de_time = arrive_time + curr_stop_dur;
    // 当前车站所属的供电臂id
    supply_arm_id_t arm_id = config_.supply_arm().at(curr_id);
    // 进入当前车站过程中, 产能开始时刻
    second_t prod_beg_time = arrive_time - config_.produce_duration();
    // 离开当前车站的过程中, 用能结束时刻
    second_t cons_end_time = de_time + config_.consume_duration();
    // 构造第i个station
    seq.emplace_back(curr_id, arm_id, prod_beg_time, arrive_time, de_time,
                     cons_end_time, curr_stop_dur);
    // 更新arrive_time
    if (iter + 1 != config_.stations().cend()) {
      station_id_t next_id = *(iter + 1); // 下一个station的id
      arrive_time = de_time + config_.travel_duration().at({curr_id, next_id});
    }
  }
  return std::move(seq);
}

vector<Station> Timetable::make_up_stations_vec(size_t up_id) {
  // 第i条上行运行线的发车时刻
  second_t arrive_time = config_.up_departure_time_vec().at(up_id);
  // 保存结果
  vector<Station> seq;
  seq.reserve(config_.stations().size());
  // 按照上行顺序遍历每一个车站
  for (auto iter = config_.stations().crbegin();
       iter != config_.stations().crend(); ++iter) {
    // 当前station id
    station_id_t curr_id = *iter;
    // 当前车站停站时长
    second_t curr_stop_dur =
        config_.up_stop_duration_vec().at(up_id).at(curr_id);
    // 当前车站离站时刻
    second_t de_time = arrive_time + curr_stop_dur;
    // 当前车站所属的供电臂id
    supply_arm_id_t arm_id = config_.supply_arm().at(curr_id);
    // 进入当前车站过程中, 产能开始时刻
    second_t prod_beg_time = arrive_time - config_.produce_duration();
    // 离开当前车站的过程中, 用能结束时刻
    second_t cons_end_time = de_time + config_.consume_duration();
    // 构造第i个station
    seq.emplace_back(curr_id, arm_id, prod_beg_time, arrive_time, de_time,
                     cons_end_time, curr_stop_dur);
    // 更新arrive_time
    if (iter + 1 != config_.stations().crend()) {
      station_id_t next_id = *(iter + 1); // 下一个station的id
      arrive_time = de_time + config_.travel_duration().at({curr_id, next_id});
    }
  }
  return std::move(seq);
}

vector<Interval>
Timetable::make_down_intervals_vec(const vector<Station> &stations_seq) {
  return std::move(make_intervals_vec(stations_seq));
}

vector<Interval>
Timetable::make_up_intervals_vec(const vector<Station> &stations_seq) {
  return std::move(make_intervals_vec(stations_seq));
}
vector<Interval>
Timetable::make_intervals_vec(const vector<Station> &stations_seq) {
  vector<Interval> seq;
  seq.reserve(stations_seq.size());
  // for each station
  for (size_t i = 0; i != stations_seq.size() - 1; ++i) {
    const Station &front_station = stations_seq.at(i);    // 当前的station
    const Station &back_station = stations_seq.at(i + 1); // 下一个车站
    seq.emplace_back(
        std::make_pair(front_station.station_id(), back_station.station_id()),
        front_station.supply_arm_id(), front_station.consume_beg_time(),
        front_station.consume_end_time(), back_station.supply_arm_id(),
        back_station.produce_begin_time(), back_station.produce_end_time());
  }
  return std::move(seq);
}

Timetable::Timetable(const TimetableConfig &config) : config_(config) {
  // 预分配内存
  missions_.clear();
  this->missions_.reserve(config_.down_missions_cnt() +
                          config_.up_missions_cnt());
  mission_id_t mission_cnt = 0; // 运行线id

  // 生成下行运行线
  for (size_t i = 0; i != config_.down_missions_cnt(); ++i) {
    // 制作第i条下行运行线的vector<Station>序列
    vector<Station> station_seq = make_down_stations_vec(i);
    // 制作第i条下行运行线的vector<Interval>序列
    vector<Interval> interval_seq = make_down_intervals_vec(station_seq);
    this->missions_.emplace_back(mission_cnt++, true, std::move(station_seq),
                                 std::move(interval_seq));
  }

  // 生成上行运行线
  for (size_t i = 0; i != config_.up_missions_cnt(); ++i) {
    // 制作第i条上行运行线的vector<Station>序列
    vector<Station> station_seq = make_up_stations_vec(i);
    // 制作第i条上行运行线的vector<Interval>序列
    vector<Interval> interval_seq = make_up_intervals_vec(station_seq);
    this->missions_.emplace_back(mission_cnt++, false, std::move(station_seq),
                                 std::move(interval_seq));
  }
}

tb_plot_data_t Timetable::get_plot_data() const {
  tb_plot_data_t plot_data;
  for (const Mission &m : missions_) {
    plot_data.emplace_back(m.plot_info_mission());
  }
  return plot_data;
}

std::pair<energy_map_t, energy_map_t>
Timetable::energy_exchange_duration() const {
  energy_map_t consume_map;
  energy_map_t produce_map;
  for (const Mission &mission : missions_) {
    for (const Interval &interval : mission.intervals()) {
      // 用能阶段
      auto finder_c = consume_map.find(interval.consume_supply_arm_id());
      if (finder_c == consume_map.end()) {
        consume_map.insert(std::make_pair(
            interval.consume_supply_arm_id(),
            std::vector<std::pair<second_t, second_t>>{std::make_pair(
                interval.consume_beg_time(), interval.consume_end_time())}));
      } else {
        finder_c->second.emplace_back(interval.consume_beg_time(),
                                      interval.consume_end_time());
      }
      // 产能阶段
      auto finder_p = produce_map.find(interval.produce_supply_arm_id());
      if (finder_p == produce_map.end()) {
        produce_map.insert(std::make_pair(
            interval.produce_supply_arm_id(),
            std::vector<std::pair<second_t, second_t>>{std::make_pair(
                interval.produce_begin_time(), interval.produce_end_time())}));
      } else {
        finder_p->second.emplace_back(interval.produce_begin_time(),
                                      interval.produce_end_time());
      }
    }
  }
  return std::make_pair(std::move(consume_map), std::move(produce_map));
}

// 各个供电臂的产能分布和各个供电臂的用能分布
std::pair<energy_distribution_t, energy_distribution_t>
Timetable::energy_distribution() const {
  auto energy_exchange_duration = this->energy_exchange_duration();
  const auto &consume_map = energy_exchange_duration.first;
  const auto &produce_map = energy_exchange_duration.second;

  // 各个供电臂一个运行图周期内的用能分布
  map<supply_arm_id_t, vector<joule_t>> consume_distribution;
  for (const auto &p : consume_map) {
    supply_arm_id_t curr_arm_id = p.first;
    // 首先插入一个初始化k-v对
    consume_distribution.insert(
        make_pair(curr_arm_id, vector<joule_t>(100000, 0.0)));
    auto finder = consume_distribution.find(curr_arm_id);
    // 然后开始累计能量
    for (const auto &beg_end_time_pair : p.second) {
      second_t beg_time = beg_end_time_pair.first;
      second_t end_time = beg_end_time_pair.second;
      for (size_t i = beg_time; i != end_time; ++i) {
        size_t curr_index = i - beg_time;
        // 增加consume_vec_.at(curr_index)千焦
        (finder->second).at(i) += config_.consume_vec().at(curr_index);
      }
    }
  }

  // 各个供电臂一个运行图周期内的产能情况
  map<supply_arm_id_t, vector<joule_t>> produce_distribution;
  for (const auto &p : produce_map) {
    supply_arm_id_t curr_arm_id = p.first;
    // 首先插入一个初始化k-v对
    produce_distribution.insert(
        make_pair(curr_arm_id, vector<joule_t>(100000, 0.0)));
    auto finder = produce_distribution.find(curr_arm_id);
    // 然后开始累计能量
    for (const auto &beg_end_time_pair : p.second) {
      second_t beg_time = beg_end_time_pair.first;
      second_t end_time = beg_end_time_pair.second;
      for (size_t i = beg_time; i != end_time; ++i) {
        size_t curr_index = i - beg_time;
        // 增加produce_vec_.at(curr_index)千焦
        (finder->second).at(i) += config_.produce_vec().at(curr_index);
      }
    }
  }
  return std::make_pair(std::move(consume_distribution),
                        std::move(produce_distribution));
}

// 各个供电臂的总能量利用率
double Timetable::total_reuse_ratio() const {
  auto energy_distribution = this->energy_distribution();
  const auto &energy_consume_distribution = energy_distribution.first;
  const auto &energy_produce_distribution = energy_distribution.second;
  double total_produce_energy = 0.0;
  double total_reuse_energy = 0.0;
  for (const auto &consume_kv : energy_consume_distribution) {
    const auto curr_supply_arm = consume_kv.first;
    const auto &curr_consume_v = consume_kv.second;
    auto finder = energy_produce_distribution.find(curr_supply_arm);
    const auto &curr_produce_v = finder->second;
    // 当前供电臂的总产能
    double curr_arm_produce_energy = 0.0;
    // 当前供电臂重利用的能量
    double curr_arm_reuse_energy = 0.0;
    for (size_t i = 0; i != curr_produce_v.size(); ++i) {
      // 总产能
      curr_arm_produce_energy += curr_produce_v[i];
      // 再利用的能量
      curr_arm_reuse_energy +=
          (curr_produce_v[i] > curr_consume_v[i] ? curr_consume_v.at(i)
                                                 : curr_produce_v.at(i));
    }
    // 将计算结果累计
    total_produce_energy += curr_arm_produce_energy;
    total_reuse_energy += curr_arm_reuse_energy;
  }
  return total_reuse_energy / total_produce_energy;
}

void Timetable::output_energy_distribution(std::string pre_name) const {
  auto energy_distribution = this->energy_distribution();
  const auto &energy_consume_distribution = energy_distribution.first;
  const auto &energy_produce_distribution = energy_distribution.second;
  // 输出用能曲线
  for (const auto &kv : energy_consume_distribution) {
    supply_arm_id_t curr_arm_id = kv.first;
    string curr_arm_id_str = to_string(curr_arm_id);
    string out_file_name = pre_name + "-";
    out_file_name.append("consume-supply-id-");
    out_file_name.append(curr_arm_id_str);
    out_file_name.append(".csv");
    ofstream of(out_file_name, std::ios::out);
    if (!of.is_open()) {
      std::cout << "Failed to open [" << out_file_name << "] !" << std::endl;
    }

    for (const auto &item : kv.second) {
      of << to_string(item) << "\n";
    }
    of.close();
    std::cout << "Save file [" << out_file_name << "] successful!" << std::endl;
  }
  // 输出产能曲线
  for (const auto &kv : energy_produce_distribution) {
    supply_arm_id_t curr_arm_id = kv.first;
    string curr_arm_id_str = to_string(curr_arm_id);
    string out_file_name = pre_name + "-";
    out_file_name.append("produce-supply-id-");
    out_file_name.append(curr_arm_id_str);
    out_file_name.append(".csv");
    ofstream of(out_file_name, std::ios::out);
    if (!of.is_open()) {
      std::cout << "Failed to open [" << out_file_name << "] !" << std::endl;
    }

    for (const auto &item : kv.second) {
      of << to_string(item) << "\n";
    }
    of.close();
    std::cout << "Save file [" << out_file_name << "] successful!" << std::endl;
  }
}

void Timetable::write_to_file(std::string json_name) const {
  json timetable_j;
  timetable_j["timetable_id"] = timetable_id_;

  json missions_j;
  // 遍历每一条运行线
  for (const auto &mission : missions_) {
    json mission_j;
    mission_j["mission_id"] = mission.id();
    mission_j["is_down_direction"] = mission.is_down_direction();
    // 遍历当前运行线的每一个车站
    json station_seq_j;
    for (const auto &station : mission.stations()) {
      json station_info_j;
      station_info_j["station_id"] = station.station_id();       // 车站id
      station_info_j["supply_arm_id"] = station.supply_arm_id(); // 所属供电臂id
      station_info_j["produce_begin_time"] =
          station.produce_begin_time(); // 产能开始时刻
      station_info_j["arrive_time"] =
          station.arrive_time(); // 进站时刻-秒(产能结束时刻)
      station_info_j["departure_time"] =
          station.departure_time(); // 离站时刻-秒(用能开始时刻)
      station_info_j["consume_end_time"] =
          station.consume_end_time(); // 用能结束时刻
      station_info_j["stop_duration"] = station.stop_duration(); // 停站时长
      station_seq_j.push_back(station_info_j);
    }
    mission_j["stations_seq"] = station_seq_j;
    // 遍历当前运行线的每一个区间
    json interval_seq_j;
    for (const auto &interval : mission.intervals()) {
      json interval_info_j;
      interval_info_j["interval_id_first"] =
          interval.interval_id_first(); // 行车区间id first
      interval_info_j["interval_id_second"] =
          interval.interval_id_second(); // 行车区间id second
      interval_info_j["consume_supply_arm_id"] =
          interval.consume_supply_arm_id(); // 用能阶段所属供电臂
      interval_info_j["consume_beg_time"] =
          interval.consume_beg_time(); // 用能开始时刻
      interval_info_j["consume_end_time"] =
          interval.consume_end_time(); // 用能结束时刻
      interval_info_j["produce_supply_arm_id"] =
          interval.produce_supply_arm_id(); // 产能阶段所属供电臂
      interval_info_j["produce_begin_time"] =
          interval.produce_begin_time(); // 产能开始时刻
      interval_info_j["produce_end_time"] =
          interval.produce_end_time(); // 产能结束时刻
      interval_seq_j.push_back(interval_info_j);
    }
    mission_j["intervals_seq"] = interval_seq_j;
    missions_j.push_back(mission_j);
  }
  timetable_j["missions"] = missions_j;

  // 输出到文件
  string output_str = timetable_j.dump(2); // 两个空格缩进
  ofstream of(json_name, fstream::out);
  if (!of.is_open()) {
    std::cout << "Failed to open [" << json_name << "] !" << std::endl;
  }
  of << output_str;
  of.close();
  std::cout << "Save file [" << json_name << "] successful!" << std::endl;
}

void Timetable::output_plot_data(std::string f_name) const {
  // 运行图画图数据输出到文件
  auto plot_info = this->get_plot_data();
  // 输出到文件
  stringstream out_ss;
  for (const auto &m : plot_info) {
    for (size_t i = 0; i != m.size(); ++i) {
      out_ss << m[i].first << "," << m[i].second;
      if (i != m.size() - 1) {
        out_ss << ",";
      }
    }
    out_ss << "\n";
  }

  fstream out_file;
  out_file.open(f_name, ios::out);
  if (!out_file.is_open()) {
    std::cout << "Failed to open [" << f_name << "] !" << std::endl;
  }
  out_file << out_ss.str();
  out_file.close();
  std::cout << "Save file [" << f_name << "] successful!" << std::endl;
}

} // namespace yaohui
