
namespace c {

    extern cfunc fopen(*const char_t, *const char_t)                -> *file_t;
    extern cfunc fclose(*file_t)                                    -> i32;
    extern cfunc fflush(*file_t)                                    -> i32;
    
    extern cfunc setbuf(*file_t, *char_t);
    extern cfunc setvbuf(*file_t, *char_t, i32, size_t)             -> i32;
    
    extern cfunc fread(void_ptr_t, size_t, size_t, *file_t)         -> size_t;
    extern cfunc fwrite(const_void_ptr_t, size_t, size_t, *file_t)  -> size_t;
    
    extern cfunc fgetc(*file_t)                                     -> i32;
    extern cfunc fgets(*char_t, i32, *file_t)                       -> *char_t;
    extern cfunc fputc(i32, *file_t)                                -> i32;
    extern cfunc fputs(*char_t, *file_t)                            -> i32;
    
    extern cfunc fscanf(*file_t, *const char_t, ...)                -> i32;
    extern cfunc sscanf(*const char_t, *const char_t, ...)          -> i32;
    
    extern cfunc fprintf(*file_t, *const char_t, ...)               -> i32;
    extern cfunc sprintf(*const char_t, *const char_t, ...)         -> i32;
    
    //...
}