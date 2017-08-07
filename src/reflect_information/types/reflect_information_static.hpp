#ifndef REFL_INFORMATION_STATIC_COMMON_HPP
#define REFL_INFORMATION_STATIC_COMMON_HPP

#include "../../meta_utils/meta_utils.hpp"

namespace reflect {

namespace info {

/**
 * @brief SFINAE check if type is pointer to static field
 *
 */
template <class T> class is_static {

  template <class C>
  static constexpr ::std::true_type check(decltype(&C::is_static));
  template <class> static constexpr ::std::false_type check(...);

public:
  static constexpr bool value =
      ::std::is_same<::std::true_type, decltype(check<T>(nullptr))>::value;
};

template <class T>
constexpr bool is_static_v =
    is_static<T>::value; /**< Helper variable template for is_static */

/**
 * @brief The StaticIndexGenerator class - generate indices where static fields
 * are located
 */
template <class ParentGenerator, bool condition = true>
class StaticIndexGenerator final {
  template <class Item, long long Index>
  constexpr static decltype(auto) check_metadata_variable() {
    if
      constexpr(is_static_v<::std::decay_t<Item>> == condition) return ::boost::
          hana::make_tuple(::boost::hana::llong_c<Index>);
    else
      return ::boost::hana::make_tuple();
  }

  template <class Tuple, long long... Indices>
  constexpr static decltype(auto) generate_impl(
      const ::boost::hana::tuple<::boost::hana::llong<Indices>...> &) {
    return metautils::multiple_concat(
        check_metadata_variable<decltype(::boost::hana::at_c<Indices>(
                                    ::std::declval<Tuple>())),
                                Indices>()...);
  }

public:
  using reverse = StaticIndexGenerator<ParentGenerator,
                                       !condition>; /**< Reverse generator */

  /**
   * @brief generate function
   * @return ::boost::hana::tuple of indices
   */
  template <class Tuple> constexpr static decltype(auto) generate() {
    return generate_impl<Tuple>(ParentGenerator::template generate<Tuple>());
  }
};
}
}

#endif // REFLECT_INFORMATION_STATIC_COMMON_HPP
