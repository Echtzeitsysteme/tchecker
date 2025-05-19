enum tck_reach_algorithm_t {
  ALGO_REACH,    /*!< Reachability algorithm */
  ALGO_CONCUR19, /*!< Covering reachability algorithm over the local-time zone graph */
  ALGO_COVREACH, /*!< Covering reachability algorithm */
  ALGO_NONE,     /*!< No algorithm */
};

enum tck_reach_certificate_t {
  CERTIFICATE_GRAPH,    /*!< Graph of state-space */
  CERTIFICATE_SYMBOLIC, /*!< Symbolic counter-example */
  CERTIFICATE_CONCRETE, /*!< Concrete counter-example */
  CERTIFICATE_NONE,     /*!< No certificate */
};

enum tck_reach_search_order_t {
  BFS,
  DFS
};

#ifdef __cplusplus
extern "C" {
#endif

const void tck_reach(const char * output_filename, 
  const char * sysdecl_filename, 
  const char * labels, 
  tck_reach_algorithm_t algorithm, 
  tck_reach_search_order_t search_order, 
  tck_reach_certificate_t certificate, 
  int* block_size, 
  int* table_size);

#ifdef __cplusplus
}
#endif
