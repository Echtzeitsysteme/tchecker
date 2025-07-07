enum tck_compare_relationship_t {
  STRONG_TIMED_BISIM, /*!< Strong Timed Bisimilarity */
};

#ifdef __cplusplus
extern "C" {
#endif

const void tck_compare(const char * output_filename, 
  const char * first_sysdecl_filename, 
  const char * second_sysdecl_filename,
  tck_compare_relationship_t relationship,
  int * block_size, 
  int * table_size);

#ifdef __cplusplus
}
#endif
