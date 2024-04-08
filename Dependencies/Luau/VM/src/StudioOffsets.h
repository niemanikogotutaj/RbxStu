//
// Created by Dottik on 3/4/2024.
//
#pragma once

#include "Windows.h"
#include <cstdint>

#define RebaseAddress(x) x - 0x140000000 + reinterpret_cast<uintptr_t>(GetModuleHandleA("RobloxStudioBeta.exe"))

namespace RBX::Studio::Offsets {
    const static std::uintptr_t luau_execute = RebaseAddress(0x142f5fb90);          // Search for "C Stack Overflow"
    const static std::uintptr_t luau_load = RebaseAddress(0x142f697a0);             // Search for "Bytecode Version"
    const static std::uintptr_t lua_newthread = RebaseAddress(0x142f511a0);         // search for coroutine.wrap

    const static std::uintptr_t _luaO_nilobject = RebaseAddress(
                                                          0x1465dd4b0);             // Get to lua_pushvalue and get inside of pseudo2addr, you will find the data xref.
    const static std::uintptr_t _luaH_dummynode = RebaseAddress(
                                                          0x1465dd358);             // Find " ,"metatable": ", and go to the top of the second xref, a ldebug function. It will contain two references to luaH_dummynode.

    // The way we get our lua state is the classic, hooking.
    const static std::uintptr_t pseudo2addr = RebaseAddress(0x142f53230);

    const static std::uintptr_t rTask_spawn = RebaseAddress(
                                                      0x141ca6090);           // Search for spawn. It should only take 1 argument, and return an int32_t, see luaC_Function
    const static std::uintptr_t rluaE_newthread = RebaseAddress(0x142f54720);
    const static std::uintptr_t rlua_newthread = RebaseAddress(0x142f511a0);
    const static std::uintptr_t rbxAllocate = RebaseAddress(0x14128c580);       // Exported function.
    const static std::uintptr_t rTask_defer = RebaseAddress(0x141ca54f0);

    const static std::uintptr_t rFromLuaState = RebaseAddress(
                                                        0x141af7920);     // Appears to copy ones' L->userdata into another for new states. Search for "Failed to create Lua State", on the userthread user callback.
}

struct lua_State;

namespace FunctionTypes {
    using luau_load = int32_t (__fastcall *)(struct lua_State *L, const char *chunkName, const char *bytecode,
                                             int32_t bytecodeSize,
                                             int32_t env);
    using luau_execute = void (__fastcall *)(struct lua_State *L);
    using pseudo2addr = void *(__fastcall *)(lua_State *L, int32_t lua_index);
    using lua_newthread = lua_State *(__fastcall *)(lua_State *L);
    using rTask_spawn = int (__fastcall *)(lua_State *L);
    using rTask_defer = int (__fastcall *)(lua_State *L);
    using rluaE_newthread = lua_State *(__fastcall *)(lua_State *L);
    using rlua_newthread = lua_State *(__fastcall *)(lua_State *L);
    using rbxAllocate = void *(__fastcall *)(std::uintptr_t size);
    using rFromLuaState = void (__fastcall *)(lua_State *LP, lua_State *L);
    using rLuaC_step = size_t (__fastcall *)(lua_State *L, bool assist);
};

namespace RBX::Studio::Functions {
    const static auto luau_load = reinterpret_cast<FunctionTypes::luau_load>(RBX::Studio::Offsets::luau_load);
    const static auto luau_execute = reinterpret_cast<FunctionTypes::luau_execute>(RBX::Studio::Offsets::luau_execute);
    const static auto rTask_spawn = reinterpret_cast<FunctionTypes::rTask_spawn>(RBX::Studio::Offsets::rTask_spawn);
    const static auto rTask_defer = reinterpret_cast<FunctionTypes::rTask_defer>(RBX::Studio::Offsets::rTask_defer);
    const static auto rluaE_newthread = reinterpret_cast<FunctionTypes::rluaE_newthread>(RBX::Studio::Offsets::rluaE_newthread);
    const static auto rlua_newthread = reinterpret_cast<FunctionTypes::rlua_newthread>(RBX::Studio::Offsets::rlua_newthread);
    const static auto rbxAllocate = reinterpret_cast<FunctionTypes::rbxAllocate>(RBX::Studio::Offsets::rbxAllocate);
    const static auto rFromLuaState = reinterpret_cast<FunctionTypes::rFromLuaState>(RBX::Studio::Offsets::rFromLuaState);
    //  We don't require of Robloxs' luaC_step
    //  const static auto rLuaC_step = reinterpret_cast<FunctionTypes::rLuaC_step>(RBX::Studio::Offsets::rLuaC_step);
}

/*
 *  How to get this to compile when updating Luau?
 *      - Modify lobject.cpp and lobject.h to use Studios' luaO_nilobject, same thing with ltable.cpp and ltable.h and luaH_dummynode, as well as modifying lvm.cpp to use luau_execute. You must use luau_load when compiling code, for anyone using this to develop anything.
 */