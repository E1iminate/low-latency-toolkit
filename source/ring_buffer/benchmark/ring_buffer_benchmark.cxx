/*************************************************************************
* Copyright 2025 Vladislav Riabov
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#include "ring_buffer.hxx"
#include <benchmark/benchmark.h>
#include <utility>

template<class RingBufferType>
void BM_LoadRingBuffer(benchmark::State& state) {
  int buffer_size = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();
    RingBufferType ringBuffer(buffer_size);
    state.ResumeTiming();

    for (int i = 0; i < buffer_size; ++i) {
      benchmark::DoNotOptimize(ringBuffer.Push(i));
    }
  }
}

template<class RingBufferType>
void BM_EmptyRingBuffer(benchmark::State& state) {
  int buffer_size = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();
    RingBufferType ringBuffer(buffer_size);
    for (int i = 0; i < buffer_size; ++i) {
      benchmark::DoNotOptimize(ringBuffer.Push(i));
    }
    state.ResumeTiming();
    int64_t value = 0;
    for (int i = 0; i < buffer_size; ++i) {
      benchmark::DoNotOptimize(ringBuffer.Pop(value));
    }
    benchmark::DoNotOptimize(value);
  }
}

BENCHMARK_TEMPLATE(BM_LoadRingBuffer, RingBufferSPSCSimple<int64_t>)->Ranges({{1 << 10, 1 << 16}});
BENCHMARK_TEMPLATE(BM_LoadRingBuffer, RingBufferSPSCOptimized<int64_t>)->Ranges({{1 << 10, 1 << 16}});
BENCHMARK_TEMPLATE(BM_EmptyRingBuffer, RingBufferSPSCSimple<int64_t>)->Ranges({ {1 << 10, 1 << 16} });
BENCHMARK_TEMPLATE(BM_EmptyRingBuffer, RingBufferSPSCOptimized<int64_t>)->Ranges({ {1 << 10, 1 << 16} });