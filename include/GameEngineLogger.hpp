// GameEngineLogger.h
// Generated by AI (Claude Sonnet 3.7)
#pragma once

#include <deque>
#include <future>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <atomic>
#include <windows.h>
#undef ERROR
#include <lua.hpp>

namespace GameLogger {

// Forward declarations
class LogChannel;
class LogSink;
class LogFormatter;

// Define log levels
enum class LogLevel { TRACE, DEBUG, INFO, WARNING, ERROR, CRITICAL, OFF };

// Convert LogLevel to string
inline std::string LogLevelToString(LogLevel level) {
    switch (level) {
    case LogLevel::TRACE:
        return "TRACE";
    case LogLevel::DEBUG:
        return "DEBUG";
    case LogLevel::INFO:
        return "INFO";
    case LogLevel::WARNING:
        return "WARNING";
    case LogLevel::ERROR:
        return "ERROR";
    case LogLevel::CRITICAL:
        return "CRITICAL";
    case LogLevel::OFF:
        return "OFF";
    default:
        return "UNKNOWN";
    }
}

// Log message structure
struct LogMessage {
    std::string content;
    LogLevel level;
    std::string channel;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::thread::id threadId;
    std::unordered_map<std::string, std::string> tags;
};

// Base log formatter interface
class LogFormatter {
  public:
    virtual ~LogFormatter() = default;
    virtual std::string Format(const LogMessage& message) = 0;
};

// Default formatter implementation
class DefaultFormatter : public LogFormatter {
  public:
    std::string Format(const LogMessage& message) override {
        auto time = std::chrono::system_clock::to_time_t(message.timestamp);
        std::tm tm;

#ifdef _WIN32
        localtime_s(&tm, &time);
#else
        localtime_r(&time, &tm);
#endif

        std::stringstream ss;
        ss << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "."
           << std::chrono::duration_cast<std::chrono::milliseconds>(
                  message.timestamp.time_since_epoch())
                      .count() %
                  1000
           << "] [" << message.threadId << "] ["
           << LogLevelToString(message.level) << "] [" << message.channel
           << "] " << message.content;

        // Add file and line if available
        if (!message.file.empty()) {
            ss << " (" << message.file << ":" << message.line << ") ";
        }
        if (!message.function.empty()) {
            ss << " [" << message.function << "]";
        }

        return ss.str();
    }
};

// JSON formatter implementation
class JsonFormatter : public LogFormatter {
  public:
    std::string Format(const LogMessage& message) override {
        auto time = std::chrono::system_clock::to_time_t(message.timestamp);
        std::tm tm;

#ifdef _WIN32
        localtime_s(&tm, &time);
#else
        localtime_r(&time, &tm);
#endif

        std::stringstream ss;
        ss << "{";

        // Add timestamp
        ss << "\"timestamp\":\"" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
           << "."
           << std::chrono::duration_cast<std::chrono::milliseconds>(
                  message.timestamp.time_since_epoch())
                      .count() %
                  1000
           << "\",";

        // Add level
        ss << "\"level\":\"" << LogLevelToString(message.level) << "\",";

        // Add channel
        ss << "\"channel\":\"" << message.channel << "\",";

        // Add thread ID
        ss << "\"thread_id\":\"" << message.threadId << "\",";

        // Add message content
        ss << "\"message\":\"" << EscapeJsonString(message.content) << "\"";

        // Add file and line if available
        if (!message.file.empty()) {
            ss << ",\"file\":\"" << message.file
               << "\",\"line\":" << message.line;
        }

        // Add function if available
        if (!message.function.empty()) {
            ss << ",\"function\":\"" << message.function << "\"";
        }

        // Add tags if available
        if (!message.tags.empty()) {
            ss << ",\"tags\":{";
            bool first = true;
            for (const auto& tag : message.tags) {
                if (!first)
                    ss << ",";
                ss << "\"" << tag.first << "\":\""
                   << EscapeJsonString(tag.second) << "\"";
                first = false;
            }
            ss << "}";
        }

        ss << "}";
        return ss.str();
    }

  private:
    std::string EscapeJsonString(const std::string& input) {
        std::string output;
        output.reserve(input.length());

        for (char c : input) {
            switch (c) {
            case '\"':
                output += "\\\"";
                break;
            case '\\':
                output += "\\\\";
                break;
            case '\b':
                output += "\\b";
                break;
            case '\f':
                output += "\\f";
                break;
            case '\n':
                output += "\\n";
                break;
            case '\r':
                output += "\\r";
                break;
            case '\t':
                output += "\\t";
                break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    output += buf;
                } else {
                    output += c;
                }
            }
        }

        return output;
    }
};

// Base log sink interface
class LogSink {
  public:
    virtual ~LogSink() = default;
    virtual void Write(const LogMessage& message) = 0;
    virtual void Flush() = 0;

    void SetFormatter(std::shared_ptr<LogFormatter> formatter) {
        m_formatter = formatter;
    }

    std::shared_ptr<LogFormatter> GetFormatter() const { return m_formatter; }

  protected:
    std::shared_ptr<LogFormatter> m_formatter =
        std::make_shared<DefaultFormatter>();
};

// Console sink implementation
class ConsoleSink : public LogSink {
  public:
    void Write(const LogMessage& message) override {
        std::string formattedMessage = m_formatter->Format(message);

// Apply console colors based on log level
#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
        WORD originalAttrs = consoleInfo.wAttributes;

        WORD textColor;
        switch (message.level) {
        case LogLevel::TRACE:
            textColor = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case LogLevel::DEBUG:
            textColor = FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;
        case LogLevel::INFO:
            textColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;
        case LogLevel::WARNING:
            textColor =
                FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case LogLevel::ERROR:
            textColor = FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case LogLevel::CRITICAL:
            textColor = FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_RED;
            break;
        default:
            textColor = originalAttrs;
        }

        SetConsoleTextAttribute(hConsole, textColor);
        std::cout << formattedMessage << std::endl;
        SetConsoleTextAttribute(hConsole, originalAttrs);
#else
        // ANSI color codes for Unix/Linux/Mac
        const char* colorCode;
        switch (message.level) {
        case LogLevel::TRACE:
            colorCode = "\033[94m";
            break; // Bright blue
        case LogLevel::DEBUG:
            colorCode = "\033[36m";
            break; // Cyan
        case LogLevel::INFO:
            colorCode = "\033[0m";
            break; // Default
        case LogLevel::WARNING:
            colorCode = "\033[33m";
            break; // Yellow
        case LogLevel::ERROR:
            colorCode = "\033[31m";
            break; // Red
        case LogLevel::CRITICAL:
            colorCode = "\033[97m\033[41m";
            break; // White on red
        default:
            colorCode = "\033[0m";
        }

        std::cout << colorCode << formattedMessage << "\033[0m" << std::endl;
#endif
    }

    void Flush() override { std::cout.flush(); }
};

// File sink implementation
class FileSink : public LogSink {
  public:
    FileSink(const std::string& filePath, bool append = true)
        : m_filePath(filePath) {
        m_fileStream.open(filePath, append ? std::ios::app : std::ios::trunc);
        if (!m_fileStream.is_open()) {
            throw std::runtime_error("Failed to open log file: " + filePath);
        }
    }

    ~FileSink() override {
        if (m_fileStream.is_open()) {
            m_fileStream.close();
        }
    }

    void Write(const LogMessage& message) override {
        if (m_fileStream.is_open()) {
            m_fileStream << m_formatter->Format(message) << std::endl;
        }
    }

    void Flush() override {
        if (m_fileStream.is_open()) {
            m_fileStream.flush();
        }
    }

  private:
    std::string m_filePath;
    std::ofstream m_fileStream;
};

// Rotating file sink based on size
class RotatingFileSink : public LogSink {
  public:
    RotatingFileSink(const std::string& filePath,
                     size_t maxFileSize = 10 * 1024 * 1024, int maxFiles = 5)
        : m_filePath(filePath), m_maxFileSize(maxFileSize),
          m_maxFiles(maxFiles) {
        m_fileStream.open(filePath, std::ios::app);
        if (!m_fileStream.is_open()) {
            throw std::runtime_error("Failed to open log file: " + filePath);
        }

        // Get current file size
        m_fileStream.seekp(0, std::ios::end);
        m_currentFileSize = m_fileStream.tellp();
    }

    ~RotatingFileSink() override {
        if (m_fileStream.is_open()) {
            m_fileStream.close();
        }
    }

    void Write(const LogMessage& message) override {
        if (!m_fileStream.is_open()) {
            return;
        }

        std::string formattedMessage = m_formatter->Format(message) + "\n";

        // Check if we need to rotate
        if (m_currentFileSize + formattedMessage.size() > m_maxFileSize) {
            Rotate();
        }

        // Write the message
        m_fileStream << formattedMessage;
        m_currentFileSize += formattedMessage.size();
    }

    void Flush() override {
        if (m_fileStream.is_open()) {
            m_fileStream.flush();
        }
    }

  private:
    void Rotate() {
        // Close current file
        m_fileStream.close();

        // Delete oldest file if we have too many
        std::string oldestFile =
            m_filePath + "." + std::to_string(m_maxFiles - 1);
        if (FileExists(oldestFile)) {
            std::remove(oldestFile.c_str());
        }

        // Shift all files down
        for (int i = m_maxFiles - 2; i >= 0; --i) {
            std::string currentFile =
                (i == 0) ? m_filePath : m_filePath + "." + std::to_string(i);
            std::string nextFile = m_filePath + "." + std::to_string(i + 1);

            if (FileExists(currentFile)) {
                std::rename(currentFile.c_str(), nextFile.c_str());
            }
        }

        // Open a new file
        m_fileStream.open(m_filePath, std::ios::trunc);
        if (!m_fileStream.is_open()) {
            std::cerr << "Failed to open new log file after rotation: "
                      << m_filePath << std::endl;
        }

        // Reset file size
        m_currentFileSize = 0;
    }

    bool FileExists(const std::string& path) {
        std::ifstream file(path);
        return file.good();
    }

    std::string m_filePath;
    size_t m_maxFileSize;
    int m_maxFiles;
    std::ofstream m_fileStream;
    size_t m_currentFileSize;
};

// Memory sink for testing or debugging
class MemorySink : public LogSink {
  public:
    MemorySink(size_t maxMessages = 1000) : m_maxMessages(maxMessages) {}

    void Write(const LogMessage& message) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_messages.push_back(m_formatter->Format(message));

        // Keep only the last m_maxMessages
        if (m_messages.size() > m_maxMessages) {
            m_messages.pop_front();
        }
    }

    void Flush() override {
        // Nothing to do
    }

    std::vector<std::string> GetMessages() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return std::vector<std::string>(m_messages.begin(), m_messages.end());
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_messages.clear();
    }

  private:
    std::deque<std::string> m_messages;
    std::mutex m_mutex;
    size_t m_maxMessages;
};

// Log channel class
class LogChannel {
  public:
    LogChannel(const std::string& name, LogLevel level = LogLevel::INFO)
        : m_name(name), m_level(level) {}

    void AddSink(std::shared_ptr<LogSink> sink) {
        std::lock_guard<std::mutex> lock(m_sinkMutex);
        m_sinks.push_back(sink);
    }

    void RemoveSink(std::shared_ptr<LogSink> sink) {
        std::lock_guard<std::mutex> lock(m_sinkMutex);
        m_sinks.erase(std::remove(m_sinks.begin(), m_sinks.end(), sink),
                      m_sinks.end());
    }

    void SetLevel(LogLevel level) { m_level.store(level); }

    LogLevel GetLevel() const { return m_level; }

    const std::string& GetName() const { return m_name; }

    void Log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "") {
        if (level < m_level) {
            return;
        }

        LogMessage logMessage;
        logMessage.content = message;
        logMessage.level = level;
        logMessage.channel = m_name;
        logMessage.file = file;
        logMessage.line = line;
        logMessage.function = function;
        logMessage.timestamp = std::chrono::system_clock::now();
        logMessage.threadId = std::this_thread::get_id();

        // Add any additional tags
        {
            std::lock_guard<std::mutex> lock(m_tagsMutex);
            logMessage.tags = m_tags;
        }

        // Distribute to all sinks
        {
            std::lock_guard<std::mutex> lock(m_sinkMutex);
            for (auto& sink : m_sinks) {
                sink->Write(logMessage);
            }
        }
    }

    void AddTag(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(m_tagsMutex);
        m_tags[key] = value;
    }

    void RemoveTag(const std::string& key) {
        std::lock_guard<std::mutex> lock(m_tagsMutex);
        m_tags.erase(key);
    }

    void ClearTags() {
        std::lock_guard<std::mutex> lock(m_tagsMutex);
        m_tags.clear();
    }

    void Flush() {
        std::lock_guard<std::mutex> lock(m_sinkMutex);
        for (auto& sink : m_sinks) {
            sink->Flush();
        }
    }

  private:
    std::string m_name;
    std::atomic<LogLevel> m_level;
    std::vector<std::shared_ptr<LogSink>> m_sinks;
    std::mutex m_sinkMutex;
    std::unordered_map<std::string, std::string> m_tags;
    std::mutex m_tagsMutex;
};

// Log manager singleton
class LogManager {
  public:
    static LogManager& Instance() {
        static LogManager instance;
        return instance;
    }

    // Create default channel and sink
    void Initialize() {
        auto defaultChannel =
            std::make_shared<LogChannel>("default", LogLevel::INFO);
        auto consoleSink = std::make_shared<ConsoleSink>();
        defaultChannel->AddSink(consoleSink);

        // Add default channel
        AddChannel(defaultChannel);
    }

    void AddChannel(std::shared_ptr<LogChannel> channel) {
        std::lock_guard<std::mutex> lock(m_channelMutex);
        // Add default sinks to new channel
        for (auto& sink : m_defaultSinks) {
            channel->AddSink(sink);
        }
        m_channels[channel->GetName()] = channel;
    }

    std::shared_ptr<LogChannel> GetChannel(const std::string& name) {
        std::lock_guard<std::mutex> lock(m_channelMutex);
        auto it = m_channels.find(name);
        if (it != m_channels.end()) {
            return it->second;
        }
        return nullptr;
    }

    std::shared_ptr<LogChannel>
    GetOrCreateChannel(const std::string& name,
                       LogLevel level = LogLevel::INFO) {
        std::lock_guard<std::mutex> lock(m_channelMutex);
        auto it = m_channels.find(name);
        if (it != m_channels.end()) {
            return it->second;
        }

        // Create new channel
        auto channel = std::make_shared<LogChannel>(name, level);
        AddChannel(channel);
        return channel;
    }

    void RemoveChannel(const std::string& name) {
        std::lock_guard<std::mutex> lock(m_channelMutex);
        m_channels.erase(name);
    }

    void AddDefaultSink(std::shared_ptr<LogSink> sink) {
        std::lock_guard<std::mutex> lock(m_channelMutex);
        m_defaultSinks.push_back(sink);

        // Add to all existing channels
        for (auto& channelPair : m_channels) {
            channelPair.second->AddSink(sink);
        }
    }

    void SetGlobalLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(m_channelMutex);
        for (auto& channelPair : m_channels) {
            channelPair.second->SetLevel(level);
        }
    }

    void Flush() {
        std::lock_guard<std::mutex> lock(m_channelMutex);
        for (auto& channelPair : m_channels) {
            channelPair.second->Flush();
        }
    }

    void Shutdown() {
        Flush();

        // Clear all channels to release resources
        std::lock_guard<std::mutex> lock(m_channelMutex);
        m_channels.clear();
        m_defaultSinks.clear();
    }

    // TODO: Move lua functionality out of LogManager
    //  Lua integration
    void RegisterLuaFunctions(lua_State* L) {
        // Create a metatable for the logger
        luaL_newmetatable(L, "GameLogger");

        lua_pushcfunction(L, LuaLog);
        lua_setfield(L, -2, "__call");

        lua_pushstring(L, "__index");
        lua_pushvalue(L, -2);
        lua_settable(L, -3);

        // Create a global logger object
        lua_newtable(L);
        luaL_getmetatable(L, "GameLogger");
        lua_setmetatable(L, -2);

        // Add functions to the logger
        lua_pushcfunction(L, LuaTrace);
        lua_setfield(L, -2, "trace");

        lua_pushcfunction(L, LuaDebug);
        lua_setfield(L, -2, "debug");

        lua_pushcfunction(L, LuaInfo);
        lua_setfield(L, -2, "info");

        lua_pushcfunction(L, LuaWarning);
        lua_setfield(L, -2, "warning");

        lua_pushcfunction(L, LuaError);
        lua_setfield(L, -2, "error");

        lua_pushcfunction(L, LuaCritical);
        lua_setfield(L, -2, "critical");

        // Set the global logger
        lua_setglobal(L, "Logger");
    }

  private:
    LogManager() = default;
    ~LogManager() = default;

    std::unordered_map<std::string, std::shared_ptr<LogChannel>> m_channels;
    std::mutex m_channelMutex;
    std::vector<std::shared_ptr<LogSink>> m_defaultSinks;

    // Lua functions
    static int LuaLog(lua_State* L) {
        LogLevel level = static_cast<LogLevel>(luaL_checkinteger(L, 1));
        const char* message = luaL_checkstring(L, 2);

        // Get channel name if provided, otherwise use default
        const char* channelName =
            lua_isstring(L, 3) ? lua_tostring(L, 3) : "lua";

        // Get filename and line if provided
        const char* file = lua_isstring(L, 4) ? lua_tostring(L, 4) : "";
        int line = lua_isnumber(L, 5) ? lua_tointeger(L, 5) : 0;

        // Get or create channel
        auto channel = Instance().GetOrCreateChannel(channelName);

        // Log the message
        channel->Log(level, message, file, line);

        return 0;
    }

    static int LuaTrace(lua_State* L) {
        const char* message = luaL_checkstring(L, 1);

        // Get channel name if provided, otherwise use lua
        const char* channelName =
            lua_isstring(L, 2) ? lua_tostring(L, 2) : "lua";

        // Get filename and line if provided
        const char* file = lua_isstring(L, 3) ? lua_tostring(L, 3) : "";
        int line = lua_isnumber(L, 4) ? lua_tointeger(L, 4) : 0;

        auto channel = Instance().GetOrCreateChannel(channelName);
        channel->Log(LogLevel::TRACE, message, file, line);

        return 0;
    }

    static int LuaDebug(lua_State* L) {
        const char* message = luaL_checkstring(L, 1);
        const char* channelName =
            lua_isstring(L, 2) ? lua_tostring(L, 2) : "lua";
        const char* file = lua_isstring(L, 3) ? lua_tostring(L, 3) : "";
        int line = lua_isnumber(L, 4) ? lua_tointeger(L, 4) : 0;

        auto channel = Instance().GetOrCreateChannel(channelName);
        channel->Log(LogLevel::DEBUG, message, file, line);

        return 0;
    }

    static int LuaInfo(lua_State* L) {
        const char* message = luaL_checkstring(L, 1);
        const char* channelName =
            lua_isstring(L, 2) ? lua_tostring(L, 2) : "lua";
        const char* file = lua_isstring(L, 3) ? lua_tostring(L, 3) : "";
        int line = lua_isnumber(L, 4) ? lua_tointeger(L, 4) : 0;

        auto channel = Instance().GetOrCreateChannel(channelName);
        channel->Log(LogLevel::INFO, message, file, line);

        return 0;
    }

    static int LuaWarning(lua_State* L) {
        const char* message = luaL_checkstring(L, 1);
        const char* channelName =
            lua_isstring(L, 2) ? lua_tostring(L, 2) : "lua";
        const char* file = lua_isstring(L, 3) ? lua_tostring(L, 3) : "";
        int line = lua_isnumber(L, 4) ? lua_tointeger(L, 4) : 0;

        auto channel = Instance().GetOrCreateChannel(channelName);
        channel->Log(LogLevel::WARNING, message, file, line);

        return 0;
    }

    static int LuaError(lua_State* L) {
        const char* message = luaL_checkstring(L, 1);
        const char* channelName =
            lua_isstring(L, 2) ? lua_tostring(L, 2) : "lua";
        const char* file = lua_isstring(L, 3) ? lua_tostring(L, 3) : "";
        int line = lua_isnumber(L, 4) ? lua_tointeger(L, 4) : 0;

        auto channel = Instance().GetOrCreateChannel(channelName);
        channel->Log(LogLevel::ERROR, message, file, line);

        return 0;
    }

    static int LuaCritical(lua_State* L) {
        const char* message = luaL_checkstring(L, 1);
        const char* channelName =
            lua_isstring(L, 2) ? lua_tostring(L, 2) : "lua";
        const char* file = lua_isstring(L, 3) ? lua_tostring(L, 3) : "";
        int line = lua_isnumber(L, 4) ? lua_tointeger(L, 4) : 0;

        auto channel = Instance().GetOrCreateChannel(channelName);
        channel->Log(LogLevel::CRITICAL, message, file, line);

        return 0;
    }
};

// Macros for easy logging
#define LOG_TRACE(channel, message)                                            \
    GameLogger::LogManager::Instance().GetOrCreateChannel(channel)->Log(       \
        GameLogger::LogLevel::TRACE, message, __FILE__, __LINE__, __func__)

#define LOG_DEBUG(channel, message)                                            \
    GameLogger::LogManager::Instance().GetOrCreateChannel(channel)->Log(       \
        GameLogger::LogLevel::DEBUG, message, __FILE__, __LINE__, __func__)

#define LOG_INFO(channel, message)                                             \
    GameLogger::LogManager::Instance().GetOrCreateChannel(channel)->Log(       \
        GameLogger::LogLevel::INFO, message, __FILE__, __LINE__, __func__)

#define LOG_WARNING(channel, message)                                          \
    GameLogger::LogManager::Instance().GetOrCreateChannel(channel)->Log(       \
        GameLogger::LogLevel::WARNING, message, __FILE__, __LINE__, __func__)

#define LOG_ERROR(channel, message)                                            \
    GameLogger::LogManager::Instance().GetOrCreateChannel(channel)->Log(       \
        GameLogger::LogLevel::ERROR, message, __FILE__, __LINE__, __func__)

#define LOG_CRITICAL(channel, message)                                         \
    GameLogger::LogManager::Instance().GetOrCreateChannel(channel)->Log(       \
        GameLogger::LogLevel::CRITICAL, message, __FILE__, __LINE__, __func__)
} // namespace GameLogger