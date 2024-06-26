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


#include "mace/core/ops/operator.h"
#include "mace/core/registry/ops_registry.h"

namespace mace {
namespace ops {

class IdentityOp : public Operation {
 public:
  explicit IdentityOp(OpConstructContext *context)
      : Operation(context) {}

  MaceStatus Run(OpContext *context) override {
    MACE_UNUSED(context);
    const Tensor *input = this->Input(0);
    Tensor *output = this->Output(0);
    output->Copy(*input);
    output->Reshape(input->shape());
    return MaceStatus::MACE_SUCCESS;
  }

  int ReuseTensorMapId(size_t output_idx) const override {
    if (output_idx == 0) {
      return 0;
    } else {
      return -1;
    }
  }
};

void RegisterIdentity(OpRegistry *op_registry) {
  MACE_REGISTER_OP_BY_CLASS(op_registry, "Identity", IdentityOp,
                            RuntimeType::RT_CPU, float);
  MACE_REGISTER_BF16_OP_BY_CLASS(op_registry, "Identity", IdentityOp,
                                 RuntimeType::RT_CPU);
  MACE_REGISTER_OP_BY_CLASS(op_registry, "Identity", IdentityOp,
                            RuntimeType::RT_CPU, int32_t);
#ifdef MACE_ENABLE_OPENCL
  MACE_REGISTER_OP_BY_CLASS(op_registry, "Identity", IdentityOp,
                            RuntimeType::RT_OPENCL, float);
#endif  // MACE_ENABLE_OPENCL
}

}  // namespace ops
}  // namespace mace
