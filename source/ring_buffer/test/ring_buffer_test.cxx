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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

namespace low_latency_toolkit::test {

template <typename Element>
class RingBufferTest : public ::testing::Test {
};

using RingBufferDataTypes = ::testing::Types<int64_t, uint64_t, float, double>;

TYPED_TEST_SUITE(RingBufferTest, RingBufferDataTypes);

TYPED_TEST(RingBufferTest, RingBufferIsEmptyInitially) {
  RingBuffer<TypeParam, 1> ringBuffer;
  EXPECT_EQ(ringBuffer.Size(), static_cast<size_t>(0));
  EXPECT_TRUE(ringBuffer.IsEmpty());
  EXPECT_FALSE(ringBuffer.IsFull());
}

TYPED_TEST(RingBufferTest, RingBufferIsFull) {
  RingBuffer<TypeParam, 1> ringBuffer;

  ringBuffer.Push(TypeParam(0x1337));

  EXPECT_EQ(ringBuffer.Size(), static_cast<size_t>(1));
  EXPECT_FALSE(ringBuffer.IsEmpty());
  EXPECT_TRUE(ringBuffer.IsFull());
}

TYPED_TEST(RingBufferTest, CheckCapacity) {
  constexpr size_t capacity = 0x1337;
  RingBuffer<TypeParam, capacity> ringBuffer;
  EXPECT_EQ(capacity, ringBuffer.Capacity());
}

TYPED_TEST(RingBufferTest, ExtractMultiplePushedElements) {
  RingBuffer<TypeParam, 5> ringBuffer;
  std::vector<TypeParam> values {static_cast<TypeParam>(1.73),
                                 static_cast<TypeParam>(123819.32312),
                                 static_cast<TypeParam>(0xFABABAB),
                                 static_cast<TypeParam>(87u),
                                 static_cast<TypeParam>(-77777.9999)};

  for (auto& value : values) {
    ringBuffer.Push(TypeParam(value));
  }

  EXPECT_EQ(ringBuffer.Size(), static_cast<size_t>(5));

  std::vector<TypeParam> bufferValues;
  for (size_t i = 0; i < values.size(); i++) {
    bufferValues.push_back(ringBuffer.Pop());
  }

  EXPECT_EQ(values, bufferValues);
  EXPECT_TRUE(ringBuffer.IsEmpty());
}

TYPED_TEST(RingBufferTest, BufferLoopover) {
  RingBuffer<TypeParam, 3> ringBuffer;
  std::vector<TypeParam> values{ static_cast<TypeParam>(1.73),
                                 static_cast<TypeParam>(123819.32312),
                                 static_cast<TypeParam>(0xFABABAB),
                                 static_cast<TypeParam>(87u),
                                 static_cast<TypeParam>(-77777.9999) };

  for (size_t i = 0u; i < ringBuffer.Capacity(); i++) {
    ringBuffer.Push(TypeParam(values[i]));
  }

  EXPECT_TRUE(ringBuffer.IsFull());
  EXPECT_EQ(values[0], ringBuffer.Pop());
  EXPECT_EQ(values[1], ringBuffer.Pop());

  for (size_t i = ringBuffer.Capacity(); i < values.size(); i++) {
    ringBuffer.Push(TypeParam(values[i]));
  }

  EXPECT_EQ(values[2], ringBuffer.Pop());
  EXPECT_EQ(values[3], ringBuffer.Pop());
  EXPECT_EQ(values[4], ringBuffer.Pop());
  EXPECT_TRUE(ringBuffer.IsEmpty());
}

} // namespace low_latency_toolkit