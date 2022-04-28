#ifndef YAOHUI_MASTER_THESIS_INDIVIDUAL_HPP
#define YAOHUI_MASTER_THESIS_INDIVIDUAL_HPP

#include "BaseDef.hpp"
#include "Timetable.hpp"
#include "TimetableConfig.hpp"
#include <cstdint>
#include <map>
#include <utility>
#include <vector>

namespace yaohui {

class Individual {

private:
  TimetableConfig timetable_config_; // 个体的染色体信息
  double score_ = 0.0;               // 个体的适应度评分

public:
  Individual() = default;                              // 默认构造
  Individual(const Individual &) = default;            // 拷贝构造
  Individual(Individual &&) = default;                 // 移动构造
  Individual &operator=(const Individual &) = default; // 拷贝赋值
  Individual &operator=(Individual &&) = default;      // 移动赋值
  ~Individual() = default;                             // 默认析构
  explicit Individual(TimetableConfig tb_config);

public:
  double score() const;
  double &score();
  const TimetableConfig &timetable_config() const;
  TimetableConfig &timetable_config();
  void update_score();
  std::vector<double> random_walk(size_t k);
};

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_INDIVIDUAL_HPP
