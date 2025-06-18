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

#include <atomic>
#include <utility>
#include <vector>
#include <type_traits>

template<class Element>
class RingBufferSPSCSimple {
public:

RingBufferSPSCSimple(size_t capacity) 
  : buffer(capacity + 1)
{}

template<class PushedElement>
bool Push(PushedElement&& e) {
  if (IsFull()) {
    return false;
  }

  size_t writeIndex = tail;
  tail = (tail + 1) % buffer.size();
  buffer[writeIndex] = std::forward<PushedElement>(e);

  return true;
}

bool Pop(Element& popped) {
  if (IsEmpty()) {
    return false;
  }

  size_t readIndex = head;
  head = (head + 1) % buffer.size();
  popped = buffer[readIndex];

  return true;
}

size_t Size() const { return (tail + buffer.size() - head) % buffer.size(); }
bool IsEmpty() const { return Size() == static_cast<size_t>(0); }
bool IsFull() const { return Size() == buffer.size() - 1; }
constexpr size_t Capacity() { return buffer.size() - 1; }

private:
  std::vector<Element> buffer;
  std::atomic_size_t head = 0u;
  std::atomic_size_t tail = 0u;
};

template <typename T>
struct AlignedBuffer {
  T* const data = nullptr;
  const size_t size = 0u;
  const size_t alignment = 0u;

public:

  AlignedBuffer(size_t size, size_t alignment) :
    data(static_cast<T*>(::operator new(size * sizeof(T), std::align_val_t{alignment}))),
    size(size),
    alignment(alignment)
  {}

  ~AlignedBuffer() {
    ::operator delete(data, std::align_val_t{ alignment });
  }
};

template<class Element>
class RingBufferSPSCOptimized {
public:
  RingBufferSPSCOptimized(size_t capacity)
    : buffer(capacity + 1, std::hardware_constructive_interference_size)
  {}

  template<class PushedElement>
  bool Push(PushedElement&& e) {
    if (IsFull()) {
      return false;
    }

    size_t writeIndex = tail;
    tail = (tail + 1) % buffer.size;
    buffer.data[writeIndex] = std::forward<PushedElement>(e);

    return true;
  }

  bool Pop(Element& popped) {
    if (IsEmpty()) {
      return false;
    }

    size_t readIndex = head;
    head = (head + 1) % buffer.size;
    popped = buffer.data[readIndex];

    return true;
  }

  size_t Size() const { return (tail + buffer.size - head) % buffer.size; }
  bool IsEmpty() const { return Size() == static_cast<size_t>(0); }
  bool IsFull() const { return Size() == buffer.size - 1; }
  constexpr size_t Capacity() { return buffer.size - 1; }

private:
  AlignedBuffer<Element> buffer;
  std::atomic_size_t head = 0u;
  std::atomic_size_t tail = 0u;
};