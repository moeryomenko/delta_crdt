#ifndef CRDT_TRAITS_H
#define CRDT_TRAITS_H

#include <concepts>
#include <functional>
#include <type_traits>

namespace crdt {

template <typename container_type, typename _key, typename _mapped_type>
concept associative_type =
    std::is_same_v<_key, typename container_type::key_type> &&
    std::is_same_v<_mapped_type, typename container_type::mapped_type> &&
    requires(container_type t, typename container_type::key_type key) {
      typename container_type::key_type;
      typename container_type::mapped_type;
      { t[key] } -> std::convertible_to<typename container_type::mapped_type>;
    };

template <typename T, typename _key>
concept iterable_type =
    std::is_same_v<_key, typename T::key_type> &&
    requires(T t, typename T::iterator it, typename T::iterator endIt,
             const typename T::key_type &key) {
      typename T::key_type;
      typename T::iterator;
      typename T::const_iterator;
      { t.begin() } -> std::convertible_to<typename T::iterator>;
      { t.cbegin() } -> std::convertible_to<typename T::const_iterator>;
      { t.end() } -> std::convertible_to<typename T::iterator>;
      { t.cend() } -> std::convertible_to<typename T::const_iterator>;
      { t.erase(it) } -> std::convertible_to<typename T::iterator>;
      { t.insert(it, endIt) } -> std::convertible_to<void>;
      { t.find(key) } -> std::convertible_to<typename T::iterator>;
      { t.contains(key) } -> std::convertible_to<bool>;
    };

template <typename T, typename _key>
concept set_type = iterable_type<T, _key>;

template <typename container_type, typename _key, typename _mapped_type>
concept iterable_assiative_type =
    associative_type<container_type, _key, _mapped_type> &&
    iterable_type<container_type, _key>;

template <typename T>
concept value_type =
    std::default_initializable<T> && std::equality_comparable<T>;

template <typename F, typename T>
concept hashable = std::regular_invocable<F, T> &&
                   std::convertible_to<std::invoke_result_t<F, T>, size_t>;

template <typename T, typename F = std::hash<T>>
concept actor_type = value_type<T> && std::copyable<T> && hashable<F, T>;

template <typename T>
concept incrementable_type = requires(T a) {
  { a++ } -> std::convertible_to<T>;
  { ++a } -> std::convertible_to<T>;
  { a + 1 } -> std::convertible_to<T>;
};

template <typename T>
concept cvrdt = requires(T a, T b) {
  { a.merge(b) };
};

} // namespace crdt.

template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

#endif // CRDT_TRAITS_H
