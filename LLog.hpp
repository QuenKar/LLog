#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <iosfwd>
#include <type_traits>

namespace llog
{
    enum class LogLevel : uint8_t
    {
        INFO,
        WARN,
        CRIT
    };

    class LLogLine
    {
    public:
        LLogLine(LogLevel level, const char *file, const char *function, uint32_t line);
        ~LLogLine();

        LLogLine(LLogLine &&) = default;
        LLogLine &operator=(LLogLine &&) = default;

        void stringify(std::ostream &os);

        LLogLine &operator<<(char arg);
        LLogLine &operator<<(int32_t arg);
        LLogLine &operator<<(uint32_t arg);
        LLogLine &operator<<(int64_t arg);
        LLogLine &operator<<(uint64_t arg);
        LLogLine &operator<<(double arg);
        LLogLine &operator<<(const std::string &arg);

        template <size_t N>
        LLogLine &operator<<(const char (&arg)[N])
        {
            encode(string_literal_t(arg));
            return *this;
        }

        template <typename Arg>
        typename std::enable_if<std::is_same<Arg, const char *>::value, LLogLine &>::type
        operator<<(Arg const &arg)
        {
            encode(arg);
            return *this;
        }

        template <typename Arg>
        typename std::enable_if<std::is_same<Arg, char *>::value, LLogLine &>::type
        operator<<(Arg const &arg)
        {
            encode(arg);
            return *this;
        }

        struct string_literal_t
        {
            explicit string_literal_t(char const *s) : m_s(s) {}

            char const *m_s;
        };

    private:
        char *buffer();

        template <typename Arg>
        void encode(Arg arg);

        template <typename Arg>
        void encode(Arg arg, uint8_t type_id);

        void encode(char *arg);
        void encode(char const *arg);
        void encode(string_literal_t arg);
        void encode_c_string(char const *arg, size_t length);
        void resize_buffer_if_needed(size_t additional_bytes);
        void stringify(std::ostream &os, char *state, char const *const end);

    private:
        size_t m_bytes_used;
        size_t m_buffer_size;
        std::unique_ptr<char[]> m_heap_buffer;
        char m_stack_buffer[256 - 2 * sizeof(size_t) - sizeof(decltype(m_heap_buffer)) - 8];
    };

    struct LLog
    {
        bool operator==(LLogLine &);
    };

    void set_log_level(LogLevel level);

    bool is_logged(LogLevel level);

    struct NonGuaranteedLogger
    {
        NonGuaranteedLogger(uint32_t ring_buffer_size_mb_) : ring_buffer_size_mb(ring_buffer_size_mb_) {}
        uint32_t ring_buffer_size_mb;
    };

    struct GuaranteedLogger
    {
    };

    void initialize(GuaranteedLogger gl, std::string const &log_directory, std::string const &log_file_name, uint32_t log_file_roll_size_mb);
    void initialize(NonGuaranteedLogger ngl, std::string const &log_directory, std::string const &log_file_name, uint32_t log_file_roll_size_mb);

} //namespace llog

/*
__FILE__：在源文件中插入当前源文件名；
__LINE__：在源代码中插入当前源代码行号；
*/
#define LLOG(LEVEL) llog::LLog() == llog::LLogLine(LEVEL, __FILE__, __func__, __LINE__)

#define LOG_INFO llog::is_logged(llog::LogLevel::INFO) && LLOG(llog::LogLevel::INFO)
#define LOG_WARN llog::is_logged(llog::LogLevel::WARN) && LLOG(llog::LogLevel::WARN)
#define LOG_CRIT llog::is_logged(llog::LogLevel::CRIT) && LLOG(llog::LogLevel::CRIT)