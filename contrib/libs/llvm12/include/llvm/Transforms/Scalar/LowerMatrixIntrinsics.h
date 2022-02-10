#pragma once

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

//===- LowerMatrixIntrinsics.h - Lower matrix intrinsics. -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This pass lowers matrix intrinsics down to vector operations.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_SCALAR_LOWERMATRIXINTRINSICSPASS_H
#define LLVM_TRANSFORMS_SCALAR_LOWERMATRIXINTRINSICSPASS_H

#include "llvm/IR/PassManager.h"

namespace llvm {
class LowerMatrixIntrinsicsPass 
    : public PassInfoMixin<LowerMatrixIntrinsicsPass> { 
  bool Minimal; 
 
public: 
  LowerMatrixIntrinsicsPass(bool Minimal = false) : Minimal(Minimal) {} 
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
  static bool isRequired() { return true; } 
};
} // namespace llvm

#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
