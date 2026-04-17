/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserved.\n\nLicensed under the Apache License, Version 2.0 (the "License");\nyou may not use this file except in compliance with the License.\nYou may obtain a copy of the License at\n\n    http://www.apache.org/licenses/LICENSE-2.0\n\nUnless required by applicable law or agreed to in writing, software\ndistributed under the License is distributed on an "AS IS" BASIS,\nWITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\nSee the License for the specific language governing permissions and\nlimitations under the License. */\n\n#include "lite/backends/x86/math/cos_sim_functor.h"\n
#if defined(__clang__)
#pragma clang attribute push (__attribute__((target("avx,avx2,fma,f16c"))), apply_to=any(function))
#elif defined(__GNUC__)
#pragma GCC push_options
#pragma GCC target("avx,avx2,fma,f16c")
#endif
\n\nnamespace paddle {\nnamespace lite {\nnamespace x86 {\nnamespace math {\n\ntemplate <typename T>\nstruct CosSimDyFunctor<lite::TargetType::kX86, T> {\n  void operator()(const lite::X86Context& ctx,\n                  const T* x_norm,\n                  const T* y_norm,\n                  const T* x,\n                  const T* y,\n                  const T* z,\n                  const T* dz,\n                  const size_t rows,\n                  const size_t cols,\n                  T* dy) const {\n    for (size_t row_id = 0; row_id < rows; ++row_id) {\n      auto xy_norm_prod = x_norm[row_id] * y_norm[0];\n      auto dz_data = dz[row_id];\n      auto z_data = z[row_id];\n      auto* x_data = x + cols * row_id;\n      auto reciprocal_xy_norm_prod = 1 / xy_norm_prod;\n\n      auto y_norm_square = y_norm[0] * y_norm[0];\n      auto reciprocal_y_norm_square = 1 / y_norm_square;\n      for (size_t i = 0; i < cols; ++i) {\n        dy[i] += dz_data * (x_data[i] * reciprocal_xy_norm_prod -\n                            z_data * y[i] * reciprocal_y_norm_square);\n      }\n    }\n  }\n};\n\ntemplate struct CosSimDyFunctor<lite::TargetType::kX86, float>;\ntemplate struct CosSimDyFunctor<lite::TargetType::kX86, double>;\n\n}  // namespace math\n}  // namespace x86\n}  // namespace lite\n}  // namespace paddle\n
#if defined(__clang__)
#pragma clang attribute pop
#elif defined(__GNUC__)
#pragma GCC pop_options
#endif
