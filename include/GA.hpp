#ifndef YH_MASTER_THESIS_GA_HPP
#define YH_MASTER_THESIS_GA_HPP

#include "Individual.hpp"
#include <chrono>
#include <cstdint>
#include <random>
#include <vector>

namespace yaohui {

/*
namespace GAA {

// 这里的容器内装的是TimetableConfig
template <typename T>
T random_choose(const std::vector<T> &population,
                const std::vector<double> &weight) {
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
}

} // namespace GAA
 */

class GA {
private:
  using station_id_t = int32_t; // 车站索引类型
  using mission_id_t = int32_t; // 运输任务(运行线)索引类型
  using second_t = int32_t;     // "秒"类型

public:
  //  static std::vector<Individual>
  //  random_generate_population(std::size_t population_sz,
  //                             yaohui::GA::secont_t departure_time_offset,
  //                             yaohui::GA::secont_t stop_duration_offset);

  //  static std::vector<TimetableConfig>
  //  random_generate_population(std::size_t population_sz,
  //                             yaohui::GA::secont_t departure_time_offset,
  //                             yaohui::GA::secont_t stop_duration_offset);

  static Individual
  random_choose(const std::vector<Individual> &population,
                const std::vector<double> &weight);

  static std::vector<TimetableConfig>
  generate_default_population(std::size_t population_sz);

  static std::vector<TimetableConfig> &
  random_offset_population(std::vector<TimetableConfig> &population,
                           second_t departure_time_offset_range,
                           second_t stop_duration_offset_range);

  static double evaluate_fitness_score(const TimetableConfig &config);

  static std::vector<double>
  evaluate_fitness_score(const std::vector<TimetableConfig> &population);

  static void parents_cross(Individual &father, Individual &mother);

  static void child_mutate(Individual &child);

  static std::vector<Individual>
  select_individuals(std::size_t target_population_sz,
                     const std::vector<Individual> &alternatives);

  static Individual reproduce(const Individual &father,
                              const Individual &mother);
};

} // namespace yaohui

#endif // YH_MASTER_THESIS_GA_HPP
