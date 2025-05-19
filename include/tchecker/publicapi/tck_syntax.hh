#ifdef __cplusplus
extern "C" {    
#endif

const void tck_syntax_check_syntax(const char * output_filename, const char * sysdecl_filename);
const void tck_syntax_to_dot(const char * output_filename, const char * sysdecl_filename);
const void tck_syntax_to_json(const char * output_filename, const char * sysdecl_filename);
const void tck_syntax_create_synchronized_product(const char * output_filename, const char * sysdecl_filename, const char * new_system_name);

#ifdef __cplusplus
}
#endif
