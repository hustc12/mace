// Copyright 2018 The MACE Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <functional>
#include <memory>
#include <vector>

#include "mace/core/ops/operator.h"
#include "mace/core/registry/ops_registry.h"
#include "mace/utils/memory.h"

namespace mace {
namespace ops {

template<RuntimeType D, class T>
class TileOp : public Operation {
 public:
  explicit TileOp(OpConstructContext *context)
      : Operation(context),
        has_data_format_(Operation::GetOptionalArg<int>("has_data_format", 0)) {
  }

  MaceStatus Run(OpContext *context) override {
    MACE_UNUSED(context);
    const Tensor *input = this->Input(0);
    const Tensor *multiples = this->Input(1);
    const index_t input_dims = input->dim_size();

    const T *input_data = input->data<T>();
    const int32_t *multiples_data = multiples->data<int32_t>();

    MACE_CHECK(multiples->dim_size() == 1, "multiples must be 1-dimensional. ",
               multiples->dim_size());
    MACE_CHECK(input_dims == multiples->size(),
               "multiples length must be the same as the dim_size of input",
               input_dims, " vs. ", multiples->size());

    std::vector<int32_t> multiples_vec(multiples_data,
                                       multiples_data + multiples->size());
    if (has_data_format_ && input_dims) {
      int32_t h = multiples_vec[1];
      int32_t w = multiples_vec[2];
      int32_t c = multiples_vec[3];

      multiples_vec[1] = c;
      multiples_vec[2] = h;
      multiples_vec[3] = w;
    }

    Tensor *output = this->Output(0);
    std::vector<index_t> output_shape = {};
    for (index_t i = 0; i < input_dims; ++i) {
      output_shape.push_back(input->dim(i) * multiples_vec[i]);
    }
    MACE_RETURN_IF_ERROR(output->Resize(output_shape));

    T *output_data = output->mutable_data<T>();

    auto *runtime = context->runtime();
    Tensor fake_input(runtime, DataTypeToEnum<T>::v(),
                      MemoryType::CPU_BUFFER, output_shape);
    runtime->AllocateBufferForTensor(&fake_input, RENT_SCRATCH);
    T *fake_input_data = fake_input.mutable_data<T>();
    std::memcpy(fake_input_data, input_data, input->size() * sizeof(T));

    index_t inner_dim = 1;
    index_t outer_dim = input->size();
    index_t acc_multiples = 1;
    const index_t total_multiples =
        std::accumulate(multiples_vec.begin(), multiples_vec.end(), 1,
                        std::multiplies<index_t>());
    for (int64_t i = input_dims - 1;; --i) {
      inner_dim *= input->dim(i);
      outer_dim /= input->dim(i);
      for (int64_t o = 0; o < outer_dim; ++o) {
        for (int64_t m = 0; m < multiples_vec[i]; ++m) {
          std::memcpy(output_data + (o * multiples_vec[i] + m) * inner_dim,
                      fake_input_data + o * inner_dim, inner_dim * sizeof(T));
        }
      }
      acc_multiples *= multiples_vec[i];
      if (acc_multiples == total_multiples) {
        break;
      }
      std::memcpy(fake_input_data, output_data,
                  input->size() * acc_multiples * sizeof(T));
      inner_dim *= multiples_vec[i];
    }

    return MaceStatus::MACE_SUCCESS;
  }

 private:
  int has_data_format_;
};

void RegisterTile(OpRegistry *op_registry) {
  MACE_REGISTER_OP(op_registry, "Tile", TileOp, RuntimeType::RT_CPU, float);
  MACE_REGISTER_BF16_OP(op_registry, "Tile", TileOp, RuntimeType::RT_CPU);
  MACE_REGISTER_OP_CONDITION(
      op_registry, OpConditionBuilder("Tile").SetDevicePlacerFunc(
      [](OpConditionContext *context) -> std::set<RuntimeType> {
        auto op = context->operator_def();
        if (op->output_shape_size() != op->output_size()) {
          return {RuntimeType::RT_CPU};
        }
        return {RuntimeType::RT_CPU};
      }));
}

}  // namespace ops
}  // namespace mace
