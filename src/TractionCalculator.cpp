#include "TractionCalculator.hpp"

#include <cmath>
#include <iostream>

using namespace std;

namespace yaohui {

double TractionCalculator::f_w0(double v) const {
  return 2.755102 + 0.000429 * pow(v, 2);
}
double TractionCalculator::f_wr() const { return 600.0 / r_; }
double TractionCalculator::f_wi() const { return is_uphill_ ? i_ : -i_; }
double TractionCalculator::f_ws(double v) const {
  return ((0.00357 * L_) / train_m_) *
         (pow(1.0 - (1.0 / (1.0 + sqrt((61.475 + 0.177 * (Ls_ - L_)) / L_))),
              2)) *
         pow(v, 2);
}
double TractionCalculator::f_total(double v) const {
  return ((f_w0(v) + f_wi() + f_wr() + f_ws(v)) * train_m_ * g_) / 1000.0;
}

double TractionCalculator::F_traction(double v) const {
  if (v <= 35.0) {
    return 352.0;
  }
  return 352.0;
}

double TractionCalculator::P_traction(double v) const {
  return F_traction(v) * (v / 3.6);
}
void TractionCalculator::show() const {
  cout << "乘客平均质量(t): " << passenger_m_avg_ << endl;
  cout << "定员载客量(人): " << passenger_capacity_ << endl;
  cout << "带司机室的动车数目: " << MCP_cnt_ << endl;
  cout << "不带司机室的拖车数目: " << T_cnt << endl;
  cout << "带司机室的动车质量(t): " << MCP_m_ << endl;
  cout << "不带司机室的拖车质量(t): " << T_m_ << endl;
  cout << "列车质量 (t): " << train_m_ << endl;
  cout << "重力加速度 m·s^(-2): " << g_ << endl;
  cout << "隧道长度 (m): " << Ls_ << endl;
  cout << "列车长度 (m): " << L_ << endl;
  cout << "坡度的千分数: " << i_ << endl;
  cout << "曲线半径 (m): " << r_ << endl;
  cout << "是否为上坡, 上坡取true, 下坡取false.: " << is_uphill_ << endl;
}
double TractionCalculator::time_to_P_limit() const {
  double t = 0.0;                // 初始时刻(s)
  double v0 = 0.0;               // 初始速度(km/h)
  const double vt = v_P_limit(); // 到达功率限制阶段初的速度(km/h)
  const double epsilon = 0.0001; // 每次迭代v0的增量(km/h)
  while (v0 < vt) {
    double acc =
        (F_const_ - f_total(v0)) / train_m_; // v0时刻的加速度m/(s^(-2))
    t += (epsilon / 3.6) / acc;              // 需要的时间(s)
    v0 += epsilon;                           // 循环末的v0(km/h)
  }
  return t;
}
double TractionCalculator::v_P_limit() const {
  return P_limit_ / F_const_ * 3.6; //  P = F * v;
}
map<double, double> TractionCalculator::accelerating_stage_W(double tm) const {
  double t0 = 0.0;                     // 初始时刻(s)
  double v0 = 0.0;                     // 初始速度(km/h)
  const double t1 = time_to_P_limit(); // 牵引功率限制阶段初的时刻(s)
  const double epsilon = 0.0001;       // 每次迭代的时间增量(s)

  map<double, double> W_map;
  while (t0 <= tm) {
    double P_curr = t0 < t1 ? F_const_ * (v0 / 3.6) : P_limit_; // 当前功率(kW)
    double W_curr = P_curr * epsilon * 1000; // 当前epsilon时段内做功(J)
    W_map.insert(std::make_pair(t0, W_curr));
    double acc_curr =
        (F_const_ - f_total(v0)) / train_m_; // 当前时刻的加速度m/(s^(-2))
    v0 += (acc_curr * epsilon) * 3.6;        // 下一时刻的速度(km/h)
    t0 += epsilon;
  }
  return std::move(W_map);
}
std::map<double, double> TractionCalculator::brake_stage_W(double tm) const {
  double t0 = 0.0;               // 初始时刻(s)
  double v0 = 0.0;               // 列车静止时的速度(km/h)
  const double v1 = 10.0;        // 再生制动初始速度(km/h)(逆过程)
  const double vm = 80.0;        // 再生制动末速度(km/h)(逆过程)
  const double epsilon = 0.0001; // 每次迭代的时间增量(s)
  map<double, double> W_map;
  while (t0 <= tm) {
    const double acc_curr = F_brake_ / train_m_; // 当前时刻的加速度m/(s^(-2))
    const double delta_v0 = acc_curr * epsilon; // 下一时刻速度的改变量(m/s)
    if (v0 < v1) {
      W_map.insert(std::make_pair(t0, 0.0));
    } else if (v0 >= v1 && v0 <= vm) {
      const double delta_kinetic_energy =
          0.5 * 1000 * train_m_ *
          (pow(v0 / 3.6 + delta_v0, 2) -
           pow(v0 / 3.6, 2)); // 逆过程动能的增加量(J)
      const double produce_reuse_energy =
          delta_kinetic_energy * brake_eta_; // 动能转化而来的再生能量(J)
      W_map.insert(std::make_pair(t0, produce_reuse_energy));
    } else {
      W_map.insert(std::make_pair(t0, 0.0));
    }

    v0 += delta_v0 * 3.6; // 下一时刻的速度(km/h)
    t0 += epsilon;
  }
  return std::move(W_map);
}

// void TractionCalculator::init_train_m() {
//   cout<<passenger_capacity_<<endl;
//   cout<<passenger_m_avg_<<endl;
//   cout<<MCP_cnt_<<endl;
//   cout<<MCP_m_<<endl;
//   cout<<T_cnt<<endl;
//   cout<<T_m_<<endl;
//   train_m_ =
//       passenger_capacity_ * passenger_m_avg_ + MCP_cnt_ * MCP_m_ + T_cnt *
//       T_m_;
//   cout << train_m_ << endl;
// }

} // namespace yaohui
