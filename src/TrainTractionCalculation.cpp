#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

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
  vector<double> W_acc_stage(30, 0.0); // 加速阶段每一秒钟内做功的值
  for (const auto &item : W_map) {
    size_t index = floor(item.first);
    W_acc_stage.at(index) += (item.second / 1000.0); // (kJ)
  }

  for (size_t i = 0; i != W_acc_stage.size(); ++i) {
    cout << "区间: " << i << " 做功: " << W_acc_stage.at(i) << endl;
  }

  // 输出用能关系表
  ofstream of;
  of.open("W_consume.csv", std::ios::out);
  if (!of.is_open()) {
    cout << "文件[W_consume.csv]打开失败!" << endl;
  }
  // 写入数据
  for (const auto &p : W_map) {
    of << to_string(p.first) << "," << to_string(p.second) << "\n";
  }
  of.close();
  // 输出用能关系曲线(按秒)
  of.open("W_consume_second.csv", std::ios::out);
  if (!of.is_open()) {
    cout << "文件[W_consume_second.csv]打开失败!" << endl;
  }
  // 写入数据
  for (const auto &item : W_acc_stage) {
    of << to_string(item) << "\n";
  }
  of.close();

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

  // 输出用能关系表
  of.open("W_produce.csv", std::ios::out);
  if (!of.is_open()) {
    cout << "文件[W_produce.csv]打开失败!" << endl;
  }
  // 写入数据
  for (const auto &p : W_brake_map) {
    of << to_string(15.0 - p.first) << "," << to_string(p.second) << "\n";
  }
  of.close();
  // 输出用能关系曲线(按秒)
  of.open("W_produce_second.csv", std::ios::out);
  if (!of.is_open()) {
    cout << "文件[W_consume_second.csv]打开失败!" << endl;
  }
  // 写入数据
  for (const auto &item : W_brake_stage) {
    of << to_string(item) << "\n";
  }
  of.close();

  return 0;
}
