#include <poincare/matrix_complex.h>
#include <poincare/matrix.h>
#include <poincare/expression.h>
#include <poincare/undefined.h>
#include <ion.h>
#include <cmath>
#include <assert.h>
#include <float.h>
#include <utility>

namespace Poincare {

template<typename T>
void MatrixComplexNode<T>::didAddChildAtIndex(int newNumberOfChildren) {
  setNumberOfRows(1);
  setNumberOfColumns(newNumberOfChildren);
}

template<typename T>
std::complex<T> MatrixComplexNode<T>::complexAtIndex(int index) const {
  EvaluationNode<T> * child = EvaluationNode<T>::childAtIndex(index);
  if (child->type() == EvaluationNode<T>::Type::Complex) {
    return *(static_cast<ComplexNode<T> *>(child));
  }
  return std::complex<T>(NAN, NAN);
}

template<typename T>
bool MatrixComplexNode<T>::isUndefined() const {
  if (numberOfRows() != 1 || numberOfColumns() != 1) {
    return false;
  }
  EvaluationNode<T> * child = const_cast<MatrixComplexNode<T> *>(this)->childAtIndex(0);
  if (child->type() == EvaluationNode<T>::Type::Complex && std::isnan(static_cast<ComplexNode<T> *>(child)->real()) && std::isnan(static_cast<ComplexNode<T> *>(child)->imag())) {
    return true;
  }
  return false;
}

template<typename T>
Expression MatrixComplexNode<T>::complexToExpression(Preferences::ComplexFormat complexFormat) const {
  Matrix matrix = Matrix::Builder();
  int i = 0;
  for (EvaluationNode<T> * c : this->children()) {
    Expression childExpression = Undefined::Builder();
    if (c->type() == EvaluationNode<T>::Type::Complex) {
      childExpression = c->complexToExpression(complexFormat);
    }
    if (childExpression.isUndefined()) {
        return Undefined::Builder();
    }
    matrix.addChildAtIndexInPlace(childExpression, i, i);
    i++;
  }
  matrix.setDimensions(numberOfRows(), numberOfColumns());
  return std::move(matrix);
}

template<typename T>
std::complex<T> MatrixComplexNode<T>::trace() const {
  if (numberOfRows() != numberOfColumns() || numberOfRows() == 0) {
    return std::complex<T>(NAN, NAN);
  }
  int dim = numberOfRows();
  std::complex<T> c = std::complex<T>(0);
  for (int i = 0; i < dim; i++) {
    c += complexAtIndex(i*dim+i);
    if (std::isnan(c.real()) || std::isnan(c.imag())) {
      return std::complex<T>(NAN, NAN);
    }
  }
  return c;
}

template<typename T>
std::complex<T> MatrixComplexNode<T>::determinant() const {
  if (numberOfRows() != numberOfColumns() || numberOfChildren() == 0 || numberOfChildren() > Matrix::k_maxNumberOfCoefficients) {
    return std::complex<T>(NAN, NAN);
  }
  std::complex<T> operandsCopy[Matrix::k_maxNumberOfCoefficients];
  for (int i = 0; i < numberOfChildren(); i++) {
    operandsCopy[i] = complexAtIndex(i); // Returns complex<T>(NAN, NAN) if Node type is not Complex
  }
  std::complex<T> determinant = std::complex<T>(1);
  Matrix::ArrayRowCanonize(operandsCopy, m_numberOfRows, m_numberOfColumns, &determinant);
  return determinant;
}

template<typename T>
MatrixComplex<T> MatrixComplexNode<T>::inverse() const {
  if (numberOfRows() != numberOfColumns() || numberOfChildren() == 0 || numberOfChildren() > Matrix::k_maxNumberOfCoefficients) {
    return MatrixComplex<T>::Undefined();
  }
  std::complex<T> operandsCopy[Matrix::k_maxNumberOfCoefficients];
  int i = 0;
  for (EvaluationNode<T> * c : this->children()) {
    if (c->type() != EvaluationNode<T>::Type::Complex) {
      return MatrixComplex<T>::Undefined();
    }
    operandsCopy[i] = complexAtIndex(i);
    i++;
  }
  int result = Matrix::ArrayInverse(operandsCopy, m_numberOfRows, m_numberOfColumns);
  if (result == 0) {
    /* Intentionally swapping dimensions for inverse, although it doesn't make a
     * difference because it is square. */
    return MatrixComplex<T>::Builder(operandsCopy, m_numberOfColumns, m_numberOfRows);
  }
  return MatrixComplex<T>::Undefined();
}

template<typename T>
MatrixComplex<T> MatrixComplexNode<T>::transpose() const {
  // Intentionally swapping dimensions for transpose
  MatrixComplex<T> result = MatrixComplex<T>::Builder();
  for (int j = 0; j < numberOfColumns(); j++) {
    for (int i = 0; i < numberOfRows(); i++) {
      result.addChildAtIndexInPlace(Complex<T>::Builder(complexAtIndex(i*numberOfColumns()+j)), result.numberOfChildren(), result.numberOfChildren());
    }
  }
  result.setDimensions(numberOfColumns(), numberOfRows());
  return result;
}

template<typename T>
MatrixComplex<T> MatrixComplexNode<T>::ref(bool reduced) const {
  // Compute Matrix Row Echelon Form
  if (numberOfChildren() == 0 || numberOfChildren() > Matrix::k_maxNumberOfCoefficients) {
    return MatrixComplex<T>::Undefined();
  }
  std::complex<T> operandsCopy[Matrix::k_maxNumberOfCoefficients];
  for (int i = 0; i < numberOfChildren(); i++) {
    operandsCopy[i] = complexAtIndex(i); // Returns complex<T>(NAN, NAN) if Node type is not Complex
  }
  /* Reduced row echelon form is also called row canonical form. To compute the
   * row echelon form (non reduced one), fewer steps are required. */
  Matrix::ArrayRowCanonize(operandsCopy, m_numberOfRows, m_numberOfColumns, static_cast<std::complex<T>*>(nullptr), reduced);
  return MatrixComplex<T>::Builder(operandsCopy, m_numberOfRows, m_numberOfColumns);
}

template<typename T>
std::complex<T> MatrixComplexNode<T>::norm() const {
  if (vectorType() == Array::VectorType::None) {
    return std::complex<T>(NAN, NAN);
  }
  std::complex<T> sum = 0;
  for (int i = 0; i < numberOfChildren(); i++) {
    sum += std::norm(complexAtIndex(i));
  }
  return std::sqrt(sum);
}

template<typename T>
std::complex<T> MatrixComplexNode<T>::dot(Evaluation<T> * e) const {
  if (e->type() != EvaluationNode<T>::Type::MatrixComplex) {
    return std::complex<T>(NAN, NAN);
  }
  MatrixComplex<T> * b  = static_cast<MatrixComplex<T>*>(e);
  if (vectorType() == Array::VectorType::None || vectorType() != b->vectorType() || numberOfChildren() != b->numberOfChildren()) {
    return std::complex<T>(NAN, NAN);
  }
  std::complex<T> sum = 0;
  for (int i = 0; i < numberOfChildren(); i++) {
    sum += complexAtIndex(i) * b->complexAtIndex(i);
  }
  return sum;
}

template<typename T>
Evaluation<T> MatrixComplexNode<T>::cross(Evaluation<T> * e) const {
  if (e->type() != EvaluationNode<T>::Type::MatrixComplex) {
    return MatrixComplex<T>::Undefined();
  }
  MatrixComplex<T> * b  = static_cast<MatrixComplex<T>*>(e);
  if (vectorType() == Array::VectorType::None || vectorType() != b->vectorType() || numberOfChildren() != 3 || b->numberOfChildren() != 3) {
    return MatrixComplex<T>::Undefined();
  }
  std::complex<T> operandsCopy[3];
  operandsCopy[0] = complexAtIndex(1) * b->complexAtIndex(2) - complexAtIndex(2) * b->complexAtIndex(1);
  operandsCopy[1] = complexAtIndex(2) * b->complexAtIndex(0) - complexAtIndex(0) * b->complexAtIndex(2);
  operandsCopy[2] = complexAtIndex(0) * b->complexAtIndex(1) - complexAtIndex(1) * b->complexAtIndex(0);
  return MatrixComplex<T>::Builder(operandsCopy, numberOfRows(), numberOfColumns());
}

// MATRIX COMPLEX REFERENCE

template<typename T>
MatrixComplex<T> MatrixComplex<T>::Builder(std::complex<T> * operands, int numberOfRows, int numberOfColumns) {
  MatrixComplex<T> m = MatrixComplex<T>::Builder();
  for (int i=0; i<numberOfRows*numberOfColumns; i++) {
    m.addChildAtIndexInPlace(Complex<T>::Builder(operands[i]), i, i);
  }
  m.setDimensions(numberOfRows, numberOfColumns);
  return m;
}

template<typename T>
MatrixComplex<T> MatrixComplex<T>::Undefined() {
  std::complex<T> undef = std::complex<T>(NAN, NAN);
  return MatrixComplex<T>::Builder((std::complex<T> *)&undef, 1, 1);
}

template<typename T>
MatrixComplex<T> MatrixComplex<T>::CreateIdentity(int dim) {
  MatrixComplex<T> result = MatrixComplex<T>::Builder();
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      Complex<T> c = i == j ? Complex<T>::Builder(1.0) : Complex<T>::Builder(0.0);
      result.addChildAtIndexInPlace(c, i*dim+j, i*dim+j);
    }
  }
  result.setDimensions(dim, dim);
  return result;
}

template<typename T>
void MatrixComplex<T>::setDimensions(int rows, int columns) {
  setNumberOfRows(rows);
  setNumberOfColumns(columns);
}

template<typename T>
void MatrixComplex<T>::addChildAtIndexInPlace(Evaluation<T> t, int index, int currentNumberOfChildren) {
  if (t.type() != EvaluationNode<T>::Type::Complex) {
    t = Complex<T>::Undefined();
  }
  Evaluation<T>::addChildAtIndexInPlace(t, index, currentNumberOfChildren);
}

template class MatrixComplexNode<float>;
template class MatrixComplexNode<double>;

template class MatrixComplex<float>;
template class MatrixComplex<double>;

}
