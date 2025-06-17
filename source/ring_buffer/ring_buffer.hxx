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

#include <array>
#include <utility>
#include <type_traits>

template<class Element, size_t capacity>
class RingBuffer {

public:
template<class PushedElement>
void Push(PushedElement&& e) {
  buffer[tail++] = std::forward<PushedElement>(e);
}
Element Pop() {
  return buffer[head++];
}
size_t Size() const { return tail - head; }
bool IsEmpty() const { return Size() == static_cast<size_t>(0); }
bool IsFull() const { return Size() == capacity; }

private:
  std::array<Element, capacity> buffer {};
  size_t head = 0u;
  size_t tail = 0u;
};