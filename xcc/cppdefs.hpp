/*
 * cppdefs.hpp
 *
 *  Created on: Dec 1, 2017
 *      Author: derick
 */

#ifndef XCC_CPPDEFS_HPP_
#define XCC_CPPDEFS_HPP_


#if 1
#   define  __constexpr
#   define  __constif(e)                if(e)
#else
#   define  __constexpr                 constexpr
#   define  __constif(e)                if(constexpr e)
#endif


#endif /* XCC_CPPDEFS_HPP_ */
