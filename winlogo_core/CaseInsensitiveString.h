/**
 * This file implements a case insensitive version of string and string_view.
 */
#pragma once
#include <string>
#include <iostream>
#include <cctype>
#include <cwctype>
#include <algorithm>

namespace winlogo::utils {

template<typename CharType>
typename std::char_traits<CharType>::int_type toUpper(const CharType ch) {
    static_assert(false, "toUpper must be implemented for this character type!");
}

template<>
inline int toUpper(char ch) {
    return std::toupper(ch);
}

template<>
inline wint_t toUpper(const wchar_t ch) {
    return std::towupper(ch);
}

/**
 * Case-insensitive char traits for use by the ci_string, ci_string_view classes.
 */
template<typename CharType>
struct ci_char_traits : public std::char_traits<CharType> {
    using std::char_traits<CharType>::int_type;

    static constexpr bool eq(const CharType c1, const CharType c2) {
        return toUpper(c1) == toUpper(c2);
    }

    static constexpr bool lt(const CharType c1, const CharType c2) {
        return toUpper(c1) < toUpper(c2);
    }

    static constexpr int compare(const CharType* s1, const CharType* s2, size_t size) {
        for (; size != 0; --size, ++s1, ++s2) {
            auto upper1 = toUpper(*s1);
            auto upper2 = toUpper(*s2);
            if (upper1 < upper2) {
                return -1;
            }
            if (upper1 > upper2) {
                return 1;
            }
        }

        return 0;
    }

    static const CharType* find(const CharType* str, int size, CharType valueToFind) {
        auto result =
            std::find_if(str, str + size, [upperValueToFind = toUpper(valueToFind)](CharType chr) {
                return toUpper(chr) == upperValueToFind;
            });

        if (result == str + size) {
            return nullptr;
        }
        return result;
    }
};

// string versions

template<typename CharType>
using basic_ci_string = std::basic_string<CharType, ci_char_traits<CharType>>;

using ci_string = basic_ci_string<char>;
using ci_wstring = basic_ci_string<wchar_t>;

// string_view versions

template<typename CharType>
using basic_ci_string_view = std::basic_string_view<CharType, ci_char_traits<CharType>>;

using ci_string_view = basic_ci_string_view<char>;
using ci_wstring_view = basic_ci_string_view<wchar_t>;

// Output operators

template<typename CharType>
std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>& os,
                                         const basic_ci_string<CharType>& str) {
    return os.write(str.data(), str.size());
}

template<typename CharType>
std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>& os,
                                         const basic_ci_string_view<CharType>& str) {
    return os.write(str.data(), str.size());
}

}  // namespace winlogo::utils
