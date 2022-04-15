#ifndef YAOHUI_MASTER_THESIS_SOLVER_HPP
#define YAOHUI_MASTER_THESIS_SOLVER_HPP

#include "Individual.hpp"
#include <cmath>
#include <fstream>
#include <future>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace yaohui {

class Solver {

private:
  size_t gene_cnt_ = 200;              // 进化次数
  size_t population_cnt_ = 50;         // 种群规模
  double cross_p_ = 0.8;               // 交叉概率
  double mutate_p_ = 0.01;             // 变异概率
  double alpha_ = 0.05;                // 选择参数alpha
  size_t thread_cnt_ = 8;              // 线程数目
  std::vector<double> weights_;        // 选择权重
  std::vector<Individual> population_; // 初始种群
  Individual first_best_individual_;   // 初代最好的解
  Individual last_best_individual_;    // 末代最好的解
  std::vector<double> max_fitness_vec_; // 每代最大的适应度构成的数组
  std::vector<double> min_fitness_vec_; // 每代最小适应度构成的数组
  std::vector<double> avg_fitness_vec_; // 每代平均适应度构成的数组
public:
  Solver() = delete;                          // 默认构造
  Solver(const Solver &) = delete;            // 拷贝构造
  Solver(Solver &&) = delete;                 // 移动构造
  Solver &operator=(const Solver &) = delete; // 拷贝赋值
  Solver &operator=(Solver &&) = delete;      // 移动赋值
  ~Solver() = default;                        // 默认析构
  Solver(size_t gene_cnt, size_t population_cnt, double cross_p,
         double mutate_p, double alpha, size_t thread_cnt);
  const Individual &individual_before_optimize() const;
  const Individual &individual_after_optimize() const;
  void do_optimization();
  void output_optimization_result(std::string file_name);

private:
  static bool is_better(const Individual &lhs, const Individual &rhs);
  void init_weights();
  void init_population();
  static Individual random_choose(const std::vector<Individual> &population,
                                  const std::vector<double> &weight);
  static void parents_cross(Individual &father, Individual &mother);
  static std::vector<Individual>
  birth_single_threading(const std::vector<Individual> &population,
                         const std::vector<double> &weights, double cross_p,
                         size_t child_cnt);

  std::vector<Individual> birth_multi_threading() const;
  void child_mutate(Individual &child) const;
};

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_SOLVER_HPP
