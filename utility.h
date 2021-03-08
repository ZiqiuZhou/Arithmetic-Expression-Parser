#ifndef INCLUDED_CPP_PRACTICE_EX08_UTILITY_H_
#define INCLUDED_CPP_PRACTICE_EX08_UTILITY_H_


namespace asc::cpp_practice_ws20::ex08 {


    // Helper class for overloading with lambdas.
    template <typename... Ts> struct overload : Ts... { using Ts::operator()...; };
    template <typename... Ts> overload(Ts...)->overload<Ts...>;


} // namespace asc::cpp_practice_ws20::ex08


#endif // INCLUDED_CPP_PRACTICE_EX08_UTILITY_H_
