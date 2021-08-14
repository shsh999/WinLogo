#pragma once

namespace winlogo::hooks {
namespace details {

template<auto hookFunction, auto originalFunction, typename Func>
struct HookedFunctionInner;

template<auto hookFunction, auto originalFunction, typename OriginalRet, typename... OriginalArgs>
struct HookedFunctionInner<hookFunction, originalFunction,
                           OriginalRet(__stdcall*)(OriginalArgs...)> {
    // This is defined as non-constexpr in order to get the original value dynamically.
    static const decltype(originalFunction) originalPtr;

    static OriginalRet __stdcall call(OriginalArgs... args) {
        return hookFunction(originalPtr, args...);
    }
};

// Initialize the static originalPtr.
template<auto hookFunction, auto originalFunction, typename OriginalRet, typename... OriginalArgs>
const decltype(originalFunction) HookedFunctionInner<
    hookFunction, originalFunction, OriginalRet(__stdcall*)(OriginalArgs...)>::originalPtr =
    originalFunction;

/**
 * This static class is used to generate functions used for hooking.
 * It is assumed that the type of the hook function is the same as the type of the original
 * function, except for receiving a function in the first paramter. The resulting
 * HookedFunction::call function is a function with the same interface as the original function
 * (calling convention, return value, arguments), that calls the hook function with the original
 * function as the first parameter, and the rest of the parameters afterwards.
 */
template<auto originalFunction, auto hookFunction>
struct HookedFunctionStruct : public details::HookedFunctionInner<hookFunction, originalFunction,
                                                                  decltype(originalFunction)> {
    // Intentionally left blank.
};

}  // namespace details

/**
 * Get the pointer to an intermediate hook function with the same signature as originalFunction,
 * used to hook originalFunction into hookFunction (a function getting originalFunction as its first
 * paramter, and the rest of the parameters are the same as originalFunction's).
 */
template<auto originalFunction, auto hookFunction>
inline constexpr auto HookedFunction =
    details::HookedFunctionStruct<originalFunction, hookFunction>::call;

}  // namespace winlogo::hooks
