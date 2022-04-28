#ifndef YAOHUI_MASTER_THESIS_RANDOMWALK_HPP
#define YAOHUI_MASTER_THESIS_RANDOMWALK_HPP

#include "Individual.hpp"
#include "TimetableConfig.hpp"
#include <fstream>
#include <string>

namespace yaohui {

class RandomWalk {
private:
  size_t pop_cnt_ = 0;
  size_t walk_times_ = 0;
  std::vector<Individual> pop_ = {};
  std::vector<std::vector<double>> rw_result_ = {}; // 保存适应度
  Individual best_individual_;                      // 保存最优个体

  void init_pop() {
    // 生成n个个体
    std::vector<Individual> pop;
    pop.reserve(pop_cnt_);
    while (pop_.size() < pop_cnt_) {
      TimetableConfig default_config;
      Individual default_individual(default_config);
      pop_.push_back(default_individual);
    }
  }

public:
  RandomWalk() = delete;
  ~RandomWalk() = default;
  RandomWalk(const RandomWalk &) = default;
  RandomWalk(RandomWalk &&) = default;
  RandomWalk &operator=(const RandomWalk &) = default;
  RandomWalk &operator=(RandomWalk &&) = default;

  RandomWalk(size_t pop_cnt, size_t walk_times);

  void do_random_walk();

  void output_process_result(const std::string &s) const;
  void output_plot_data(const std::string &s) const;
};

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_RANDOMWALK_HPP