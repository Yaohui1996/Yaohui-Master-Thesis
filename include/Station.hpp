#ifndef YAOHUI_MASTER_THESIS_STATION_HPP
#define YAOHUI_MASTER_THESIS_STATION_HPP

#include "BaseDef.hpp"
#include <climits>
#include <vector>

namespace yaohui {

class Station {
private:
  station_id_t station_id_ = INT32_MIN;     // 车站id
  supply_arm_id_t supply_arm_ = INT32_MIN;  // 所属供电臂id
  second_t produce_begin_time_ = INT32_MIN; // 产能开始时刻
  second_t arrive_time_ = INT32_MIN;    // 进站时刻-秒(产能结束时刻)
  second_t departure_time_ = INT32_MIN; // 离站时刻-秒(用能开始时刻)
  second_t consume_end_time_ = INT32_MIN; // 用能结束时刻
  second_t stop_duration_ = INT32_MIN;    // 停站时长
public:
  /**
   * @brief Construct a new Station object
   *
   * @param id station id
   * @param arm station 所属供电臂id
   * @param prod_beg 产能开始时刻
   * @param arrive 到站时刻
   * @param departure 离站时刻
   * @param cons_end 用能结束时刻
   * @param stop_duration 停站时长
   */
  Station(station_id_t id, supply_arm_id_t arm, second_t prod_beg,
          second_t arrive, second_t departure, second_t cons_end,
          second_t stop_duration)
      : station_id_(id), supply_arm_(arm), produce_begin_time_(prod_beg),
        arrive_time_(arrive), departure_time_(departure),
        consume_end_time_(cons_end), stop_duration_(stop_duration) {}
  Station() = delete;

public:
  station_id_t station_id() const { return station_id_; }
  supply_arm_id_t supply_arm_id() const { return supply_arm_; }
  second_t consume_beg_time() const { return departure_time_; }
  second_t consume_end_time() const { return consume_end_time_; }
  second_t produce_begin_time() const { return produce_begin_time_; }
  second_t produce_end_time() const { return arrive_time_; }
  second_t stop_duration() const { return stop_duration_; }
  second_t arrive_time() const { return arrive_time_; }
  second_t departure_time() const { return departure_time_; }
};

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_STATION_HPP