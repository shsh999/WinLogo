#pragma once

namespace winlogo::utils {

/**
 * This is a version of the std::find_if algorithm that supports a sentinel type as the end
 * iterator.
 */
template<typename InputIterator, typename Sentinel, typename Predicate>
InputIterator find_if(InputIterator first, Sentinel last, Predicate predicate) {
    for (; first != last; ++first) {
        if (predicate(*first)) {
            break;
        }
    }
    return first;
}

}  // namespace winlogo::utils
