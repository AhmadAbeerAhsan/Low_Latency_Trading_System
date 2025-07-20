#pragma once

#include <string>
#include <fstream>
#include <cstdio>

#include "lock_free_queue.hpp"
#include "thread_utils.hpp"
#include "time_utils.hpp"
namespace Common
{
    constexpr size_t LOG_QUEUE_SIZE = 8 * 1024 * 1024;

    enum class LogType : int8_t
    {
        CHAR = 0,
        INTEGER = 1,
        LONG_INTEGER = 2,
        LONG_LONG_INTEGER = 3,
        UNSIGNED_INTEGER = 4,
        UNSIGNED_LONG_INTEGER = 5,
        UNSIGNED_LONG_LONG_INTEGER = 6,
        FLOAT = 7,
        DOUBLE = 8
    };

    union S
        {
            char c;
            int i;
            long l;
            long long ll;
            unsigned u;
            unsigned long ul;
            unsigned long long ull;
            float f;
            double d;
        };

    struct LogElement
    {
        LogType type_ = LogType::CHAR;
        union
        S u_;
    };

    class Logger final
    {
    private:
        const std::string file_name_;
        std::ofstream file_;                 // a file object is the log file that data is written to
        LFQueue<LogElement> queue_;          // the lock-free queue to transfer data from the main thread to the logger thread
        std::atomic<bool> running_ = {true}; // stops the logger thread’s processing when needed
        std::thread *logger_thread_ = nullptr;

    public:
        explicit Logger(const std::string &file_name)
            : file_name_(file_name), queue_(LOG_QUEUE_SIZE)
        {
            file_.open(file_name);
            assert(file_.is_open(), "Could not open log file:" + file_name);
            logger_thread_ = createAndStartThread("Common/Logger", [this]()
                                                  { flushQueue(); });
            assert(logger_thread_ != nullptr, "Failed to start Logger thread.");
        }
        Logger() = delete;
        Logger(const Logger &) = delete;
        Logger(const Logger &&) = delete;
        Logger &operator=(const Logger &) = delete;
        Logger &operator=(const Logger &&) = delete;

        ~Logger()
        {
            std::string time_str;
            std::cerr << Common::getCurrentTimeStr(&time_str) << " Flushing and closing Logger for " << file_name_ << std::endl;

            while (queue_.size())
            {
                using namespace std::literals::chrono_literals;
                std::this_thread::sleep_for(1s);
            }
            running_ = false;
            logger_thread_->join();

            file_.close();
            std::cerr << Common::getCurrentTimeStr(&time_str) << " Logger for " << file_name_ << " exiting." << std::endl;
        }

        auto pushValue(const LogElement &log_element) noexcept
        {
            *(queue_.getNextToWriteTo()) = log_element;
            queue_.updateWriteIndex();
        }

        auto pushValue(const char value) noexcept
        {
            S u {};
            u.c = value;
            pushValue(LogElement{LogType::CHAR, {value}});
        }

        auto pushValue(const int value) noexcept
        {
            S u {};
            u.i = value;
            pushValue(LogElement{LogType::INTEGER, u});
        }

        auto pushValue(const long value) noexcept
        {
            S u {};
            u.l = value;
            pushValue(LogElement{LogType::LONG_INTEGER, u});
        }

        auto pushValue(const long long value) noexcept
        {
            S u {};
            u.ll = value;
            pushValue(LogElement{LogType::LONG_LONG_INTEGER, u});
        }

        auto pushValue(const unsigned value) noexcept
        {
            S u {};
            u.u = value;
            pushValue(LogElement{LogType::UNSIGNED_INTEGER, u});
        }

        auto pushValue(const unsigned long value) noexcept
        {
            S u {};
            u.ul = value;
            pushValue(LogElement{LogType::UNSIGNED_LONG_INTEGER, u});
        }

        auto pushValue(const unsigned long long value) noexcept
        {
            S u {};
            u.ull = value;
            pushValue(LogElement{LogType::UNSIGNED_LONG_LONG_INTEGER, u});
        }

        auto pushValue(const float value) noexcept
        {
            S u {};
            u.f = value;
            pushValue(LogElement{LogType::FLOAT, u});
        }

        auto pushValue(const double value) noexcept
        {
            S u {};
            u.d = value;
            pushValue(LogElement{LogType::DOUBLE, u});
        }

        auto pushValue(const char *value) noexcept
        {
            while (*value)
            {
                pushValue(*value);
                ++value;
            }
        }

        auto pushValue(const std::string &value) noexcept
        {
            pushValue(value.c_str());
        }

        template <typename T, typename... A>
        auto log(const char *s, const T &value, A... args) noexcept
        {
            while (*s)
            {
                if (*s == '%')
                {
                    if (*(s + 1) == '%')
                    { // to allow %% -> % escape character.
                        ++s;
                    }
                    else
                    {
                        pushValue(value);    // substitute % with the value specified in the arguments.
                        log(s + 1, args...); // pop an argument and call self recursively.
                        return;
                    }
                }
                pushValue(*s++);
            }
            printf("extra arguments provided to log()");
        }

        // note that this is overloading not specialization. gcc does not allow inline specializations.
        auto log(const char *s) noexcept
        {
            while (*s)
            {
                if (*s == '%')
                {
                    if (*(s + 1) == '%')
                    { // to allow %% -> % escape character.
                        ++s;
                    }
                    else
                    {
                        printf("missing arguments to log()");
                    }
                }
                pushValue(*s++);
            }
        }

    private:
        void flushQueue() noexcept
        {
            while (running_)
            {
                for (auto next = queue_.getNextToRead();
                     queue_.size() && next; next = queue_.getNextToRead())
                {
                    switch (next->type_)
                    {
                    case LogType::CHAR:
                        file_ << next->u_.c;
                        break;
                    case LogType::INTEGER:
                        file_ << next->u_.i;
                        break;
                    case LogType::LONG_INTEGER:
                        file_ << next->u_.l;
                        break;
                    case LogType::LONG_LONG_INTEGER:
                        file_ << next->u_.ll;
                        break;
                    case LogType::UNSIGNED_INTEGER:
                        file_ << next->u_.u;
                        break;
                    case LogType::UNSIGNED_LONG_INTEGER:
                        file_ << next->u_.ul;
                        break;
                    case LogType::UNSIGNED_LONG_LONG_INTEGER:
                        file_
                            << next->u_.ull;
                        break;
                    case LogType::FLOAT:
                        file_ << next->u_.f;
                        break;
                    case LogType::DOUBLE:
                        file_ << next->u_.d;
                        break;
                    }
                    queue_.updateReadIndex();
                }
                using namespace std::literals::chrono_literals;
                std::this_thread::sleep_for(1ms);
            }
        }
    };
}