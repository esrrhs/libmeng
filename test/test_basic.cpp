#include "meng.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_test_count = 0;
static int g_test_pass = 0;

#define TEST_ASSERT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "[FAIL] %s:%d: Assertion '%s' failed!\n", __FILE__, __LINE__, #cond); \
        abort(); \
    } \
} while (0)

#define RUN_TEST(name) do { \
    g_test_count++; \
    printf("[RUN ] %s\n", #name); \
    name(); \
    g_test_pass++; \
    printf("[PASS] %s\n", #name); \
} while (0)

// Test 1: Basic Create, Run, Yield, End
static void basic_worker(meng* m, void* arg, size_t argsize) {
    (void)arg;
    (void)argsize;
    int* val = (int*)arg;
    TEST_ASSERT(*val == 42);

    *val += 1;
    meng_yield(m);

    *val += 1;
    meng_yield(m);

    *val += 1;
}

void test_basic_lifecycle() {
    int val = 42;
    meng* m = meng_create(basic_worker, 16 * 1024, &val, sizeof(val));
    TEST_ASSERT(m != NULL);
    TEST_ASSERT(!meng_end(m));
    TEST_ASSERT(meng_get_stack_size(m) >= 16 * 1024);
    TEST_ASSERT(meng_get_arg_size(m) == sizeof(val));

    // First run
    meng_run(m);
    int* stored_arg = (int*)meng_get_arg(m);
    TEST_ASSERT(*stored_arg == 43);
    TEST_ASSERT(!meng_end(m));

    // Second run
    meng_run(m);
    TEST_ASSERT(*stored_arg == 44);
    TEST_ASSERT(!meng_end(m));

    // Third run (finishes)
    meng_run(m);
    TEST_ASSERT(*stored_arg == 45);
    TEST_ASSERT(meng_end(m));

    // Running an ended coroutine should be a no-op
    meng_run(m);
    TEST_ASSERT(meng_end(m));

    meng_delete(m);
}

// Test 2: Current Running and Yield Current
static void yield_current_worker(meng* m, void* arg, size_t argsize) {
    (void)arg;
    (void)argsize;
    TEST_ASSERT(meng_running() == m);
    meng_yield_current();
    TEST_ASSERT(meng_running() == m);
}

void test_running_and_yield_current() {
    TEST_ASSERT(meng_running() == NULL);

    meng* m = meng_create(yield_current_worker, 0, NULL, 0);
    TEST_ASSERT(!meng_end(m));

    meng_run(m);
    TEST_ASSERT(meng_running() == NULL);
    TEST_ASSERT(!meng_end(m));

    meng_run(m);
    TEST_ASSERT(meng_running() == NULL);
    TEST_ASSERT(meng_end(m));

    meng_delete(m);
}

// Test 3: Multiple Interleaved Coroutines
static void ping_pong_worker(meng* m, void* arg, size_t argsize) {
    (void)argsize;
    int id = *(int*)arg;
    for (int i = 0; i < 5; ++i) {
        meng_yield(m);
    }
}

void test_interleaved_coroutines() {
    int id1 = 1, id2 = 2, id3 = 3;
    meng* m1 = meng_create(ping_pong_worker, 8 * 1024, &id1, sizeof(id1));
    meng* m2 = meng_create(ping_pong_worker, 8 * 1024, &id2, sizeof(id2));
    meng* m3 = meng_create(ping_pong_worker, 8 * 1024, &id3, sizeof(id3));

    int rounds = 0;
    while (!meng_end(m1) || !meng_end(m2) || !meng_end(m3)) {
        if (!meng_end(m1)) meng_run(m1);
        if (!meng_end(m2)) meng_run(m2);
        if (!meng_end(m3)) meng_run(m3);
        rounds++;
    }

    TEST_ASSERT(rounds == 6);
    TEST_ASSERT(meng_end(m1));
    TEST_ASSERT(meng_end(m2));
    TEST_ASSERT(meng_end(m3));

    meng_delete(m1);
    meng_delete(m2);
    meng_delete(m3);
}

// Test 4: Nested Coroutines (Coroutine inside Coroutine)
static void inner_worker(meng* m, void* arg, size_t argsize) {
    (void)m; (void)argsize;
    int* val = (int*)arg;
    *val += 10;
    meng_yield(m);
    *val += 10;
}

static void outer_worker(meng* m, void* arg, size_t argsize) {
    (void)argsize;
    int* val = (int*)arg;
    *val += 1;

    int inner_val = 100;
    meng* inner = meng_create(inner_worker, 16 * 1024, &inner_val, sizeof(inner_val));
    meng_run(inner); // inner runs first half (+10)

    *val += 1;
    meng_yield(m); // outer yields back to main thread

    meng_run(inner); // inner finishes second half (+10)
    TEST_ASSERT(meng_end(inner));
    meng_delete(inner);

    *val += 1;
}

void test_nested_coroutines() {
    int val = 0;
    meng* outer = meng_create(outer_worker, 32 * 1024, &val, sizeof(val));
    meng_run(outer);

    int* stored = (int*)meng_get_arg(outer);
    TEST_ASSERT(*stored == 2);
    TEST_ASSERT(!meng_end(outer));

    meng_run(outer);
    TEST_ASSERT(*stored == 3);
    TEST_ASSERT(meng_end(outer));

    meng_delete(outer);
}

// Test 5: String and Struct Arguments
struct UserData {
    char name[32];
    double balance;
    int id;
};

static void struct_worker(meng* m, void* arg, size_t argsize) {
    (void)m; (void)argsize;
    UserData* u = (UserData*)arg;
    TEST_ASSERT(strcmp(u->name, "Alice") == 0);
    TEST_ASSERT(u->id == 12345);
    u->balance += 50.0;
}

void test_struct_arguments() {
    UserData user;
    strncpy(user.name, "Alice", sizeof(user.name));
    user.balance = 100.5;
    user.id = 12345;

    meng* m = meng_create(struct_worker, 16 * 1024, &user, sizeof(user));
    meng_run(m);
    TEST_ASSERT(meng_end(m));

    UserData* stored = (UserData*)meng_get_arg(m);
    TEST_ASSERT(stored->balance == 150.5);

    meng_delete(m);
}

int main() {
    printf("=== Running C API Unit Tests ===\n");
    RUN_TEST(test_basic_lifecycle);
    RUN_TEST(test_running_and_yield_current);
    RUN_TEST(test_interleaved_coroutines);
    RUN_TEST(test_nested_coroutines);
    RUN_TEST(test_struct_arguments);

    printf("\nAll %d tests passed!\n", g_test_pass);
    return 0;
}
