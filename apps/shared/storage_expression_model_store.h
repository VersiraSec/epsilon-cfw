#ifndef SHARED_STORAGE_EXPRESSION_MODEL_STORE_H
#define SHARED_STORAGE_EXPRESSION_MODEL_STORE_H

#include "storage_expression_model.h"
#include <ion/storage.h>
#include <assert.h>

namespace Shared {

// StorageExpressionModelStore is a handle to Ion::Storage::sharedStorage()

class StorageExpressionModelStore {
  // TODO find better name (once we remove ExpressionModelStore?)
public:
  StorageExpressionModelStore();

  // Getters
  int numberOfModels() const;
  int numberOfDefinedModels() const { return numberOfModelsSatisfyingTest([](StorageExpressionModel * m) { return m->isDefined(); }); }
  Ion::Storage::Record recordAtIndex(int i) const;
  Ion::Storage::Record definedRecordAtIndex(int i) const { return recordStatifyingTestAtIndex(i, [](StorageExpressionModel * m) { return m->isDefined(); }); }
  virtual StorageExpressionModel * modelForRecord(Ion::Storage::Record record) const;

  // Add and Remove
  virtual Ion::Storage::Record::ErrorStatus addEmptyModel() = 0;
  void removeAll();
  void removeModel(Ion::Storage::Record record);

  // Other
  void tidy();
protected:
  constexpr static int k_maxNumberOfMemoizedModels = 5;
  typedef bool (*ModelTest)(StorageExpressionModel * model);
  int numberOfModelsSatisfyingTest(ModelTest test) const;
  Ion::Storage::Record recordStatifyingTestAtIndex(int i, ModelTest test) const;
private:
  void resetMemoizedModels() const;
  virtual void setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record) const = 0;
  virtual StorageExpressionModel * memoizedModelAtIndex(int cacheIndex) const = 0;
  virtual const char * modelExtension() const = 0;
  /* Memoization of k_maxNumberOfMemoizedModels. When the required model is not
   * present, we override the m_oldestMemoizedIndex model. This actually
   * overrides the oldest memoized model because models are all reset at the
   * same time. Otherwise, we should use a queue to decide which was the last
   * memoized model. */
  mutable int m_oldestMemoizedIndex;
  mutable uint32_t m_storageChecksum;

};

}

#endif
