/*
 * ast_context.hpp
 *
 *  Created on: Mar 6, 2017
 *      Author: derick
 */

#ifndef AST_CONTEXT_HPP_
#define AST_CONTEXT_HPP_


#include "ast.hpp"

namespace xcc {


/**
 * The base class for all contexts. Represents visibility in a location of code
 */
struct ast_context {
public:

    explicit inline ast_context()                  noexcept : parent(nullptr) { };
    explicit inline ast_context(ast_context* prev) noexcept : parent(prev)    { };
    virtual ~ast_context() = default;

    /**
     * Find the first declaration with the given name
     * @param name              the given name
     * @param search_parent     if true, search all parents for this name
     * @return                  a managed pointer to the declaration if one was found, otherwise a null managed pointer
     */
    ptr<ast_decl>       find(const char* name, bool search_parent = true);

    /**
     * Find all declarations with the given name
     * @param name              The name to search for
     * @param search_parent     If true, search all parents for this name
     * @param keep_looking      If true, keep searching parent scopes, even if values are already found
     * @return                  A managed pointer to list of all declarations of the given name
     */
    ptr<list<ast_decl>> findall(const char* name, bool search_parent = true, bool keep_looking = false);

    /**
     * Convenience function for find. Returns the declarations casted to a base type
     * @param name              The name to search for
     * @param search_parent     If true, search all parents for this name
     * @return                  A managed pointer to the declaration if one was found, otherwise a null managed pointer
     */
    template<typename TTreeType,
             typename std::enable_if<std::is_base_of<ast_decl, TTreeType>::value, int>::type = 0>
    ptr<TTreeType> find_of(const char* name, bool search_parent = true) {
        auto ff = this->find_first_impl(name);
        if(ff == nullptr || !ff->is<TTreeType>()) {
            if(search_parent && this->parent != nullptr) {
                return this->parent->find_of<TTreeType>(name, search_parent);
            }
            else {
                return box<TTreeType>(nullptr);
            }
        }
        else {
            return box(ff->as<TTreeType>());
        }
    }

    /**
     * Convenience function for findall. Find all declarations with the given name, cast to a common base type
     * @param name              The given name
     * @param search_parent     If true, search all parents for this name
     * @param keep_looking      If true, keep searching parent scopes, even if values are already found
     * @return                  A managed pointer to list of all declarations of the given name
     */
    template<typename TTreeType,
             typename std::enable_if<std::is_base_of<ast_decl, TTreeType>::value, int>::type = 0>
    ptr<list<TTreeType>> findall_of(const char* name, bool search_parent = true, bool keep_looking = false) {
        ptr<list<TTreeType>> olist = box(new list<TTreeType>());
        this->findall_of<TTreeType>(olist, name, search_parent);
        return olist;
    }

    template<typename TPredicate,
             typename TTreeType = ast_decl>
    ptr<TTreeType> find_if(const char* name, TPredicate pred, bool search_parent = true) {
        // ...
    }

    /**
     * Create a context within this current context
     * @param args              Arguments to pass to the constructor
     * @return                  A managed pointer to a new context of type TContext
     */
    template<typename TContext, typename... TArgs>
    ptr<ast_context> push_context(TArgs&&... args) {
        return box<ast_context>(new TContext(this, std::forward<TArgs>(args)...));
    }

    /**
     * Leave this context, and return its parent
     * @return                  The parent context
     */
    virtual ptr<ast_context> pop_context() noexcept;

protected:

    /**
     * Helper function for finding the first declaration in a list
     * @param l
     * @param name
     * @return
     */
    template<typename T>
    static inline ptr<ast_decl> first_from_list(__tree_property_list<T>& l, const char* name) {
        for(auto d: l) {
            if(std::string(name) == (std::string) d->name) {
                return box<ast_decl>(d);
            }
        }
        return nullptr;
    }

    /**
     * Helper function for finding all declarations in a list
     * @param l
     * @param olist
     * @param name
     */
    template<typename T>
    static inline void all_from_list(__tree_property_list<T>& l, ptr<list<ast_decl>> olist, const char* name) {
        for(auto d: l) {
            if(std::string(name) == (std::string) d->name) {
                olist->append(d);
            }
        }
    }

    /**
     * implementation of find_fist, searching only the current context
     * @param name
     * @return
     */
    virtual ptr<ast_decl> find_first_impl(const char* name) = 0;

    /**
     * implementation of find_all, searching only the current context
     * @param
     * @param
     */
    virtual void find_all_impl(ptr<list<ast_decl>>, const char*) = 0;

    ptr<ast_context>                                                    parent;    //! The parent context

private:

    void findall(ptr<list<ast_decl>> olist, const char* name, bool search_parent, bool keep_looking);

    template<typename TTreeType,
             typename std::enable_if<std::is_base_of<ast_decl, TTreeType>::value, int>::type = 0>
    void findall_of(ptr<list<TTreeType>> olist, const char* name, bool search_parent) {
        ptr<list<ast_decl>> dlist = box(new list<ast_decl>());
        this->find_all_impl(dlist, name);
        for(auto decl: unbox(dlist)) {
            if(decl->is<TTreeType>()) {
                olist->append(decl->as<TTreeType>());
            }
        }
        if(search_parent && this->parent != nullptr) {
            this->parent->findall_of<TTreeType>(olist, name, search_parent);
        }
    }

};


/**
 * A namespace context
 */
struct ast_namespace_context : public ast_context {
public:

    explicit inline ast_namespace_context(ast_context* prev, ast_namespace_decl* ns) noexcept
            : ast_context(prev),
              _ns(ns),
              _is_global(prev == nullptr) {
        // do nothing
    }
    virtual ~ast_namespace_context() = default;

protected:

    ptr<ast_decl> find_first_impl(const char*) override;
    void find_all_impl(ptr<list<ast_decl>>, const char*) override;

private:

    ptr<ast_namespace_decl>                                             _ns;        //! The namespace that this context belongs to
    bool                                                                _is_global; //! True if this is a global namespace

};


/**
 * A block context
 */
struct ast_block_context final : public ast_context {
public:

    explicit inline ast_block_context(ast_context* prev, ast_block_stmt* block) noexcept
            : ast_context(prev),
              _block(block) {
        // do nothing
    }
    virtual ~ast_block_context() = default;

protected:

    ptr<ast_decl> find_first_impl(const char*) final override;
    void find_all_impl(ptr<list<ast_decl>>, const char*) final override;

private:

    ptr<ast_block_stmt>                                                 _block; //! The block that this context belongs to

};


/**
 * A function context
 */
struct ast_function_context final : public ast_context {
public:

    explicit inline ast_function_context(ast_context* prev, ast_function_decl* func) noexcept
            : ast_context(prev),
              _func(func) {
        // do nothing
    }
    virtual ~ast_function_context() = default;

protected:

    ptr<ast_decl> find_first_impl(const char*) final override;
    void find_all_impl(ptr<list<ast_decl>>, const char*) final override;

private:

    ptr<ast_function_decl>                                              _func; //! The function that this context belongs to

};
}


#endif /* AST_CONTEXT_HPP_ */
