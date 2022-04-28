#include "Individual.hpp"
#include "RandomWalk.hpp"
#include "Solver.hpp"
#include "Timetable.hpp"
#include <chrono>
#include <iostream>

using namespace std;
using namespace yaohui;

int main() {
  size_t gene_cnt = 200;       // 进化次数
  size_t population_cnt = 100; // 种群规模
  double alpha = 0.015;        // 选择参数alpha
  double cross_p = 0.8;        // 交叉概率
  double mutate_p = 0.05;      // 变异概率
  size_t thread_cnt = 8;       // 线程数目

  auto start = std::chrono::system_clock::now();
  // construct solver
  Solver solver(gene_cnt, population_cnt, cross_p, mutate_p, alpha, thread_cnt);
  solver.do_optimization();
  auto end = std::chrono::system_clock::now();
  std::cout << "The cost of time for optimizing timetable: "
            << std::chrono::duration<double>(end - start).count() << " second."
            << std::endl;
  // output processing data
  solver.output_optimization_result("processing-data.csv"); // output

  // get best solution
  Timetable best_solution =
      Timetable(solver.individual_after_optimize().timetable_config());

  // output the file of energy distribution
  best_solution.output_energy_distribution("optimized");

  // output the json file of timetable
  best_solution.write_to_file("optimized-timetable.json");

  // output plot data of the best timetable
  best_solution.output_plot_data("best-timetable-plot-data.csv");

  // default timetable
  TimetableConfig tbc;
  Timetable default_tb(tbc);
  std::cout << "The reuse ratio of default timetable: "
            << default_tb.total_reuse_ratio() << std::endl;
  // output plot data
  default_tb.output_plot_data("default-timetable-plot-data.csv");

  // random walk
  start = std::chrono::system_clock::now();
  RandomWalk rw = RandomWalk(population_cnt, gene_cnt);
  rw.do_random_walk();
  end = std::chrono::system_clock::now();
  std::cout << "The cost of time for random walk: "
            << std::chrono::duration<double>(end - start).count() << " second."
            << std::endl;
  rw.output_process_result("random-walk-process-result.csv");
  rw.output_plot_data("random-walk-timetable-plot-data.csv");
  return 0;
}
