include(GoogleTest)

function(crdt_test)
    cmake_parse_arguments(CRDT_TEST "" "NAME" "" ${ARGN})

    string(TOUPPER ${CRDT_TEST_NAME} SUIT)
    string(CONCAT SUIT_TEST ${SUIT} "_SUIT")
    string(CONCAT CRDT_TEST_SRC ${CRDT_TEST_NAME} ".cpp")

    add_executable(${CRDT_TEST_NAME} "${CRDT_TEST_SRC}")
    target_link_libraries(
        ${CRDT_TEST_NAME}
        PRIVATE delta_crdt GTest::gtest_main
    )
    gtest_discover_tests(${CRDT_TEST_NAME})
endfunction()

crdt_test(NAME "grow_counter_test")

crdt_test(NAME "pn_counter_test")

crdt_test(NAME "gset_test")

crdt_test(NAME "awor_set_test")

crdt_test(NAME "mvreg_test")

crdt_test(NAME "lwwreg_test")

crdt_test(NAME "awor_map_test")

crdt_test(NAME "lww_map_test")

crdt_test(NAME "dwflag_test")

crdt_test(NAME "ewflag_test")

crdt_test(NAME "causal_counter_test")

crdt_test(NAME "rwor_set_test")

crdt_test(NAME "rwor_map_test")
