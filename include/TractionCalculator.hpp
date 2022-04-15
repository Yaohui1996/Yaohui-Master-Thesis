#ifndef YAOHUI_MASTER_THESIS_TRACTIONCALCULATOR_HPP
#define YAOHUI_MASTER_THESIS_TRACTIONCALCULATOR_HPP

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <map>

namespace yaohui {

class TractionCalculator {
public:
  void show() const;
  double v_P_limit() const;       // 功率限制速度(km/h)
  double time_to_P_limit() const; // 从v0=0加速到功率限制阶段初的时间(s)
  // 加速阶段0-tm秒时段内的牵引做功(J)
  std::map<double, double> accelerating_stage_W(double tm) const;
  // 再生制动阶段内0-tm秒的制动回收能量(J)
  std::map<double, double> brake_stage_W(double tm) const;

private:
  const int32_t passenger_capacity_ = 674 * 2; // 定员载客量(人)
  const double passenger_m_avg_ = 0.06;        // 乘客平均质量(t)
  const int32_t MCP_cnt_ = 4;                  // 带司机室的动车数目
  const double MCP_m_ = 37.4;                  // 带司机室的动车质量(t)
  const int32_t T_cnt = 2;                     // 不带司机室的拖车数目
  const double T_m_ = 34.2; // 不带司机室的拖车质量(t)
  const double train_m_ = passenger_capacity_ * passenger_m_avg_ +
                          MCP_cnt_ * MCP_m_ + T_cnt * T_m_; // 列车质量 (t)
  const double F_const_ = 352.0;  // 最大牵引力(kN)
  const double P_limit_ = 3680.0; // 最大牵引功率(kW)
  const double g_ = 9.8;          // 重力加速度 m·s^(-2)
  const double Ls_ = 2000.0;      // 隧道长度 (m)
  const double L_ = 119.88;       // 列车长度 (m)
  const double i_ = 0.0;          // 坡度的千分数
  const double r_ = std::numeric_limits<double>::infinity(); // 曲线半径 (m)
  const bool is_uphill_ = false; // 是否为上坡, 上坡取true, 下坡取false.
  const double F_brake_ = 384.0;   // 再生制动阶段制动力(kN)
  const double brake_eta_ = 0.629; // 再生制动能量占动能的比
  std::map<int32_t, double> power_time_data_ = {};

  /**
   *
   * @param v 列车运行速度 (km/h)
   * @return 单位基本阻力 (N/kN)
   */
  double f_w0(double v) const;

  /**
   *
   * @return 单位曲线附加阻力 (N/kN)
   */
  double f_wr() const;

  /**
   *
   * @return 单位坡道附加阻力 (N/kN)
   */
  double f_wi() const;

  /**
   *
   * @param v 列车运行速度 (km/h)
   * @return 单位隧道附加阻力 (N/kN)
   */
  double f_ws(double v) const;

  /**
   *
   * @param v 列车运行速度 (km/h)
   * @return 列车受到的总运行阻力 (kN)
   */
  double f_total(double v) const;

  /**
   *
   * @param v 列车运行速度 (km/h)
   * @return 列车受到的总牵引力  (kN)
   */
  double F_traction(double v) const;

  /**
   *
   * @param v 列车运行速度 (km/h)
   * @return 列车牵引功率 (kW)
   */
  double P_traction(double v) const;
};

} // namespace yaohui

#endif // YAOHUI_MASTER_THESIS_TRACTIONCALCULATOR_HPP
