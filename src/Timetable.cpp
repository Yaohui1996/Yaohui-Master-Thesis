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

vector<Station>
Timetable::make_down_stations_vec(size_t down_id,
                                  const TimetableConfig &config) {
  // 第i条下行运行线的发车时刻
  auto arrive_time = config.down_basic_departure_time_sequence().at(down_id);
  // 保存结果
  vector<Station> seq;
  // 按照下行顺序遍历每一个车站
  for (auto iter = config.stations().cbegin(); iter != config.stations().cend();
       ++iter) {
    // 当前station id
    const auto &curr_id = *iter;
    // 当前车站停站时长
    const auto &curr_stop_dur =
        config.down_stop_duration_vec().at(down_id).at(curr_id);
    // 当前车站离站时刻
    const auto &de_time = arrive_time + curr_stop_dur;
    // 当前车站所属的供电臂id
    const auto &arm_id = config.supply_arm().at(curr_id);
    // 进入当前车站过程中, 产能开始时刻
    const auto &prod_beg_time = arrive_time - config.produce_duration();
    // 离开当前车站的过程中, 用能结束时刻
    const auto &cons_end_time = de_time + config.consume_duration();
    // 构造第i个station
    seq.emplace_back(curr_id, arm_id, prod_beg_time, arrive_time, de_time,
                     cons_end_time, curr_stop_dur);
    // 更新arrive_time
    if (iter + 1 != config.stations().cend()) {
      const auto &next_id = *(iter + 1); // 下一个station的id
      arrive_time = de_time + config.travel_duration().at({curr_id, next_id});
    }
  }
  return seq;
}

vector<Station> Timetable::make_up_stations_vec(size_t up_id,
                                                const TimetableConfig &config) {
  // 第i条上行运行线的发车时刻
  auto arrive_time = config.up_basic_departure_time_sequence().at(up_id);
  // 保存结果
  vector<Station> seq;
  // 按照上行顺序遍历每一个车站
  for (auto iter = config.stations().crbegin();
       iter != config.stations().crend(); ++iter) {
    // 当前station id
    const auto &curr_id = *iter;
    // 当前车站停站时长
    const auto &curr_stop_dur =
        config.up_stop_duration_vec().at(up_id).at(curr_id);
    // 当前车站离站时刻
    const auto &de_time = arrive_time + curr_stop_dur;
    // 当前车站所属的供电臂id
    const auto &arm_id = config.supply_arm().at(curr_id);
    // 进入当前车站过程中, 产能开始时刻
    const auto &prod_beg_time = arrive_time - config.produce_duration();
    // 离开当前车站的过程中, 用能结束时刻
    const auto &cons_end_time = de_time + config.consume_duration();
    // 构造第i个station
    seq.emplace_back(curr_id, arm_id, prod_beg_time, arrive_time, de_time,
                     cons_end_time, curr_stop_dur);
    // 更新arrive_time
    if (iter + 1 != config.stations().crend()) {
      const auto &next_id = *(iter + 1); // 下一个station的id
      arrive_time = de_time + config.travel_duration().at({curr_id, next_id});
    }
  }
  return seq;
}

vector<Interval>
Timetable::make_down_intervals_vec(const vector<Station> &stations_seq) {
  return make_intervals_vec(stations_seq);
}

vector<Interval>
Timetable::make_up_intervals_vec(const vector<Station> &stations_seq) {
  return make_intervals_vec(stations_seq);
}
vector<Interval>
Timetable::make_intervals_vec(const vector<Station> &stations_seq) {
  vector<Interval> seq;
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
  return seq;
}

Timetable::Timetable(const TimetableConfig &config) {
  // 保存结果
  vector<Mission> missions_vec;
  mission_id_t mission_cnt = 0;
  // 生成下行运行线
  vector<Mission> down_missions;
  //预分配内存
  down_missions.reserve(config.down_missions_cnt() + config.up_missions_cnt());
  for (size_t i = 0; i != config.down_missions_cnt(); ++i) {
    // 制作第i条下行运行线的vector<Station>序列
    vector<Station> station_seq = make_down_stations_vec(i, config);
    // 制作第i条下行运行线的vector<Interval>序列
    vector<Interval> interval_seq = make_down_intervals_vec(station_seq);
    down_missions.emplace_back(mission_cnt++, true, station_seq, interval_seq);
  }

  // 生成上行运行线
  vector<Mission> up_missions;
  up_missions.reserve(config.up_missions_cnt()); // 预分配内存
  for (size_t i = 0; i != config.up_missions_cnt(); ++i) {
    // 制作第i条上行运行线的vector<Station>序列
    vector<Station> station_seq = make_up_stations_vec(i, config);
    // 制作第i条上行运行线的vector<Interval>序列
    vector<Interval> interval_seq = make_up_intervals_vec(station_seq);
    up_missions.emplace_back(mission_cnt++, false, station_seq, interval_seq);
  }

  // 组合上下行数据
  down_missions.insert(down_missions.end(), up_missions.begin(),
                       up_missions.end());
  // timetable构造结束
  missions_ = std::move(down_missions);
}

Timetable::Timetable(timetable_id_t id, std::vector<Mission> missions)
    : timetable_id_(id), missions_(std::move(missions)) {}

const vector<Mission> &Timetable::missions() const { return missions_; }

Timetable::plot_data_t Timetable::get_plot_data() const {
  Timetable::plot_data_t plot_data;
  for (const Mission &m : missions_) {
    plot_data.emplace_back(m.plot_info_mission());
  }
  return plot_data;
}

std::map<Timetable::supply_arm_id_t,
         std::vector<std::pair<Timetable::second_t, Timetable::second_t>>>
Timetable::energy_produce_distribution_helper() const {
  // 供电臂用能map
  // key: 供电臂id
  // value: 供电臂id对应的用能区间构成的vector
  map<supply_arm_id_t, vector<pair<second_t, second_t>>> produce_map;
  for (const auto &mission : missions_) {
    for (const auto &interval : mission.intervals()) {
      auto finder = produce_map.find(interval.produce_supply_arm_id());
      if (finder == produce_map.end()) {
        produce_map.insert(std::make_pair(
            interval.produce_supply_arm_id(),
            std::vector<std::pair<second_t, second_t>>{std::make_pair(
                interval.produce_begin_time(), interval.produce_end_time())}));
      } else {
        finder->second.emplace_back(interval.produce_begin_time(),
                                    interval.produce_end_time());
      }
    }
  }
  return std::move(produce_map);
}

map<Timetable::supply_arm_id_t,
    vector<pair<Timetable::second_t, Timetable::second_t>>>
Timetable::energy_consume_distribution_helper() const {
  // 供电臂用能map
  // key: 供电臂id
  // value: 供电臂id对应的用能区间构成的vector
  map<supply_arm_id_t, vector<pair<second_t, second_t>>> consume_map;
  for (const auto &mission : missions_) {
    for (const auto &interval : mission.intervals()) {
      // 用能阶段
      auto finder = consume_map.find(interval.consume_supply_arm_id());
      if (finder == consume_map.end()) {
        consume_map.insert(std::make_pair(
            interval.consume_supply_arm_id(),
            std::vector<std::pair<second_t, second_t>>{std::make_pair(
                interval.consume_beg_time(), interval.consume_end_time())}));
      } else {
        finder->second.emplace_back(interval.consume_beg_time(),
                                    interval.consume_end_time());
      }
    }
  }
  return std::move(consume_map);
}

// 用能分布
map<Timetable::supply_arm_id_t, vector<Timetable::joule_t>>
Timetable::energy_consume_distribution() const {
  auto consume_map = energy_consume_distribution_helper();
  map<supply_arm_id_t, vector<joule_t>>
      consume_distribution; // 各个供电臂一天内的用能分布
  for (const auto &item : consume_map) {
    // 首先插入一个初始化k-v对
    consume_distribution.insert(
        make_pair(item.first, vector<joule_t>(129600, 0.0)));
    auto pointer = consume_distribution.find(item.first);
    // 然后开始累计能量
    for (const auto &beg_end_time_pair : item.second) {
      for (size_t i = beg_end_time_pair.first; i != beg_end_time_pair.second;
           ++i) {
        size_t curr_index = i - beg_end_time_pair.first;
        (pointer->second).at(i) +=
            consume_vec_.at(curr_index); // 增加consume_vec_.at(curr_index)千焦
        //        (pointer->second).at(i) += 1.0; // 能量增加1.0焦耳
      }
    }
  }
  return std::move(consume_distribution);
}

// 产能分布
map<Timetable::supply_arm_id_t, vector<Timetable::joule_t>>
Timetable::energy_produce_distribution() const {
  auto produce_map = energy_produce_distribution_helper();
  map<supply_arm_id_t, vector<joule_t>>
      produce_distribution; // 各个供电臂一天内的产能分布
  for (const auto &item : produce_map) {
    // 首先插入一个初始化k-v对
    produce_distribution.insert(
        make_pair(item.first, vector<joule_t>(129600, 0.0)));
    auto pointer = produce_distribution.find(item.first);
    // 然后开始累计能量
    for (const auto &beg_end_time_pair : item.second) {
      for (size_t i = beg_end_time_pair.first; i != beg_end_time_pair.second;
           ++i) {
        size_t curr_index = i - beg_end_time_pair.first;
        (pointer->second).at(i) +=
            produce_vec_.at(curr_index); // 增加produce_vec_.at(curr_index)千焦
        //        (pointer->second).at(i) += 1.0; // 能量增加1.0焦耳
      }
    }
  }
  return std::move(produce_distribution);
}

// 各个供电臂的总能量利用率
double Timetable::total_reuse_ratio() const {
  auto energy_produce_distribution = this->energy_produce_distribution();
  auto energy_consume_distribution = this->energy_consume_distribution();

  double total_produce_energy = 0.0;
  double total_reuse_energy = 0.0;
  for (const auto &consume_kv : energy_consume_distribution) {
    const auto curr_supply_arm = consume_kv.first;
    const auto &curr_consume_v = consume_kv.second;
    auto finder = energy_produce_distribution.find(curr_supply_arm);
    const auto &curr_produce_v = finder->second;

    // 当前供电臂的总产能
    //    double curr_arm_produce_energy =
    //        accumulate(curr_produce_v.begin(), curr_produce_v.end(), 0.0);
    double curr_arm_produce_energy = 0.0;
    // 当前供电臂重利用的能量
    double curr_arm_reuse_energy = 0.0;
    for (size_t i = 0; i != curr_produce_v.size(); ++i) {
      // 总产能
      curr_arm_produce_energy += curr_produce_v[i];
      // 产能-用能的差值
      double produce_minus_consume = curr_produce_v[i] - curr_consume_v[i];
      curr_arm_reuse_energy +=
          (produce_minus_consume > 0 ? produce_minus_consume : 0.0);
    }
    // 将计算结果累计
    total_produce_energy += curr_arm_produce_energy;
    total_reuse_energy += curr_arm_reuse_energy;
  }
  //  cout << "总产能: " << total_produce_energy
  //       << " 总重利用: " << total_reuse_energy << endl;
  return total_reuse_energy / total_produce_energy;
}

map<Timetable::supply_arm_id_t, double> Timetable::reuse_ratio() const {
  auto energy_produce_distribution = this->energy_produce_distribution();
  auto energy_consume_distribution = this->energy_consume_distribution();

  // 分别计算各个供电臂的能量利用率
  map<supply_arm_id_t, double> reuse_ratio;
  for (const auto &consume_kv : energy_consume_distribution) {
    auto curr_supply_arm = consume_kv.first;
    auto curr_consume_v = consume_kv.second;
    //        cout << "当前处理供电臂: " << curr_supply_arm << endl;
    auto finder = energy_produce_distribution.find(curr_supply_arm);
    auto curr_produce_v = finder->second;

    // 当前供电臂总产能
    double total_produce_energy =
        accumulate(curr_produce_v.begin(), curr_produce_v.end(), 0.0);

    // 当前供电臂利用上的能量
    double reuse_energy = 0.0;
    for (size_t i = 0; i != curr_produce_v.size(); ++i) {
      // 产能-用能的差值
      double produce_minus_consume = curr_produce_v[i] - curr_consume_v[i];
      reuse_energy += (produce_minus_consume > 0 ? produce_minus_consume : 0.0);
    }

    // 利用率
    //    cout << "供电臂: " << curr_supply_arm << " 总产能: " <<
    //    total_produce_energy
    //         << " 重利用: " << reuse_energy << endl;
    double ratio = reuse_energy / total_produce_energy;
    reuse_ratio.insert(make_pair(curr_supply_arm, ratio));
  }
  return reuse_ratio;
}

void Timetable::output_consume_distribution(
    const string &output_dir_path) const {
  auto energy_consume_distribution = this->energy_consume_distribution();
  for (const auto &kv : energy_consume_distribution) {
    string supply_arm_id = to_string(kv.first);
    string out_file_path;
    out_file_path.append(output_dir_path);
    out_file_path.append("consume_supply_");
    out_file_path.append(supply_arm_id);
    out_file_path.append("_.txt");
    ofstream of(out_file_path, ofstream::out);
    if (!of.is_open()) {
      cout << "打开文件" << out_file_path << "失败! " << endl;
    }

    for (const auto &item : kv.second) {
      of << to_string(item) << "\n";
    }
    of.close();
  }
}
void Timetable::output_produce_distribution(
    const string &output_dir_path) const {
  auto energy_produce_distribution = this->energy_produce_distribution();
  for (const auto &kv : energy_produce_distribution) {
    string supply_arm_id = to_string(kv.first);
    string out_file_path;
    out_file_path.append(output_dir_path);
    out_file_path.append("produce_supply_");
    out_file_path.append(supply_arm_id);
    out_file_path.append("_.txt");
    ofstream of(out_file_path, ofstream::out);
    if (!of.is_open()) {
      cout << "打开文件" << out_file_path << "失败! " << endl;
    }

    for (const auto &item : kv.second) {
      of << to_string(item) << "\n";
    }
    of.close();
  }
}

void Timetable::write_to_file() const {
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
  ofstream of("./timetable.json", fstream::out);
  if (!of.is_open()) {
    cout << "打开文件[timetable.json]失败! " << endl;
  }
  of << output_str;
  of.close();
}

} // namespace yaohui

//{
//  "timetable_id":0,
//      "missions":[
//        {
//          "mission_id":0,
//          "is_down_direction":"true",
//          "stations_seq":[
//            {
//              "station_id":0,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":1,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":2,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":3,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//          ],
//          "intervals_seq":[
//            {
//              "interval_id":0,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":1,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":2,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":3,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//          ]
//        },
//        {
//          "mission_id":0,
//          "is_down_direction":"true",
//          "stations_seq":[
//            {
//              "station_id":0,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":1,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":2,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":3,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//          ],
//          "intervals_seq":[
//            {
//              "interval_id":0,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":1,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":2,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":3,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//          ]
//        },
//        {
//          "mission_id":0,
//          "is_down_direction":"true",
//          "stations_seq":[
//            {
//              "station_id":0,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":1,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":2,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":3,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//          ],
//          "intervals_seq":[
//            {
//              "interval_id":0,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":1,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":2,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":3,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//          ]
//        },
//        {
//          "mission_id":0,
//          "is_down_direction":"true",
//          "stations_seq":[
//            {
//              "station_id":0,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":1,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":2,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":3,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//          ],
//          "intervals_seq":[
//            {
//              "interval_id":0,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":1,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":2,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":3,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//          ]
//        },
//        {
//          "mission_id":0,
//          "is_down_direction":"true",
//          "stations_seq":[
//            {
//              "station_id":0,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":1,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":2,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//            {
//              "station_id":3,
//              "supply_arm_id":0,
//              "produce_begin_time":132,
//              "arrive_time":765,
//              "departure_time":13214,
//              "consume_end_time":,54334
//              "stop_duration":412376783
//            },
//          ],
//          "intervals_seq":[
//            {
//              "interval_id":0,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":1,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":2,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//            {
//              "interval_id":3,
//              "consume_supply_arm_id":3,
//              "consume_beg_time":123142,
//              "consume_end_time":812537812,
//              "produce_supply_arm_id":3,
//              "produce_begin_time":8125738,
//              "produce_end_time":2167378
//            },
//          ]
//        }
//      ]
//}