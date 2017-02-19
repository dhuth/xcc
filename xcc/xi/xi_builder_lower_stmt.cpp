/*
 * xi_builder_lower_stmt.cpp
 *
 *  Created on: Feb 15, 2017
 *      Author: derick
 */


#include "xi_builder.hpp"

namespace xcc {




ast_stmt* xi_builder::lower(ast_stmt* stmt) {
    switch(stmt->get_tree_type()) {
    case tree_type_id::ast_expr_stmt:
        {
            stmt->as<ast_expr_stmt>()->expr =
                    this->lower(stmt->as<ast_expr_stmt>()->expr);
            return stmt;
        }
    case tree_type_id::ast_assign_stmt:
        {
            stmt->as<ast_assign_stmt>()->lhs =
                    this->lower(stmt->as<ast_assign_stmt>()->lhs);
            stmt->as<ast_assign_stmt>()->rhs =
                    this->lower(stmt->as<ast_assign_stmt>()->rhs);
            return stmt;
        }
    case tree_type_id::ast_block_stmt:
        {
            auto bstmt = stmt->as<ast_block_stmt>();
            this->push_block(bstmt);
            bstmt->decls = map<ast_local_decl, ast_local_decl>(bstmt->decls,
                            [&](ast_local_decl* ldecl)->ast_local_decl* {
                                return this->lower(ldecl)->as<ast_local_decl>();
                            });
            bstmt->stmts = map<ast_stmt, ast_stmt>(bstmt->stmts,
                            [&](ast_stmt* s)->ast_stmt* {
                                return this->lower(s);
                            });
            this->pop();
            return bstmt;
        }
    case tree_type_id::ast_if_stmt:
        {
            auto istmt = stmt->as<ast_if_stmt>();
            istmt->condition    = this->lower(istmt->condition);
            istmt->true_stmt    = this->lower(istmt->true_stmt);
            istmt->false_stmt   = this->lower(istmt->false_stmt);
            return istmt;
        }
    case tree_type_id::ast_while_stmt:
        {
            auto wstmt = stmt->as<ast_while_stmt>();
            wstmt->condition    = this->lower(wstmt->condition);
            wstmt->stmt         = this->lower(wstmt->stmt);
            return wstmt;
        }
    case tree_type_id::ast_for_stmt:
        {
            auto fstmt = stmt->as<ast_for_stmt>();
            fstmt->init_stmt    = this->lower(fstmt->init_stmt);
            fstmt->condition    = this->lower(fstmt->condition);
            fstmt->each_stmt    = this->lower(fstmt->each_stmt);
            fstmt->body         = this->lower(fstmt->body);
            return fstmt;
        }
    case tree_type_id::ast_return_stmt:
        {
            auto rstmt = stmt->as<ast_return_stmt>();
            if(rstmt->expr) {
                rstmt->expr = this->lower(rstmt->expr);
            }
            return rstmt;
        }
    case tree_type_id::ast_nop_stmt:
    case tree_type_id::ast_break_stmt:
    case tree_type_id::ast_continue_stmt:
        return stmt;
    }

    assert(false);
    return nullptr;
}

}

