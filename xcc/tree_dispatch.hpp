/*
 * tree_dispatch.hpp
 *
 *  Created on: Feb 9, 2018
 *      Author: derick
 */

#ifndef XCC_TREE_DISPATCH_HPP_
#define XCC_TREE_DISPATCH_HPP_

#include "tree_base.hpp"
#include "tree_reference.hpp"

namespace xcc {

template<typename TReturnType, typename... TParamTypes>
struct __dispatch_visitor_base {
protected:

    __dispatch_visitor_base() = default;
    virtual ~__dispatch_visitor_base() = default;

protected:

    virtual TReturnType handle_null_tree(TParamTypes... args) {
        throw std::runtime_error("Null tree in dispatch handler");
    }
    virtual TReturnType handle_unregisterd_tree_type(tree_type_id id, TParamTypes... args) {
        throw std::runtime_error(std::string("No registered function for type id ") + __all_tree_types[(uint64_t) id].name);
    }

    typedef std::function<TReturnType(__tree_base*, TParamTypes...)>
                                                                    dispatch_wrapper_t;

    bool find_visitor_function(tree_type_id id, dispatch_wrapper_t& func) {
        while(true) {
            if(this->_function_map.find(id) == this->_function_map.end()) {
                id = __all_tree_types[(uint64_t) id].base_id;
                if(id == tree_type_id::tree || id == tree_type_id::dummy) {
                    return false;
                }
            }
            else {
                func = this->_function_map[id];
                return true;
            }
        }
    }

    std::map<tree_type_id, dispatch_wrapper_t>                  _function_map;
};

template<typename    TReturnType,
         typename... TParamTypes>
struct __dispatch_visitor_base_notvoid : public __dispatch_visitor_base<TReturnType, TParamTypes...> {
protected:

    typedef typename __dispatch_visitor_base<TReturnType, TParamTypes...>::dispatch_wrapper_t
                                                                dispatch_wrapper_t;

public:

    __dispatch_visitor_base_notvoid() = default;
    virtual ~__dispatch_visitor_base_notvoid() = default;

    TReturnType visit(tree_t* t, TParamTypes... args) {
        if(t == nullptr) {
            return this->handle_null_tree(args...);
        }
        tree_type_id id = t->get_tree_type();
        dispatch_wrapper_t func;
        if(this->find_visitor_function(id, func)) {
            return func(t, args...);
        }
        else {
            return this->handle_unregisterd_tree_type(id, args...);
        }
    }

    template<typename T>
    inline TReturnType visit(__tree_property_tree<T>& p, TParamTypes... args) {
        return this->visit((__tree_base*)p, args...);
    }

    template<typename TFuncReturnType,
             typename TTreeType>
    using dispatch_function_type = TFuncReturnType(*)(TTreeType*, TParamTypes...);

    template<typename TFuncReturnType,
             typename TClassType,
             typename TTreeType>
    using dispatch_method_type  = TFuncReturnType(TClassType::*)(TTreeType*, TParamTypes...);

    template<typename TFuncReturnType,
             typename TTreeType>
    void addfunction(dispatch_function_type<TFuncReturnType, TTreeType> func) {
        auto wfunc = [=](__tree_base* t, TParamTypes... args) {
            return func(t->as<TTreeType>(), args...);
        };
        this->_function_map[tree_type_id_from<TTreeType>()] = wfunc;
    }

protected:

    template<typename TFuncReturnType,
             typename TClassType,
             typename TTreeType>
    void addmethod(dispatch_method_type<TFuncReturnType, TClassType, TTreeType> func) {
        auto wfunc = [=](__tree_base* t, TParamTypes... args) -> TReturnType {
            return (TReturnType) (static_cast<TClassType*>(this)->*func)(t->as<TTreeType>(), args...);
        };
        this->_function_map[tree_type_id_from<TTreeType>()] = wfunc;
    }
};

template<typename... TParamTypes>
struct __dispatch_visitor_base_void : public __dispatch_visitor_base<void, TParamTypes...>{
protected:

    typedef typename __dispatch_visitor_base<void, TParamTypes...>::dispatch_wrapper_t
                                                                    dispatch_wrapper_t;
public:

    __dispatch_visitor_base_void() = default;
    virtual ~__dispatch_visitor_base_void() = default;

    inline void visit(__tree_base* t, TParamTypes... args) {
        if(t == nullptr) {
            this->handle_null_tree(args...);
        }
        tree_type_id id = t->get_tree_type();
        dispatch_wrapper_t func;
        if(this->find_visitor_function(id, func)) {
            func(t, args...);
        }
        else {
            this->handle_unregisterd_tree_type(id, args...);
        }
    }
    template<typename T>
    inline void visit(__tree_property_tree<T>& p, TParamTypes... args) {
        return this->visit((__tree_base*)p, args...);
    }

    template<typename TTreeType>
    using dispatch_function_type = void(*)(TTreeType*, TParamTypes...);

    template<typename TClassType,
             typename TTreeType>
    using dispatch_method_type  = void(TClassType::*)(TTreeType*, TParamTypes...);

    template<typename TTreeType>
    void addfunction(dispatch_function_type<TTreeType> func) {
        auto wfunc = [=](__tree_base* t, TParamTypes... args) {
            return func(t->as<TTreeType>(), args...);
        };
        this->_function_map[tree_type_id_from<TTreeType>()] = wfunc;
    }

protected:

    template<typename TClassType,
             typename TTreeType>
    void addmethod(dispatch_method_type<TClassType, TTreeType> func) {
        auto wfunc = [=](__tree_base* t, TParamTypes... args) {
            return (static_cast<TClassType*>(this)->*func)(t->as<TTreeType>(), args...);
        };
        this->_function_map[tree_type_id_from<TTreeType>()] = wfunc;
    }
};


template<typename T, typename... TArgs>
struct __dispatch_visitor_selector {
    typedef __dispatch_visitor_base_notvoid<T, TArgs...> type;
};

template<typename... TArgs>
struct __dispatch_visitor_selector<void, TArgs...> {
    typedef __dispatch_visitor_base_void<TArgs...>         type;
};


/* ==================================== *
 * Dynamic Dispatch Translator / Walker *
 * ==================================== */

template<typename TBaseType, typename... TParamTypes>
struct __dispatch_tree_walker_base {
public:

    inline __dispatch_tree_walker_base()
            : _use_ignore(false),
              _use_include(false) {
        //...
    }
    virtual ~__dispatch_tree_walker_base() = default;


    template<typename TTreeType,
             enable_if_tree_t<TTreeType> = 0>
    using visit_func_t = void(*)(TTreeType*, TParamTypes...);

    template<typename TReturnType,
             typename TTreeType,
             enable_if_base_of_t<TReturnType, TBaseType, int> = 0,
             enable_if_base_of_t<TTreeType, TBaseType, int> = 0>
    using translate_func_t = TReturnType*(*)(TTreeType*, TParamTypes...);

    template<typename TReturnType,
             typename TTreeType,
             typename TClassType,
             enable_if_base_of_t<TBaseType, TTreeType, int> = 0,
             enable_if_base_of_t<TBaseType, TReturnType, int> = 0,
             enable_if_base_of_t<__dispatch_tree_walker_base<TBaseType, TParamTypes...>, TClassType, int> = 0>
    using translate_method_t = TReturnType*(TClassType::*)(TTreeType*, TParamTypes...);

    template<typename TTreeType,
             typename TClassType,
             enable_if_base_of_t<TBaseType, TTreeType, int> = 0,
             enable_if_base_of_t<__dispatch_tree_walker_base<TBaseType, TParamTypes...>, TClassType, int> = 0>
    using visit_method_t = void(TClassType::*)(TTreeType*, TParamTypes...);




    template<typename TTreeType>
    inline void add(visit_func_t<TTreeType> func) {
        auto wfunc = [=](__tree_base**, __tree_base* node, TParamTypes... args) {
            func(node->as<TTreeType>(), args...);
        };
        this->_functions[tree_type_id_from<TTreeType>()] = wfunc;
    }

    template<typename TReturnType, typename TTreeType>
    inline void add(translate_func_t<TReturnType, TTreeType> func) {
        auto wfunc = [=](__tree_base** retv, __tree_base* node, TParamTypes... args) {
            *retv = static_cast<__tree_base*>(
                    func(node->as<TTreeType>(), args...));
        };
        this->_functions[tree_type_id_from<TTreeType>()] = wfunc;
    }

    template<typename TReturnType,
             typename TTreeType,
             typename TClassType>
    inline void add(translate_method_t<TReturnType, TTreeType, TClassType> mtd) {
        auto wfunc = [=](__tree_base** retv, __tree_base* node, TParamTypes... args) {
            *retv = static_cast<__tree_base*>(
                    (static_cast<TClassType*>(this)->*mtd)(node->as<TTreeType>(), args...));
        };
        this->_functions[tree_type_id_from<TTreeType>()] = wfunc;
    }

    template<typename TTreeType, typename TClassType>
    inline void add(visit_method_t<TTreeType, TClassType> mtd) {
        auto wfunc = [=](__tree_base** retv, __tree_base* node, TParamTypes... args) {
            (static_cast<TClassType*>(this)->*mtd)(node->as<TTreeType>(), args...);
        };
        this->_functions[tree_type_id_from<TTreeType>()] = wfunc;
    }

    virtual void begin(tree_type_id, TBaseType*, TParamTypes...)     { }
    virtual void end(tree_type_id,   TBaseType*, TParamTypes...)     { }
    virtual void postvisit(tree_type_id, TBaseType*, TParamTypes...) { } // ???

    TBaseType* visit(TBaseType* t, TParamTypes... args) {
        return static_cast<TBaseType*>(this->visit_internal(static_cast<__tree_base*>(t), args...));
    }

    inline void set(TBaseType* t, TBaseType* v) noexcept {
        this->_visited[t] = box((__tree_base*)v);
    }

    inline void ignore(std::vector<tree_type_id>&& ignore_ids) {
        this->_use_ignore = true;
        this->_ignore = ignore_ids;
    }

    inline void include(std::vector<tree_type_id>&& include_ids) {
        this->_use_include = true;
        this->_include = include_ids;
        this->_include.push_back(tree_type_id::tree_list);
    }

    inline void reset() noexcept {
        this->_visited.clear();
    }


private:

    __tree_base* visit_internal(__tree_base* t, TParamTypes... args) {
        if(t != nullptr) {

            if(this->_use_ignore) {
                for(auto ignore_id: this->_ignore) {
                    if(t->is(ignore_id)) {
                        return t;
                    }
                }
            }

            if(this->_use_include) {
                bool is_included = false;
                for(auto include_id: this->_include) {
                    if(t->is(include_id)) {
                        is_included = true;
                        break;
                    }
                }
                if(!is_included) {
                    return t;
                }
            }

            if(this->_visited.find(t) == this->_visited.end()) {
                auto new_t = this->visit_impl(t, args...);
                if(new_t != nullptr && t->is<TBaseType>()) {
                    this->postvisit(new_t->get_tree_type(), static_cast<TBaseType*>(new_t), args...);
                }
                return new_t;
            }
            return this->_visited[t];
        }
        return nullptr;
    }

protected:

    virtual __tree_base* visit_impl(__tree_base*, TParamTypes...) = 0;
    inline void walk_children(__tree_base* t, TParamTypes... args) {
        if(t->is<TBaseType>()) {
            this->begin(t->get_tree_type(), static_cast<TBaseType*>(t), args...);
        }
        for(size_t i = 0; i < t->_strong_references.size(); i++) {
            auto child = unbox(t->_strong_references[i]);
            if(child != nullptr) {
                t->_strong_references[i] = box(this->visit_internal(child, args...));
            }
        }
        if(t->is<TBaseType>()) {
            this->end(t->get_tree_type(), static_cast<TBaseType*>(t), args...);
        }
    }

    typedef std::function<void(__tree_base**,__tree_base*,TParamTypes...)>
                                                                            fwalk_t;

    __tree_base* do_visit(__tree_base* t, TParamTypes... args) {
        auto tpid = t->get_tree_type();
        fwalk_t func;
        if(this->find_visitor_func(tpid, func)) {
            __tree_base*    retval = t;
            func(&retval, t, args...);
            _visited[t] = retval;
            return retval;
        }
        return t;
    }

    std::map<tree_type_id, fwalk_t>                                     _functions;
    std::map<__tree_base*, __tree_base*>                                _visited;
    std::vector<tree_type_id>                                           _ignore;
    std::vector<tree_type_id>                                           _include;
    bool                                                                _use_ignore;
    bool                                                                _use_include;

private:

    bool find_visitor_func(tree_type_id id, fwalk_t& func) {
        while(true) {
            auto fiter = this->_functions.find(id);
            if(fiter == this->_functions.end()) {
                id = __all_tree_types[(uint64_t) id].base_id;
                if(id == tree_type_id::tree || id == tree_type_id::dummy) {
                    return false;
                }
            }
            else {
                func = fiter->second;
                return true;
            }
        }
    }

};

template<typename TBaseType, typename... TParamTypes>
struct dispatch_preorder_tree_walker : public __dispatch_tree_walker_base<TBaseType, TParamTypes...> {
public:

    virtual ~dispatch_preorder_tree_walker() = default;

protected:

    virtual __tree_base* visit_impl(__tree_base* t, TParamTypes... args) final override {
        auto new_t = this->do_visit(t, args...);
        this->walk_children(new_t, args...);
        return new_t;
    }
};

template<typename TBaseType, typename... TParamTypes>
struct dispatch_postorder_tree_walker : public __dispatch_tree_walker_base<TBaseType, TParamTypes...> {
public:

    virtual ~dispatch_postorder_tree_walker() = default;

protected:

    virtual __tree_base* visit_impl(__tree_base* t, TParamTypes... args) final override {
        this->walk_children(t, args...);
        auto new_t = this->do_visit(t, args...);
        return new_t;
    }
};

template<typename T, typename... TArgs>
using dispatch_visitor = typename __dispatch_visitor_selector<T, TArgs...>::type;

}


#endif /* XCC_TREE_DISPATCH_HPP_ */
