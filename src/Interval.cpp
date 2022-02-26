#include "Interval.hpp"

#include <utility>

namespace yaohui {

Interval::Interval(interval_id_t id, supply_arm_id_t cons_arm,
                   second_t cons_beg, second_t cons_end,
                   supply_arm_id_t prod_arm, second_t prod_beg,
                   second_t prod_end)
    : interval_id_(std::move(id)), consume_supply_arm_id_(cons_arm),
      consume_beg_time_(cons_beg), consume_end_time_(cons_end),
      produce_supply_arm_id_(prod_arm), produce_begin_time_(prod_beg),
      produce_end_time_(prod_end) {}

Interval::station_id_t Interval::interval_id_first() const {
  return interval_id_.first;
}
Interval::station_id_t Interval::interval_id_second() const {
  return interval_id_.second;
}
Interval::supply_arm_id_t Interval::consume_supply_arm_id() const {
  return consume_supply_arm_id_;
}
Interval::supply_arm_id_t Interval::produce_supply_arm_id() const {
  return produce_supply_arm_id_;
}
Interval::second_t Interval::consume_beg_time() const {
  return consume_beg_time_;
}
Interval::second_t Interval::consume_end_time() const {
  return consume_end_time_;
}
Interval::second_t Interval::produce_begin_time() const {
  return produce_begin_time_;
}
Interval::second_t Interval::produce_end_time() const {
  return produce_end_time_;
}

} // namespace yaohui