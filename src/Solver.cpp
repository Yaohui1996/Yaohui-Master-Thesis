#include "Solver.hpp"

namespace yaohui {

Solver::Solver(size_t gene_cnt, size_t population_cnt, double cross_p,
               double mutate_p, double alpha, size_t thread_cnt)
    : gene_cnt_(gene_cnt), population_cnt_(population_cnt), cross_p_(cross_p),
      mutate_p_(mutate_p), alpha_(alpha), thread_cnt_(thread_cnt) {
  init_weights();    // 初始化权重vec
  init_population(); // 生成初始种群并按适应度由大到小排列
  first_best_individual_ = population_.front();
  last_best_individual_ = population_.front();
}

const Individual &Solver::individual_before_optimize() const {
  return first_best_individual_;
}
const Individual &Solver::individual_after_optimize() const {
  return last_best_individual_;
}

bool Solver::is_better(const Individual &lhs, const Individual &rhs) {
  return lhs.score() > rhs.score();
}

void Solver::init_weights() {
  weights_.reserve(population_cnt_);
  for (size_t i = 0; i != population_cnt_; ++i) {
    weights_.push_back(alpha_ * pow(1 - alpha_, i));
  }
}

void Solver::do_optimization() {
  {
    for (size_t loop_times = 0; loop_times != gene_cnt_; ++loop_times) {
      // 按照适应度选择个体并进行交叉生成子代
      population_ = birth_multi_threading();
      // 变异
      for (auto &item : population_) {
        child_mutate(item);
      }
      // 排序
      std::sort(population_.begin(), population_.end(), is_better);
      max_fitness_vec_.push_back(population_.front().score());
      min_fitness_vec_.push_back(population_.back().score());
      // 保存平均值
      double avg_fitness = 0.0; // 输出当前代的平均适应度
      for (const auto &individual : population_) {
        avg_fitness += individual.score();
      }
      avg_fitness = avg_fitness / static_cast<double>(population_.size());
      avg_fitness_vec_.push_back(avg_fitness);

      // 保存最优解
      last_best_individual_ = population_.front();

      // 输出
      std::cout << "当前进化代数: " << loop_times
                << "\t最好的适应度为: " << max_fitness_vec_.back()
                << "\t最差的适应度为: " << min_fitness_vec_.back()
                << "\t平均适应度为: " << avg_fitness_vec_.back() << std::endl;
    }

    std::cout << "优化完毕!" << std::endl;
  }
}

void Solver::output_optimization_result(std::string file_name) {
  std::vector<std::string> lines = {
      "generation,best_fitness,worst_fitness,avg_fitness\n"};
  for (size_t i = 0; i != gene_cnt_; ++i) {
    lines.push_back(std::to_string(i + 1) + "," +
                    std::to_string(max_fitness_vec_.at(i)) + "," +
                    std::to_string(min_fitness_vec_.at(i)) + "," +
                    std::to_string(avg_fitness_vec_.at(i)) + "\n");
  }

  // 进化过程画图用迭代数据写入文件
  std::ofstream plot_data_output;
  plot_data_output.open(file_name, std::ios::out);
  if (!plot_data_output.is_open()) {
    std::cout << "打开文件失败! [file=进化过程画图数据.csv]" << std::endl;
  }
  for (const auto &line : lines) {
    plot_data_output << line;
  }
  plot_data_output.close();
  std::cout << "保存成功!" << std::endl;
}

// 生成初始种群
void Solver::init_population() {
  population_.reserve(population_cnt_);
  for (size_t i = 0; i < population_cnt_; ++i) {
    TimetableConfig default_config;
    Individual default_individual(std::move(default_config));
    population_.push_back(std::move(default_individual));
  }
  // 将个体按照适应度由大到小排序
  std::sort(population_.begin(), population_.end(), is_better);
}

Individual Solver::random_choose(const std::vector<Individual> &population,
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
  static std::default_random_engine rand_choose_e(tp_epoch);
  static std::uniform_real_distribution<double> rand_choose_u(0, weight_cum);
  double temp = rand_choose_u(rand_choose_e); // 进行1次抽取
  for (size_t i = 0; i != weight_cum_vec.size(); ++i) {
    if (weight_cum_vec.at(i) >= temp) {
      // 选取第i个脚标对应的元素
      return population.at(i);
    }
  }
  return population.back();
}

void Solver::parents_cross(Individual &father, Individual &mother) {
  auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static std::default_random_engine cross_down_index_e(tp_epoch);
  TimetableConfig &father_tb_config = father.timetable_config();
  TimetableConfig &mother_tb_config = mother.timetable_config();

  auto &father_down_de = father_tb_config.down_departure_time_vec();
  auto &father_down_stop = father_tb_config.down_stop_duration_vec();
  auto &mother_down_de = mother_tb_config.down_departure_time_vec();
  auto &mother_down_stop = mother_tb_config.down_stop_duration_vec();
  assert(father_down_de.size() == father_down_stop.size());
  assert(father_down_de.size() == mother_down_de.size());
  assert(father_down_de.size() == mother_down_stop.size());

  static std::uniform_int_distribution<size_t> random_down_i(
      0, father_down_de.size());
  size_t rdi = random_down_i(cross_down_index_e);
  for (size_t i = 0; i != rdi; ++i) {
    std::swap(father_down_de.at(i), mother_down_de.at(i));
  }
  rdi = random_down_i(cross_down_index_e);
  for (size_t i = 0; i != rdi; ++i) {
    std::swap(father_down_stop.at(i), mother_down_stop.at(i));
    tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine cross_index_j_e(tp_epoch);
    static std::uniform_int_distribution<size_t> random_down_j(
        0, father_down_stop.front().size() - 1);
    for (station_id_t j = 0; j < random_down_j(cross_index_j_e); ++j) {
      std::swap(father_down_stop.at(i).at(j), mother_down_stop.at(i).at(j));
    }
  }

  auto &father_up_de = father_tb_config.up_departure_time_vec();
  auto &father_up_stop = father_tb_config.up_stop_duration_vec();
  auto &mother_up_de = mother_tb_config.up_departure_time_vec();
  auto &mother_up_stop = mother_tb_config.up_stop_duration_vec();
  assert(father_up_de.size() == father_up_stop.size());
  assert(father_up_de.size() == mother_up_de.size());
  assert(father_up_de.size() == mother_up_stop.size());

  tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static std::default_random_engine cross_up_index_e(tp_epoch);
  static std::uniform_int_distribution<size_t> random_up_i(0,
                                                           father_up_de.size());
  rdi = random_up_i(cross_up_index_e);
  for (size_t i = 0; i != rdi; ++i) {
    std::swap(father_up_de.at(i), mother_up_de.at(i));
  }
  rdi = random_up_i(cross_up_index_e);
  for (size_t i = 0; i != rdi; ++i) {
    std::swap(father_up_stop.at(i), mother_up_stop.at(i));
    tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine cross_index_up_j_e(tp_epoch);
    static std::uniform_int_distribution<size_t> random_up_j(
        0, father_down_stop.front().size() - 1);
    for (station_id_t j = 0; j < random_up_j(cross_index_up_j_e); ++j) {
      std::swap(father_down_stop.at(i).at(j), mother_down_stop.at(i).at(j));
    }
  }
  father.update_score();
  mother.update_score();
}

std::vector<Individual>
Solver::birth_single_threading(const std::vector<Individual> &population,
                               const std::vector<double> &weights,
                               double cross_p, size_t child_cnt) {
  std::vector<Individual> ret;
  for (int i = 0; i < child_cnt; ++i) {
    auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine cross_e(tp_epoch);
    static std::uniform_real_distribution<double> cross_u(0, 1.0);
    // 根据权重选出父母
    Individual father = random_choose(population, weights);
    Individual mother = random_choose(population, weights);
    // 父母交叉获得子代
    if (cross_u(cross_e) < cross_p) {
      parents_cross(father, mother);
    }
    // 选择适应度大的作为子代
    Individual &child = father.score() > mother.score() ? father : mother;
    ret.push_back(std::move(child));
  }
  return std::move(ret);
}

std::vector<Individual> Solver::birth_multi_threading() const {
  // 多线程
  int sz = static_cast<int>(population_cnt_);
  int th_cnt = static_cast<int>(thread_cnt_);
  int avg_task_cnt = sz / th_cnt;
  int last_task_cnt = sz - (th_cnt - 1) * avg_task_cnt;

  // 前th_cnt-1个线程
  std::vector<std::future<std::vector<Individual>>> fut_vec;
  fut_vec.reserve(th_cnt);
  for (int i = 0; i < th_cnt - 1; ++i) {
    fut_vec.emplace_back(std::async(birth_single_threading, population_,
                                    weights_, cross_p_, avg_task_cnt));
  }
  // 最后一个线程
  fut_vec.emplace_back(std::async(birth_single_threading, population_, weights_,
                                  cross_p_, last_task_cnt));

  // 最终结果
  std::vector<Individual> result;
  result.reserve(sz);
  for (auto &fut : fut_vec) {
    auto re_v = fut.get();
    result.insert(result.end(), re_v.begin(), re_v.end());
  }
  return std::move(result);
}

void Solver::child_mutate(Individual &child) const {
  auto tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static std::default_random_engine mutate_e(tp_epoch);
  static std::uniform_real_distribution<double> mutate_u(0, 1.0);

  if (mutate_u(mutate_e) >= mutate_p_) {
    return;
  }

  auto find_T = [](second_t t, const departure_T_t &dT) -> second_t {
    // 寻找追踪间隔
    second_t departure_T;
    for (const auto &dt : dT) {
      if (t >= dt.first.first && t < dt.first.second) {
        departure_T = dt.second;
        break;
      }
    }
    return departure_T;
  };

  TimetableConfig &config = child.timetable_config();

  // down departure
  // 获取下行首站发车时刻序列
  auto &down_de_vec = config.down_departure_time_vec();

  tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static std::default_random_engine down_departure_mutate_index_e(tp_epoch);
  static std::uniform_int_distribution<size_t> down_departure_mutate_index_u(
      0, down_de_vec.size() - 1);
  size_t r = down_departure_mutate_index_u(down_departure_mutate_index_e);
  for (size_t i = r; i <= down_de_vec.size() - 1; ++i) {
    if (i == 0) {
      continue;
    }
    second_t di_1 = down_de_vec.at(i - 1);
    second_t di = down_de_vec.at(i);
    // 寻找i-1时刻的最小追踪间隔
    second_t departure_T_min = find_T(di_1, config.departure_T_min());
    second_t oft = di - di_1 - departure_T_min; // 更新oft
    // 寻找i-1时刻的最大追踪间隔
    second_t departure_T_max = find_T(di_1, config.departure_T_max());
    second_t oyt = departure_T_max - (di - di_1); // 更新oyt

    // 随机偏移量
    tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine down_departure_mutate_offset_e(tp_epoch);
    static std::uniform_int_distribution<second_t>
        down_departure_mutate_offset_u(-oft, oyt);
    second_t r2 =
        down_departure_mutate_offset_u(down_departure_mutate_offset_e);
    // 更新di
    down_de_vec.at(i) += r2;
  }
  // up departure
  // 获取上行首站发车时刻序列
  auto &up_de_vec = config.up_departure_time_vec();

  tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
  static std::default_random_engine up_departure_mutate_index_e(tp_epoch);
  static std::uniform_int_distribution<size_t> up_departure_mutate_index_u(
      0, up_de_vec.size() - 1);
  r = up_departure_mutate_index_u(up_departure_mutate_index_e);
  for (size_t i = r; i <= up_de_vec.size() - 1; ++i) {
    if (i == 0) {
      continue;
    }
    second_t di_1 = up_de_vec.at(i - 1);
    second_t di = up_de_vec.at(i);
    // 寻找i-1时刻的最小追踪间隔
    second_t departure_T_min = find_T(di_1, config.departure_T_min());
    second_t oft = di - di_1 - departure_T_min; // 更新oft
    // 寻找i-1时刻的最大追踪间隔
    second_t departure_T_max = find_T(di_1, config.departure_T_max());
    second_t oyt = departure_T_max - (di - di_1); // 更新oyt

    // 随机偏移量
    tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine up_departure_mutate_offset_e(tp_epoch);
    static std::uniform_int_distribution<second_t> up_departure_mutate_offset_u(
        -oft, oyt);
    second_t r2 = up_departure_mutate_offset_u(up_departure_mutate_offset_e);
    // 更新di
    down_de_vec.at(i) += r2;
  }

  // down stop
  for (auto &l : config.down_stop_duration_vec()) {
    tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine down_stop_mutate_index_e(tp_epoch);
    static std::uniform_int_distribution<station_id_t> down_stop_mutate_index_u(
        config.stations().front(), config.stations().back());
    station_id_t r1 = down_stop_mutate_index_u(down_stop_mutate_index_e);
    if (r1 == config.stations().front() || r1 == config.stations().back()) {
      continue;
    }

    second_t LB = config.stop_duration_min().at(r1);
    second_t UB = config.stop_duration_max().at(r1);

    tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine down_stop_mutate_offset_e(tp_epoch);
    static std::uniform_int_distribution<second_t> down_stop_mutate_offset_u(
        LB, UB);
    second_t r2 = down_stop_mutate_offset_u(down_stop_mutate_offset_e);
    l.at(r1) = r2;
  }

  // up stop
  for (auto &l : config.up_stop_duration_vec()) {
    tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine up_stop_mutate_index_e(tp_epoch);
    static std::uniform_int_distribution<station_id_t> up_stop_mutate_index_u(
        config.stations().front(), config.stations().back());
    station_id_t r1 = up_stop_mutate_index_u(up_stop_mutate_index_e);
    if (r1 == config.stations().front() || r1 == config.stations().back()) {
      continue;
    }

    second_t LB = config.stop_duration_min().at(r1);
    second_t UB = config.stop_duration_max().at(r1);

    tp_epoch = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine up_stop_mutate_offset_e(tp_epoch);
    static std::uniform_int_distribution<second_t> up_stop_mutate_offset_u(LB,
                                                                           UB);
    second_t r2 = up_stop_mutate_offset_u(up_stop_mutate_offset_e);
    l.at(r1) = r2;
  }
  // 更新score
  child.update_score();
}

} // namespace yaohui