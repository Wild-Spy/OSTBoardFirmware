#define FE_1(WHAT, X) WHAT(X)
#define FE_2(WHAT, X, ...) WHAT(X)FE_1(WHAT, __VA_ARGS__)
#define FE_3(WHAT, X, ...) WHAT(X)FE_2(WHAT, __VA_ARGS__)
#define FE_4(WHAT, X, ...) WHAT(X)FE_3(WHAT, __VA_ARGS__)
#define FE_5(WHAT, X, ...) WHAT(X)FE_4(WHAT, __VA_ARGS__)
//... repeat as needed

#define GET_MACRO(_1,_2,_3,_4,_5,NAME,...) NAME
#define FOR_EACH(action,...) \
  GET_MACRO(__VA_ARGS__,FE_5,FE_4,FE_3,FE_2,FE_1)(action,__VA_ARGS__)

#define RUN_TEST_GROUP(name)  run_##name();
#define DEFINE_TEST_GROUP(name)  void run_##name(void);

//#define RUN_TESTS()   run_mem_tests();

void run_mem_tests();
void run_nvm_tests();
void run_ds3232sn_tests();
void run_rule_runner_tests();

//#define RUN_TESTS()   FOR_EACH(RUN_TEST_GROUP, rule_runner_tests)
#define RUN_TESTS()   FOR_EACH(RUN_TEST_GROUP, mem_tests, nvm_tests, ds3232sn_tests, rule_runner_tests)