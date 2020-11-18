#pragma once

#include "p3a_execution.hpp"
#include "p3a_functions.hpp"

namespace p3a {

template <class ForwardIt, class UnaryFunction>
CPL_ALWAYS_INLINE inline constexpr
void for_each(
    local_execution,
    ForwardIt first,
    ForwardIt last,
    UnaryFunction f)
{
  for (; first != last; ++first) {
    f(*first);
  }
}

template <class ForwardIt, class UnaryFunction>
CPL_NEVER_INLINE
void for_each(
    serial_execution,
    ForwardIt first,
    ForwardIt last,
    UnaryFunction f)
{
  for (; first != last; ++first) {
    f(*first);
  }
}

#ifdef __CUDACC__

namespace details {

template <class F, class ForwardIt>
__global__
void cuda_for_each(F f, ForwardIt first, ForwardIt last) {
  using difference_type = typename std::iterator_traits<ForwardIt>::difference_type;
  auto const i = static_cast<difference_type>(
          threadIdx.x + blockIdx.x * blockDim.x);
  ForwardIt const it = first + i;
  if (it < last) f(*it);
}

}

template <class ForwardIt, class UnaryFunction>
CPL_NEVER_INLINE
void for_each(
    cuda_execution policy,
    ForwardIt first,
    ForwardIt last,
    UnaryFunction f)
{
  auto const n = last - first;
  if (n == 0) return;
  dim3 const cuda_block(32, 1, 1);
  dim3 const cuda_grid(ceildiv(unsigned(n), cuda_block.x), 1, 1);
  std::size_t const shared_memory_bytes = 0;
  cudaStream_t const cuda_stream = policy.cuda_stream();
  details::cuda_for_each<<<
    cuda_grid,
    cuda_block,
    shared_memory_bytes,
    cuda_stream>>>(f, first, last);
}

#endif

template <class Functor>
CPL_NEVER_INLINE void for_each(
    serial_execution,
    grid3 grid,
    Functor functor)
{
  for (int k = 0; k < grid.extents().z(); ++k) {
    for (int j = 0; j < grid.extents().y(); ++j) {
      for (int i = 0; i < grid.extents().x(); ++i) {
        functor(vector3<int>(i, j, k));
      }
    }
  }
}

template <class Functor>
CPL_ALWAYS_INLINE constexpr void for_each(
    local_execution,
    grid3 const& grid,
    Functor const& functor)
{
  for (int k = 0; k < grid.extents().z(); ++k) {
    for (int j = 0; j < grid.extents().y(); ++j) {
      for (int i = 0; i < grid.extents().x(); ++i) {
        functor(vector3<int>(i, j, k));
      }
    }
  }
}

template <class Functor>
CPL_NEVER_INLINE void for_each(
    serial_execution,
    subgrid3 subgrid,
    Functor functor)
{
  for (int k = subgrid.lower().z(); k < subgrid.upper().z(); ++k) {
    for (int j = subgrid.lower().y(); j < subgrid.upper().y(); ++j) {
      for (int i = subgrid.lower().x(); i < subgrid.upper().x(); ++i) {
        functor(vector3<int>(i, j, k));
      }
    }
  }
}

template <class Functor>
CPL_ALWAYS_INLINE constexpr void for_each(
    local_execution,
    subgrid3 const& subgrid,
    Functor const& functor)
{
  for (int k = subgrid.lower().z(); k < subgrid.upper().z(); ++k) {
    for (int j = subgrid.lower().y(); j < subgrid.upper().y(); ++j) {
      for (int i = subgrid.lower().x(); i < subgrid.upper().x(); ++i) {
        functor(vector3<int>(i, j, k));
      }
    }
  }
}

}