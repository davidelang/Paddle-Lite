//   Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.\n//\n// Licensed under the Apache License, Version 2.0 (the "License");\n// you may not use this file except in compliance with the License.\n// You may obtain a copy of the License at\n//\n//     http://www.apache.org/licenses/LICENSE-2.0\n//\n// Unless required by applicable law or agreed to in writing, software\n// distributed under the License is distributed on an "AS IS" BASIS,\n// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n// See the License for the specific language governing permissions and\n// limitations under the License.\n\n#include "lite/backends/x86/math/blas.h"\n\n#include <utility>\n
#if defined(__clang__)
#pragma clang attribute push (__attribute__((target("avx,avx2,fma,f16c"))), apply_to=any(function))
#elif defined(__GNUC__)
#pragma GCC push_options
#pragma GCC target("avx,avx2,fma,f16c")
#endif
\n\nnamespace paddle {\nnamespace lite {\nnamespace x86 {\nnamespace math {\nMatDescriptor CreateMatrixDescriptor(const lite::DDimLite &tensor_dim,\n                                     int num_flatten_cols,\n                                     bool trans) {\n  CHECK_GT(tensor_dim.size(), 1u);\n  MatDescriptor retv;\n  if (num_flatten_cols > 1) {\n    auto flatten_dim = tensor_dim.Flatten2D(num_flatten_cols);\n    retv.height_ = flatten_dim[0];\n    retv.width_ = flatten_dim[1];\n  } else {\n    if (tensor_dim.size() == 2) {\n      retv.height_ = tensor_dim[0];\n      retv.width_ = tensor_dim[1];\n    } else {\n      auto dim_vec = tensor_dim.Vectorize();\n      retv.batch_size_ = 1;\n      for (size_t i = 0; i < dim_vec.size() - 2; ++i) {\n        retv.batch_size_ *= dim_vec[i];\n      }\n      retv.height_ = dim_vec[dim_vec.size() - 2];\n      retv.width_ = dim_vec[dim_vec.size() - 1];\n      retv.stride_ = retv.height_ * retv.width_;\n    }\n  }\n  if (trans) {\n    std::swap(retv.width_, retv.height_);\n  }\n  retv.trans_ = trans;\n  return retv;\n}\n\n}  // namespace math\n}  // namespace x86\n}  // namespace lite\n}  // namespace paddle\n
#if defined(__clang__)
#pragma clang attribute pop
#elif defined(__GNUC__)
#pragma GCC pop_options
#endif
