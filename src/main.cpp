#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <vector>

#include "GA.hpp"
#include "Individual.hpp"
#include "Timetable.hpp"
#include "TimetableConfig.hpp"
#include "TractionCalculator.hpp"
#include "matplotlibcpp.h"

using namespace std;
using namespace yaohui;

namespace plt = matplotlibcpp;

template <typename T> void print_vec(const vector<T> &vec) {
  for (const auto &item : vec) {
    cout << item << "\n";
  }
}

template <typename K, typename V> void print_map(const map<K, V> &m) {
  for (const auto &item : m) {
    cout << "key: " << item.first << " value: " << item.second << endl;
  }
}

int main() {
  // 配置参数
  const size_t gene_cnt = 200;      // 进化次数
  const size_t population_cnt = 100; // 种群规模
  const double mutation_p = 0.01;    // 变异概率
  auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static default_random_engine mutate_e(tp_epoch);
  static uniform_real_distribution<double> mutate_u(0, 1.0);

  // 构造population_cnt个默认的config
  vector<TimetableConfig> configs_vec =
      GA::generate_default_population(population_cnt);
  // 对每一个config进行随机调整
  configs_vec = GA::random_offset_population(configs_vec, 30, 5);
  // 计算每个config的适应度
  vector<double> scores = GA::evaluate_fitness_score(configs_vec);
  // 构造population_cnt个个体
  vector<Individual> population;
  population.reserve(configs_vec.size()); // 预分配内存
  assert(configs_vec.size() == scores.size());
  for (size_t i = 0; i != configs_vec.size(); ++i) {
    population.emplace_back(configs_vec.at(i), scores.at(i));
  }

  Individual best_individual; // 保存最后一代最好的个体
  for (size_t loop_times = 0; loop_times != gene_cnt; ++loop_times) {
    // 将个体按照适应度由大到小排序
    // 谓词
    auto isBetter = [](const Individual &lhs, const Individual &rhs) -> bool {
      return lhs.score() > rhs.score();
    };
    // 排序
    std::sort(population.begin(), population.end(), isBetter);

    // 构造选择权重vec
    auto get_weights_vec = [](size_t cnt, double a) -> vector<double> {
      vector<double> weights;
      weights.reserve(cnt);
      for (size_t i = 0; i != cnt; ++i) {
        weights.push_back(a * pow(1 - a, i));
      }
      return std::move(weights);
    };

    // 根据个体适应度配置个体被选中的权重
    vector<double> weights = get_weights_vec(population.size(), 0.3);

    // 按照适应度选择个体并进行交叉生成子代
    vector<Individual> Children;
    Children.reserve(population_cnt); // 预分配内存
    while (Children.size() != population_cnt) {
      // 根据权重选出父母
      Individual father = GA::random_choose(population, weights);
      Individual mother = GA::random_choose(population, weights);
      // 父母交叉获得子代
      GA::parents_cross(father, mother);
      // 选择适应度大的作为子代
      Individual &child = father.score() > mother.score() ? father : mother;
      // 根据概率变异
      if (mutate_u(mutate_e) < mutation_p) {
        GA::child_mutate(child); // 执行变异操作
      }
      Children.push_back(child);
    }

    double max_fitness = 0.0; // 输出当前代的最好的适应度
    double avg_fitness = 0.0; // 输出当前代的平均适应度

    for (const auto &child : Children) {
      avg_fitness += child.score();
      if (child.score() > max_fitness) {
        max_fitness = child.score();
        best_individual = child;
      }
    }
    avg_fitness = avg_fitness / static_cast<double>(Children.size());
    cout << "当前进化代数: " << loop_times << " 最好的适应度为: " << max_fitness
         << "平均适应度为: " << avg_fitness << endl;

    // 更新
    population = Children;
  }

  cout << "最后一代最好的个体的评分: " << best_individual.score() << endl;
  Timetable best_solution(best_individual.timetable_config());
  cout << "最后一代最好的个体的利用率: " << best_solution.total_reuse_ratio()
       << endl;

  TimetableConfig default_config;
  Timetable tb(default_config);
  cout << "默认运行图利用率: " << tb.total_reuse_ratio() << endl;

  cout << "开始绘图: " << endl;
  auto plot_info = best_solution.get_plot_data();
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
  out_file.open("TimetableInfo.txt", ios::out);
  out_file << out_ss.str();
  out_file.close();

  // 转换成画图专用数据
  using station_id_t = int32_t;
  vector<pair<vector<int32_t>, vector<station_id_t>>> plot_data;
  for (const auto &m : plot_info) {
    vector<int32_t> x;
    vector<station_id_t> y;
    for (const auto &point : m) {
      x.emplace_back(point.second);
      y.emplace_back(point.first);
    }
    plot_data.emplace_back(x, y);
  }

  // plot
  plt::figure_size(1200, 780);
  // 竖线
  for (int32_t i = 0; i <= 3600 * 28; i += 60) {
    //        cout << i << "\n";
    vector<int32_t> x{i, i};
    vector<int32_t> y{0, 15};
    plt::plot(x, y,
              map<string, string>{
                  {"color", "gray"},
                  //   {"marker", "o"},
                  {"linestyle", "dashed"},
                  {"linewidth", "0.03"}
                  //   {"markersize", "0"}
              });
  }

  // 横线
  for (int32_t i = 0; i <= 15; ++i) {
    vector<int32_t> x{0, 3600 * 28};
    vector<int32_t> y{i, i};
    plt::plot(x, y,
              map<string, string>{
                  {"color", "gray"},
                  //   {"marker", "o"},
                  {"linestyle", "dashed"},
                  {"linewidth", "0.03"}
                  //   {"markersize", "0"}
              });
  }

  const std::map<std::string, std::string> keywords{
      {"color", "green"},
      //   {"marker", "o"},
      //   {"linestyle", "dashed"},
      {"linewidth", "0.1"}
      //   {"markersize", "0.1"}
  };
  for (const auto &line : plot_data) {
    if (!plt::plot(line.first, line.second, keywords)) {
      cout << "false\n";
    }
  }

  plt::save("./basic.pdf", 900);
  // plt::grid(true);
  plt::show();

  return 0;
}
