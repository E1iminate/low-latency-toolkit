##########################################################################
# Copyright 2025 Vladislav Riabov
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################


add_library(ring_buffer
    ring_buffer.hxx
    ring_buffer.cxx
)

add_executable(ring_buffer_test
    test/ring_buffer_test.cxx
)

target_link_libraries(ring_buffer_test
    PRIVATE
        ring_buffer
        gtest_main
        gmock
)

target_include_directories(ring_buffer_test
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(ring_buffer_benchmark
    benchmark/ring_buffer_benchmark.cxx
)

target_link_libraries(ring_buffer_benchmark
    PRIVATE
        ring_buffer
        benchmark::benchmark_main
)

target_include_directories(ring_buffer_benchmark
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)