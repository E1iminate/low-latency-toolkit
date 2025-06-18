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

template<class Element>
void BM_LoadRingBuffer(benchmark::State& state) {
  int buffer_size = state.range(0);
  int load_count = state.range(1);


  for (auto _ : state) {
    state.PauseTiming();
    RingBuffer<Element> ringBuffer(buffer_size);
    state.ResumeTiming();

    for (int i = 0; i < load_count; ++i) {
      benchmark::DoNotOptimize(i);
      ringBuffer.Push(i);
    }
  }
}

BENCHMARK_TEMPLATE(BM_LoadRingBuffer, int64_t)->Ranges({ {1 << 10, 1 << 16}, {100, 1000} });
