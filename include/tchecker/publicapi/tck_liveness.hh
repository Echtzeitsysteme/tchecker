enum tck_liveness_algorithm_t {
  ALGO_COUVSCC, /*!< Couvreur's SCC algorithm */
  ALGO_NDFS,    /*!< Nested DFS algorithm */
  ALGO_NONE,    /*!< No algorithm */
};

enum tck_liveness_certificate_t {
  CERTIFICATE_GRAPH,    /*!< Graph of state-space */
  CERTIFICATE_SYMBOLIC, /*!< Symbolic counter-example */
  CERTIFICATE_NONE,     /*!< No certificate */
};

enum tck_liveness_search_order_t {
  BFS,
  DFS
};

#ifdef __cplusplus
extern "C" {
#endif

const void tck_liveness(const char * output_filename, 
    const char * sysdecl_filename, 
    const char * labels, 
    tck_liveness_algorithm_t algorithm, 
    tck_liveness_search_order_t search_order, 
    tck_liveness_certificate_t 
    certificate, 
    int* block_size, 
    int* table_size);

#ifdef __cplusplus
}
#endif
