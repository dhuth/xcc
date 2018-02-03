
namespace c {
    
    // memory
    
    extern cfunc malloc(size_t)                                 -> void_ptr_t;
    extern cfunc calloc(size_t)                                 -> void_ptr_t;
    extern cfunc realloc(size_t)                                -> void_ptr_t;
    extern cfunc free(*void);
    
    // program termination
    
    extern cfunc abort();
    extern cfunc exit(i32);
    
    // environment
    
    extern cfunc system(*const char_t)                                  -> i32;
    extern cfunc getenv(*const char_t)                                  -> *char_t;
    
    // signals
    
    extern cfunc signal(i32, func(i32)->void)                           -> func(i32)->void;
    extern cfunc raise(i32)                                             -> i32;
    
    // character classification
    
    extern cfunc isalnum(char_t)                                        -> bool32_t;
    extern cfunc isalpha(char_t)                                        -> bool32_t;
    extern cfunc islower(char_t)                                        -> bool32_t;
    extern cfunc isupper(char_t)                                        -> bool32_t;
    extern cfunc isdigit(char_t)                                        -> bool32_t;
    extern cfunc isxdigit(char_t)                                       -> bool32_t;
    extern cfunc iscntrl(char_t)                                        -> bool32_t;
    extern cfunc isgraph(char_t)                                        -> bool32_t;
    extern cfunc isspace(char_t)                                        -> bool32_t;
    extern cfunc isblank(char_t)                                        -> bool32_t;
    extern cfunc isprint(char_t)                                        -> bool32_t;
    extern cfunc ispunct(char_t)                                        -> bool32_t;
    
    // character manipulation
    
    extern cfunc tolower(char_t)                                        -> char_t;
    extern cfunc toupper(char_t)                                        -> char_t;
    
    // conversion to numeric formats
    
    extern cfunc atof(*const char_t)                                    -> f32;
    extern cfunc atoi(*const char_t)                                    -> i32;
    extern cfunc atol(*const char_t)                                    -> i64;
    extern cfunc strtol(*const char_t, **char_t, i32)                   -> i64;
    extern cfunc strtoul(*const char_t, **char_t, i32)                  -> u64;
    extern cfunc strtof(*const char_t, **char_t, i32)                   -> f32;
    extern cfunc strtod(*const char_t, **char_t, i32)                   -> f64;
    
    // string manipulation
    
    extern cfunc strcpy(*char_t, *const char_t)                         -> *char_t;
    extern cfunc strncpy(*char_t, *const char_t, size_t)                -> *char_t;
    extern cfunc strcat(*char_t, *const char_t)                         -> *char_t;
    extern cfunc strncat(*char_t, *const char_t, size_t)                -> *char_t;
    extern cfunc strxfrm(*char_t, *const char_t, size_t)                -> size_t;
    
    // string examination
    
    extern cfunc strlen(*const char_t)                                  -> size_t;
    extern cfunc strcmp(*const char_t, *const char_t)                   -> i32;
    extern cfunc strncmp(*const char_t, *const char_t)                  -> i32;
    extern cfunc strcoll(*const char_t, *const char_t)                  -> i32;
    extern cfunc strchr(*const char_t, i32)                             -> *char_t;
    extern cfunc strrchr(*const char_t, i32)                            -> *char_t;
    extern cfunc strspn(*const char_t, *const char_t)                   -> size_t;
    extern cfunc strcspn(*const char_t, *const char_t)                  -> size_t;
    extern cfunc strpbrk(*const char_t, *const char_t)                  -> *char_t;
    extern cfunc strstr(*const char_t, *const char_t)                   -> *char_t;
    
    // string memory manipulation
    
    extern cfunc memchr(const_void_ptr_t, i32, size_t)                  -> void_ptr_t;
    extern cfunc memcmp(const_void_ptr_t, const_void_ptr_t, size_t)     -> i32;
    extern cfunc memset(void_ptr_t, i32, size_t)                        -> void_ptr_t;
    extern cfunc memcpy(void_ptr_t, const_void_ptr_t, size_t)           -> void_ptr_t;
    extern cfunc memmove(void_ptr_t, const_void_ptr_t, size_t)          -> void_ptr_t;
    
    
    
}
