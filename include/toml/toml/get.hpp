//     Copyright Toru Niina 2017.
// Distributed under the MIT License.
#ifndef TOML11_GET_HPP
#define TOML11_GET_HPP
#include "from.hpp"
#include "result.hpp"
#include "value.hpp"
#include <algorithm>

namespace toml
{

// ============================================================================
// exact toml::* type

template<typename T, typename std::enable_if<
    detail::is_exact_toml_type<T>::value, std::nullptr_t>::type = nullptr>
inline T& get(value& v)
{
    return v.cast<detail::toml_value_t<T>::value>();
}

template<typename T, typename std::enable_if<
    detail::is_exact_toml_type<T>::value, std::nullptr_t>::type = nullptr>
inline T const& get(const value& v)
{
    return v.cast<detail::toml_value_t<T>::value>();
}

template<typename T, typename std::enable_if<
    detail::is_exact_toml_type<T>::value, std::nullptr_t>::type = nullptr>
inline T&& get(value&& v)
{
    return std::move(v.cast<detail::toml_value_t<T>::value>());
}

// ============================================================================
// T == toml::value; identity transformation.

template<typename T, typename std::enable_if<
    std::is_same<T, ::toml::value>::value, std::nullptr_t>::type = nullptr>
inline T& get(value& v)
{
    return v;
}

template<typename T, typename std::enable_if<
    std::is_same<T, ::toml::value>::value, std::nullptr_t>::type = nullptr>
inline T const& get(const value& v)
{
    return v;
}

template<typename T, typename std::enable_if<
    std::is_same<T, ::toml::value>::value, std::nullptr_t>::type = nullptr>
inline T&& get(value&& v)
{
    return std::move(v);
}

// ============================================================================
// integer convertible from toml::Integer

template<typename T, typename std::enable_if<detail::conjunction<
    std::is_integral<T>,                            // T is integral
    detail::negation<std::is_same<T, bool>>,        // but not bool
    detail::negation<detail::is_exact_toml_type<T>> // but not toml::integer
    >::value, std::nullptr_t>::type = nullptr>
inline T get(const value& v)
{
    return static_cast<T>(v.cast<value_t::Integer>());
}

// ============================================================================
// floating point convertible from toml::Float

template<typename T, typename std::enable_if<detail::conjunction<
    std::is_floating_point<T>,                      // T is floating_point
    detail::negation<detail::is_exact_toml_type<T>> // but not toml::Float
    >::value, std::nullptr_t>::type = nullptr>
inline T get(const value& v)
{
    return static_cast<T>(v.cast<value_t::Float>());
}

// ============================================================================
// std::string; toml uses its own toml::string, but it should be convertible to
// std::string seamlessly

template<typename T, typename std::enable_if<
    std::is_same<T, std::string>::value, std::nullptr_t>::type = nullptr>
inline std::string& get(value& v)
{
    return v.cast<value_t::String>().str;
}

template<typename T, typename std::enable_if<
    std::is_same<T, std::string>::value, std::nullptr_t>::type = nullptr>
inline std::string const& get(const value& v)
{
    return v.cast<value_t::String>().str;
}

template<typename T, typename std::enable_if<
    std::is_same<T, std::string>::value, std::nullptr_t>::type = nullptr>
inline std::string get(value&& v)
{
    return std::move(v.cast<value_t::String>().str);
}

// ============================================================================
// std::string_view

#if __cplusplus >= 201703L
template<typename T, typename std::enable_if<
    std::is_same<T, std::string_view>::value, std::nullptr_t>::type = nullptr>
inline std::string_view get(const value& v)
{
    return std::string_view(v.cast<value_t::String>().str);
}
#endif

// ============================================================================
// std::chrono::duration from toml::local_time.

template<typename T, typename std::enable_if<
    detail::is_chrono_duration<T>::value, std::nullptr_t>::type = nullptr>
inline T get(const value& v)
{
    return std::chrono::duration_cast<T>(
            std::chrono::nanoseconds(v.cast<value_t::LocalTime>()));
}

// ============================================================================
// std::chrono::system_clock::time_point from toml::datetime variants

template<typename T, typename std::enable_if<
    std::is_same<std::chrono::system_clock::time_point, T>::value,
    std::nullptr_t>::type = nullptr>
inline T get(const value& v)
{
    switch(v.type())
    {
        case value_t::LocalDate:
        {
            return std::chrono::system_clock::time_point(
                    v.cast<value_t::LocalDate>());
        }
        case value_t::LocalDatetime:
        {
            return std::chrono::system_clock::time_point(
                    v.cast<value_t::LocalDatetime>());
        }
        default:
        {
            return std::chrono::system_clock::time_point(
                    v.cast<value_t::OffsetDatetime>());
        }
    }
}

// ============================================================================
// forward declaration to use this recursively. ignore this and go ahead.

template<typename T, typename std::enable_if<detail::conjunction<
    detail::is_container<T>,                        // T is container
    detail::has_resize_method<T>,                   // T::resize(N) works
    detail::negation<detail::is_exact_toml_type<T>> // but not toml::array
    >::value, std::nullptr_t>::type = nullptr>
T get(const value& v);
template<typename T, typename std::enable_if<detail::conjunction<
    detail::is_container<T>,                        // T is container
    detail::negation<detail::has_resize_method<T>>, // no T::resize() exists
    detail::negation<detail::is_exact_toml_type<T>> // not toml::array
    >::value, std::nullptr_t>::type = nullptr>
T get(const value& v);
template<typename T, typename std::enable_if<
    detail::is_std_pair<T>::value, std::nullptr_t>::type = nullptr>
T get(const value& v);
template<typename T, typename std::enable_if<
    detail::is_std_tuple<T>::value, std::nullptr_t>::type = nullptr>
T get(const value& v);
template<typename T, typename std::enable_if<detail::conjunction<
    detail::is_map<T>,                              // T is map
    detail::negation<detail::is_exact_toml_type<T>> // but not toml::table
    >::value, std::nullptr_t>::type = nullptr>
T get(const toml::value& v);

template<typename T, typename std::enable_if<detail::conjunction<
    detail::negation<detail::is_exact_toml_type<T>>, // not a toml::value
    detail::has_from_toml_method<T>, // but has from_toml(toml::value) memfn
    std::is_default_constructible<T> // and default constructible
    >::value, std::nullptr_t>::type = nullptr>
T get(const toml::value& v);

template<typename T, typename std::enable_if<detail::conjunction<
    detail::negation<detail::is_exact_toml_type<T>> // not a toml::value
    >::value, std::nullptr_t>::type = nullptr,
    std::size_t = sizeof(::toml::from<T>) // and has from<T> specialization
    >
T get(const toml::value& v);

// ============================================================================
// array-like types; most likely STL container, like std::vector, etc.

template<typename T, typename std::enable_if<detail::conjunction<
    detail::is_container<T>,                        // T is container
    detail::has_resize_method<T>,                   // T::resize(N) works
    detail::negation<detail::is_exact_toml_type<T>> // but not toml::array
    >::value, std::nullptr_t>::type>
T get(const value& v)
{
    using value_type = typename T::value_type;
    const auto& ar = v.cast<value_t::Array>();

    T container; container.resize(ar.size());
    std::transform(ar.cbegin(), ar.cend(), container.begin(),
                   [](const value& x){return ::toml::get<value_type>(x);});
    return container;
}

// ============================================================================
// array-like types; but does not have resize(); most likely std::array.

template<typename T, typename std::enable_if<detail::conjunction<
    detail::is_container<T>,                        // T is container
    detail::negation<detail::has_resize_method<T>>, // no T::resize() exists
    detail::negation<detail::is_exact_toml_type<T>> // not toml::array
    >::value, std::nullptr_t>::type>
T get(const value& v)
{
    using value_type = typename T::value_type;
    const auto& ar = v.cast<value_t::Array>();

    T container;
    if(ar.size() != container.size())
    {
        throw std::out_of_range(detail::format_underline(concat_to_string(
            "[erorr] toml::get specified container size is ", container.size(),
            " but there are ", ar.size(), " elements in toml array."), {
                {std::addressof(detail::get_region(v)), "here"}
            }));
    }
    std::transform(ar.cbegin(), ar.cend(), container.begin(),
                   [](const value& x){return ::toml::get<value_type>(x);});
    return container;
}

// ============================================================================
// std::pair.

template<typename T, typename std::enable_if<
    detail::is_std_pair<T>::value, std::nullptr_t>::type>
T get(const value& v)
{
    using first_type  = typename T::first_type;
    using second_type = typename T::second_type;

    const auto& ar = v.cast<value_t::Array>();
    if(ar.size() != 2)
    {
        throw std::out_of_range(detail::format_underline(concat_to_string(
            "[erorr] toml::get specified std::pair but there are ", ar.size(),
            " elements in toml array."), {
                {std::addressof(detail::get_region(v)), "here"}
            }));
    }
    return std::make_pair(::toml::get<first_type >(ar.at(0)),
                          ::toml::get<second_type>(ar.at(1)));
}

// ============================================================================
// std::tuple.

namespace detail
{

template<typename T, std::size_t ...I>
T get_tuple_impl(const toml::Array& a, index_sequence<I...>)
{
    return std::make_tuple(
        ::toml::get<typename std::tuple_element<I, T>::type>(a.at(I))...);
}

} // detail

template<typename T, typename std::enable_if<
    detail::is_std_tuple<T>::value, std::nullptr_t>::type>
T get(const value& v)
{
    const auto& ar = v.cast<value_t::Array>();
    if(ar.size() != std::tuple_size<T>::value)
    {
        throw std::out_of_range(detail::format_underline(concat_to_string(
            "[erorr] toml::get specified std::tuple with ",
            std::tuple_size<T>::value, "elements, but there are ", ar.size(),
            " elements in toml array."), {
                {std::addressof(detail::get_region(v)), "here"}
            }));
    }
    return detail::get_tuple_impl<T>(ar,
            detail::make_index_sequence<std::tuple_size<T>::value>{});
}

// ============================================================================
// map-like types; most likely STL map, like std::map or std::unordered_map.

template<typename T, typename std::enable_if<detail::conjunction<
    detail::is_map<T>,                              // T is map
    detail::negation<detail::is_exact_toml_type<T>> // but not toml::table
    >::value, std::nullptr_t>::type>
T get(const toml::value& v)
{
    using key_type    = typename T::key_type;
    using mapped_type = typename T::mapped_type;
    static_assert(std::is_convertible<std::string, key_type>::value,
                  "toml::get only supports map type of which key_type is "
                  "convertible from std::string.");
    T map;
    for(const auto& kv : v.cast<value_t::Table>())
    {
        map[key_type(kv.first)] = ::toml::get<mapped_type>(kv.second);
    }
    return map;
}


// ============================================================================
// user-defined, but compatible types.

template<typename T, typename std::enable_if<detail::conjunction<
    detail::negation<detail::is_exact_toml_type<T>>, // not a toml::value
    detail::has_from_toml_method<T>, // but has from_toml(toml::value) memfn
    std::is_default_constructible<T> // and default constructible
    >::value, std::nullptr_t>::type>
T get(const toml::value& v)
{
    T ud;
    ud.from_toml(v);
    return ud;
}
template<typename T, typename std::enable_if<detail::conjunction<
    detail::negation<detail::is_exact_toml_type<T>> // not a toml::value
    >::value, std::nullptr_t>::type, std::size_t>   // and has from<T>
T get(const toml::value& v)
{
    return ::toml::from<T>::from_toml(v);
}

// ============================================================================
// find and get

template<typename T = ::toml::value>
decltype(::toml::get<T>(std::declval<const ::toml::value&>()))
find(const toml::table& tab, const toml::key& ky,
     std::string tablename = "unknown table")
{
    if(tab.count(ky) == 0)
    {
        throw std::out_of_range(concat_to_string("[error] key \"", ky,
                    "\" not found in ", tablename));
    }
    return ::toml::get<T>(tab.at(ky));
}
template<typename T = ::toml::value>
decltype(::toml::get<T>(std::declval<::toml::value&>()))
find(toml::table& tab, const toml::key& ky,
     std::string tablename = "unknown table")
{
    if(tab.count(ky) == 0)
    {
        throw std::out_of_range(concat_to_string("[error] key \"", ky,
                    "\" not found in ", tablename));
    }
    return ::toml::get<T>(tab[ky]);
}
template<typename T = ::toml::value>
decltype(::toml::get<T>(std::declval<::toml::value&&>()))
find(toml::table&& tab, const toml::key& ky,
     std::string tablename = "unknown table")
{
    if(tab.count(ky) == 0)
    {
        throw std::out_of_range(concat_to_string("[error] key \"", ky,
                    "\" not found in ", tablename));
    }
    return ::toml::get<T>(std::move(tab[ky]));
}

template<typename T = ::toml::value>
decltype(::toml::get<T>(std::declval<const ::toml::value&>()))
find(const toml::value& v, const toml::key& ky)
{
    const auto& tab = ::toml::get<toml::table>(v);
    if(tab.count(ky) == 0)
    {
        throw std::out_of_range(detail::format_underline(concat_to_string(
            "[error] key \"", ky, "\" not found"), {
                {std::addressof(detail::get_region(v)), "in this table"}
            }));
    }
    return ::toml::get<T>(tab.at(ky));
}
template<typename T = ::toml::value>
decltype(::toml::get<T>(std::declval<::toml::value&>()))
find(toml::value& v, const toml::key& ky)
{
    auto& tab = ::toml::get<toml::table>(v);
    if(tab.count(ky) == 0)
    {
        throw std::out_of_range(detail::format_underline(concat_to_string(
            "[error] key \"", ky, "\" not found"), {
                {std::addressof(detail::get_region(v)), "in this table"}
            }));
    }
    return ::toml::get<T>(tab.at(ky));
}
template<typename T = ::toml::value>
decltype(::toml::get<T>(std::declval<::toml::value&&>()))
find(toml::value&& v, const toml::key& ky)
{
    auto tab = ::toml::get<toml::table>(std::move(v));
    if(tab.count(ky) == 0)
    {
        throw std::out_of_range(detail::format_underline(concat_to_string(
            "[error] key \"", ky, "\" not found"), {
                {std::addressof(detail::get_region(v)), "in this table"}
            }));
    }
    return ::toml::get<T>(std::move(tab[ky]));
}


// ============================================================================
// get_or(value, fallback)

// ----------------------------------------------------------------------------
// specialization for the exact toml types (return type becomes lvalue ref)

template<typename T, typename std::enable_if<
    detail::is_exact_toml_type<T>::value, std::nullptr_t>::type = nullptr>
T const& get_or(const toml::value& v, const T& opt)
{
    try
    {
        return get<typename std::remove_cv<
            typename std::remove_reference<T>::type>::type>(v);
    }
    catch(...)
    {
        return opt;
    }
}
template<typename T, typename std::enable_if<
    detail::is_exact_toml_type<T>::value, std::nullptr_t>::type = nullptr>
T& get_or(toml::value& v, T& opt)
{
    try
    {
        return get<typename std::remove_cv<
            typename std::remove_reference<T>::type>::type>(v);
    }
    catch(...)
    {
        return opt;
    }
}
template<typename T, typename std::enable_if<
    detail::is_exact_toml_type<T>::value, std::nullptr_t>::type = nullptr>
T&& get_or(toml::value&& v, T&& opt)
{
    try
    {
        return get<typename std::remove_cv<
            typename std::remove_reference<T>::type>::type>(v);
    }
    catch(...)
    {
        return opt;
    }
}

// ----------------------------------------------------------------------------
// specialization for std::string (return type becomes lvalue ref)

template<typename T, typename std::enable_if<
    std::is_same<T, std::string>::value, std::nullptr_t>::type = nullptr>
std::string const& get_or(const toml::value& v, const T& opt)
{
    try
    {
        return get<std::string>(v);
    }
    catch(...)
    {
        return opt;
    }
}
template<typename T, typename std::enable_if<
    std::is_same<T, std::string>::value, std::nullptr_t>::type = nullptr>
std::string& get_or(toml::value& v, T& opt)
{
    try
    {
        return get<std::string>(v);
    }
    catch(...)
    {
        return opt;
    }
}
template<typename T, typename std::enable_if<
    std::is_same<T, std::string>::value, std::nullptr_t>::type = nullptr>
std::string get_or(toml::value&& v, T&& opt)
{
    try
    {
        return get<std::string>(v);
    }
    catch(...)
    {
        return opt;
    }
}
template<typename T, typename std::enable_if<
    detail::is_string_literal<typename std::remove_reference<T>::type>::value,
    std::nullptr_t>::type = nullptr>
std::string get_or(const toml::value& v, T&& opt)
{
    try
    {
        return get<std::string>(v);
    }
    catch(...)
    {
        return std::string(opt);
    }
}

// ----------------------------------------------------------------------------
// others (require type conversion and return type cannot be lvalue reference)

template<typename T, typename std::enable_if<detail::conjunction<
    detail::negation<detail::is_exact_toml_type<T>>,
    detail::negation<std::is_same<T, std::string>>,
    detail::negation<detail::is_string_literal<typename std::remove_reference<T>::type>>
    >::value, std::nullptr_t>::type = nullptr>
T get_or(const toml::value& v, T&& opt)
{
    try
    {
        return get<typename std::remove_cv<
            typename std::remove_reference<T>::type>::type>(v);
    }
    catch(...)
    {
        return opt;
    }
}

// ===========================================================================
// find_or(value, key, fallback)

// ---------------------------------------------------------------------------
// exact types (return type can be a reference)
template<typename T, typename std::enable_if<
    detail::is_exact_toml_type<T>::value, std::nullptr_t>::type = nullptr>
T const& find_or(const toml::value& v, const toml::key& ky, const T& opt)
{
    if(!v.is_table()) {return opt;}
    const auto& tab = toml::get<toml::table>(v);
    if(tab.count(ky) == 0) {return opt;}
    return get_or(tab.at(ky), opt);
}

template<typename T, typename std::enable_if<
    detail::is_exact_toml_type<T>::value, std::nullptr_t>::type = nullptr>
T& find_or(toml::value& v, const toml::key& ky, T& opt)
{
    if(!v.is_table()) {return opt;}
    auto& tab = toml::get<toml::table>(v);
    if(tab.count(ky) == 0) {return opt;}
    return get_or(tab[ky], opt);
}

template<typename T, typename std::enable_if<
    detail::is_exact_toml_type<T>::value, std::nullptr_t>::type = nullptr>
T&& find_or(toml::value&& v, const toml::key& ky, T&& opt)
{
    if(!v.is_table()) {return opt;}
    auto tab = toml::get<toml::table>(std::move(v));
    if(tab.count(ky) == 0) {return opt;}
    return get_or(std::move(tab[ky]), std::forward<T>(opt));
}

// ---------------------------------------------------------------------------
// std::string (return type can be a reference)
template<typename T, typename std::enable_if<
    std::is_same<T, std::string>::value, std::nullptr_t>::type = nullptr>
std::string const& find_or(const toml::value& v, const toml::key& ky, const T& opt)
{
    if(!v.is_table()) {return opt;}
    const auto& tab = toml::get<toml::table>(v);
    if(tab.count(ky) == 0) {return opt;}
    return get_or(tab.at(ky), opt);
}
template<typename T, typename std::enable_if<
    std::is_same<T, std::string>::value, std::nullptr_t>::type = nullptr>
std::string& find_or(toml::value& v, const toml::key& ky, T& opt)
{
    if(!v.is_table()) {return opt;}
    auto& tab = toml::get<toml::table>(v);
    if(tab.count(ky) == 0) {return opt;}
    return get_or(tab[ky], opt);
}
template<typename T, typename std::enable_if<
    std::is_same<T, std::string>::value, std::nullptr_t>::type = nullptr>
std::string find_or(toml::value&& v, const toml::key& ky, T&& opt)
{
    if(!v.is_table()) {return opt;}
    auto tab = toml::get<toml::table>(std::move(v));
    if(tab.count(ky) == 0) {return opt;}
    return get_or(std::move(tab[ky]), std::forward<T>(opt));
}

// ---------------------------------------------------------------------------
// string literal (deduced as std::string)
template<typename T, typename std::enable_if<
    detail::is_string_literal<typename std::remove_reference<T>::type>::value,
    std::nullptr_t>::type = nullptr>
std::string find_or(const toml::value& v, const toml::key& ky, T&& opt)
{
    if(!v.is_table()) {return opt;}
    const auto& tab = toml::get<toml::table>(v);
    if(tab.count(ky) == 0) {return std::string(opt);}
    return get_or(tab.at(ky), std::forward<T>(opt));
}

// ---------------------------------------------------------------------------
// others (require type conversion and return type cannot be lvalue reference)
template<typename T, typename std::enable_if<detail::conjunction<
    detail::negation<detail::is_exact_toml_type<T>>,
    detail::negation<std::is_same<T, std::string>>,
    detail::negation<detail::is_string_literal<typename std::remove_reference<T>::type>>
    >::value, std::nullptr_t>::type = nullptr>
T find_or(const toml::value& v, const toml::key& ky, T&& opt)
{
    if(!v.is_table()) {return opt;}
    const auto& tab = toml::get<toml::table>(v);
    if(tab.count(ky) == 0) {return opt;}
    return get_or(tab.at(ky), std::forward<T>(opt));
}

// ===========================================================================
// find_or(table, key, opt)

// ---------------------------------------------------------------------------
// exact types (return type can be a reference)
template<typename T, typename std::enable_if<
    detail::is_exact_toml_type<T>::value, std::nullptr_t>::type = nullptr>
T const& find_or(const toml::table& tab, const toml::key& ky, const T& opt)
{
    if(tab.count(ky) == 0) {return opt;}
    return get_or(tab.at(ky), opt);
}

template<typename T, typename std::enable_if<
    detail::is_exact_toml_type<T>::value, std::nullptr_t>::type = nullptr>
T& find_or(toml::table& tab, const toml::key& ky, T& opt)
{
    if(tab.count(ky) == 0) {return opt;}
    return get_or(tab[ky], opt);
}

template<typename T, typename std::enable_if<
    detail::is_exact_toml_type<T>::value, std::nullptr_t>::type = nullptr>
T&& find_or(toml::table&& tab, const toml::key& ky, T&& opt)
{
    if(tab.count(ky) == 0) {return opt;}
    return get_or(std::move(tab[ky]), std::forward<T>(opt));
}

// ---------------------------------------------------------------------------
// std::string (return type can be a reference)
template<typename T, typename std::enable_if<
    std::is_same<T, std::string>::value, std::nullptr_t>::type = nullptr>
std::string const& find_or(const toml::table& tab, const toml::key& ky, const T& opt)
{
    if(tab.count(ky) == 0) {return opt;}
    return get_or(tab.at(ky), opt);
}
template<typename T, typename std::enable_if<
    std::is_same<T, std::string>::value, std::nullptr_t>::type = nullptr>
std::string& find_or(toml::table& tab, const toml::key& ky, T& opt)
{
    if(tab.count(ky) == 0) {return opt;}
    return get_or(tab[ky], opt);
}
template<typename T, typename std::enable_if<
    std::is_same<T, std::string>::value, std::nullptr_t>::type = nullptr>
std::string find_or(toml::table&& tab, const toml::key& ky, T&& opt)
{
    if(tab.count(ky) == 0) {return opt;}
    return get_or(std::move(tab[ky]), std::forward<T>(opt));
}

// ---------------------------------------------------------------------------
// string literal (deduced as std::string)
template<typename T, typename std::enable_if<
    detail::is_string_literal<typename std::remove_reference<T>::type>::value,
    std::nullptr_t>::type = nullptr>
std::string find_or(const toml::table& tab, const toml::key& ky, T&& opt)
{
    if(tab.count(ky) == 0) {return std::string(opt);}
    return get_or(tab.at(ky), std::forward<T>(opt));
}

// ---------------------------------------------------------------------------
// others (require type conversion and return type cannot be lvalue reference)
template<typename T, typename std::enable_if<detail::conjunction<
    detail::negation<detail::is_exact_toml_type<T>>,
    detail::negation<std::is_same<T, std::string>>,
    detail::negation<detail::is_string_literal<typename std::remove_reference<T>::type>>
    >::value, std::nullptr_t>::type = nullptr>
T find_or(const toml::table& tab, const toml::key& ky, T&& opt)
{
    if(tab.count(ky) == 0) {return opt;}
    return get_or(tab.at(ky), std::forward<T>(opt));
}

// ============================================================================
// expect

template<typename T>
result<T, std::string> expect(const toml::value& v) noexcept
{
    try
    {
        return ok(get<T>(v));
    }
    catch(const std::exception& e)
    {
        return err(e.what());
    }
}
template<typename T>
result<T, std::string> expect(const toml::value& v, const toml::key& k) noexcept
{
    try
    {
        return ok(find<T>(v, k));
    }
    catch(const std::exception& e)
    {
        return err(e.what());
    }
}
template<typename T>
result<T, std::string> expect(const toml::table& t, const toml::key& k,
        std::string tablename = "unknown table") noexcept
{
    try
    {
        return ok(find<T>(t, k, std::move(tablename)));
    }
    catch(const std::exception& e)
    {
        return err(e.what());
    }
}

} // toml
#endif// TOML11_GET
