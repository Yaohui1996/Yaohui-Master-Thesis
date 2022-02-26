#include "Mission.hpp"

#include <sstream>
#include <utility>

using namespace std;

namespace yaohui {

// Mission::Mission(mission_id_t id, std::vector<Station> stations,
//                  std::vector<Interval> intervals)
//     : mission_id_(id), stations_(std::move(stations)),
//       intervals_(std::move(intervals)) {}

Mission::Mission(mission_id_t id, bool is_down, std::vector<Station> stations,
                 std::vector<Interval> intervals)
    : mission_id_(id), is_down_direction_(is_down),
      stations_(std::move(stations)), intervals_(std::move(intervals)) {}

vector<pair<Mission::station_id_t, Mission::second_t>>
Mission::plot_info_mission() const {
  vector<pair<station_id_t, second_t>> mission_info_vec;
  for (const Station &s : stations_) {
    mission_info_vec.emplace_back(s.station_id(), s.arrive_time());
    mission_info_vec.emplace_back(s.station_id(), s.departure_time());
  }
  return std::move(mission_info_vec);
}

Mission::mission_id_t Mission::id() const { return mission_id_; }
bool Mission::is_down_direction() const { return is_down_direction_; }
const std::vector<Station> &Mission::stations() const { return stations_; }
const std::vector<Interval> &Mission::intervals() const { return intervals_; }

} // namespace yaohui
