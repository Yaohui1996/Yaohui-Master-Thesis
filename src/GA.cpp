#include "GA.hpp"
//#include "Population.hpp"
#include "Individual.hpp"
#include "Timetable.hpp"
#include <algorithm>
#include <chrono>
#include <iterator>
#include <map>
#include <random>
#include <utility>
#include <vector>
using namespace std;

namespace yaohui {

vector<TimetableConfig>
GA::generate_default_population(std::size_t population_sz) {
  vector<TimetableConfig> default_config_vec;
  default_config_vec.reserve(population_sz); // 预分配内存
  TimetableConfig default_config;            // 构造母版
  // 从母版构造config
  fill_n(back_inserter(default_config_vec), population_sz, default_config);
  return std::move(default_config_vec);
}

vector<TimetableConfig> &
GA::random_offset_population(vector<TimetableConfig> &population,
                             second_t departure_time_offset_range,
                             second_t stop_duration_offset_range) {
  for (auto &config : population) {
    // 随机调整下行标准发车时刻序列
    config.random_offset_down_first_departure_time_vec(
        departure_time_offset_range);
    // 随机调整上行标准发车时刻序列
    config.random_offset_up_first_departure_time_vec(
        departure_time_offset_range);
    // 随机调整各条下行运行线的停站时长
    config.random_offset_down_stop_duration_vec(stop_duration_offset_range);
    // 随机调整各条上行运行线的停站时长
    config.random_offset_up_stop_duration_vec(stop_duration_offset_range);
  }
  return population;
}

double GA::evaluate_fitness_score(const TimetableConfig &config) {
  Timetable temp_timetable(config);
  return temp_timetable.total_reuse_ratio();
}

vector<double>
GA::evaluate_fitness_score(const vector<TimetableConfig> &population) {
  vector<double> scores;
  scores.reserve(population.size()); // 预分配内存
  for (const auto &config : population) {
    Timetable temp_timetable(config);
    scores.push_back(temp_timetable.total_reuse_ratio());
  }
  return std::move(scores);
}

// vector<TimetableConfig>
// GA::random_generate_population(size_t population_sz,
//                                second_t departure_time_offset,
//                                second_t stop_duration_offset) {
//   vector<TimetableConfig> individual_vec;
//   individual_vec.reserve(population_sz); // 预分配内存
//   while (individual_vec.size() != population_sz) {
//     individual_vec.emplace_back(
//         Individual(departure_time_offset, stop_duration_offset));
//   }
//   return std::move(individual_vec);
// }

vector<Individual>
GA::select_individuals(size_t target_population_sz,
                       const vector<Individual> &alternatives) {
  // 首先计算累计概率
  double weight_cum = 0.0;
  map<pair<double, double>, size_t> m;
  for (size_t i = 0; i != alternatives.size(); ++i) {
    m.insert(make_pair(
        make_pair(weight_cum, weight_cum + alternatives[i].score()), i));
    weight_cum += alternatives[i].score();
  }
  //  cout << weight_cum << endl;

  auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static default_random_engine e(tp_epoch);
  static uniform_real_distribution<double> u(0, weight_cum);

  vector<Individual> ret;
  while (ret.size() != target_population_sz) {
    // 生成随机数, 根据随机数和累计概率map, 确定选择的个体
    double temp_random_real = u(e);
    for (const auto &p : m) {
      if (temp_random_real >= p.first.first &&
          temp_random_real <= p.first.second) {
        // 选出index = p.second的个体
        ret.push_back(alternatives.at(p.second));
        break;
      }
    }
  }
  return std::move(ret);
}

Individual GA::reproduce(const Individual &father, const Individual &mother) {
  assert(father.timetable_config().down_missions_cnt() ==
         mother.timetable_config().down_missions_cnt());
  assert(father.timetable_config().up_missions_cnt() ==
         mother.timetable_config().up_missions_cnt());

  size_t down_mission_cnt = father.timetable_config().down_missions_cnt();
  size_t up_mission_cnt = father.timetable_config().up_missions_cnt();
  auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static default_random_engine reproduce_e(tp_epoch);
  static uniform_int_distribution<size_t> down_u(0, down_mission_cnt);
  static uniform_int_distribution<size_t> up_u(0, up_mission_cnt);

  // 下行首班车发车时刻基因重组
  const auto &father_down_departure_time_vec =
      father.timetable_config().down_basic_departure_time_sequence();
  const auto &mother_down_departure_time_vec =
      mother.timetable_config().down_basic_departure_time_sequence();
  vector<int32_t>::size_type random_index = down_u(reproduce_e);
  vector<int32_t> new_down_departure_time_vec(
      father_down_departure_time_vec.begin(),
      father_down_departure_time_vec.begin() + random_index);
  new_down_departure_time_vec.insert(new_down_departure_time_vec.end(),
                                     mother_down_departure_time_vec.begin() +
                                         random_index,
                                     mother_down_departure_time_vec.end());
  // 上行首班车发车时刻基因重组
  const auto &father_up_departure_time_vec =
      father.timetable_config().up_basic_departure_time_sequence();
  const auto &mother_up_departure_time_vec =
      mother.timetable_config().up_basic_departure_time_sequence();
  random_index = up_u(reproduce_e);
  vector<int32_t> new_up_departure_time_vec(
      father_up_departure_time_vec.begin(),
      father_up_departure_time_vec.begin() + random_index);
  new_up_departure_time_vec.insert(new_up_departure_time_vec.end(),
                                   mother_up_departure_time_vec.begin() +
                                       random_index,
                                   mother_up_departure_time_vec.end());
  // 下行停站时长基因重组
  const auto &father_down_stop_duration_vec =
      father.timetable_config().down_stop_duration_vec();
  const auto &mother_down_stop_duration_vec =
      mother.timetable_config().down_stop_duration_vec();
  random_index = down_u(reproduce_e);
  vector<map<station_id_t, second_t>> new_down_stop_duration_vec(
      father_down_stop_duration_vec.begin(),
      father_down_stop_duration_vec.begin() + random_index);
  new_down_stop_duration_vec.insert(new_down_stop_duration_vec.end(),
                                    mother_down_stop_duration_vec.begin() +
                                        random_index,
                                    mother_down_stop_duration_vec.end());

  // 上行停站时长基因重组

  const auto &father_up_stop_duration_vec =
      father.timetable_config().up_stop_duration_vec();
  const auto &mother_up_stop_duration_vec =
      mother.timetable_config().up_stop_duration_vec();
  random_index = up_u(reproduce_e);
  vector<map<station_id_t, second_t>> new_up_stop_duration_vec(
      father_up_stop_duration_vec.begin(),
      father_up_stop_duration_vec.begin() + random_index);
  new_up_stop_duration_vec.insert(new_up_stop_duration_vec.end(),
                                  mother_up_stop_duration_vec.begin() +
                                      random_index,
                                  mother_up_stop_duration_vec.end());

  // 构造TimetableConfig
  return Individual(
      TimetableConfig(new_down_departure_time_vec, new_up_departure_time_vec,
                      new_down_stop_duration_vec, new_up_stop_duration_vec));
}

void GA::parents_cross(Individual &father, Individual &mother) {
  auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static std::default_random_engine cross_index_e(tp_epoch);
  TimetableConfig &father_tb_config = father.timetable_config();
  TimetableConfig &mother_tb_config = mother.timetable_config();

  auto &father_down_de = father_tb_config.down_basic_departure_time_sequence();
  auto &father_down_stop = father_tb_config.down_stop_duration_vec();
  auto &mother_down_de = mother_tb_config.down_basic_departure_time_sequence();
  auto &mother_down_stop = mother_tb_config.down_stop_duration_vec();
  assert(father_down_de.size() == father_down_stop.size());
  assert(father_down_de.size() == mother_down_de.size());
  assert(father_down_de.size() == mother_down_stop.size());

  static std::uniform_int_distribution<size_t> random_down_i(
      0, father_down_de.size());
  size_t rdi = random_down_i(cross_index_e);
  for (size_t i = 0; i != rdi; ++i) {
    std::swap(father_down_de.at(i), mother_down_de.at(i));
  }
  rdi = random_down_i(cross_index_e);
  for (size_t i = 0; i != rdi; ++i) {
    std::swap(father_down_stop.at(i), mother_down_stop.at(i));
  }

  auto &father_up_de = father_tb_config.up_basic_departure_time_sequence();
  auto &father_up_stop = father_tb_config.up_stop_duration_vec();
  auto &mother_up_de = mother_tb_config.up_basic_departure_time_sequence();
  auto &mother_up_stop = mother_tb_config.up_stop_duration_vec();
  assert(father_up_de.size() == father_up_stop.size());
  assert(father_up_de.size() == mother_up_de.size());
  assert(father_up_de.size() == mother_up_stop.size());

  static std::uniform_int_distribution<size_t> random_up_i(0,
                                                           father_up_de.size());
  rdi = random_up_i(cross_index_e);
  for (size_t i = 0; i != rdi; ++i) {
    std::swap(father_up_de.at(i), mother_up_de.at(i));
  }
  rdi = random_up_i(cross_index_e);
  for (size_t i = 0; i != rdi; ++i) {
    std::swap(father_up_stop.at(i), mother_up_stop.at(i));
  }

  // 分别计算father和mother的适应度
  father.score() = evaluate_fitness_score(father_tb_config);
  mother.score() = evaluate_fitness_score(mother_tb_config);
}

void GA::child_mutate(Individual &child) {
  auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static std::default_random_engine mutate_index_e(tp_epoch);
  TimetableConfig &tb_config = child.timetable_config();
  auto &down_de = tb_config.down_basic_departure_time_sequence();
  auto &down_stop = tb_config.down_stop_duration_vec();
  assert(down_de.size() == down_stop.size());

  auto &up_de = tb_config.up_basic_departure_time_sequence();
  auto &up_stop = tb_config.up_stop_duration_vec();
  assert(up_de.size() == up_stop.size());

  tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static default_random_engine mutate_sec_e(tp_epoch);
  static uniform_int_distribution<second_t> de_sec(-30, 30);
  static uniform_int_distribution<second_t> stop_sec(-5, 5);

  TimetableConfig default_config;

  // down departure
  static uniform_int_distribution<size_t> down_index_u(0, down_de.size() - 1);
  size_t rdi = down_index_u(mutate_index_e);
  auto temp_de_val =
      default_config.down_basic_departure_time_sequence().at(rdi);
  temp_de_val += de_sec(mutate_sec_e);
  down_de.at(rdi) = temp_de_val;

  // down stop
  rdi = down_index_u(mutate_index_e);
  auto temp_stop_val = default_config.down_stop_duration_vec().at(rdi);
  for (auto &p : temp_stop_val) {
    if (!(p.first == 0 || p.first == 15)) {
      (p.second) += stop_sec(mutate_sec_e); // 调整
    }
  }
  down_stop.at(rdi) = temp_stop_val;

  // up departure
  static uniform_int_distribution<size_t> up_index_u(0, up_de.size() - 1);
  rdi = up_index_u(mutate_index_e);
  temp_de_val = default_config.up_basic_departure_time_sequence().at(rdi);
  temp_de_val += de_sec(mutate_sec_e);
  up_de.at(rdi) = temp_de_val;

  // up stop
  rdi = up_index_u(mutate_index_e);
  temp_stop_val = default_config.up_stop_duration_vec().at(rdi);
  for (auto &p : temp_stop_val) {
    if (!(p.first == 0 || p.first == 15)) {
      (p.second) += stop_sec(mutate_sec_e); // 调整
    }
  }
  up_stop.at(rdi) = temp_stop_val;

  // 计算适应度
  child.score() = evaluate_fitness_score(tb_config);
}
Individual GA::random_choose(const vector<Individual> &population,
                             const vector<double> &weight) {
  assert(population.size() == weight.size());
  // 首先计算累计概率
  double weight_cum = 0.0;
  std::vector<double> weight_cum_vec;
  weight_cum_vec.reserve(population.size()); // 预分配内存
  for (size_t i = 0; i != population.size(); ++i) {
    weight_cum += weight.at(i);
    weight_cum_vec.push_back(weight_cum);
  }

  // 根据累计概率随机抽取元素
  auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static std::default_random_engine e(tp_epoch);
  static std::uniform_real_distribution<double> u(0, weight_cum);
  double temp = u(e); // 进行1次抽取
  for (size_t i = 0; i != weight_cum_vec.size(); ++i) {
    if (weight_cum_vec.at(i) >= temp) {
      // 选取第i个脚标对应的元素
      return population.at(i);
    }
  }
  return population.back();
}

} // namespace yaohui
