#pragma once
#include <lua.hpp>
#include <string>
#include <sol/sol.hpp>
#include <filesystem>


// The Lua core helps with functions and utility surrounding lua.
class LuaCore {
  public:
    LuaCore();
    ~LuaCore();

    inline static const std::string Version = "0.1.3";

    void Init();
    // Run a lua script or directory of lua scripts
    void Run(std::string path, bool recursive = false);
    // Run a lua script from a file
    void RunFile(std::string path);

  private:
    sol::state m_solState;
};
