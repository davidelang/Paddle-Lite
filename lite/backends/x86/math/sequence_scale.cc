/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserved.\n\nLicensed under the Apache License, Version 2.0 (the "License");\nyou may not use this file except in compliance with the License.\nYou may obtain a copy of the License at\n\n    http://www.apache.org/licenses/LICENSE-2.0\n\nUnless required by applicable law or agreed to in writing, software\ndistributed under the License is distributed on an "AS IS" BASIS,\nWITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\nSee the License for the specific language governing permissions and\nlimitations under the License. */\n\n#include "lite/backends/x86/math/sequence_scale.h"\n#include "lite/backends/x86/fluid/lod.h"\n
#if defined(__clang__)
#pragma clang attribute push (__attribute__((target("avx,avx2,fma,f16c"))), apply_to=any(function))
#elif defined(__GNUC__)
#pragma GCC push_options
#pragma GCC target("avx,avx2,fma,f16c")
#endif
\n\nnamespace paddle {\nnamespace lite {\nnamespace x86 {\nnamespace math {\n\ntemplate <typename T>\nclass ScaleLoDTensorFunctor<lite::TargetType::kX86, T> {\n public:\n  void operator()(const lite::Context<lite::TargetType::kX86>& context,\n                  const T* scales,\n                  lite::Tensor* seq) {\n    const size_t level = 0;\n    auto lod = seq->lod();\n    const size_t num_seq = lod[level].size() - 1;\n    size_t seq_width = seq->dims()[1];\n    lite::LoD abs_offset_lod = lite::fluid::ToAbsOffset(lod);\n\n    T* seq_data = seq->template mutable_data<T>(lite::TargetType::kX86);\n    for (size_t i = 0; i < num_seq; ++i) {\n      for (size_t j = lod[level][i] * seq_width;\n           j < lod[level][i + 1] * seq_width;\n           ++j) {\n        seq_data[j] *= scales[i];\n      }\n    }\n  }\n};\n\ntemplate class ScaleLoDTensorFunctor<lite::TargetType::kX86, float>;\n\n}  // namespace math\n}  // namespace x86\n}  // namespace lite\n}  // namespace paddle\n
#if defined(__clang__)
#pragma clang attribute pop
#elif defined(__GNUC__)
#pragma GCC pop_options
#endif
