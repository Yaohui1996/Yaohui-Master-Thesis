#include "Individual.hpp"
#include "Solver.hpp"
#include "Timetable.hpp"

using namespace std;
using namespace yaohui;

int main() {
  size_t gene_cnt = 200;       // 进化次数
  size_t population_cnt = 50; // 种群规模
  double cross_p = 0.8;        // 交叉概率
  double mutate_p = 0.05;      // 变异概率
  double alpha = 0.05;         // 选择参数alpha
  size_t thread_cnt = 8;       // 线程数目

  // 构造求解器
  Solver solver(gene_cnt, population_cnt, cross_p, mutate_p, alpha, thread_cnt);
  solver.do_optimization();                                  // 求解
  solver.output_optimization_result("进化过程画图数据.csv"); // 输出到文件

  Individual unoptimized_individual = solver.individual_before_optimize();
  Individual optimized_individual = solver.individual_after_optimize();

  Timetable initial_solution =
      Timetable(unoptimized_individual.timetable_config());
  Timetable best_solution = Timetable(optimized_individual.timetable_config());

  // 输出能量分布曲线
  best_solution.output_energy_distribution("优化后");
  initial_solution.output_energy_distribution("优化前");

  // 输出运行图参数
  best_solution.write_to_file("优化后");
  initial_solution.write_to_file("优化前");

  // 输出运行图画图数据
  initial_solution.output_plot_data("优化前");
  best_solution.output_plot_data("优化后");
  return 0;
}
