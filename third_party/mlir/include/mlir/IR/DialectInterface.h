//===- DialectInterface.h - IR Dialect Interfaces ---------------*- C++ -*-===//
//
// Copyright 2019 The MLIR Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =============================================================================

#ifndef MLIR_IR_DIALECTINTERFACE_H
#define MLIR_IR_DIALECTINTERFACE_H

#include "mlir/IR/Dialect.h"
#include "mlir/Support/STLExtras.h"
#include "llvm/ADT/DenseMap.h"

namespace mlir {
class Dialect;
class MLIRContext;
class Operation;

//===----------------------------------------------------------------------===//
// DialectInterface
//===----------------------------------------------------------------------===//
namespace detail {
/// The base class used for all derived interface types. This class provides
/// utilities necessary for registration.
template <typename ConcreteType, typename BaseT>
class DialectInterfaceBase : public BaseT {
public:
  using Base = DialectInterfaceBase<ConcreteType, BaseT>;

  /// Get a unique id for the derived interface type.
  static ClassID *getInterfaceID() { return ClassID::getID<ConcreteType>(); }

protected:
  DialectInterfaceBase(Dialect *dialect) : BaseT(dialect, getInterfaceID()) {}
};
} // end namespace detail

/// This class represents an interface overridden for a single dialect.
class DialectInterface {
public:
  virtual ~DialectInterface();

  /// The base class used for all derived interface types. This class provides
  /// utilities necessary for registration.
  template <typename ConcreteType>
  using Base = detail::DialectInterfaceBase<ConcreteType, DialectInterface>;

  /// Return the dialect that this interface represents.
  Dialect *getDialect() const { return dialect; }

  /// Return the derived interface id.
  ClassID *getID() const { return interfaceID; }

protected:
  DialectInterface(Dialect *dialect, ClassID *id)
      : dialect(dialect), interfaceID(id) {}

private:
  /// The dialect that represents this interface.
  Dialect *dialect;

  /// The unique identifier for the derived interface type.
  ClassID *interfaceID;
};

//===----------------------------------------------------------------------===//
// DialectInterfaceCollection
//===----------------------------------------------------------------------===//

namespace detail {
/// This class is the base class for a collection of instances for a specific
/// interface kind.
class DialectInterfaceCollectionBase {
public:
  DialectInterfaceCollectionBase(MLIRContext *ctx, ClassID *interfaceKind);
  virtual ~DialectInterfaceCollectionBase();

protected:
  /// Get the interface for the dialect of given operation, or null if one
  /// is not registered.
  const DialectInterface *getInterfaceFor(Operation *op) const;

  /// Get the interface for the given dialect.
  const DialectInterface *getInterfaceFor(Dialect *dialect) const {
    return interfaces.lookup(dialect);
  }

private:
  /// A map of registered dialect interface instances.
  DenseMap<Dialect *, const DialectInterface *> interfaces;
};
} // namespace detail

/// A collection of dialect interfaces within a context, for a given concrete
/// interface type.
template <typename InterfaceType>
class DialectInterfaceCollection
    : public detail::DialectInterfaceCollectionBase {
public:
  using Base = DialectInterfaceCollection<InterfaceType>;

  /// Collect the registered dialect interfaces within the provided context.
  DialectInterfaceCollection(MLIRContext *ctx)
      : detail::DialectInterfaceCollectionBase(
            ctx, InterfaceType::getInterfaceID()) {}

  /// Get the interface for a given object, or null if one is not registered.
  /// The object may be a dialect or an operation instance.
  template <typename Object>
  const InterfaceType *getInterfaceFor(Object *obj) const {
    return static_cast<const InterfaceType *>(
        detail::DialectInterfaceCollectionBase::getInterfaceFor(obj));
  }
};

} // namespace mlir

#endif
