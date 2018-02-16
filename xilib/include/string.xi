
#include "cstdlib.xi"


namespace core {

struct string {
    ._ptr           : *c::char_t;
}



extern func __ctor__(&string);
extern func __ctor__(&string, *  const c::char_t);
extern func __dtor__(&string);
extern func __copy__(&string, &  const string);
extern func __move__(&string, &&       string);

}
