#ifndef REFL_TUPLE_TRAITS_HPP
#define REFL_TUPLE_TRAITS_HPP

#include "type_traits.hpp"
#include <boost/hana/concat.hpp>
#include <boost/hana/size.hpp>
#include <boost/hana/tuple.hpp>

namespace reflect {

namespace metautils {

/**
 * @brief Concatenating multiple tuples into one
 */
constexpr decltype(auto) multiple_concat() {
  return ::boost::hana::make_tuple();
}

template <class T> constexpr decltype(auto) multiple_concat(T &&value) {
  return ::boost::hana::concat(::std::forward<T>(value), multiple_concat());
}

/**
 * @brief Concating multiple tuples into one
 *
 * @param value boost::hana::tuple
 * @param args template pack of boost::hana::tuple
 * @return concatenated tuple
 */
template <class T, class... Args>
constexpr decltype(auto) multiple_concat(T &&value, Args &&... args) {
  return ::boost::hana::concat(::std::forward<T>(value),
                               multiple_concat(::std::forward<Args>(args)...));
}

namespace detail {

/**
 * @brief Compare types index implementation
 *
 * @param A type A
 * @param B type B
 * @return tuple of index if A and B same type or empty tuple
 */
template <long long Index, class A, class B>
constexpr decltype(auto) compare_types_index_impl(A &&, B &&) {
  if constexpr (::std::is_same_v<::std::decay_t<A>, ::std::decay_t<B>>)
    return ::boost::hana::tuple_c<long long, Index>;
  else
    return ::boost::hana::make_tuple();
}

/**
 * @brief Find same type of object in tuple implementation
 *
 * @param std::integer_sequence<long long,Indices...> index sequence
 * @param value object to find
 * @param tup tuple where to find
 */
template <class T, class Tp, long long... Indices>
constexpr decltype(auto)
find_values_args_impl(::std::integer_sequence<long long, Indices...> &&,
                      T &&value, Tp &&tup) {
  return metautils::multiple_concat(compare_types_index_impl<Indices>(
      ::std::forward<T>(value),
      ::boost::hana::at_c<Indices>(::std::forward<Tp>(tup)))...);
}

/**
 * @brief Tuple range of integral constants implementation
 *
 * @param std::nteger_sequence<long long,Indices...> index sequence
 */
template <long long Offset, long long... Indices>
constexpr decltype(auto)
gen_inds_tup_seq_impl(::std::integer_sequence<long long, Indices...> &&) {
  return ::boost::hana::tuple_c<long long, Indices + Offset...>;
}

/**
 * @brief Create a copy of the tuple by the given element indices
 * @param ::std::nteger_sequence<long long,Indices...> - index sequence
 * @param tup - tuple to copy
 * @param seq_tup - tuple of element indices
 */
template <long long... Indices, class Tuple, class IndexTuple>
constexpr decltype(auto)
copy_tuple_sequence_impl(::std::integer_sequence<long long, Indices...> &&,
                         Tuple &&tup, IndexTuple &&seq_tup) {
  return ::boost::hana::make_tuple(
      ::boost::hana::at(tup, ::boost::hana::at_c<Indices>(seq_tup))...);
}

/**
 * @brief Concating all tuples in one
 * @param ::std::nteger_sequence<long long,Indices...> - index sequence
 * @param tup - tuple of tuples
 */
template <long long... Indices, class Tuple>
constexpr decltype(auto)
merge_tuple_of_tuples_impl(::std::integer_sequence<long long, Indices...> &&,
                           Tuple &&tup) {
  return multiple_concat(::boost::hana::at_c<Indices>(tup)...);
}
} // namespace detail

/**
 * @brief compare types
 * @return true if types same, othervise false
 */
template <class A, class B>
constexpr decltype(auto) compare_types(A &&, B &&) noexcept {
  if constexpr (::std::is_same_v<::std::decay_t<A>, ::std::decay_t<B>>)
    return true;
  else
    return false;
}

/**
 * @brief Function that finds same type of object in tuple
 *
 * @param value object to find
 * @param tup tuple where to find
 * @return Tuple of integral constants, which are indices of tuple elements with
 * same type
 */
template <class T, class... Args>
constexpr decltype(auto) find_value_types(T &&value,
                                          ::boost::hana::tuple<Args...> &&tup) {
  return detail::find_values_args_impl(
      ::std::make_integer_sequence<long long, sizeof...(Args)>(),
      ::std::forward<T>(value),
      ::std::forward<::boost::hana::tuple<Args...>>(tup));
}

/**
 * @brief Function that finds same type of object in tuple
 *
 * @param value Object to find
 * @param tup Tuple where to find
 * @return tuple of integral constants, which are indices of tuple elements with
 * same type
 */
template <class T, class... Args>
constexpr decltype(auto)
find_value_types(T const &value, ::boost::hana::tuple<Args...> const &tup) {
  return detail::find_values_args_impl(
      ::std::make_integer_sequence<long long, sizeof...(Args)>(), value, tup);
}

/**
 * @brief Generate tuple of integral constants from [Offset; N+Offset)
 * @return Ruple of intergral constants
 */
template <class N, class Offset = ::boost::hana::llong<0>>
constexpr decltype(auto) gen_inds_tup() {
  return detail::gen_inds_tup_seq_impl<::std::decay_t<Offset>::value>(
      ::std::make_integer_sequence<long long, ::std::decay_t<N>::value>());
}

/**
 * @brief Create a copy of the tuple by the given element indices
 * @param tup - tuple to copy
 * @param seq_tup - tuple of element indices
 */
template <class Tuple, class IndexTuple>
constexpr decltype(auto) copy_tuple_sequence(Tuple &&tup,
                                             IndexTuple &&seq_tup) {
  return detail::copy_tuple_sequence_impl(
      ::std::make_integer_sequence<long long, decltype(::boost::hana::size(
                                                  seq_tup))::value>(),
      ::std::forward<Tuple>(tup), ::std::forward<IndexTuple>(seq_tup));
}

/**
 * @brief Concating all tuples in one
 * @param tup - tuple of tuples
 */
template <class Tuple>
constexpr decltype(auto) merge_tuple_of_tuples(Tuple &&tup) {
  return detail::merge_tuple_of_tuples_impl(
      ::std::make_integer_sequence<long long,
                                   decltype(::boost::hana::size(tup))::value>(),
      ::std::forward<Tuple>(tup));
}
} // namespace metautils
} // namespace reflect

#endif // TUPLE_TRAITS_HPP
