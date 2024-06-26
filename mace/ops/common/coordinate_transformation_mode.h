// Copyright 2021 The MACE Authors. All Rights Reserved.
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

#ifndef MACE_OPS_COMMON_COORDINATE_TRANSFORMATION_MODE_H_
#define MACE_OPS_COMMON_COORDINATE_TRANSFORMATION_MODE_H_

#include <functional>

#include "mace/core/types.h"

namespace mace {
namespace ops {

enum CoordinateTransformationMode {
  NONE = 0,
  HALF_PIXEL = 1,
  PYTORCH_HALF_PIXEL = 2,
};

enum NearestMode {
  FLOOR = 0,
  ROUND_PREFER_FLOOR = 1,
};
typedef std::function<index_t(float)> NearestFunc;

}  // namespace ops
}  // namespace mace

#endif  // MACE_OPS_COMMON_COORDINATE_TRANSFORMATION_MODE_H_
