
#ifndef BOOSTLIB_H
#define BOOSTLIB_H

#pragma warning(disable:4996)  

// REMEMBER to update "lu.hpp" header includes from boost-CVS
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <map>
#include <vector>
namespace ublas = boost::numeric::ublas;
using namespace boost::numeric::ublas;
/* Matrix inversion routine.
Uses lu_factorize and lu_substitute in uBLAS to invert a matrix */
template <class T>
bool InvertMatrix(const ublas::matrix<T> &input, ublas::matrix<T> &inverse) {

  typedef permutation_matrix<std::size_t> pmatrix;
  // create a working copy of the input
  matrix<T> A(input);
  // create a permutation matrix for the LU-factorization
  pmatrix pm(A.size1());
  // perform LU-factorization
  int res = lu_factorize(A, pm);
  if (res != 0)
    return false;
  // create identity matrix of "inverse"
  inverse.assign(ublas::identity_matrix<T>(A.size1()));
  // backsubstitute to get the inverse
  lu_substitute(A, pm, inverse);
  return true;
}

template <class T>
void LinearFit(const ublas::matrix<T> &input_A, const ublas::matrix<T> &input_b,
               std::vector<T> &v_best_fit) {
  v_best_fit.clear();
  matrix<T> product = prod(trans(input_A), input_A);
  matrix<T> temp_A(product);
  matrix<T> t_best_fit;
  bool inverted;
  inverted = InvertMatrix(product, temp_A);
  if (inverted) {
    matrix<T> producttwo = prod(temp_A, trans(input_A));
    t_best_fit = prod(producttwo, input_b);
    for (size_t it = 0; it != t_best_fit.size1(); ++it)
      v_best_fit.push_back(t_best_fit(it, 0));
  }
}

template <class T>
void ConvertVector2Matrix(const std::vector<T> &t_parameter,
                          ublas::matrix<T> &ouput_A,
                          ublas::matrix<T> &ouput_b) {
  if (t_parameter.size() % 2 == 0) {
    size_t m = t_parameter.size() / 2;
    matrix<T> A(m, 2);
    matrix<T> b(m, 1);
    for (size_t it = 0; it != m; ++it) {
      A(it, 0) = t_parameter[2 * it + 1];
      A(it, 1) = 1;
      b(it, 0) = t_parameter[2 * it];
    }
    ouput_A = A;
    ouput_b = b;
  }
}

template <class T>
void LinearFitV2V(const std::vector<T> &t_parameter,
                  std::vector<T> &v_best_fit) {
  matrix<T> A;
  matrix<T> b;
  ConvertVector2Matrix(t_parameter, A, b);
  LinearFit(A, b, v_best_fit);
}

#endif // INVERT_MATRIX_HPP