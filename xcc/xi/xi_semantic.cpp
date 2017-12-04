/*
 * xi_semantic.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"


namespace xcc {

static ast_expr* xi_rn_id_expr(xi_id_expr* idexpr, xi_builder& b) {
    ptr<list<ast_decl>> decls = b.find_all_declarations(idexpr->name->c_str());

    if(decls->size() == 0) {
        // TODO: error
    }
    else if(decls->size() == 1) {
        ast_decl* decl = (*decls)[0];
        ast_type* type = b.get_declaration_type(decl);

        return copyloc(b.make_declref_expr(decl), idexpr);
    }
    else {
        return new xi_group_expr(map(decls, [&](ast_decl* d) -> ast_expr* {
            return copyloc(b.make_declref_expr(d), idexpr);
        }));
    }
}

struct xi_semantic_check_func : public xi_postorder_walker {
public:

    xi_semantic_check_func() : xi_postorder_walker() {
        this->add(&xi_rn_id_expr);
    }

};

void xi_builder::semantic_check() noexcept {
    xi_semantic_check_func f;
    f(this->global_namespace, *this);
}


}
