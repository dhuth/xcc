/*
 * xi_lower.cpp
 *
 *  Created on: Feb 22, 2017
 *      Author: derick
 */

#include <xi_old/xi_lower.hpp>


namespace xcc {

void xi_lower_walker::begin(tree_type_id id, ast_tree* t) {
    switch(id) {
    case tree_type_id::ast_block_stmt: this->begin_block(t->as<ast_block_stmt>()); break;
    }
}

void xi_lower_walker::end(tree_type_id id, ast_tree* t) {
    switch(id) {
    case tree_type_id::ast_block_stmt: this->end_block(t->as<ast_block_stmt>()); break;
    }
}

}

