#ifndef YAOHUI_MASTER_THESIS_INTERVAL_HPP
#define YAOHUI_MASTER_THESIS_INTERVAL_HPP

#include "BaseDef.hpp"
#include <climits>

namespace yaohui {

class Interval {

private:
  interval_id_t interval_id_ =
      std::make_pair(INT32_MIN, INT32_MIN);           // 行车区间id
  supply_arm_id_t consume_supply_arm_id_ = INT32_MIN; // 用能阶段所属供电臂
  second_t consume_beg_time_ = INT32_MIN;             // 用能开始时刻
  second_t consume_end_time_ = INT32_MIN;             // 用能结束时刻
  supply_arm_id_t produce_supply_arm_id_ = INT32_MIN; // 产能阶段所属供电臂
  second_t produce_begin_time_ = INT32_MIN;           // 产能开始时刻
  second_t produce_end_time_ = INT32_MIN;             // 产能结束时刻

public:
  /**
   * @brief Construct a new Interval object
   *
   * @param id interval id
   * @param cons_arm 用能供电臂id
   * @param cons_beg 用能起始时刻
   * @param cons_end 用能结束时刻
   * @param prod_arm 产能供电臂id
   * @param prod_beg 产能起始时刻
   * @param prod_end 产能结束时刻
   */
  Interval(interval_id_t id, supply_arm_id_t cons_arm, second_t cons_beg,
           second_t cons_end, supply_arm_id_t prod_arm, second_t prod_beg,
           second_t prod_end)
      : interval_id_(std::move(id)), consume_supply_arm_id_(cons_arm),
        consume_beg_time_(cons_beg), consume_end_time_(cons_end),
        produce_supply_arm_id_(prod_arm), produce_begin_time_(prod_beg),
        produce_end_time_(prod_end) {}
  Interval() = delete;

  station_id_t interval_id_first() const { return interval_id_.first; }
  station_id_t interval_id_second() const { return interval_id_.second; }
  supply_arm_id_t consume_supply_arm_id() const {
    return consume_supply_arm_id_;
  }
  supply_arm_id_t produce_supply_arm_id() const {
    return produce_supply_arm_id_;
  }
  second_t consume_beg_time() const { return consume_beg_time_; }
  second_t consume_end_time() const { return consume_end_time_; }
  second_t produce_begin_time() const { return produce_begin_time_; }
  second_t produce_end_time() const { return produce_end_time_; }
};

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_INTERVAL_HPP