#ifdef __cplusplus
extern "C" {    
#endif

const char * tck_syntax_check_syntax(const char * filename);
const char * tck_syntax_to_dot(const char * filename);
const char * tck_syntax_to_json(const char * filename);
const char * tck_syntax_create_synchronized_product(const char * filename, const char * process_name);

#ifdef __cplusplus
}
#endif
