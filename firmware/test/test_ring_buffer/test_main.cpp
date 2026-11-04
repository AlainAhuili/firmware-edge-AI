// Host-side unit tests (pio test -e native). Run in the commit stage, no hardware needed.
#include <unity.h>

#include "ring_buffer.h"

void setUp() {}
void tearDown() {}

void test_new_buffer_is_empty() {
  RingBuffer<int, 4> rb;
  int out = -1;
  TEST_ASSERT_TRUE(rb.empty());
  TEST_ASSERT_FALSE(rb.pop(out));
  TEST_ASSERT_EQUAL_INT(-1, out);  // pop on empty must not touch out
}

// Regression test for the off-by-one in eval case 001 of dev-growth-copilot:
// a full buffer must reject the next push instead of overwriting unread data.
void test_full_buffer_rejects_push_and_keeps_oldest() {
  RingBuffer<int, 3> rb;
  TEST_ASSERT_TRUE(rb.push(1));
  TEST_ASSERT_TRUE(rb.push(2));
  TEST_ASSERT_TRUE(rb.push(3));
  TEST_ASSERT_TRUE(rb.full());
  TEST_ASSERT_FALSE(rb.push(4));
  TEST_ASSERT_EQUAL_UINT(3, rb.size());
  int out = 0;
  TEST_ASSERT_TRUE(rb.pop(out));
  TEST_ASSERT_EQUAL_INT(1, out);
}

void test_fifo_order_across_wraparound() {
  RingBuffer<int, 3> rb;
  int out = 0;
  for (int round = 0; round < 5; ++round) {  // wraps head/tail several times
    TEST_ASSERT_TRUE(rb.push(round * 10));
    TEST_ASSERT_TRUE(rb.push(round * 10 + 1));
    TEST_ASSERT_TRUE(rb.pop(out));
    TEST_ASSERT_EQUAL_INT(round * 10, out);
    TEST_ASSERT_TRUE(rb.pop(out));
    TEST_ASSERT_EQUAL_INT(round * 10 + 1, out);
  }
  TEST_ASSERT_TRUE(rb.empty());
}

void test_at_indexes_from_oldest() {
  RingBuffer<int, 3> rb;
  int out = 0;
  rb.push(1);
  rb.push(2);
  rb.pop(out);
  rb.push(3);
  rb.push(4);  // storage has wrapped
  TEST_ASSERT_EQUAL_INT(2, rb.at(0));
  TEST_ASSERT_EQUAL_INT(4, rb.at(2));
}

void test_window_mean() {
  RingBuffer<uint16_t, 4> rb;
  TEST_ASSERT_EQUAL_FLOAT(0.0f, window_mean(rb));
  rb.push(10);
  rb.push(20);
  rb.push(30);
  TEST_ASSERT_EQUAL_FLOAT(20.0f, window_mean(rb));
}

void test_capacity_one() {
  RingBuffer<int, 1> rb;
  int out = 0;
  TEST_ASSERT_TRUE(rb.push(7));
  TEST_ASSERT_FALSE(rb.push(8));
  TEST_ASSERT_TRUE(rb.pop(out));
  TEST_ASSERT_EQUAL_INT(7, out);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_new_buffer_is_empty);
  RUN_TEST(test_full_buffer_rejects_push_and_keeps_oldest);
  RUN_TEST(test_fifo_order_across_wraparound);
  RUN_TEST(test_at_indexes_from_oldest);
  RUN_TEST(test_window_mean);
  RUN_TEST(test_capacity_one);
  return UNITY_END();
}
