#ifndef YAOHUI_MASTER_THESIS_STATION_HPP
#define YAOHUI_MASTER_THESIS_STATION_HPP

#include <climits>
#include <cstdint>
#include <vector>

namespace yaohui {

class Station {
private:
  using station_id_t = int32_t;    // 车站索引类型
  using supply_arm_id_t = int32_t; // 供电臂索引类型
  using second_t = int32_t;        // "秒"类型

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
          second_t stop_duration);
  Station() = delete;

public:
  station_id_t station_id() const;
  supply_arm_id_t supply_arm_id() const;
  second_t consume_beg_time() const;
  second_t consume_end_time() const;
  second_t produce_begin_time() const;
  second_t produce_end_time() const;
  second_t stop_duration() const;
  second_t arrive_time() const;
  second_t departure_time() const;
  //  std::map<std::string, int32_t> to_map() const;
};

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_STATION_HPP