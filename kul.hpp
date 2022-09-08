#pragma once

/* The Kokkos-ified Units Library
 *
 * This is a header-only C++17 library based on Kokkos:
 * https://github.com/kokkos/kokkos
 * This library provides C++ classes to represent physical units
 * and physical quantities (floating-point quantities which have
 * physical units).
 * What distinguishes KUL from similar projects is that it is
 * based on Kokkos so the compile-time-unit quantities can be used
 * on all hardware Kokkos supports including NVIDIA and AMD GPUs
 * inside CUDA and HIP kernels.
 */

#include <string>
#include <memory>
#include <vector>

#include <Kokkos_Core.hpp>

namespace kul {

// Section [rational]: constexpr-compatible "runtime" rational number type

KOKKOS_INLINE_FUNCTION constexpr std::int64_t abs(std::int64_t a)
{
  return (a < 0) ? -a : a;
}

KOKKOS_INLINE_FUNCTION constexpr std::int64_t gcd(std::int64_t a, std::int64_t b)
{
  while (b != 0) {
    auto const t = b;
    b = a % b;
    a = t;
  }
  return a;
}

class rational {
  std::int64_t m_numerator;
  std::int64_t m_denominator;
 public:
  KOKKOS_INLINE_FUNCTION constexpr rational(std::int64_t numerator_arg, std::int64_t denominator_arg)
    :m_numerator(0)
    ,m_denominator(0)
  {
    auto const abs_num_arg = kul::abs(numerator_arg);
    auto const abs_den_arg = kul::abs(denominator_arg);
    auto const common = kul::gcd(abs_num_arg, abs_den_arg);
    auto const abs_num = abs_num_arg / common;
    auto const abs_den = abs_den_arg / common;
    auto const is_negative = (!(numerator_arg < 0)) != (!(denominator_arg < 0));
    m_numerator = is_negative ? -abs_num : abs_num;
    m_denominator = abs_den;
  }
  KOKKOS_INLINE_FUNCTION constexpr rational(std::int64_t numerator_arg)
    :rational(numerator_arg, 1)
  {
  }
  KOKKOS_INLINE_FUNCTION constexpr std::int64_t numerator() const
  {
    return m_numerator;
  }
  KOKKOS_INLINE_FUNCTION constexpr std::int64_t denominator() const
  {
    return m_denominator;
  }
  template <class T>
  KOKKOS_INLINE_FUNCTION constexpr T convert_to() const
  {
    return T(m_numerator) / T(m_denominator);
  }
};

KOKKOS_INLINE_FUNCTION constexpr rational inverse(rational const& a)
{
  return rational(a.denominator(), a.numerator());
}

KOKKOS_INLINE_FUNCTION constexpr rational operator*(rational const& a, rational const& b)
{
  return rational(a.numerator() * b.numerator(), a.denominator() * b.denominator());
}

KOKKOS_INLINE_FUNCTION constexpr rational operator/(rational const& a, rational const& b)
{
  return a * inverse(b);
}

KOKKOS_INLINE_FUNCTION constexpr bool operator==(rational const& a, rational const& b)
{
  return (a.numerator() == b.numerator()) && (a.denominator() == b.denominator());
}

KOKKOS_INLINE_FUNCTION constexpr bool operator!=(rational const& a, rational const& b)
{
  return !operator==(a, b);
}

KOKKOS_INLINE_FUNCTION constexpr rational pow(rational const& b, int e)
{
  rational result{1};
  for (int i = 0; i < e; ++i) {
    result = result * b;
  }
  for (int i = 0; i < -e; ++i) {
    result = result / b;
  }
  return result;
}

// Section [dimension]: constexpr-compatible "runtime" SI dimension type

class dimension {
  int m_time_exponent;
  int m_length_exponent;
  int m_mass_exponent;
  int m_electric_current_exponent;
  int m_temperature_exponent;
  int m_amount_of_substance_exponent;
  int m_luminous_intensity_exponent;
 public:
  KOKKOS_INLINE_FUNCTION constexpr dimension(
      int time_exponent_arg,
      int length_exponent_arg,
      int mass_exponent_arg,
      int electric_current_exponent_arg = 0,
      int temperature_exponent_arg = 0,
      int amount_of_substance_exponent_arg = 0,
      int luminous_intensity_arg = 0)
    :m_time_exponent(time_exponent_arg)
    ,m_length_exponent(length_exponent_arg)
    ,m_mass_exponent(mass_exponent_arg)
    ,m_electric_current_exponent(electric_current_exponent_arg)
    ,m_temperature_exponent(temperature_exponent_arg)
    ,m_amount_of_substance_exponent(amount_of_substance_exponent_arg)
    ,m_luminous_intensity_exponent(luminous_intensity_arg)
  {
  }
  KOKKOS_INLINE_FUNCTION constexpr int time_exponent() const
  {
    return m_time_exponent;
  }
  KOKKOS_INLINE_FUNCTION constexpr int length_exponent() const
  {
    return m_length_exponent;
  }
  KOKKOS_INLINE_FUNCTION constexpr int mass_exponent() const
  {
    return m_mass_exponent;
  }
  KOKKOS_INLINE_FUNCTION constexpr int electric_current_exponent() const
  {
    return m_electric_current_exponent;
  }
  KOKKOS_INLINE_FUNCTION constexpr int temperature_exponent() const
  {
    return m_temperature_exponent;
  }
  KOKKOS_INLINE_FUNCTION constexpr int amount_of_substance_exponent() const
  {
    return m_amount_of_substance_exponent;
  }
  KOKKOS_INLINE_FUNCTION constexpr int luminous_intensity_exponent() const
  {
    return m_luminous_intensity_exponent;
  }
  KOKKOS_INLINE_FUNCTION static constexpr dimension dimensionless()
  {
    return dimension(0, 0, 0);
  }
  KOKKOS_INLINE_FUNCTION static constexpr dimension time()
  {
    return dimension(1, 0, 0);
  }
  KOKKOS_INLINE_FUNCTION static constexpr dimension length()
  {
    return dimension(0, 1, 0);
  }
  KOKKOS_INLINE_FUNCTION static constexpr dimension mass()
  {
    return dimension(0, 0, 1);
  }
  KOKKOS_INLINE_FUNCTION static constexpr dimension electric_current()
  {
    return dimension(0, 0, 0, 1);
  }
  KOKKOS_INLINE_FUNCTION static constexpr dimension temperature()
  {
    return dimension(0, 0, 0, 0, 1);
  }
  KOKKOS_INLINE_FUNCTION static constexpr dimension amount_of_substance()
  {
    return dimension(0, 0, 0, 0, 0, 1);
  }
  KOKKOS_INLINE_FUNCTION static constexpr dimension luminous_intensity()
  {
    return dimension(0, 0, 0, 0, 0, 0, 1);
  }
};

KOKKOS_INLINE_FUNCTION constexpr dimension operator*(dimension const& a, dimension const& b)
{
  return dimension(
      a.time_exponent() + b.time_exponent(),
      a.length_exponent() + b.length_exponent(),
      a.mass_exponent() + b.mass_exponent(),
      a.electric_current_exponent() + b.electric_current_exponent(),
      a.temperature_exponent() + b.temperature_exponent(),
      a.amount_of_substance_exponent() + b.amount_of_substance_exponent(),
      a.luminous_intensity_exponent() + b.luminous_intensity_exponent());
}

KOKKOS_INLINE_FUNCTION constexpr dimension operator/(dimension const& a, dimension const& b)
{
  return dimension(
      a.time_exponent() - b.time_exponent(),
      a.length_exponent() - b.length_exponent(),
      a.mass_exponent() - b.mass_exponent(),
      a.electric_current_exponent() - b.electric_current_exponent(),
      a.temperature_exponent() - b.temperature_exponent(),
      a.amount_of_substance_exponent() - b.amount_of_substance_exponent(),
      a.luminous_intensity_exponent() - b.luminous_intensity_exponent());
}

KOKKOS_INLINE_FUNCTION constexpr dimension pow(dimension const& d, int e)
{
  return dimension(
      d.time_exponent() * e,
      d.length_exponent() * e,
      d.mass_exponent() * e,
      d.electric_current_exponent() * e,
      d.temperature_exponent() * e,
      d.amount_of_substance_exponent() * e,
      d.luminous_intensity_exponent() * e);
}

KOKKOS_INLINE_FUNCTION constexpr bool operator==(dimension const& a, dimension const& b)
{
  return (a.time_exponent() == b.time_exponent()) &&
         (a.length_exponent() == b.length_exponent()) &&
         (a.mass_exponent() == b.mass_exponent()) &&
         (a.electric_current_exponent() == b.electric_current_exponent()) &&
         (a.temperature_exponent() == b.temperature_exponent()) &&
         (a.amount_of_substance_exponent() == b.amount_of_substance_exponent()) &&
         (a.luminous_intensity_exponent() == b.luminous_intensity_exponent());
}

KOKKOS_INLINE_FUNCTION constexpr bool operator!=(dimension const& a, dimension const& b)
{
  return !operator==(a, b);
}

// Section [optiona]: constexpr-compatible and Kokkosified version of std::optional<T>

class nullopt_t {};
inline constexpr nullopt_t nullopt = {};

template <class T>
class optional
{
  bool m_has_value{false};
  alignas(alignof(T)) char m_value[sizeof(T)];
 public:
  KOKKOS_INLINE_FUNCTION constexpr optional(nullopt_t)
  {
  }
  KOKKOS_INLINE_FUNCTION constexpr optional(T const& value)
  {
    ::new (static_cast<void*>(m_value)) T(value);
    m_has_value = true;
  }
  KOKKOS_INLINE_FUNCTION constexpr bool has_value() const
  {
    return m_has_value;
  }
  KOKKOS_INLINE_FUNCTION constexpr T& value()
  {
    return *(reinterpret_cast<T*>(m_value));
  }
  KOKKOS_INLINE_FUNCTION constexpr T const& value() const
  {
    return *(reinterpret_cast<T const*>(m_value));
  }
};

template <class T>
KOKKOS_INLINE_FUNCTION constexpr bool operator==(optional<T> const& a, optional<T> const& b)
{
  if ((!a.has_value()) && (!b.has_value())) {
    return true;
  }
  if (a.has_value() && b.has_value()) {
    return a.value() == b.value();
  }
  return false;
}

// Section [unit]: virtual base, derived Curiously Recurring Template Pattern classes for physical unit types,

class unit {
 public:
  virtual ~unit() = default;
  virtual std::string name() const = 0;
  virtual kul::dimension dimension() const = 0; 
  virtual rational magnitude() const = 0;
  virtual optional<rational> origin() const = 0;
  virtual std::unique_ptr<unit> copy() const = 0;
  virtual std::unique_ptr<unit> simplify() const = 0;
};

inline bool operator==(unit const& a, unit const& b)
{
  return a.dimension() == b.dimension() &&
    a.magnitude() == b.magnitude() &&
    a.origin() == b.origin();
}

inline bool operator!=(unit const& a, unit const& b)
{
  return !operator==(a, b);
}

class named : public unit {
 public:
};

template <class Unit>
class crtp : public named {
 public:
  std::string name() const override
  {
    return Unit::static_name();
  }
  kul::dimension dimension() const override 
  {
    return Unit::static_dimension();
  }
  rational magnitude() const override 
  {
    return Unit::static_magnitude();
  }
  optional<rational> origin() const override 
  {
    return Unit::static_origin();
  }
  std::unique_ptr<unit> copy() const override
  {
    return std::make_unique<Unit>();
  }
  std::unique_ptr<unit> simplify() const override
  {
    return this->copy();
  }
};

class unitless : public crtp<unitless> {
 public:
  static std::string static_name() { return "1"; }
  KOKKOS_INLINE_FUNCTION static constexpr kul::dimension static_dimension() { return kul::dimension::dimensionless(); }
  KOKKOS_INLINE_FUNCTION static constexpr rational static_magnitude() { return rational(1); }
  KOKKOS_INLINE_FUNCTION static constexpr optional<rational> static_origin() { return nullopt; }
};

// Section [dynamic]: classes for runtime representation of derived units

class dynamic_unit : public unit {
  std::unique_ptr<unit> m_pointer;
 public:
  dynamic_unit(unit const& u)
    :m_pointer(u.copy())
  {
  }
  dynamic_unit(std::unique_ptr<unit>&& ptr)
    :m_pointer(std::move(ptr))
  {
  }
  dynamic_unit(std::unique_ptr<unit> const& ptr)
    :m_pointer(ptr->copy())
  {
  }
  dynamic_unit(dynamic_unit&&) = default;
  dynamic_unit& operator=(dynamic_unit&&) = default;
  dynamic_unit(dynamic_unit const& other)
    :m_pointer(other.m_pointer->copy())
  {
  }
  dynamic_unit& operator=(dynamic_unit const& other)
  {
    m_pointer = other.m_pointer->copy();
    return *this;
  }
  std::string name() const override
  {
    return m_pointer->name();
  }
  kul::dimension dimension() const override
  {
    return m_pointer->dimension();
  }
  rational magnitude() const override
  {
    return m_pointer->magnitude();
  }
  optional<rational> origin() const override
  {
    return m_pointer->origin();
  }
  std::unique_ptr<unit> copy() const override
  {
    return m_pointer->copy();
  }
  std::unique_ptr<unit> simplify() const override
  {
    return m_pointer->simplify();
  }
  unit const* pointer() const
  {
    return m_pointer.get();
  }
  unit* pointer()
  {
    return m_pointer.get();
  }
  bool is_unitless() const
  {
    return dynamic_cast<unitless const*>(m_pointer.get()) != nullptr;
  }
};

class dynamic_exp : public unit {
  dynamic_unit m_base;
  int m_exponent;
 public:
  dynamic_exp(dynamic_unit base_arg, int exponent_arg)
    :m_base(base_arg)
    ,m_exponent(exponent_arg)
  {
  }
  std::string name() const override
  {
    return m_base.name() + "^" + std::to_string(m_exponent);
  }
  kul::dimension dimension() const override
  {
    return kul::pow(m_base.dimension(), m_exponent);
  }
  rational magnitude() const override
  {
    return kul::pow(m_base.magnitude(), m_exponent);
  }
  optional<rational> origin() const override
  {
    return nullopt;
  }
  std::unique_ptr<unit> copy() const override
  {
    return std::make_unique<dynamic_exp>(*this);
  }
  std::unique_ptr<unit> simplify() const override
  {
    if (m_exponent == 0) return unitless().copy();
    if (m_exponent == 1) return m_base.copy();
    return copy();
  }
  dynamic_unit const& base() const
  {
    return m_base;
  }
  int exponent() const
  {
    return m_exponent;
  }
};

class dynamic_product : public unit {
  std::vector<dynamic_unit> m_terms;
 public:
  void push_back(dynamic_unit const& term)
  {
    m_terms.push_back(term);
  }
  void push_back_unless_unitless(dynamic_unit const& term)
  {
    if (dynamic_cast<unitless const*>(term.pointer()) == nullptr) {
      push_back(term);
    }
  }
  void multiply_with(dynamic_exp const& new_exp)
  {
    for (auto& existing_any : m_terms) {
      auto& existing_exp = dynamic_cast<dynamic_exp&>(*(existing_any.pointer()));
      if (existing_exp.base() == new_exp.base()) {
        existing_exp = dynamic_exp(new_exp.base(),
            existing_exp.exponent() + new_exp.exponent());
        return;
      }
    }
    push_back(new_exp);
  }
  void divide_by(dynamic_exp const& new_exp)
  {
    multiply_with(dynamic_exp(new_exp.base(), -new_exp.exponent()));
  }
  void multiply_with(named const& new_named)
  {
    multiply_with(dynamic_exp(new_named, 1));
  }
  void divide_by(named const& new_named)
  {
    divide_by(dynamic_exp(new_named, 1));
  }
  void multiply_with(dynamic_product const& other_product)
  {
    for (auto& term : other_product.m_terms) {
      multiply_with(term);
    }
  }
  void divide_by(dynamic_product const& other_product)
  {
    for (auto& term : other_product.m_terms) {
      divide_by(term);
    }
  }
  void multiply_with(dynamic_unit const& new_unit)
  {
    auto ptr = new_unit.pointer();
    auto product_ptr = dynamic_cast<dynamic_product const*>(ptr);
    if (product_ptr) {
      multiply_with(*product_ptr);
      return;
    }
    auto exp_ptr = dynamic_cast<dynamic_exp const*>(ptr);
    if (exp_ptr) {
      multiply_with(*exp_ptr);
      return;
    }
    auto named_ptr = dynamic_cast<named const*>(ptr);
    if (named_ptr) {
      multiply_with(*named_ptr);
      return;
    }
  }
  void divide_by(dynamic_unit const& new_unit)
  {
    auto ptr = new_unit.pointer();
    auto product_ptr = dynamic_cast<dynamic_product const*>(ptr);
    if (product_ptr) {
      divide_by(*product_ptr);
      return;
    }
    auto exp_ptr = dynamic_cast<dynamic_exp const*>(ptr);
    if (exp_ptr) {
      divide_by(*exp_ptr);
      return;
    }
    auto named_ptr = dynamic_cast<named const*>(ptr);
    if (named_ptr) {
      divide_by(*named_ptr);
      return;
    }
  }
  std::string name() const override
  {
    auto it = m_terms.begin();
    auto result = it->name();
    ++it;
    while (it != m_terms.end()) {
      result += " * ";
      result += it->name();
      ++it;
    }
    return result;
  }
  kul::dimension dimension() const override
  {
    auto it = m_terms.begin();
    auto result = it->dimension();
    ++it;
    while (it != m_terms.end()) {
      result = result * it->dimension();
      ++it;
    }
    return result;
  }
  rational magnitude() const override
  {
    auto it = m_terms.begin();
    auto result = it->magnitude();
    ++it;
    while (it != m_terms.end()) {
      result = result * it->magnitude();
      ++it;
    }
    return result;
  }
  optional<rational> origin() const override
  {
    return nullopt;
  }
  std::unique_ptr<unit> copy() const override
  {
    return std::make_unique<dynamic_product>(*this);
  }
  std::unique_ptr<unit> simplify() const override
  {
    auto result = dynamic_product();
    for (auto& u : m_terms) {
      result.push_back_unless_unitless(u.simplify());
    }
    if (result.m_terms.empty()) return unitless().copy();
    if (result.m_terms.size() == 1) return result.m_terms.front().copy();
    return result.copy();
  }
  std::vector<dynamic_unit> const& terms() const
  {
    return m_terms;
  }
};

dynamic_unit operator*(dynamic_unit const& a, dynamic_unit const& b)
{
  dynamic_product p;
  p.multiply_with(a);
  p.multiply_with(b);
  return p.simplify();
}

dynamic_unit operator/(dynamic_unit const& a, dynamic_unit const& b)
{
  dynamic_product p;
  p.multiply_with(a);
  p.divide_by(b);
  return p.simplify();
}

inline dynamic_unit root(dynamic_unit const& base, int exponent)
{
  auto ptr = base.pointer();
  auto named_ptr = dynamic_cast<named const*>(ptr);
  if (named_ptr) {
    throw std::runtime_error("cannot take " + std::to_string(exponent) + "th root of named unit");
  }
  auto exp_ptr = dynamic_cast<dynamic_exp const*>(ptr);
  if (exp_ptr) {
    if (exp_ptr->exponent() % exponent != 0) {
      throw std::runtime_error("taking " + std::to_string(exponent) + "th root of non-divisible "
          + std::to_string(exp_ptr->exponent()) + "th power of " + exp_ptr->base().name());
    }
    return dynamic_exp(exp_ptr->base(), exp_ptr->exponent() / exponent).simplify();
  }
  auto product_ptr = dynamic_cast<dynamic_product const*>(ptr);
  if (product_ptr) {
    auto result = dynamic_product();
    for (auto& term : product_ptr->terms()) {
      result.multiply_with(root(term, exponent));
    }
    return result.simplify();
  }
  throw std::logic_error("unexpected type");
}

inline dynamic_unit sqrt(dynamic_unit const& base)
{
  return root(base, 2);
}

inline dynamic_unit cbrt(dynamic_unit const& base)
{
  return root(base, 3);
}

// Section [static]: Compile-time implementations of derived unit operations

template <class Base, int Exponent>
class static_pow : public crtp<static_pow<Base, Exponent>> {
 public:
  static std::string static_name() { return Base::static_name() + "^" + std::to_string(Exponent); }
  KOKKOS_INLINE_FUNCTION static constexpr kul::dimension static_dimension() { return kul::pow(Base::static_dimension(), Exponent); }
  KOKKOS_INLINE_FUNCTION static constexpr rational static_magnitude() { return kul::pow(Base::static_magnitude(), Exponent); }
  KOKKOS_INLINE_FUNCTION static constexpr optional<rational> static_origin() { return nullopt; }
  std::unique_ptr<unit> copy() const override
  {
    return dynamic_exp(Base(), Exponent).copy();
  }
};

template <class... Units>
class static_product;

template <>
class static_product<> : public crtp<static_product<>> {
 public:
  static std::string static_name() { return "1"; }
  KOKKOS_INLINE_FUNCTION static constexpr kul::dimension static_dimension() { return kul::dimension::dimensionless(); }
  KOKKOS_INLINE_FUNCTION static constexpr rational static_magnitude() { return kul::rational(1); }
  KOKKOS_INLINE_FUNCTION static constexpr optional<rational> static_origin() { return nullopt; }
  std::unique_ptr<unit> copy() const override
  {
    return dynamic_product().copy();
  }
};

template <class Unit>
class static_product<Unit> : public crtp<static_product<Unit>> {
 public:
  static std::string static_name() { return Unit::static_name(); }
  KOKKOS_INLINE_FUNCTION static constexpr kul::dimension static_dimension() { return Unit::static_dimension(); }
  KOKKOS_INLINE_FUNCTION static constexpr rational static_magnitude() { return Unit::static_magnitude(); }
  KOKKOS_INLINE_FUNCTION static constexpr optional<rational> static_origin() { return nullopt; }
  std::unique_ptr<unit> copy() const override
  {
    auto p = dynamic_product();
    p.multiply_with(Unit());
    return p.copy();
  }
};

template <class FirstUnit, class... OtherUnits>
class static_product<FirstUnit, OtherUnits...> : public crtp<static_product<FirstUnit, OtherUnits...>> {
 public:
  using tail_type = static_product<OtherUnits...>;
  static std::string static_name()
  {
    return FirstUnit::static_name() + " * " + tail_type::static_name();
  }
  KOKKOS_INLINE_FUNCTION static constexpr kul::dimension static_dimension()
  {
    return FirstUnit::static_dimension() * tail_type::static_dimension();
  }
  KOKKOS_INLINE_FUNCTION static constexpr rational static_magnitude()
  {
    return FirstUnit::static_magnitude() * tail_type::static_magnitude();
  }
  KOKKOS_INLINE_FUNCTION static constexpr optional<rational> static_origin() { return nullopt; }
  std::unique_ptr<unit> copy() const override
  {
    auto p = dynamic_product();
    p.multiply_with(FirstUnit());
    p.multiply_with(tail_type::copy());
    return p.copy();
  }
};

template <class A, class B>
class push_back;

template <class... Units, class LastUnit>
class push_back<static_product<Units...>, LastUnit> {
 public:
  using type = static_product<Units..., LastUnit>;
};

template <class A, class B>
using push_back_t = typename push_back<A, B>::type;

template <class A, class B>
class prepend;

template <class FirstUnit, class... Units>
class prepend<FirstUnit, static_product<Units...>> {
 public:
  using type = static_product<FirstUnit, Units...>;
};

template <class A, class B>
using prepend_t = typename prepend<A, B>::type;

template <class A, class B>
class push_back_unless_unitless {
 public:
  using type = push_back_t<A, B>;
};

template <class A>
class push_back_unless_unitless<A, unitless> {
 public:
  using type = A;
};

template <class A, class B>
using push_back_unless_unitless_t = typename push_back_unless_unitless<A, B>::type;

template <class A, class B>
class prepend_unless_unitless {
 public:
  using type = prepend_t<A, B>;
};

template <class B>
class prepend_unless_unitless<unitless, B> {
 public:
  using type = B;
};

template <class A, class B>
using prepend_unless_unitless_t = typename prepend_unless_unitless<A, B>::type;

template <class A, class B>
class multiply_with {
 public:
  using type = typename multiply_with<A, static_pow<B, 1>>::type;
};

template <class Base, int Exponent>
class multiply_with<static_product<>, static_pow<Base, Exponent>> {
 public:
  using type = static_product<static_pow<Base, Exponent>>;
};

template <class FirstUnit, class... NextUnits, class Base, int Exponent>
class multiply_with<static_product<FirstUnit, NextUnits...>, static_pow<Base, Exponent>> {
 public:
  using type = prepend_t<FirstUnit, typename multiply_with<static_product<NextUnits...>, static_pow<Base, Exponent>>::type>;
};

template <class... NextUnits, class Base, int Exponent1, int Exponent2>
class multiply_with<static_product<static_pow<Base, Exponent1>, NextUnits...>, static_pow<Base, Exponent2>> {
 public:
  using type = prepend_t<static_pow<Base, Exponent1 + Exponent2>, static_product<NextUnits...>>;
};

template <class LHS>
class multiply_with<LHS, static_product<>> {
 public:
  using type = LHS;
};

template <class LHS, class FirstUnit, class... NextUnits>
class multiply_with<LHS, static_product<FirstUnit, NextUnits...>> {
  using product_with_first = typename multiply_with<LHS, FirstUnit>::type;
 public:
  using type = typename multiply_with<product_with_first, static_product<NextUnits...>>::type;
};

template <class A, class B>
using multiply_with_t = typename multiply_with<A, B>::type;

template <class A, class B>
class divide_by {
 public:
  using type = typename divide_by<A, static_pow<B, 1>>::type;
};

template <class Base, int Exponent>
class divide_by<static_product<>, static_pow<Base, Exponent>> {
 public:
  using type = static_product<static_pow<Base, -Exponent>>;
};

template <class FirstUnit, class... NextUnits, class Base, int Exponent>
class divide_by<static_product<FirstUnit, NextUnits...>, static_pow<Base, Exponent>> {
 public:
  using type = prepend_t<FirstUnit, typename divide_by<static_product<NextUnits...>, static_pow<Base, Exponent>>::type>;
};

template <class... NextUnits, class Base, int Exponent1, int Exponent2>
class divide_by<static_product<static_pow<Base, Exponent1>, NextUnits...>, static_pow<Base, Exponent2>> {
 public:
  using type = prepend_t<static_pow<Base, Exponent1 - Exponent2>, static_product<NextUnits...>>;
};

template <class LHS>
class divide_by<LHS, static_product<>> {
 public:
  using type = LHS;
};

template <class LHS, class FirstUnit, class... NextUnits>
class divide_by<LHS, static_product<FirstUnit, NextUnits...>> {
  using product_with_first = typename divide_by<LHS, FirstUnit>::type;
 public:
  using type = typename divide_by<product_with_first, static_product<NextUnits...>>::type;
};

template <class A, class B>
using divide_by_t = typename divide_by<A, B>::type;

template <class T>
class simplify {
 public:
  using type = T;
};

template <class Base>
class simplify<static_pow<Base, 0>> {
 public:
  using type = unitless;
};

template <class Base>
class simplify<static_pow<Base, 1>> {
 public:
  using type = Base;
};

template <class T>
class simplify_terms;

template <>
class simplify_terms<static_product<>> {
 public:
  using type = static_product<>;
};

template <class FirstUnit, class... NextUnits>
class simplify_terms<static_product<FirstUnit, NextUnits...>> {
  using simplified_first_unit = typename simplify<FirstUnit>::type;
  using simplified_next_units = typename simplify_terms<static_product<NextUnits...>>::type;
 public:
  using type = prepend_unless_unitless_t<simplified_first_unit, simplified_next_units>;
};

template <class T>
class simplify_product {
 public:
  using type = T;
};

template <>
class simplify_product<static_product<>> {
 public:
  using type = unitless;
};

template <class T>
class simplify_product<static_product<T>> {
 public:
  using type = T;
};

template <class... Terms>
class simplify<static_product<Terms...>> {
  using simplified_terms = typename simplify_terms<static_product<Terms...>>::type;
 public:
  using type = typename simplify_product<simplified_terms>::type;
};

template <class T>
using simplify_t = typename simplify<T>::type;

template <class T>
using canonicalize_t = multiply_with_t<static_product<>, T>;

template <class A, class B>
using multiply = simplify_t<multiply_with_t<canonicalize_t<A>, B>>;

template <class A, class B>
using divide = simplify_t<divide_by_t<canonicalize_t<A>, B>>;

template <class A, int Exponent>
class static_root_helper;

template <class A, int Exponent1, int Exponent2>
class static_root_helper<static_pow<A, Exponent1>, Exponent2> {
 public:
  static_assert(Exponent1 % Exponent2 == 0, "taking root of non-divisible power");
  using type = static_pow<A, Exponent1 / Exponent2>;
};

template <int Exponent>
class static_root_helper<static_product<>, Exponent> {
 public:
  using type = static_product<>;
};

template <int Exponent, class FirstUnit, class... NextUnits>
class static_root_helper<static_product<FirstUnit, NextUnits...>, Exponent> {
  using first_root = typename static_root_helper<FirstUnit, Exponent>::type;
  using next_root = typename static_root_helper<static_product<NextUnits...>, Exponent>::type;
 public:
  using type = prepend_t<first_root, next_root>;
};

template <class Base, int Exponent>
using static_root = simplify_t<typename static_root_helper<canonicalize_t<Base>, Exponent>::type>;

template <class Base>
using static_sqrt = static_root<Base, 2>;

template <class Base>
using static_cbrt = static_root<Base, 3>;

template <class A, class B>
inline constexpr bool are_equal =
    A::static_dimension() == B::static_dimension() &&
    A::static_magnitude() == B::static_magnitude() &&
    A::static_origin() == B::static_origin();

template <class T>
class make_relative : public crtp<make_relative<T>> {
 public:
  static std::string static_name() { return T::static_name(); }
  KOKKOS_INLINE_FUNCTION static constexpr kul::dimension static_dimension() { return T::static_dimension(); }
  KOKKOS_INLINE_FUNCTION static constexpr rational static_magnitude() { return T::static_magnitude(); }
  KOKKOS_INLINE_FUNCTION static constexpr optional<rational> static_origin() { return nullopt; }
};

template <class T>
inline constexpr bool is_absolute = T::static_origin().has_value();

template <class T>
inline constexpr bool is_relative = !is_absolute<T>;

// Section [convert]: a constexpr-compatible class representing the conversion between two units

template <class T>
class conversion {
  T m_multiplier;
  T m_offset;
 public:
  KOKKOS_INLINE_FUNCTION constexpr conversion(
      rational const& old_magnitude,
      optional<rational> const& old_origin,
      rational const& new_magnitude,
      optional<rational> const& new_origin)
    :m_multiplier((old_magnitude / new_magnitude).convert_to<T>())
    ,m_offset(0)
  {
    if (old_origin.has_value()) m_offset += (old_origin.value() / new_magnitude).convert_to<T>();
    if (new_origin.has_value()) m_offset -= (new_origin.value() / new_magnitude).convert_to<T>();
  }
  inline conversion(unit const& from, unit const& to)
    :conversion(
        from.magnitude(),
        from.origin(),
        to.magnitude(),
        to.origin())
  {
  }
  KOKKOS_INLINE_FUNCTION constexpr T operator()(T const& old_value) const
  {
    return old_value * m_multiplier + m_offset;
  }
};

template <class T, class From, class To>
inline constexpr conversion<T> static_conversion = conversion<T>(
    From::static_magnitude(), From::static_origin(),
    To::static_magnitude(), To::static_origin());

// Section [prefix]: class template versions of metric prefixes

template <class T>
class kilo : public crtp<kilo<T>> {
 public:
  static std::string static_name() { return "k" + T::static_name(); }
  KOKKOS_INLINE_FUNCTION static constexpr kul::dimension static_dimension() { return T::static_dimension(); }
  KOKKOS_INLINE_FUNCTION static constexpr rational static_magnitude() { return rational(1000) * T::static_magnitude(); }
  KOKKOS_INLINE_FUNCTION static constexpr optional<rational> static_origin() { return nullopt; }
};

template <class T>
class milli : public crtp<milli<T>> {
 public:
  static std::string static_name() { return "m" + T::static_name(); }
  KOKKOS_INLINE_FUNCTION static constexpr kul::dimension static_dimension() { return T::static_dimension(); }
  KOKKOS_INLINE_FUNCTION static constexpr rational static_magnitude() { return rational(1, 1000) * T::static_magnitude(); }
  KOKKOS_INLINE_FUNCTION static constexpr optional<rational> static_origin() { return nullopt; }
};

// Section [named]: units accepted in physics with their own symbol

class second : public crtp<second> {
 public:
  static std::string static_name() { return "s"; }
  KOKKOS_INLINE_FUNCTION static constexpr kul::dimension static_dimension() { return kul::dimension::time(); }
  KOKKOS_INLINE_FUNCTION static constexpr rational static_magnitude() { return rational(1); }
  KOKKOS_INLINE_FUNCTION static constexpr optional<rational> static_origin() { return nullopt; }
};

class meter : public crtp<meter> {
 public:
  static std::string static_name() { return "m"; }
  KOKKOS_INLINE_FUNCTION static constexpr kul::dimension static_dimension() { return kul::dimension::length(); }
  KOKKOS_INLINE_FUNCTION static constexpr rational static_magnitude() { return rational(1); }
  KOKKOS_INLINE_FUNCTION static constexpr optional<rational> static_origin() { return nullopt; }
};

class inch : public crtp<inch> {
 public:
  static std::string static_name() { return "in"; }
  KOKKOS_INLINE_FUNCTION static constexpr kul::dimension static_dimension() { return kul::dimension::length(); }
  KOKKOS_INLINE_FUNCTION static constexpr rational static_magnitude() { return rational(254, 10'000); }
  KOKKOS_INLINE_FUNCTION static constexpr optional<rational> static_origin() { return nullopt; }
};

class gram : public crtp<gram> {
 public:
  static std::string static_name() { return "g"; }
  KOKKOS_INLINE_FUNCTION static constexpr kul::dimension static_dimension() { return kul::dimension::mass(); }
  KOKKOS_INLINE_FUNCTION static constexpr rational static_magnitude() { return rational(1, 1000); }
  KOKKOS_INLINE_FUNCTION static constexpr optional<rational> static_origin() { return nullopt; }
};

class radian : public crtp<radian> {
 public:
  static std::string static_name() { return "rad"; }
  KOKKOS_INLINE_FUNCTION static constexpr kul::dimension static_dimension() { return kul::dimension::dimensionless(); }
  KOKKOS_INLINE_FUNCTION static constexpr rational static_magnitude() { return rational(1); }
  KOKKOS_INLINE_FUNCTION static constexpr optional<rational> static_origin() { return nullopt; }
};

// Section [quantity]: class template for runtime value with associated unit

template <class T, class Unit = dynamic_unit>
class quantity {
  T m_value;
 public:
  using value_type = T;
  using unit_type = Unit;
  KOKKOS_DEFAULTED_FUNCTION quantity() = default;
  KOKKOS_DEFAULTED_FUNCTION constexpr quantity(quantity&&) = default;
  KOKKOS_DEFAULTED_FUNCTION constexpr quantity(quantity const&) = default;
  KOKKOS_DEFAULTED_FUNCTION constexpr quantity& operator=(quantity&&) = default;
  KOKKOS_DEFAULTED_FUNCTION constexpr quantity& operator=(quantity const&) = default;
  template <class U,
      std::enable_if_t<
        (!std::is_same_v<Unit, unitless>) && (std::is_constructible_v<value_type, U>),
        bool> = false>
  KOKKOS_INLINE_FUNCTION constexpr explicit quantity(U const& v)
    :m_value(v)
  {
  }
  template <class U,
      std::enable_if_t<
        std::is_same_v<Unit, unitless> && (std::is_constructible_v<value_type, U>),
        bool> = false>
  KOKKOS_INLINE_FUNCTION constexpr quantity(U const& v)
    :m_value(v)
  {
  }
  KOKKOS_INLINE_FUNCTION constexpr value_type const& value() const { return m_value; }
  KOKKOS_INLINE_FUNCTION constexpr value_type& value() { return m_value; }
  template <class T2, class Unit2,
      std::enable_if_t<are_equal<Unit, Unit2>, bool> = false>
  KOKKOS_INLINE_FUNCTION constexpr
  quantity(quantity<T2, Unit2> const& other)
    :m_value(other.value())
  {
  }
  template <class T2, class Unit2,
      std::enable_if_t<!are_equal<Unit, Unit2>, bool> = false>
  KOKKOS_INLINE_FUNCTION constexpr
  quantity(quantity<T2, Unit2> const& other)
    :m_value(static_conversion<T2, Unit2, Unit>()(other.value()))
  {
    static_assert(Unit::static_dimension() == Unit2::static_dimension(),
        "cannot convert between quantities with different dimensions");
    static_assert(
        (is_absolute<Unit> && is_absolute<Unit2>) ||
        (is_relative<Unit> && is_relative<Unit2>),
        "cannot convert from absolute to relative or vice-versa");
  }
};

template <class T, class Unit>
KOKKOS_INLINE_FUNCTION constexpr auto operator==(quantity<T, Unit> const& a, quantity<T, Unit> const& b)
{
  return a.value() == b.value();
}

template <class T, class Unit>
KOKKOS_INLINE_FUNCTION constexpr auto operator!=(quantity<T, Unit> const& a, quantity<T, Unit> const& b)
{
  return a.value() != b.value();
}

template <class T, class Unit>
KOKKOS_INLINE_FUNCTION constexpr auto operator<=(quantity<T, Unit> const& a, quantity<T, Unit> const& b)
{
  return a.value() <= b.value();
}

template <class T, class Unit>
KOKKOS_INLINE_FUNCTION constexpr auto operator>=(quantity<T, Unit> const& a, quantity<T, Unit> const& b)
{
  return a.value() >= b.value();
}

template <class T, class Unit>
KOKKOS_INLINE_FUNCTION constexpr auto operator<(quantity<T, Unit> const& a, quantity<T, Unit> const& b)
{
  return a.value() < b.value();
}

template <class T, class Unit>
KOKKOS_INLINE_FUNCTION constexpr auto operator>(quantity<T, Unit> const& a, quantity<T, Unit> const& b)
{
  return a.value() > b.value();
}

template <class T1, class Unit, class T2,
    std::enable_if_t<is_relative<Unit>, bool> = false>
KOKKOS_INLINE_FUNCTION constexpr auto operator+(quantity<T1, Unit> const& a, quantity<T2, Unit> const& b)
{
  using T3 = decltype(a.value() + b.value());
  return quantity<T3, Unit>(a.value() + b.value());
}

template <class T1, class Unit, class T2,
    std::enable_if_t<is_relative<Unit>, bool> = false>
KOKKOS_INLINE_FUNCTION constexpr auto operator-(quantity<T1, Unit> const& a, quantity<T2, Unit> const& b)
{
  using T3 = decltype(a.value() - b.value());
  return quantity<T3, Unit>(a.value() - b.value());
}

template <class T1, class Unit, class T2,
    std::enable_if_t<is_absolute<Unit>, bool> = false>
KOKKOS_INLINE_FUNCTION constexpr auto operator-(quantity<T1, Unit> const& a, quantity<T2, Unit> const& b)
{
  using T3 = decltype(a.value() - b.value());
  return quantity<T3, make_relative<Unit>>(a.value() - b.value());
}

template <class T1, class Unit1, class T2, class Unit2>
KOKKOS_INLINE_FUNCTION constexpr auto operator*(quantity<T1, Unit1> const& a, quantity<T2, Unit2> const& b)
{
  using T3 = decltype(a.value() * b.value());
  using Unit3 = multiply<Unit1, Unit2>;
  return quantity<T3, Unit3>(a.value() * b.value());
}

template <class T1, class Unit1, class T2, class Unit2>
KOKKOS_INLINE_FUNCTION constexpr auto operator/(quantity<T1, Unit1> const& a, quantity<T2, Unit2> const& b)
{
  using T3 = decltype(a.value() / b.value());
  using Unit3 = divide<Unit1, Unit2>;
  return quantity<T3, Unit3>(a.value() / b.value());
}

template <class T, class Unit>
KOKKOS_INLINE_FUNCTION constexpr auto abs(quantity<T, Unit> const& q)
{
  return quantity<T, Unit>(Kokkos::abs(q.value())); 
}

template <class T, class Unit>
KOKKOS_INLINE_FUNCTION constexpr auto sqrt(quantity<T, Unit> const& q)
{
  return quantity<T, static_sqrt<Unit>>(Kokkos::sqrt(q.value()));
}

template <class T, class Unit>
KOKKOS_INLINE_FUNCTION constexpr auto cbrt(quantity<T, Unit> const& q)
{
  return quantity<T, static_cbrt<Unit>>(Kokkos::cbrt(q.value()));
}

#define KUL_UNITLESS_UNARY_FUNCTION(FUNC) \
template <class T> \
KOKKOS_INLINE_FUNCTION constexpr auto FUNC(quantity<T, unitless> const& q) \
{ \
  return quantity<T, unitless>(Kokkos::FUNC(q.value())); \
}

KUL_UNITLESS_UNARY_FUNCTION(exp)
KUL_UNITLESS_UNARY_FUNCTION(exp2)
KUL_UNITLESS_UNARY_FUNCTION(log)
KUL_UNITLESS_UNARY_FUNCTION(log10)
KUL_UNITLESS_UNARY_FUNCTION(log2)
KUL_UNITLESS_UNARY_FUNCTION(erf)
KUL_UNITLESS_UNARY_FUNCTION(erfc)
KUL_UNITLESS_UNARY_FUNCTION(tgamma)
KUL_UNITLESS_UNARY_FUNCTION(lgamma)

#undef KUL_UNITLESS_UNARY_FUNCTION

#define KUL_UNARY_TRIG_FUNCTION(FUNC) \
template <class T> \
KOKKOS_INLINE_FUNCTION constexpr auto FUNC(quantity<T, radian> const& q) \
{ \
  return quantity<T, unitless>(Kokkos::FUNC(q.value())); \
}

KUL_UNARY_TRIG_FUNCTION(sin)
KUL_UNARY_TRIG_FUNCTION(cos)
KUL_UNARY_TRIG_FUNCTION(tan)
KUL_UNARY_TRIG_FUNCTION(sinh)
KUL_UNARY_TRIG_FUNCTION(cosh)
KUL_UNARY_TRIG_FUNCTION(tanh)

#undef KUL_UNARY_TRIG_FUNCTION

#define KUL_UNARY_INVERSE_TRIG_FUNCTION(FUNC) \
template <class T> \
KOKKOS_INLINE_FUNCTION constexpr auto FUNC(quantity<T, unitless> const& q) \
{ \
  return quantity<T, radian>(Kokkos::FUNC(q.value())); \
}

KUL_UNARY_INVERSE_TRIG_FUNCTION(asin)
KUL_UNARY_INVERSE_TRIG_FUNCTION(acos)
KUL_UNARY_INVERSE_TRIG_FUNCTION(atan)
KUL_UNARY_INVERSE_TRIG_FUNCTION(asinh)
KUL_UNARY_INVERSE_TRIG_FUNCTION(acosh)
KUL_UNARY_INVERSE_TRIG_FUNCTION(atanh)

#undef KUL_UNARY_INVERSE_TRIG_FUNCTION

#define KUL_UNITLESS_BINARY_FUNCTION(FUNC) \
template <class T> \
KOKKOS_INLINE_FUNCTION constexpr auto FUNC(quantity<T, unitless> const& a, quantity<T, unitless> const& b) \
{ \
  return quantity<T, unitless>(Kokkos::FUNC(a.value(), b.value())); \
}

KUL_UNITLESS_BINARY_FUNCTION(pow)
KUL_UNITLESS_BINARY_FUNCTION(copysign)

#undef KUL_UNITLESS_BINARY_FUNCTION

template <class T, class Unit>
KOKKOS_INLINE_FUNCTION constexpr auto hypot(quantity<T, Unit> const& a, quantity<T, Unit> const& b)
{
  return quantity<T, Unit>(Kokkos::hypot(a.value(), b.value()));
}

template <class T, class Unit>
KOKKOS_INLINE_FUNCTION constexpr auto hypot(
    quantity<T, Unit> const& a,
    quantity<T, Unit> const& b,
    quantity<T, Unit> const& c)
{
  return quantity<T, Unit>(Kokkos::hypot(a.value(), b.value(), c.value()));
}

template <class T>
KOKKOS_INLINE_FUNCTION constexpr auto atan2(quantity<T, unitless> const& a, quantity<T, unitless> const& b)
{
  return quantity<T, radian>(Kokkos::atan2(a.value(), b.value()));
}

template <class T1, class Unit1,
          class T2, class Unit2,
          class T3, class Unit3>
KOKKOS_INLINE_FUNCTION constexpr auto fma(
    quantity<T1, Unit1> const& a,
    quantity<T2, Unit2> const& b,
    quantity<T3, Unit3> const& c)
{
  using T4 = decltype(a.value() * b.value() + c.value());
  using Unit4 = multiply<Unit1, Unit2>;
  static_assert(are_equal<Unit3, Unit4>,
      "fma with compile-time units: (a*b) has different units from c");
  return quantity<T4, Unit3>(Kokkos::fma(a.value(), b.value(), c.value()));
}

template <class T>
class quantity<T, dynamic_unit> {
  T m_value;
  dynamic_unit m_unit;
 public:
  using value_type = T;
  using unit_type = dynamic_unit;
  KOKKOS_DEFAULTED_FUNCTION quantity() = default;
  KOKKOS_DEFAULTED_FUNCTION constexpr quantity(quantity&&) = default;
  KOKKOS_DEFAULTED_FUNCTION constexpr quantity(quantity const&) = default;
  KOKKOS_DEFAULTED_FUNCTION constexpr quantity& operator=(quantity&&) = default;
  KOKKOS_DEFAULTED_FUNCTION constexpr quantity& operator=(quantity const&) = default;
  value_type const& value() const { return m_value; }
  value_type& value() { return m_value; }
  dynamic_unit const& unit() const { return m_unit; }
};

}