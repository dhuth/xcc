/*
 * tree_list.hpp
 *
 *  Created on: Feb 9, 2018
 *      Author: derick
 */

#ifndef XCC_TREE_LIST_HPP_
#define XCC_TREE_LIST_HPP_

#include "tree_base.hpp"
#include "tree_reference.hpp"

namespace xcc {

template<typename, typename>        struct __tree_list_base;
template<typename>                  struct __tree_list_tree;
template<typename>                  struct __tree_list_value;
template<typename>                  struct __tree_list_type_traits;

template<typename T>                using tree_list     = __tree_list_tree<T>;
template<typename T>                using value_list    = __tree_list_value<T>;
template<typename T, typename Tvec> using list_base_t   = __tree_list_base<T, Tvec>;



template<typename TList> struct __tree_list_type_traits {
    typedef typename TList::iterator_t                      iterator_t;
    typedef typename TList::list_t                          list_t;
    typedef typename TList::list_ptr_t                      list_ptr_t;
    typedef typename TList::element_t                       element_t;
    typedef typename TList::element_ref_t                   element_ref_t;
};

template<typename T> struct __is_tree : std::false_type { };
template<typename T> struct __is_tree<__tree_list_tree<T>>  : std::true_type { };
template<typename T> struct __is_tree<__tree_list_value<T>> : std::true_type { };

template<typename T, typename R = T>
using enable_if_tree_t = typename std::enable_if<
        __is_tree<T>::value, R>::type;

/**
 * Controls tree type conversion to and from the underlying list
 */
template<typename _Element, typename _VecElement>
struct __tree_list_adapter;


/**
 * Tree list adapter for value type lists
 */
template<typename _Element>
struct __tree_list_adapter<_Element, _Element> {
public:

    typedef _Element                                        element_t;
    typedef _Element&                                       element_ref_t;
    typedef std::vector<_Element>                           vector_t;
    typedef typename vector_t::iterator                     iterator_t;
    typedef typename vector_t::const_iterator               const_iterator_t;

    /**
     * Get a reference from vec at index i
     * @param vec
     * @param i
     * @return
     */
    static inline element_ref_t get(vector_t& vec, size_t i) noexcept {
        return vec.at(i);
    }

    /**
     * Get a reference from vec at index i
     * @param vec
     * @param i
     * @return
     */
    static inline element_ref_t get(const vector_t& vec, size_t i) noexcept {
        return vec.at(i);
    }

    /**
     * Set the value in vec at index i
     * @param vec
     * @param i
     * @param v
     */
    static inline void set(vector_t& vec, size_t i, element_t v) noexcept {
        vec[i] = v;
    }

    /**
     * Remove an element from the list at pos
     * @param vec
     * @param pos
     */
    static inline void erase(vector_t& vec, const_iterator_t pos) noexcept {
        vec.erase(pos);
    }

    /**
     * Insert a range of elements, starting at pos
     * @param vec
     * @param pos
     * @param b
     * @param e
     */
    template<typename _It>
    static inline void insert(vector_t& vec, const_iterator_t pos, _It b, _It e) noexcept {
        vec.insert(pos, b, e);
    }

    /**
     * Insert a single element at pos
     * @param vec
     * @param pos
     * @param el
     */
    static inline void insert(vector_t& vec, const_iterator_t pos, element_t el) noexcept {
        vec.insert(pos, el);
    }

    /**
     * Insert a single element to the front of the list
     * @param vec
     * @param v
     */
    static inline void push_front(vector_t& vec, element_t v) noexcept {
       vec.insert(vec.begin(), v);
    }

    /**
     * Add an element to the end of the list
     * @param vec
     * @param v
     */
    static inline void push_back(vector_t& vec, element_t v) noexcept {
        vec.push_back(v);
    }

};


/**
 * Tree list iterator over node types
 */
template<typename _TreeType, typename _IterBase>
struct __tree_list_iterator : _IterBase {
public:

    __tree_list_iterator() = default;
    __tree_list_iterator(const _IterBase& i)             noexcept : _IterBase(i) { };
    __tree_list_iterator(const _IterBase&& i)            noexcept : _IterBase(i) { };
    __tree_list_iterator(const __tree_list_iterator& i)  noexcept : _IterBase(i) { };
    __tree_list_iterator(const __tree_list_iterator&& i) noexcept : _IterBase(i) { };

    // Use vectors iterator for most everything
    using _IterBase::operator++;
    using _IterBase::operator+;
    using _IterBase::operator-;

    // TODO: should return a reference
    _TreeType* operator*() noexcept {
        auto ptr = (_IterBase::operator*());
        return (_TreeType*) unbox(ptr);
    }
};


/**
 * Tree list adapter for tree node types
 */
template<typename _TreeType>
struct __tree_list_adapter<_TreeType*, ptr<tree_t>> {
public:

    typedef _TreeType*                                                          element_t;
    typedef _TreeType*                                                          element_ref_t;  // TODO
    typedef std::vector<ptr<tree_t>>                                            vector_t;
    typedef __tree_list_iterator<_TreeType, typename vector_t::iterator>        iterator_t;
    typedef __tree_list_iterator<_TreeType, typename vector_t::const_iterator>  const_iterator_t;

    /**
     * Get the element in vec at index i
     * @param vec
     * @param i
     * @return
     */
    static inline element_ref_t get(vector_t& vec, size_t i) noexcept {
        return (element_t) unbox(vec[i]);
    }

    /**
     * Get the element in vec at index i
     * @param vec
     * @param i
     * @return
     */
    static inline element_ref_t get(const vector_t& vec, size_t i) noexcept {
        return (element_t) unbox(vec[i]);
    }

    /**
     * Set the element in vec at index i
     * @param vec
     * @param i
     * @param v
     */
    static inline void set(vector_t& vec, size_t i, element_t v) noexcept {
        vec[i] = box((tree_t*) v);
    }

    /**
     * Remove the element at pos
     * @param vec
     * @param pos
     */
    static inline void erase(vector_t& vec, iterator_t pos) noexcept {
        vec.erase(pos);
    }

    /**
     * Remove the element at pos
     * @param vec
     * @param pos
     */
    static inline void erase(vector_t& vec, const_iterator_t pos) noexcept {
        vec.erase(pos);
    }

    /**
     * Insert a range into vec starting at pos
     * @param vec
     * @param pos
     * @param b
     * @param e
     */
    template<typename _It>
    static inline void insert(vector_t& vec, iterator_t pos, _It b, _It e) noexcept {
        while(b != e) {
            vec.push_back((tree_t*) *b);
            pos++;
            b++;
        }
    }

    /**
     * Insert a range into vec starting at pos
     * @param vec
     * @param pos
     * @param b
     * @param e
     */
    template<typename _It>
    static inline void insert(vector_t& vec, const_iterator_t pos, _It b, _It e) noexcept {
        while(b != e) {
            vec.push_back((tree_t*) *b);
            pos++;
            b++;
        }
    }

    /**
     * Insert a single item into vec at pos
     * @param vec
     * @param pos
     * @param el
     */
    static inline void insert(vector_t& vec, iterator_t pos, element_t el) noexcept {
        vec.insert(pos, (tree_t*) el);
    }

    /**
     * Insert a single item into vec at pos
     * @param vec
     * @param pos
     * @param el
     */
    static inline void insert(vector_t& vec, const_iterator_t pos, element_t el) noexcept {
        vec.insert(pos, (tree_t*) el);
    }

    /**
     * Insert a single to the beginning of the list
     * @param vec
     * @param el
     */
    static inline void push_front(vector_t& vec, element_t el) noexcept {
        vec.insert(vec.begin(), (tree_t*) el);
    }

    /**
     * Add an item to the end of the list
     * @param vec
     * @param el
     */
    static inline void push_back(vector_t& vec, element_t el) noexcept {
        vec.push_back(box((tree_t*) el));
    }
};


/**
 * The base list class for all lists that exist as tree nodes
 */
template<typename _Element, typename _VecElement>
struct __tree_list_base : public __extend_tree<tree_type_id::tree_list, __tree_base> {
public:

    typedef __tree_list_base<_Element, _VecElement>         base_list_t;
    typedef __tree_list_adapter<_Element, _VecElement>      adapter;
    typedef typename adapter::element_t                     element_t;
    typedef typename adapter::element_ref_t                 element_ref_t;
    typedef typename adapter::vector_t                      vector_t;
    typedef typename adapter::iterator_t                    iterator_t;
    typedef typename adapter::const_iterator_t              const_iterator_t;

    explicit inline __tree_list_base(vector_t& vec) noexcept
            : base_type(),
              _vector(vec) {
        // ...
    }

    explicit inline __tree_list_base(vector_t& vec, element_t el) noexcept
            : base_type(),
              _vector(vec) {
        adapter::push_back(vec, el);
    }

    explicit inline __tree_list_base(vector_t& vec, const base_list_t* olist) noexcept
            : base_type(),
              _vector(vec) {
        adapter::insert(_vector, vec.end(), olist->begin(), olist->end());
    }

    explicit inline __tree_list_base(vector_t& vec, const base_list_t* olist, element_t el) noexcept
            : base_type(),
              _vector(vec) {
        adapter::insert(_vector, vec.end(), olist->begin(), olist->end());
        adapter::push_back(vec, el);
    }

    explicit inline __tree_list_base(vector_t& vec, element_t el, const base_list_t* olist) noexcept
            : base_type(),
              _vector(vec) {
        adapter::push_back(vec, el);
        adapter::insert(_vector, vec.end(), olist->begin(), olist->end());
    }

    explicit inline __tree_list_base(vector_t& vec, std::initializer_list<element_t> l) noexcept
            : base_type(),
              _vector(vec) {
        adapter::insert(_vector, vec.end(), l.begin(), l.end());
    }

    explicit inline __tree_list_base(vector_t& vec, const std::vector<element_t>& v) noexcept
            : base_type(),
              _vector(vec) {
        adapter::insert(_vector, vec.end(), v.begin(), v.end());
    }

    /**
     * Returns an iterator at the begining of the list
     * @return
     */
    iterator_t begin() noexcept {
        return iterator_t(_vector.begin());
    }

    /**
     * Returns an iterator at the begining of the list
     * @return
     */
    const_iterator_t begin() const noexcept {
        return const_iterator_t(_vector.begin());
    }

    /**
     * Returns an iterator at the end of the list
     * @return
     */
    iterator_t end() noexcept {
        return iterator_t(_vector.end());
    }

    /**
     * Returns an iterator at the end of the list
     * @return
     */
    const_iterator_t end() const noexcept {
        return const_iterator_t(_vector.end());
    }

    /**
     * Returns a reference to the item at index i
     * @param i
     * @return
     */
    element_ref_t operator[](size_t i) noexcept {
        return adapter::get(_vector, i);
    }

    /**
     * Returns a reference to the item at index i
     * @param i
     * @return
     */
    element_ref_t operator[](size_t i) const noexcept {
        return adapter::get(_vector, i);
    }

    /**
     * Add an element to the end of the list
     * @param el
     */
    void push_back(element_t el) noexcept {
        adapter::push_back(_vector, el);
    }

    /**
     * Add en element to the front of the list
     * @param el
     */
    void push_front(element_t el) noexcept {
        adapter::push_front(_vector, el);
    }

    /**
     * Remove an item from the list at pos
     * @param pos
     */
    void erase(const_iterator_t pos) noexcept {
        adapter::erase(_vector, pos);
    }

    /**
     * Remove an item from he list at pos
     * @param pos
     */
    void erase(iterator_t pos) noexcept {
        adapter::erase(_vector, pos);
    }

    /**
     * Insert an item at pos
     * @param pos
     * @param el
     */
    void insert(const_iterator_t pos, element_t el) noexcept {
        adapter::insert(_vector, pos, el);
    }

    /**
     * Insert an item at pos
     * @param pos
     * @param el
     */
    void insert(iterator_t pos, element_t el) noexcept {
        adapter::insert(_vector, pos, el);
    }

    /**
     * Returns the number of elements currently in this list
     * @return
     */
    size_t size() const noexcept {
        return _vector.size();
    }

private:

    std::vector<_VecElement>&                               _vector;

};


/**
 * A list of values
 */
template<typename _Element>
struct __tree_list_value final : public __tree_list_base<_Element, _Element> {
public:

    typedef __tree_list_base<_Element, _Element>                        base_list_t;
    typedef typename base_list_t::element_t                             element_t;
    typedef typename base_list_t::element_ref_t                         element_ref_t;

    inline          __tree_list_value() noexcept
            : base_list_t(_list) { }
    inline explicit __tree_list_value(element_t f) noexcept
            : base_list_t(_list) {
        this->push_back(f);
    }
    inline explicit __tree_list_value(const base_list_t* l) noexcept
            : base_list_t(_list, l) {
        for(auto iter = l->begin(); iter != l->end(); iter++) {
            this->push_back(*iter);
        }
    }
    inline explicit __tree_list_value(const base_list_t* f, element_t l) noexcept
            : base_list_t(_list, f, l) {
        for(auto iter = f->begin(); iter != f->end(); iter++) {
            this->push_back(*iter);
        }
        this->push_back(l);
    }
    inline explicit __tree_list_value(element_t f, const base_list_t* r) noexcept
            : base_list_t(_list, f, r) {
        this->push_back(f);
        for(auto iter = r->begin(); iter != r->end(); iter++) {
            this->push_back(*iter);
        }
    }
    inline explicit __tree_list_value(std::initializer_list<element_t> l) noexcept
            : base_list_t(_list, l) {
        for(auto el: l) {
            this->push_back(l);
        }
    }
    inline explicit __tree_list_value(const std::vector<element_t>& v) noexcept
            : base_list_t(_list, v) {
        for(auto el: v) {
            this->push_back(el);
        }
    }

protected:

    std::vector<_Element>                                               _list;

};


/**
 * A list of tree nodes
 */
template<typename _TreeType>
struct __tree_list_tree final : public __tree_list_base<_TreeType*, ptr<tree_t>> {
public:

    typedef __tree_list_base<_TreeType*, ptr<tree_t>>                   base_list_t;
    typedef typename base_list_t::iterator_t                            iterator_t;
    typedef typename base_list_t::element_t                             element_t;
    typedef typename base_list_t::element_ref_t                         element_ref_t;

    inline          __tree_list_tree()                                   noexcept : base_list_t(this->_strong_references)       { }
    inline explicit __tree_list_tree(element_t el)                       noexcept : base_list_t(this->_strong_references, el)   { }
    inline explicit __tree_list_tree(const base_list_t* f)               noexcept : base_list_t(this->_strong_references, f)    { }
    inline explicit __tree_list_tree(const base_list_t* f, element_t l)  noexcept : base_list_t(this->_strong_references, f, l) { }
    inline explicit __tree_list_tree(element_t f, const base_list_t* r)  noexcept : base_list_t(this->_strong_references, f, r) { }
    inline explicit __tree_list_tree(std::initializer_list<element_t> l) noexcept : base_list_t(this->_strong_references, l)    { }
    inline explicit __tree_list_tree(const std::vector<element_t>& v)    noexcept : base_list_t(this->_strong_references, v)    { }

};

/**
 * Select a list type based on the type of element
 */
template<typename T> struct __list_type_selector {
    typedef typename std::conditional<std::is_base_of<__tree_base, T>::value,
                            __tree_list_tree<T>,
                            __tree_list_value<T>>::type         type;
};

template<typename T>
using list = typename __list_type_selector<T>::type;

template<typename _List>
struct __reference_list_impl : public __reference_impl_base<_List> {
public:

    typedef _List                                           list_t;
    typedef typename list_t::element_t                      element_t;
    typedef typename list_t::element_ref_t                  element_ref_t;
    typedef typename list_t::iterator_t                     iterator_t;

    using __reference_impl_base<_List>::operator=;
    using __reference_impl_base<_List>::__reference_impl_base;

    inline element_ref_t operator[](size_t index) const noexcept {
        assert(this->get() != nullptr);
        auto& l = *(this->get());
        return l[index];
    }

};

template<typename _Element>
struct __pick_reference_impl<value_list<_Element>> {
public:

    typedef __reference_list_impl<value_list<_Element>>     type;
};

template<typename _TreeType>
struct __pick_reference_impl<tree_list<_TreeType>> {
public:

    typedef __reference_list_impl<tree_list<_TreeType>>     type;
};



template<typename _Element,
         typename _VecElement>
inline typename list_base_t<_Element, _VecElement>::iterator_t
begin(list_base_t<_Element, _VecElement>& l) noexcept {
   return l.begin();
}

template<typename _Element,
         typename _VecElement>
inline typename list_base_t<_Element, _VecElement>::iterator_t
end(list_base_t<_Element, _VecElement>& l) noexcept {
   return l.end();
}


#define __define_begin_end(pt, lt)\
        template<typename _Element> inline typename lt<_Element>::iterator_t begin(pt<lt<_Element>> p) noexcept { return p->begin(); }\
        template<typename _Element> inline typename lt<_Element>::iterator_t end(  pt<lt<_Element>> p) noexcept { return p->end();   }

__define_begin_end(ptr, value_list)
__define_begin_end(ptr, tree_list)
__define_begin_end(ref, value_list)
__define_begin_end(ref, tree_list)

#undef  __define_begin_end
}



#endif /* XCC_TREE_LIST_HPP_ */
