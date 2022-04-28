#include "RandomWalk.hpp"

namespace yaohui {

RandomWalk::RandomWalk(size_t pop_cnt, size_t walk_times)
    : pop_cnt_(pop_cnt), walk_times_(walk_times) {
  init_pop();
}

void RandomWalk::do_random_walk() {
  std::cout << "Random walking..." << std::endl;
  for (size_t t = 0; t != pop_.size(); ++t) {
    rw_result_.emplace_back(pop_.at(t).random_walk(walk_times_));
  }

  // 线性查找最优个体
  best_individual_ = pop_.front();
  for (size_t i = 1; i != pop_.size(); ++i) {
    if (pop_.at(i).score() > best_individual_.score()) {
      best_individual_ = pop_.at(i);
    }
  }
}

void RandomWalk::output_process_result(const std::string &s) const {
  // output to file
  std::ofstream rwf;
  rwf.open(s, std::ios::out);
  if (!rwf.is_open()) {
    std::cout << "failed to open [" << s << "] !" << std::endl;
  }
  for (size_t i = 0; i != rw_result_.size(); ++i) {
    for (size_t j = 0; j != rw_result_.at(i).size(); ++j) {
      if (j == rw_result_.at(i).size() - 1) {
        rwf << std::to_string(rw_result_.at(i).at(j));
      } else {
        rwf << std::to_string(rw_result_.at(i).at(j)) << ",";
      }
    }
    rwf << "\n";
  }
  rwf.close();
}

void RandomWalk::output_plot_data(const std::string &s) const {
  Timetable best_solution = Timetable(best_individual_.timetable_config());
  best_solution.output_plot_data(s);
}
} // namespace yaohui