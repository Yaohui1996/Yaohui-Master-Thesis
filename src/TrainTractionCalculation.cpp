#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <vector>
#include <algorithm>

#include "TractionCalculator.hpp"

using namespace std;
using namespace yaohui;

int main() {
  TractionCalculator traction_calculator;
  //  traction_calculator.calculate_power_time_data_0_to_40();

  traction_calculator.show();
  cout << traction_calculator.v_P_limit() << endl;
  cout << traction_calculator.time_to_P_limit() << endl;

  // 加速阶段
  auto W_map = traction_calculator.accelerating_stage_W(30.0);
  vector<double> W_acc_stage(30, 0.0); // 加速阶段每一秒钟内做功的值(kJ)
  for (const auto &item : W_map) {
    size_t index = floor(item.first);
    W_acc_stage.at(index) += (item.second / 1000.0);
  }

  for (size_t i = 0; i != W_acc_stage.size(); ++i) {
    cout << "区间: " << i << " 做功: " << W_acc_stage.at(i) << endl;
  }

  // 再生制动阶段
  auto W_brake_map = traction_calculator.brake_stage_W(15);
  vector<double> W_brake_stage(15, 0.0); // 制动阶段每一秒内产生的再生能量(kJ)
  for (const auto &item : W_brake_map) {
    size_t index = floor(item.first);
    W_brake_stage.at(index) += (item.second / 1000.0);
  }
  // 反转
  std::reverse(W_brake_stage.begin(), W_brake_stage.end());
  for (size_t i = 0; i != W_brake_stage.size(); ++i) {
    cout << "区间: " << i << " 再生能量: " << W_brake_stage.at(i) << endl;
  }


  return 0;
}
