//
// Copyright (c) 2017 XiaoMi All rights reserved.
//

#ifndef MACE_KERNELS_RELU_H_
#define MACE_KERNELS_RELU_H_

#include "mace/core/tensor.h"

namespace mace {
namespace kernels {

template<typename T>
void ReluFuntion(const Tensor *input_tensor, Tensor *output_tensor) {
  int64 size = input_tensor->size();
  output_tensor->ResizeLike(input_tensor);
  const float* input = input_tensor->data<float>();
  float* output = output_tensor->mutable_data<float>();

  for (int64 i = 0; i < size; ++i) {
    output[i] = std::max(input[i], static_cast<T>(0));
  }
}

} //  namespace kernels
} //  namespace mace

#endif // MACE_KERNELS_RELU_H_