#include "gtest/gtest.h"

#include "p3a_quantity.hpp"

TEST(quantity, multiply) {
  static_assert(std::is_same_v<
      p3a::watt,
      p3a::unit<p3a::dimension<-3, 2, 1>>>,
      "Watt is the SI unit equal to kg * m^2 * s^-3");
  static_assert(std::is_same_v<
      p3a::second,
      p3a::unit<p3a::dimension<1, 0, 0>>>,
      "Second is the SI unit with time dimension");
  static_assert(std::is_same_v<
      p3a::joule,
      p3a::unit<p3a::dimension<-2, 2, 1>>>,
      "Joule is the SI unit equal to kg * m^2 * s^-2");
  static_assert(std::is_same_v<
      p3a::unit_multiply<p3a::watt, p3a::second>,
      p3a::joule>,
      "Watt times second = joule");
  auto a = p3a::watts<double>(1.0) * p3a::seconds<double>(2.0);
  static_assert(std::is_same_v<decltype(a), p3a::joules<double>>,
      "Watts times seconds should be Joules");
  EXPECT_FLOAT_EQ(a.value(), 2.0);
}

TEST(quantity, divide) {
  auto a = p3a::meters<double>(1.0) / p3a::seconds<double>(2.0);
  static_assert(std::is_same_v<decltype(a), p3a::meters_per_second<double>>,
      "meters times seconds should be meters per second");
  EXPECT_FLOAT_EQ(a.value(), 0.5);
}

TEST(quantity, temperature) {
  auto absolute_zero_in_kelvin = p3a::degrees_kelvin<double>(0.0);
  auto absolute_zero_in_celcius =
    p3a::degrees_celcius<double>(absolute_zero_in_kelvin);
  EXPECT_FLOAT_EQ(absolute_zero_in_celcius.value(), -273.15);
  auto absolute_zero_in_fahrenheit =
    p3a::degrees_fahrenheit<double>(absolute_zero_in_kelvin);
  EXPECT_FLOAT_EQ(absolute_zero_in_fahrenheit.value(), -459.67);
  auto const human_fever_temperature_fahrenheit =
    p3a::degrees_fahrenheit<double>(100.4);
  auto const human_fever_temperature_celcius =
    p3a::degrees_celcius<double>(human_fever_temperature_fahrenheit);
  EXPECT_FLOAT_EQ(human_fever_temperature_celcius.value(), 38.0);
  auto const water_freezing_point_celcius =
    p3a::degrees_celcius<double>(0.0);
  auto const water_freezing_point_fahrenheit =
    p3a::degrees_fahrenheit<double>(water_freezing_point_celcius);
  EXPECT_FLOAT_EQ(water_freezing_point_fahrenheit.value(), 32.0);
}

TEST(quantity, percent) {
  auto eighty_percent = p3a::percentage<double>(80.0);
  auto point_eight = p3a::unitless<double>(eighty_percent);
  EXPECT_FLOAT_EQ(point_eight.value(), 0.80);
}

TEST(quantity, thou) {
  using thou = p3a::milli<p3a::inch>;
  auto one_thou = p3a::quantity<thou, double>(1.0);
  auto one_thou_in_micrometers =
    p3a::micrometers<double>(one_thou);
  EXPECT_FLOAT_EQ(one_thou_in_micrometers.value(), 25.4);
}

TEST(quantity, electronvolt) {
  auto const fusion_plasma_temp_in_eV =
    p3a::electronvolt_temperature<double>(15.0e3);
  auto const fusion_plasma_temp_in_K =
    p3a::degrees_kelvin<double>(fusion_plasma_temp_in_eV);
  EXPECT_FLOAT_EQ(
      fusion_plasma_temp_in_K.value(),
      1.74067771800000012e+08);
}
