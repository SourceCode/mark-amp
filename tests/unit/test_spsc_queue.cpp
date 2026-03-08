// test_spsc_queue.cpp — 10 tests for SPSCQueue
#include "core/SPSCQueue.h"

#include <catch2/catch_test_macros.hpp>

#include <thread>

using namespace markamp::core;

TEST_CASE("SPSCQueue starts empty", "[spsc]")
{
    SPSCQueue<int, 16> queue;
    CHECK(queue.empty());
    CHECK(queue.size_approx() == 0);
}

TEST_CASE("SPSCQueue try_push and try_pop", "[spsc]")
{
    SPSCQueue<int, 16> queue;
    CHECK(queue.try_push(42));
    CHECK(queue.size_approx() == 1);
    int val = 0;
    CHECK(queue.try_pop(val));
    CHECK(val == 42);
    CHECK(queue.empty());
}

TEST_CASE("SPSCQueue FIFO order", "[spsc]")
{
    SPSCQueue<int, 16> queue;
    queue.try_push(1);
    queue.try_push(2);
    queue.try_push(3);
    int val = 0;
    queue.try_pop(val);
    CHECK(val == 1);
    queue.try_pop(val);
    CHECK(val == 2);
    queue.try_pop(val);
    CHECK(val == 3);
}

TEST_CASE("SPSCQueue try_pop on empty returns false", "[spsc]")
{
    SPSCQueue<int, 16> queue;
    int val = 0;
    CHECK_FALSE(queue.try_pop(val));
}

TEST_CASE("SPSCQueue try_push when full returns false", "[spsc]")
{
    SPSCQueue<int, 4> queue; // capacity is 4-1 = 3 usable slots
    CHECK(queue.try_push(1));
    CHECK(queue.try_push(2));
    CHECK(queue.try_push(3));
    CHECK_FALSE(queue.try_push(4)); // full
}

TEST_CASE("SPSCQueue with string type", "[spsc]")
{
    SPSCQueue<std::string, 16> queue;
    queue.try_push(std::string("hello"));
    queue.try_push(std::string("world"));
    std::string val;
    queue.try_pop(val);
    CHECK(val == "hello");
}

TEST_CASE("SPSCQueue producer-consumer basic", "[spsc]")
{
    SPSCQueue<int, 1024> queue;
    constexpr int count = 100;

    std::thread producer(
        [&queue]()
        {
            for (int i = 0; i < count; ++i)
            {
                while (!queue.try_push(i))
                {
                    std::this_thread::yield();
                }
            }
        });

    int received = 0;
    int val = 0;
    while (received < count)
    {
        if (queue.try_pop(val))
        {
            CHECK(val == received);
            ++received;
        }
        else
        {
            std::this_thread::yield();
        }
    }

    producer.join();
    CHECK(received == count);
}

TEST_CASE("SPSCQueue size_approx tracks items", "[spsc]")
{
    SPSCQueue<int, 16> queue;
    queue.try_push(1);
    queue.try_push(2);
    CHECK(queue.size_approx() == 2);
    int val = 0;
    queue.try_pop(val);
    CHECK(queue.size_approx() == 1);
}

TEST_CASE("SPSCQueue capacity returns Capacity-1", "[spsc]")
{
    SPSCQueue<int, 16> queue;
    CHECK(queue.capacity() == 15);
}

TEST_CASE("SPSCQueue empty after full drain", "[spsc]")
{
    SPSCQueue<int, 16> queue;
    for (int i = 0; i < 10; ++i)
    {
        queue.try_push(i);
    }
    int val = 0;
    while (queue.try_pop(val))
    {
    }
    CHECK(queue.empty());
    CHECK(queue.size_approx() == 0);
}
