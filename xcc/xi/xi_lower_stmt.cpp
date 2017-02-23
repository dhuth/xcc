/*
 * xi_builder_lower_stmt.cpp
 *
 *  Created on: Feb 15, 2017
 *      Author: derick
 */


#include "xi_builder.hpp"
#include "xi_lower.hpp"

namespace xcc {

void xi_lower_walker::begin_block(ast_block_stmt* stmt) {
    this->_xi_builder.push_block(stmt);
}

ast_stmt* xi_lower_walker::lower_block_stmt(ast_block_stmt* stmt) {
    return stmt;
}

ast_stmt* xi_lower_walker::lower_assign_stmt(xi_assign_stmt* stmt) {
    if(stmt->op == xi_operator::assign) {
        return this->_ast_builder.make_lower_assign_stmt(stmt->lhs, stmt->rhs);
    }

    ast_op newop = ast_op::none;
    switch((xi_operator) stmt->op) {
    case xi_operator::assign_add:   newop = ast_op::add;            break;
    case xi_operator::assign_sub:   newop = ast_op::sub;            break;
    case xi_operator::assign_mul:   newop = ast_op::mul;            break;
    case xi_operator::assign_div:   newop = ast_op::div;            break;
    case xi_operator::assign_mod:   newop = ast_op::mod;            break;
    case xi_operator::assign_shl:   newop = ast_op::shl;            break;
    case xi_operator::assign_shr:   newop = ast_op::shr;            break;
    case xi_operator::assign_land:  newop = ast_op::logical_and;    break;
    case xi_operator::assign_lor:   newop = ast_op::logical_or;     break;
    case xi_operator::assign_band:  newop = ast_op::binary_and;     break;
    case xi_operator::assign_bor:   newop = ast_op::binary_or;      break;
    }

    return this->_ast_builder.make_lower_assign_stmt(stmt->lhs,
            this->_ast_builder.make_op_expr(newop, stmt->lhs, stmt->rhs));
}

void xi_lower_walker::end_block(ast_block_stmt* stmt) {
    this->_xi_builder.pop();
}

/*ast_stmt* xi_builder::lower(ast_stmt* stmt) {
    switch(stmt->get_tree_type()) {
    case tree_type_id::ast_expr_stmt:
        {
            stmt->as<ast_expr_stmt>()->expr =
                    this->lower(stmt->as<ast_expr_stmt>()->expr);
            return stmt;
        }
    case tree_type_id::xi_assign_stmt:
        {
            auto astmt = stmt->as<xi_assign_stmt>();
            if(astmt->op == xi_operator::assign) {
                return this->lower(new ast_assign_stmt(astmt->lhs, astmt->rhs));
            }
            else {
                return lower_assign_stmt(this, astmt);
            }
            break;
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
}*/

}

