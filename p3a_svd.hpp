#pragma once

#include "p3a_eigen.hpp"

namespace p3a {

template <class T>
P3A_HOST P3A_DEVICE
void givens(
    T const& a,
    T const& b,
    T& c,
    T& s)
{
  c = 1.0;
  s = 0.0;
  if (b != 0.0) {
    if (absolute_value(b) > absolute_value(a)) {
      auto const t = -a / b;
      s = 1.0 / square_root(1.0 + t * t);
      c = t * s;
    } else {
      auto const t = -b / a;
      c = 1.0 / square_root(1.0 + t * t);
      s = t * c;
    }
  }
}


// Singular value decomposition (SVD) for 2x2
// bidiagonal matrix. Used for general 2x2 SVD.
// Adapted from LAPAPCK's DLASV2, Netlib's dlasv2.c
// and LBNL computational crystallography toolbox
// \param f, g, h where A = [f, g; 0, h]
// \return \f$ A = USV^T\f$

template <class T>
P3A_HOST P3A_DEVICE
void svd_bidiagonal(
    T f,
    T const& g,
    T h,
    matrix2x2<T>& U,
    matrix2x2<T>& S,
    matrix2x2<T>& V)
{
  T fa = absolute_value(f);
  T ga = absolute_value(g);
  T ha = absolute_value(h);
  T s0 = 0.0;
  T s1 = 0.0;
  T cu = 1.0;
  T su = 0.0;
  T cv = 1.0;
  T sv = 0.0;
  auto const swap_diag = (ha > fa);
  if (swap_diag == true) {
    std::swap(fa, ha);
    std::swap(f, h);
  }
  T constexpr epsilon = std::numeric_limits<T>::epsilon();
  // diagonal matrix
  if (ga == 0.0) {
    s1 = ha;
    s0 = fa;
  } else if (ga > fa && fa / ga < epsilon) {
    // case of very large ga
    s0 = ga;
    s1 = ha > 1.0 ? (fa / (ga / ha)) : ((fa / ga) * ha);
    cu = 1.0;
    su = h / g;
    cv = f / g;
    sv = 1.0;
  } else {
    // normal case
    T const d = fa - ha;
    T const l = d / fa;   // l \in [0,1]
    T const m = g / f;    // m \in (-1/macheps, 1/macheps)
    T const t = 2.0 - l;  // t \in [1,2]
    T const mm = m * m;
    T const tt = t * t;
    T const s = square_root(tt + mm);  // s \in [1,1 + 1/macheps]
    T const r = ((l != 0.0) ? (square_root(l * l + mm))
                               : (absolute_value(m)));  // r \in [0,1 + 1/macheps]
    T const a = 0.5 * (s + r);                 // a \in [1,1 + |m|]
    s1 = ha / a;
    s0 = fa * a;
    // Compute singular vectors
    T tau;  // second assignment to T in DLASV2
    if (mm != 0.0) {
      tau = (m / (s + t) + m / (r + l)) * (1.0 + a);
    } else {
      // note that m is very tiny
      tau = (l == 0.0) ? (std::copysign(2.0, f) * std::copysign(1.0, g))
                       : (g / std::copysign(d, f) + m / t);
    }
    T const lv =
        square_root(tau * tau + 4.0);  // second assignment to L in DLASV2
    cv = 2.0 / lv;
    sv = tau / lv;
    cu = (cv + sv * m) / a;
    su = (h / f) * sv / a;
  }
  // Fix signs of singular values in accordance to sign of singular vectors
  s0 = std::copysign(s0, f);
  s1 = std::copysign(s1, h);
  if (swap_diag == true) {
    std::swap(cu, sv);
    std::swap(su, cv);
  }
  U = matrix2x2<T>(cu, -su, su, cu);
  S = matrix2x2<T>(s0, 0.0, 0.0, s1);
  V = matrix2x2<T>(cv, -sv, sv, cv);
}

template <class T>
P3A_HOST P3A_DEVICE
void svd_2x2(
    matrix2x2<T> const& A,
    matrix2x2<T>& U,
    matrix2x2<T>& S,
    matrix2x2<T>& V)
{
  // First compute a givens rotation to eliminate 1,0 entry in tensor
  T c, s;
  givens(A.xx(), A.yx(), c, s);
  auto const R = matrix2x2<T>(c, -s, s, c);
  auto const B = R * A;
  // B is bidiagonal. Use specialized algorithm to compute its SVD
  matrix2x2<T> U_B, S_B, V_B;
  svd_bidiagonal(B.xx(), B.xy(), B.yy(), U_B, S_B, V_B);
  auto const X = U_B;
  S = S_B;
  V = V_B;
  // Complete general 2x2 SVD with givens rotation calculated above
  U = transpose(R) * X;
}

// R^N singular value decomposition (SVD)
// \param A tensor
// \return \f$ A = USV^T\f$

template <class T, int N>
P3A_HOST P3A_DEVICE
void decompose_svd(
    matrix<T, N, N> const& A,
    matrix<T, N, N>& U,
    matrix<T, N, N>& S,
    matrix<T, N, N>& V)
{
  // Scale first
  T const norm_a = norm(A);
  T const scale = norm_a > 0.0 ? norm_a : T(1.0);
  auto S = A / scale;
  auto U = static_matrix<T, N, N>::identity();
  auto V = static_matrix<T, N, N>::identity();
  auto off = off_diagonal_norm(S);
  T constexpr tol = std::numeric_limits<T>::epsilon();
  int const max_iter = 2048;
  int num_iter = 0;
  while (off > tol && num_iter < max_iter) {
    // Find largest off-diagonal entry
    int p, q;
    maximum_off_diagonal_indices(S, p, q);
    // Obtain left and right Givens rotations by using 2x2 SVD
    auto const Spq = matrix2x2<T>(
        S(p, p), S(p, q), S(q, p), S(q, q));
    matrix2x2<T> U_2x2, S_2x2, V_2x2;
    svd_2x2(Spq, U_2x2, S_2x2, V_2x2);
    auto const L = U_2x2;
    auto const R = V_2x2;
    T const cl = L.xx();
    T const sl = L.xy();
    T const cr = R.xx();
    T const sr =
      (sign(R.xy()) == sign(R.yx())) ?
      (-R.xy()) : (R.xy());
    // Apply both Givens rotations to matrices
    // that are converging to singular values and singular vectors
    givens_left(cl, sl, p, q, S);
    givens_right(cr, sr, p, q, S);
    givens_right(cl, sl, p, q, U);
    givens_left(cr, sr, p, q, V);
    off = off_diagonal_norm(S);
    ++num_iter;
  }
  // Fix signs for entries in the diagonal matrix S
  // that are negative
  for (int i = 0; i < N; ++i) {
    if (S(i, i) < 0.0) {
      S(i, i) = -S(i, i);
      for (int j = 0; j < N; ++j) {
        U(j, i) = -U(j, i);
      }
    }
  }
  S *= scale;
}

}