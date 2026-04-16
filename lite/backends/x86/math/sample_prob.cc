/* Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include "lite/backends/x86/math/sample_prob.h"

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC push_options
#pragma GCC target("avx,avx2,fma,f16c")
#endif

namespace paddle {
namespace lite {
namespace x86 {
namespace math {

template class SampleWithProb<lite::TargetType::kX86, float>;
template class SampleWithProb<lite::TargetType::kX86, double>;

}  // namespace math
}  // namespace x86
}  // namespace lite
}  // namespace paddle

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC pop_options
#endif
