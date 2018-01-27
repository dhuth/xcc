/*
 * cppdefs.hpp
 *
 *  Created on: Dec 1, 2017
 *      Author: derick
 */

#ifndef XCC_CPPDEFS_HPP_
#define XCC_CPPDEFS_HPP_



#if defined(__cpp_if_constexpr) &&  __cpp_if_constexpr >= 201606
#   define  __constif(e)                if constexpr (e)
#   define  __constexpr                 constexpr
#else
#   define  __constif(e)                if (e)
#   define  __constexpr
#endif


#endif /* XCC_CPPDEFS_HPP_ */
