#include "LuaCore.hpp"

#include <filesystem>
#include <sol/property.hpp>
#include <sol/raii.hpp>
#include <sol/sol.hpp>
#include <iostream>
#include <sol/state_handling.hpp>
#include <sol/state_view.hpp>
#include <sol/types.hpp>
#include <string>

#include "bx/debug.h"
#include "glm/fwd.hpp"
#include "Entity.hpp"
#include "Enums.hpp"

#include "lua/LuaPrimitive.hpp"
#include "lua/LuaVector3.hpp"
#include "lua/LuaSceneRef.hpp"
#include "lua/LuaMaterial.hpp"
#include "lua/singletons/LuaWindow.hpp"

namespace {
int luaPrintOverride(lua_State* L) {
    int n = lua_gettop(L); // number of arguments
    std::cout << "[Lua]: ";
    for (int i = 1; i <= n; i++) {
        std::cout << luaL_tolstring(L, i, nullptr) << " ";
    }
    std::cout << std::endl;
    return 0;
}

} // namespace

void LuaCore::RunFile(std::string file) {
    auto path = std::filesystem::path(file);

    if (!std::filesystem::exists(path)) {
        std::cerr << "LuaCore::Run: File or directory does not exist: " << file
                  << std::endl;
        return;
    }

    if (path.extension() != ".lua") {
        std::cerr << "LuaCore::Run: File or directory does not exist: " << file
                  << std::endl;
        return;
    }

    if (std::filesystem::is_directory(path)) {
        std::cerr << "LuaCore::Run: File is a directory: " << file << std::endl;
        return;
    }

    auto result = m_solState.safe_script_file(path.string());
    if (!result.valid()) {
        sol::error err = result;
        std::cerr << "LuaCore::Run: Error running script: " << file << "\n"
                  << err.what() << std::endl;
    }
}

void LuaCore::Run(std::string path, bool recursive) {

    std::function<void(const std::filesystem::path&)> run_children;
    run_children = [&](const std::filesystem::path& _path) {
        for (const auto& entry : std::filesystem::directory_iterator(_path)) {
            if (entry.path().extension() == ".lua") {
                RunFile(entry.path().string());
            } else if (std::filesystem::is_directory(entry.path())) {
                run_children(entry.path());
            }
        }
    };

    if (std::filesystem::is_directory(path)) {
        if (recursive) {
            run_children(path);
        }
    } else {
        RunFile(path);
    }
}

void LuaCore::Init() {
    // Ref for ease of use
    sol::state& lua = m_solState;

    bx::debugPrintf("Opening default libraries\n");
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string,
                       sol::lib::math, sol::lib::table, sol::lib::debug);

    lua["print"] = luaPrintOverride;
    lua["Version"] = LuaCore::Version;

    sol::usertype<glm::vec3> vec3 = lua.new_usertype<glm::vec3>(
        "Vector3",
        sol::constructors<glm::vec3(float, float, float), glm::vec3(float),
                          glm::vec3()>(),
        sol::meta_function::addition, &LuaVector3::__add,
        sol::meta_function::subtraction, &LuaVector3::__sub,
        sol::meta_function::multiplication, &LuaVector3::__mul,
        sol::meta_function::division, &LuaVector3::__div,
        sol::meta_function::equal_to, &LuaVector3::__eq,
        sol::meta_function::unary_minus, &LuaVector3::__unm);

    vec3["X"] = &glm::vec3::x;
    vec3["Y"] = &glm::vec3::y;
    vec3["Z"] = &glm::vec3::z;
    vec3["Length"] = &LuaVector3::length;
    vec3["Normalize"] = &LuaVector3::normalize;
    vec3["Dot"] = &LuaVector3::dot;
    vec3["Cross"] = &LuaVector3::cross;

    lua.new_usertype<LuaMaterial>(
        "Material",
        sol::constructors<Material(const std::string&, const std::string&)>());

    lua.new_usertype<LuaSceneRef<Entity>>(
        "EntitySceneRef", "Id", sol::readonly(&LuaSceneRef<Entity>::GetId),
        "Destroy", &LuaSceneRef<Entity>::Destroy);

    lua.new_enum("PrimitiveType", "Cube", PrimitiveType::Cube, "Sphere",
                 PrimitiveType::Sphere, "Plane", PrimitiveType::Plane);

    lua.new_usertype<LuaPrimitive>(
        "Primitive",
        sol::constructors<
            LuaPrimitive(PrimitiveType), LuaPrimitive(PrimitiveType, glm::vec3),
            LuaPrimitive(PrimitiveType, LuaMaterial),
            LuaPrimitive(PrimitiveType, LuaMaterial, glm::vec3)>(),
        "Position",
        sol::property(&LuaPrimitive::GetPosition, &LuaPrimitive::SetPosition),
        "Type", sol::property(&LuaPrimitive::GetType, &LuaPrimitive::SetType),
        sol::base_classes, sol::bases<LuaSceneRef<Entity>>());

    lua.new_usertype<LuaWindow>("Window", "SetTitle", &LuaWindow::SetTitle);
    lua["Window"] = &LuaWindow::Get();
}

LuaCore::LuaCore() : m_solState() {}
LuaCore::~LuaCore() {}
