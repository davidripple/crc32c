// Copyright (c) 2017 The CRC32C Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <cstddef>
#include <cstdint>

#include "crc32c/crc32c_config.h"

#include "benchmark/benchmark.h"

#ifdef CRC32C_TESTS_BUILT_WITH_GLOG
#include "glog/logging.h"
#endif  // CRC32C_TESTS_BUILT_WITH_GLOG

#include "crc32c/crc32c.h"
#include "./crc32c_arm_linux.h"
#include "./crc32c_arm_linux_check.h"
#include "./crc32c_internal.h"
#include "./crc32c_sse42.h"
#include "./crc32c_sse42_check.h"

class CRC32CBenchmark : public benchmark::Fixture {
 public:

  void SetUp(const benchmark::State& state) override {
    block_size_ = state.range(0);
    block_data_ = std::string(block_size_, 'x');
    block_buffer_ = reinterpret_cast<const std::uint8_t*>(block_data_.data());
  }

 protected:
  std::string block_data_;
  const std::uint8_t* block_buffer_;
  std::size_t block_size_;
};

BENCHMARK_DEFINE_F(CRC32CBenchmark, Public)(benchmark::State& state) {
  std::uint32_t crc = 0;
  while (state.KeepRunning())
    crc = CRC32C_Extend(crc, block_buffer_, block_size_);
  state.SetBytesProcessed(state.iterations() * block_size_);
}
BENCHMARK_REGISTER_F(CRC32CBenchmark, Public)->RangeMultiplier(16)->Range(
    256, 16777216);  // Block size.

BENCHMARK_DEFINE_F(CRC32CBenchmark, Portable)(benchmark::State& state) {
  std::uint32_t crc = 0;
  while (state.KeepRunning())
    crc = CRC32C_Extend_Portable(crc, block_buffer_, block_size_);
  state.SetBytesProcessed(state.iterations() * block_size_);
}
BENCHMARK_REGISTER_F(CRC32CBenchmark, Portable)->RangeMultiplier(16)->Range(
    256, 16777216);  // Block size.

#if defined(HAVE_ARM_LINUX_CRC32C)
BENCHMARK_DEFINE_F(CRC32CBenchmark, ArmLinux)(benchmark::State& state) {
  if (!CRC32C_CanUseArmLinux()) {
    state.SkipWithError("ARM CRC32C instructions not available or not enabled");
    return;
  }

  std::uint32_t crc = 0;
  while (state.KeepRunning())
    crc = CRC32C_Extend_ArmLinux(crc, block_buffer_, block_size_);
  state.SetBytesProcessed(state.iterations() * block_size_);
}
BENCHMARK_REGISTER_F(CRC32CBenchmark, ArmLinux)->RangeMultiplier(16)->Range(
    256, 16777216);  // Block size.
#endif  // defined(HAVE_ARM_LINUX_CRC32C)

#if defined(HAVE_SSE42)
BENCHMARK_DEFINE_F(CRC32CBenchmark, Sse42)(benchmark::State& state) {
  if (!CRC32C_CanUseSSE42()) {
    state.SkipWithError("SSE4.2 instructions not available or not enabled");
    return;
  }

  std::uint32_t crc = 0;
  while (state.KeepRunning())
    crc = CRC32C_Extend_SSE42(crc, block_buffer_, block_size_);
  state.SetBytesProcessed(state.iterations() * block_size_);
}
BENCHMARK_REGISTER_F(CRC32CBenchmark, Sse42)->RangeMultiplier(16)->Range(
    256, 16777216);  // Block size.
#endif  // defined(HAVE_SSE42)


int main(int argc, char** argv) {
#ifdef CRC32C_TESTS_BUILT_WITH_GLOG
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
#endif  // CRC32C_TESTS_BUILT_WITH_GLOG

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  return 0;
}
