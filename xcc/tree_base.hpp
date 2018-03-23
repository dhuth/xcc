/*
 * tree.hpp
 *
 *  Created on: Jan 19, 2017
 *      Author: derick
 */

#ifndef XCC_TREE_BASE_HPP_
#define XCC_TREE_BASE_HPP_

#include <cassert>
#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <iterator>
#include <initializer_list>
#include <functional>
#include <map>
#include <type_traits>
#include <vector>

#include "cppdefs.hpp"
#include "cpp_type_traits_ext.hpp"
#include "managed_ptr.hpp"


namespace xcc {

template<typename T>
using ptr = managed_ptr<T>;


/* ==================== *
 * Forward declarations *
 * ==================== */

struct tree_type;
struct __tree_base;

typedef __tree_base             tree_t;

typedef                         __tree_base*(*__tree_clone_func_t)(const __tree_base*);

#ifdef TREE_TYPE
#error "TREE_TYPE Already defined"
#endif
#define TREE_TYPE(name, ...)                                    struct name;
#include "all_tree_types.def.hpp"
#undef  TREE_TYPE

#ifdef TREE_TYPE
#error "TREE_TYPE Already defined"
#endif
#define TREE_TYPE(name, ...)                                    typedef ptr<name> ptr_##name;
#include "all_tree_types.def.hpp"
#undef  TREE_TYPE

/* ==================== *
 * Type Trait Utilities *
 * ==================== */

template<typename T>
using tree_vector_t = std::vector<T>;

template<typename T>
using is_tree = std::is_base_of<__tree_base, T>;

template<typename TTree, typename T>
struct enable_if_tree : std::enable_if<is_tree<TTree>::value, T> { };

template<typename TTree, typename T = int>
using enable_if_tree_t = typename enable_if_tree<TTree, T>::type;


/* ============== *
 * Tree Type Info *
 * ============== */

extern const tree_type __all_tree_types[];


enum class tree_type_id : uint64_t {
    tree,
    dummy,
    tree_list,
#ifdef TREE_TYPE
#error "TREE_TYPE Already defined"
#endif
#define TREE_TYPE(name, ...)                                    name,
#include "all_tree_types.def.hpp"
#undef  TREE_TYPE
    __type_count
};


struct tree_type {
    tree_type_id                                                id;
    tree_type_id                                                base_id;
    const char*                                                 name;

    __tree_clone_func_t                                         shallow_clone_func;

    inline static   constexpr size_t count() noexcept { return (size_t)tree_type_id::__type_count; }
    inline static __constexpr bool   is_base_of(const tree_type_id bt, const tree_type_id dt) noexcept {
        tree_type_id tt = dt;
        while(tt != tree_type_id::tree) {
            if(bt == tt) {
                return true;
            }
            tt = __all_tree_types[(size_t)tt].base_id;
        }
        return false;
    }
};

template<tree_type_id Id> struct __tree_id_to_type { };
template<tree_type_id Id> struct __tree_id_to_base { };

template<> struct __tree_id_to_type<tree_type_id::tree>         { typedef __tree_base                                       type; };
template<> struct __tree_id_to_base<tree_type_id::tree>         { typedef void                                              type; };
template<> struct __tree_id_to_type<tree_type_id::tree_list>    { typedef void                                              type; };
template<> struct __tree_id_to_base<tree_type_id::tree_list>    { typedef __tree_base                                       type; };

#ifdef TREE_TYPE
#error "TREE_TYPE Already defined"
#endif
#define TREE_TYPE(name, base, ...)\
    template<> struct __tree_id_to_type<tree_type_id::name>{ typedef xcc::name                                              type; };\
    template<> struct __tree_id_to_base<tree_type_id::name>{ typedef typename __tree_id_to_type<tree_type_id::base>::type   type; };
#include "all_tree_types.def.hpp"
#undef TREE_TYPE

template<tree_type_id Id>
struct tree_type_info {
    typedef typename __tree_id_to_type<Id>::type                type;
    typedef typename __tree_id_to_base<Id>::type                base_type;
};


/* =============== *
 * Tree Base Class *
 * =============== */

/** Base type for all tree types */
struct __tree_base {
public:

    static const tree_type_id type_id = tree_type_id::tree;

    explicit inline __tree_base()                noexcept : _type(tree_type_id::tree) { }
    explicit inline __tree_base(tree_type_id id) noexcept : _type(id)                 { }
    virtual ~__tree_base() = default;

    inline bool is(const tree_type_id tp) const noexcept { return tree_type::is_base_of(tp, this->_type); }
    template<typename T>
    inline enable_if_tree_t<T, bool> is() const noexcept { return this->is(T::type_id); }

    template<typename T>
    inline enable_if_tree_t<T, T*> as() const noexcept { return static_cast<T*>(const_cast<__tree_base*>(this)); }
    inline tree_type_id get_tree_type() const noexcept { return this->_type; }

    inline const char* get_tree_type_name() const noexcept { return __all_tree_types[(size_t) this->_type].name; };

    explicit inline __tree_base(const __tree_base& other) noexcept
            : _type(other._type) {
        // do nothing
    }

    inline void pin() const noexcept {
        box(this).pin();
    }

protected:

    template<typename T>
    friend struct __strong_reference;

    template<typename T>
    friend struct __weak_reference;

    template<typename TElement>
    friend struct __tree_list_tree;

    template<typename TBaseType, typename... TParamTypes>
    friend struct __dispatch_tree_walker_base;

    /**
     * Used by __tree_property_tree constructor to insert new children
     */
    inline size_t __append_child(__tree_base* v) noexcept {
        size_t idx = this->_strong_references.size();
        this->_strong_references.push_back(ptr<__tree_base>(v));
        return idx;
    }

    const tree_type_id                                          _type;
    tree_vector_t<ptr<__tree_base>>                             _strong_references;
    tree_vector_t<ptr<__tree_base>>                             _weak_references;

};


/**
 * Helper class for extending base tree types
 */
template<tree_type_id VType, typename TBase = __tree_base>
struct __extend_tree : public TBase {
private:

    typedef typename tree_type_info<VType>::type                __type;

public:

    typedef __extend_tree<VType, TBase>                         base_type;
    static const tree_type_id                                   type_id = VType;

    template<typename... TArgs>
    explicit inline __extend_tree(TArgs... args): TBase(VType, args...) { }

    template<typename... TArgs>
    explicit inline __extend_tree(tree_type_id tp, TArgs... args): TBase(tp, args...) { }

    // copy constructor
    explicit inline __extend_tree(const base_type& other) : TBase(other) { }

};



template<tree_type_id tp, typename base = __tree_base>
using extend_tree = __extend_tree<tp, base>;

template<tree_type_id tp>
using implement_tree = __extend_tree<tp, typename tree_type_info<tp>::base_type>;

template<typename T>
constexpr enable_if_tree_t<T, tree_type_id> tree_type_id_from() { return T::type_id; }

}

#endif /* XCC_TREE_BASE_HPP_ */
