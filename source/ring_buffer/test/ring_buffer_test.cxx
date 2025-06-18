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

#include <algorithm>
#include <vector>
#include <thread>

namespace low_latency_toolkit::test {

template <typename Element>
class RingBufferTest : public ::testing::Test {
};

using RingBufferDataTypes = ::testing::Types<RingBufferSPSCSimple<int64_t>, RingBufferSPSCOptimized<int64_t>>;

TYPED_TEST_SUITE(RingBufferTest, RingBufferDataTypes);

TYPED_TEST(RingBufferTest, RingBufferIsEmptyInitially) {
  TypeParam ringBuffer(1);
  EXPECT_EQ(ringBuffer.Size(), static_cast<size_t>(0));
  EXPECT_TRUE(ringBuffer.IsEmpty());
  EXPECT_FALSE(ringBuffer.IsFull());
}

TYPED_TEST(RingBufferTest, RingBufferIsFull) {
  TypeParam ringBuffer(1);

  EXPECT_TRUE(ringBuffer.Push(static_cast<int64_t>(0x1337)));

  EXPECT_EQ(ringBuffer.Size(), static_cast<size_t>(1));
  EXPECT_FALSE(ringBuffer.IsEmpty());
  EXPECT_TRUE(ringBuffer.IsFull());
}

TYPED_TEST(RingBufferTest, CheckCapacity) {
  constexpr size_t capacity = 0x1337;
  TypeParam ringBuffer(capacity);
  EXPECT_EQ(capacity, ringBuffer.Capacity());
}

TYPED_TEST(RingBufferTest, ExtractMultiplePushedElements) {
  TypeParam ringBuffer(5);
  std::vector<int64_t> values {static_cast<int64_t>(1.73),
                                 static_cast<int64_t>(123819.32312),
                                 static_cast<int64_t>(0xFABABAB),
                                 static_cast<int64_t>(87u),
                                 static_cast<int64_t>(-77777.9999)};

  for (auto& value : values) {
    EXPECT_TRUE(ringBuffer.Push(value));
  }

  EXPECT_EQ(ringBuffer.Size(), static_cast<size_t>(5));

  std::vector<int64_t> bufferValues;
  for (size_t i = 0; i < values.size(); i++) {
    int64_t value = {};
    EXPECT_TRUE(ringBuffer.Pop(value));
    bufferValues.push_back(value);
  }

  EXPECT_EQ(values, bufferValues);
  EXPECT_TRUE(ringBuffer.IsEmpty());
}

TYPED_TEST(RingBufferTest, BufferLoopover) {
  TypeParam ringBuffer(3);
  std::vector<int64_t> values{ static_cast<int64_t>(1.73),
                                 static_cast<int64_t>(123819.32312),
                                 static_cast<int64_t>(0xFABABAB),
                                 static_cast<int64_t>(87u),
                                 static_cast<int64_t>(-77777.9999) };

  for (size_t i = 0u; i < ringBuffer.Capacity(); i++) {
    EXPECT_TRUE(ringBuffer.Push(values[i]));
  }

  EXPECT_TRUE(ringBuffer.IsFull());
  int64_t value = {};
  EXPECT_TRUE(ringBuffer.Pop(value));
  EXPECT_EQ(values[0], value);
  EXPECT_TRUE(ringBuffer.Pop(value));
  EXPECT_EQ(values[1], value);

  for (size_t i = ringBuffer.Capacity(); i < values.size(); i++) {
    EXPECT_TRUE(ringBuffer.Push(values[i]));
  }

  EXPECT_TRUE(ringBuffer.Pop(value));
  EXPECT_EQ(values[2], value);
  EXPECT_TRUE(ringBuffer.Pop(value));
  EXPECT_EQ(values[3], value);
  EXPECT_TRUE(ringBuffer.Pop(value));
  EXPECT_EQ(values[4], value);
  EXPECT_TRUE(ringBuffer.IsEmpty());
}

TYPED_TEST(RingBufferTest, PushWhenFull) {
  TypeParam ringBuffer(3);
  std::vector<int64_t> values{ static_cast<int64_t>(1.73),
                                 static_cast<int64_t>(123819.32312),
                                 static_cast<int64_t>(0xFABABAB),
                                 static_cast<int64_t>(87u),
                                 static_cast<int64_t>(-77777.9999) };

  for (size_t i = 0u; i < ringBuffer.Capacity(); i++) {
    EXPECT_TRUE(ringBuffer.Push(values[i]));
  }

  ASSERT_TRUE(ringBuffer.IsFull());
  EXPECT_FALSE(ringBuffer.Push(values[3]));
  EXPECT_FALSE(ringBuffer.Push(values[4]));
}

TYPED_TEST(RingBufferTest, PopWhenEmpty) {
  TypeParam ringBuffer(3);

  ASSERT_TRUE(ringBuffer.IsEmpty());
  int64_t value = {};
  EXPECT_FALSE(ringBuffer.Pop(value));
}

TYPED_TEST(RingBufferTest, BufferLoopoverMultithreading) {
  TypeParam ringBuffer(3);
  std::vector<int64_t> values{ static_cast<int64_t>(1.73),
                                 static_cast<int64_t>(123819.32312),
                                 static_cast<int64_t>(0xFABABAB),
                                 static_cast<int64_t>(87u),
                                 static_cast<int64_t>(-77777.9999) };
  std::sort(values.begin(), values.end());

  auto producer = [&] {
    for (size_t i = 0u; i < values.size(); i++) {
      while (!ringBuffer.Push(values[i]));
    }
  };

  std::vector<int64_t> bufferValues(values.size());

  auto consumer = [&] {
    for (size_t i = 0u; i < values.size(); i++) {
      while (!ringBuffer.Pop(bufferValues[i]));
    }
  };

  std::thread produce_thread(producer);
  std::thread consumer_thread(consumer);

  produce_thread.join();
  consumer_thread.join();

  std::sort(bufferValues.begin(), bufferValues.end());

  EXPECT_EQ(values, bufferValues);
}

} // namespace low_latency_toolkit