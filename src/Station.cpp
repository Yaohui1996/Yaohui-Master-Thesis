#include "Station.hpp"

using namespace std;

namespace yaohui {

Station::Station(station_id_t id, supply_arm_id_t arm, second_t prod_beg,
                 second_t arrive, second_t departure, second_t cons_end,
                 second_t stop_duration)
    : station_id_(id), supply_arm_(arm), produce_begin_time_(prod_beg),
      arrive_time_(arrive), departure_time_(departure),
      consume_end_time_(cons_end), stop_duration_(stop_duration) {}

Station::station_id_t Station::station_id() const { return station_id_; }
Station::supply_arm_id_t Station::supply_arm_id() const { return supply_arm_; }
Station::second_t Station::consume_beg_time() const { return departure_time_; }
Station::second_t Station::consume_end_time() const {
  return consume_end_time_;
}
Station::second_t Station::produce_begin_time() const {
  return produce_begin_time_;
}
Station::second_t Station::produce_end_time() const { return arrive_time_; }
Station::second_t Station::arrive_time() const { return arrive_time_; }
Station::second_t Station::departure_time() const { return departure_time_; }
Station::second_t Station::stop_duration() const { return stop_duration_; }

/*
map<string, int32_t> Station::to_map() const {
  map<string, int32_t> info_map;
  info_map.insert({"station_id_", station_id_});
  info_map.insert({"supply_arm_", supply_arm_});
  info_map.insert({"produce_begin_time_", produce_begin_time_});
  info_map.insert({"arrive_time_", arrive_time_});
  info_map.insert({"departure_time_", departure_time_});
  info_map.insert({"consume_end_time_", consume_end_time_});
  info_map.insert({"stop_duration_", stop_duration_});
  return info_map;
}
*/

} // namespace yaohui
