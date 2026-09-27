#include "meng.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

static int g_test_count = 0;
static int g_test_pass = 0;

#define TEST_ASSERT(cond) do { \
    if (!(cond)) { \
        std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__ << ": Assertion '" #cond "' failed!" << std::endl; \
        std::abort(); \
    } \
} while (0)

#define RUN_TEST(name) do { \
    g_test_count++; \
    std::cout << "[RUN ] " << #name << std::endl; \
    name(); \
    g_test_pass++; \
    std::cout << "[PASS] " << #name << std::endl; \
} while (0)

void test_lambda_and_capture() {
    int counter = 0;
    std::string msg = "hello";

    libmeng::coroutine co([&counter, msg]() {
        TEST_ASSERT(msg == "hello");
        counter += 10;
        libmeng::yield();
        counter += 20;
        libmeng::yield();
        counter += 30;
    });

    TEST_ASSERT(!co.done());
    TEST_ASSERT(static_cast<bool>(co));

    co.resume();
    TEST_ASSERT(counter == 10);
    TEST_ASSERT(!co.done());

    co.resume();
    TEST_ASSERT(counter == 30);
    TEST_ASSERT(!co.done());

    co.resume();
    TEST_ASSERT(counter == 60);
    TEST_ASSERT(co.done());
    TEST_ASSERT(!static_cast<bool>(co));
}

void test_current_coroutine() {
    TEST_ASSERT(libmeng::current() == nullptr);

    libmeng::coroutine co([]() {
        TEST_ASSERT(libmeng::current() != nullptr);
        libmeng::yield();
        TEST_ASSERT(libmeng::current() != nullptr);
    });

    co.resume();
    TEST_ASSERT(libmeng::current() == nullptr);
    co.resume();
    TEST_ASSERT(libmeng::current() == nullptr);
}

void test_move_semantics() {
    int step = 0;
    libmeng::coroutine co1([&step]() {
        step = 1;
        libmeng::yield();
        step = 2;
    });

    co1.resume();
    TEST_ASSERT(step == 1);

    // Move construct
    libmeng::coroutine co2(std::move(co1));
    TEST_ASSERT(co1.raw() == nullptr);
    TEST_ASSERT(co2.raw() != nullptr);

    co2.resume();
    TEST_ASSERT(step == 2);
    TEST_ASSERT(co2.done());

    // Move assign
    libmeng::coroutine co3;
    co3 = std::move(co2);
    TEST_ASSERT(co3.done());
}

void test_exception_handling() {
    libmeng::coroutine co([]() {
        libmeng::yield();
        throw std::runtime_error("Error inside coroutine!");
    });

    co.resume(); // Reaches first yield
    TEST_ASSERT(!co.done());

    bool caught = false;
    try {
        co.resume(); // Throws
    } catch (const std::runtime_error& e) {
        caught = true;
        TEST_ASSERT(std::string(e.what()) == "Error inside coroutine!");
    }

    TEST_ASSERT(caught);
    TEST_ASSERT(co.done());
}

void test_nested_cpp_coroutines() {
    std::vector<int> execution_order;

    libmeng::coroutine outer([&execution_order]() {
        execution_order.push_back(1);

        libmeng::coroutine inner([&execution_order]() {
            execution_order.push_back(2);
            libmeng::yield();
            execution_order.push_back(4);
        });

        inner.resume();
        execution_order.push_back(3);
        libmeng::yield();

        inner.resume();
        execution_order.push_back(5);
    });

    outer.resume();
    execution_order.push_back(100);
    outer.resume();

    std::vector<int> expected = {1, 2, 3, 100, 4, 5};
    TEST_ASSERT(execution_order == expected);
}

int main() {
    std::cout << "=== Running Modern C++ Wrapper Tests ===" << std::endl;
    RUN_TEST(test_lambda_and_capture);
    RUN_TEST(test_current_coroutine);
    RUN_TEST(test_move_semantics);
    RUN_TEST(test_exception_handling);
    RUN_TEST(test_nested_cpp_coroutines);

    std::cout << "\nAll " << g_test_pass << " C++ tests passed!" << std::endl;
    return 0;
}
