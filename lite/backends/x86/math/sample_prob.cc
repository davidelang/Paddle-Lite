/* Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.\n\nLicensed under the Apache License, Version 2.0 (the "License");\nyou may not use this file except in compliance with the License.\nYou may obtain a copy of the License at\n\n    http://www.apache.org/licenses/LICENSE-2.0\n\nUnless required by applicable law or agreed to in writing, software\ndistributed under the License is distributed on an "AS IS" BASIS,\nWITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\nSee the License for the specific language governing permissions and\nlimitations under the License. */\n\n#include "lite/backends/x86/math/sample_prob.h"\n
#if defined(__clang__)
#pragma clang attribute push (__attribute__((target("avx,avx2,fma,f16c"))), apply_to=any(function))
#elif defined(__GNUC__)
#pragma GCC push_options
#pragma GCC target("avx,avx2,fma,f16c")
#endif
\n\nnamespace paddle {\nnamespace lite {\nnamespace x86 {\nnamespace math {\n\ntemplate class SampleWithProb<lite::TargetType::kX86, float>;\ntemplate class SampleWithProb<lite::TargetType::kX86, double>;\n\n}  // namespace math\n}  // namespace x86\n}  // namespace lite\n}  // namespace paddle\n
#if defined(__clang__)
#pragma clang attribute pop
#elif defined(__GNUC__)
#pragma GCC pop_options
#endif
