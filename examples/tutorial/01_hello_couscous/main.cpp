#include <iostream>

// Max length of a ct-string(also this parameter specifies maximum length of a
// var/method name).
// Default value: 64 (if STRING_MAXLEN isn't defined). It's important to define
// it BEFORE include the reflection headers.
#define STRING_MAXLEN 128

// Ideal couscous' main header.
#include <reflect.hpp>

namespace couscous {
// Namespace related to all reflect functions.
inline namespace utils { using namespace ::reflect::utils; }
// Namespace related to all metaprogramming functions.
inline namespace metautils { using namespace ::reflect::metautils; }
// Ideal couscous' namespace.
inline namespace reflect { using namespace ::reflect; }
}

namespace hana = boost::hana;

struct InStructReflect {
  int var;
  void foo();
  void foo(int);
  static int bar(double);
  // In class meta information(also it means that the struct is reflected).
  IN_METAINFO(InStructReflect)
  // Reflect object variable
  REFLECT_OBJ_VAR(var)
  // Reflect object method (first param - method name, second - method input
  // types(nothing if method doesn't require input args)).
  REFLECT_OBJ_MTD(foo)
  // Overloaded method can be easily reflected too.
  REFLECT_OBJ_MTD(foo, int)
  // Reflect static method
  REFLECT_STATIC_MTD(bar, double)
};
// Note: You can easily reflect method prototype and use all reflection
// functions which doesn't call this method directly.
// Warning: volatile vars can't be reflected

struct OutStructReflect {
  static std::string str;
  auto baz() const { return 10; }
  // Out of class meta information
  OUT_METAINFO(OutStructReflect)
};
// Note: The same rules works with static vars (You can reflect them even if
// they're still uninitialized).

// Metainformation for OutStructReflect.
METAINFO(OutStructReflect)
// Reflect static var
REFLECT_STATIC_VAR(str)
// Reflect const method
REFLECT_CONST_OBJ_MTD(baz)
// End of OutStructReflect metainformation.
END_METAINFO
// Note: Out of class meta information must be used when you have methods with
// trailing return type.

// Not reflected struct.
struct NotReflected;

template <class T> struct InTemplateStruct {
  T var;
  T foo();
  // Template class/struct can also be reflected.
  IN_METAINFO(InTemplateStruct)
  REFLECT_OBJ_VAR(var)
  REFLECT_OBJ_MTD(foo)
};

template <class... Args> struct OutTemplateStruct {
  int var;
  void foo(Args &&...);
  OUT_METAINFO(OutTemplateStruct)
};

// Second parameter needed to define all templetes which will be used; third
// parameter - spetialization for metainformation.
TEMPLATE_METAINFO(OutTemplateStruct, class... Args, Args...)
REFLECT_OBJ_MTD(foo, Args &&...)
END_METAINFO

// Spetialize Metainformation for OutTemplateStruct<int>.
TEMPLATE_METAINFO(OutTemplateStruct, /*nothing*/, int)
REFLECT_OBJ_VAR(var)
END_METAINFO

int main() {
  // Check if type is reflected(returns boost::hana::bool_c<true/false>.
  std::cout << std::boolalpha << "InStructReflect reflected: "
            << couscous::check_reflected<InStructReflect>()
            << "\tOutStructReflect reflected: "
            << couscous::check_reflected<OutStructReflect>()
            << "\nNotReflected reflected: "
            << couscous::check_reflected<NotReflected>()
            << "\tint reflected: " << couscous::check_reflected<int>()
            << std::endl;
  //"class_name" returns boost::hana::optional of ct-string if type is
  // reflected, otherwise it returns boost::hana::nothing.
  constexpr auto something = couscous::class_name<InStructReflect>();
  std::cout << "Optional contains something: " << hana::is_just(something)
            << "\tOptional is empty: " << hana::is_nothing(something)
            << std::endl;
  constexpr auto nothing = couscous::class_name<NotReflected>();
  std::cout << "Optional contains something: " << hana::is_just(nothing)
            << "\tOptional is empty: " << hana::is_nothing(nothing)
            << std::endl;
  // get ct-string from optional and convert to const char*
  std::cout << "Class name: " << hana::to<const char *>(something.value())
            << std::endl;
  // The other way to "interact" with optionals is to use hana::transform(now it
  // returns optional of const char*).
  constexpr auto optional_char_str =
      hana::transform(something, hana::to<const char *>);
  // It will return boost::hana::nothing.
  constexpr auto still_nothing =
      hana::transform(nothing, hana::to<const char *>);
  std::cout << "Class name: " << optional_char_str.value() << std::endl;
  std::cout << "Optional is empty: " << hana::is_nothing(still_nothing)
            << std::endl;
  // Generators: responsible to select scope of
  // metainformation(all,vars/methods,obj vars/methods,static vars/methods,
  // const methods).
  // Function named "count" returns count of items in selected
  // generator(AllMethods,StaticVars).
  std::cout << "Count of all methods: "
            << couscous::get_opt_val(
                   couscous::count<InStructReflect, couscous::AllMethods>())
            << "\tCount of static variables: "
            << couscous::get_opt_val(
                   couscous::count<OutStructReflect, couscous::StaticVars>())
            << std::endl;
  // get_opt_val(t) - returns: t.value() if t - is_just == true, otherwise: t.
  // Metainformation spetialization example.
  std::cout << "Count of all methods: "
            << couscous::get_opt_val(
                   couscous::count<OutTemplateStruct<std::string>,
                                   couscous::AllMethods>())
            << std::endl;
  std::cout << "Count of all variables: "
            << couscous::get_opt_val(
                   couscous::count<OutTemplateStruct<int>, couscous::AllVars>())
            << std::endl;
  return 0;
}
