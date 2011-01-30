SET (TEST_C_LIST
  t-test1
  t-test2
  t-test3
  t-test4
  t-subject
)

SET (TEST_CPP_LIST
  #t-opcodes
  #t-opcodes_by_pid
  #t-opcodes_by_cmd
)

FOREACH (test ${TEST_C_LIST})
  SET ("T_${test}_SOURCES" "${test}.c")
ENDFOREACH (test)

FOREACH (test ${TEST_CPP_LIST})
  SET ("T_${test}_SOURCES" "${test}.cpp")
ENDFOREACH (test)

#CREATE EXECUTABLE AND ADD IT AS TEST
FOREACH (test ${TEST_C_LIST})
  ADD_EXECUTABLE(${test} ${T_${test}_SOURCES})
  ADD_TEST(${test} ${test})
ENDFOREACH (test)

FOREACH (test ${TEST_CPP_LIST})
  ADD_EXECUTABLE(${test} ${T_${test}_SOURCES})
  ADD_TEST(${test} ${test})
ENDFOREACH (test)

# Link the librairies
FOREACH (test ${TEST_C_LIST})
  #TARGET_LINK_LIBRARIES(${test} SOMETHING)
ENDFOREACH (test)

FOREACH (test ${TEST_CPP_LIST})
  #TARGET_LINK_LIBRARIES(${test} SOMETHING)
ENDFOREACH (test)
