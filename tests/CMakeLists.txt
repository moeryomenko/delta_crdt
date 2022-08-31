function(crdt_test)
  cmake_parse_arguments(CRDT_TEST "" "NAME" "" ${ARGN})

  string(TOUPPER ${CRDT_TEST_NAME} SUIT)
  string(CONCAT SUIT_TEST ${SUIT} "_SUIT")
  string(CONCAT CRDT_TEST_SRC ${CRDT_TEST_NAME} ".cpp")

  add_executable(${CRDT_TEST_NAME} "${CRDT_TEST_SRC}")
  target_link_libraries(${CRDT_TEST_NAME} crdtxx rapidcheck)
  add_test(NAME ${SUIT_TEST} COMMAND ${CRDT_TEST_NAME})
endfunction()

crdt_test(NAME "grow_counter_test")

crdt_test(NAME "pn_counter_test")

crdt_test(NAME "gset_test")

crdt_test(NAME "awor_set_test")

crdt_test(NAME "mvreg_test")
