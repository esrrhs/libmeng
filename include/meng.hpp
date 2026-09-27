#pragma once

#include "meng.h"
#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace libmeng {

class coroutine;

namespace detail {
    inline coroutine*& current_cpp_coroutine_ref() {
        static thread_local coroutine* s_current = nullptr;
        return s_current;
    }
}

/**
 * @brief 让出当前协程的执行权（可在协程内部任意深度的调用栈中直接调用）
 */
inline void yield() {
    meng_yield_current();
}

/**
 * @brief 获取当前正在执行的 C++ 协程对象指针（若在主线程则返回 nullptr）
 */
inline coroutine* current() {
    return detail::current_cpp_coroutine_ref();
}

/**
 * @brief 现代 C++ 风格协程类，支持 RAII、Lambda 闭包、异常捕获与移动语义
 */
class coroutine {
public:
    using Task = std::function<void()>;

    /**
     * @brief 默认构造函数（空协程）
     */
    coroutine() noexcept : m_handle(nullptr), m_task(nullptr), m_exception(nullptr) {}

    /**
     * @brief 使用任意可调用对象构造协程
     * @param fn 可调用对象，如 lambda: [](){ ... }
     * @param stack_size 协程栈大小（字节），默认为 64KB
     */
    template <typename Callable,
              typename = typename std::enable_if<
                  !std::is_same<typename std::decay<Callable>::type, coroutine>::value>::type>
    explicit coroutine(Callable&& fn, size_t stack_size = 64 * 1024)
        : m_handle(nullptr),
          m_task(new Task(std::forward<Callable>(fn))),
          m_exception(nullptr) {
        m_handle = meng_create(&coroutine::entry_func, stack_size, &m_task, sizeof(Task*));
    }

    /**
     * @brief 析构函数，遵循 RAII 自动释放协程资源
     */
    ~coroutine() {
        destroy();
    }

    // 禁止拷贝
    coroutine(const coroutine&) = delete;
    coroutine& operator=(const coroutine&) = delete;

    // 允许移动
    coroutine(coroutine&& other) noexcept
        : m_handle(other.m_handle),
          m_task(other.m_task),
          m_exception(std::move(other.m_exception)) {
        other.m_handle = nullptr;
        other.m_task = nullptr;
    }

    coroutine& operator=(coroutine&& other) noexcept {
        if (this != &other) {
            destroy();
            m_handle = other.m_handle;
            m_task = other.m_task;
            m_exception = std::move(other.m_exception);
            other.m_handle = nullptr;
            other.m_task = nullptr;
        }
        return *this;
    }

    /**
     * @brief 恢复/启动协程执行
     * @return 如果协程挂起且尚未结束返回 true；执行完毕或已结束返回 false
     * @throws 如果协程体内抛出未捕获异常，会在 resume 时重新抛出该异常
     */
    bool resume() {
        if (!m_handle || done()) {
            return false;
        }

        coroutine* prev = detail::current_cpp_coroutine_ref();
        detail::current_cpp_coroutine_ref() = this;

        meng_run(m_handle);

        detail::current_cpp_coroutine_ref() = prev;

        if (m_exception) {
            std::exception_ptr ex = m_exception;
            m_exception = nullptr;
            std::rethrow_exception(ex);
        }

        return !done();
    }

    /**
     * @brief 协程主动让出执行权
     */
    void yield() {
        if (m_handle) {
            meng_yield(m_handle);
        }
    }

    /**
     * @brief 检查协程是否执行完毕
     */
    bool done() const noexcept {
        return !m_handle || meng_end(m_handle);
    }

    /**
     * @brief 是否为有效未结束协程
     */
    explicit operator bool() const noexcept {
        return m_handle != nullptr && !done();
    }

    /**
     * @brief 获取底层原生 C 句柄 meng*
     */
    meng* raw() const noexcept {
        return m_handle;
    }

    /**
     * @brief 获取协程栈大小
     */
    size_t stack_size() const noexcept {
        return meng_get_stack_size(m_handle);
    }

private:
    void destroy() noexcept {
        if (m_handle) {
            meng_delete(m_handle);
            m_handle = nullptr;
        }
        delete m_task;
        m_task = nullptr;
    }

    static void entry_func(meng* m, void* arg, size_t argsize) {
        (void)m;
        (void)argsize;
        Task* task = *reinterpret_cast<Task**>(arg);
        if (task && *task) {
            try {
                (*task)();
            } catch (...) {
                coroutine* self = current();
                if (self) {
                    self->m_exception = std::current_exception();
                }
            }
        }
    }

    meng* m_handle;
    Task* m_task;
    std::exception_ptr m_exception;
};

} // namespace libmeng
