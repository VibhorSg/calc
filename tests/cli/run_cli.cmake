# Runs ${CALC} with ${ARGS} and ${INPUT} on stdin, then checks the results.
string(RANDOM LENGTH 12 suffix)
set(input_file "${CMAKE_CURRENT_BINARY_DIR}/cli_input_${suffix}.txt")
string(REPLACE "\\n" "\n" input_text "${INPUT}")
file(WRITE "${input_file}" "${input_text}")

string(REPLACE "|" ";" args "${ARGS}")

execute_process(
  COMMAND "${CALC}" ${args}
  INPUT_FILE "${input_file}"
  OUTPUT_VARIABLE out
  ERROR_VARIABLE err
  RESULT_VARIABLE code)
file(REMOVE "${input_file}")

set(failed FALSE)
if(NOT code EQUAL EXPECT_EXIT_CODE)
  message(SEND_ERROR "exit code: expected ${EXPECT_EXIT_CODE}, got ${code}")
  set(failed TRUE)
endif()
if(NOT "${EXPECT_STDOUT}" STREQUAL "")
  string(REPLACE "\\n" "\n" pattern "${EXPECT_STDOUT}")
  if(NOT out MATCHES "${pattern}")
    message(SEND_ERROR "stdout did not match /${EXPECT_STDOUT}/")
    set(failed TRUE)
  endif()
endif()
if(NOT "${EXPECT_STDERR}" STREQUAL "")
  string(REPLACE "\\n" "\n" pattern "${EXPECT_STDERR}")
  if(NOT err MATCHES "${pattern}")
    message(SEND_ERROR "stderr did not match /${EXPECT_STDERR}/")
    set(failed TRUE)
  endif()
endif()
if(failed)
  message(FATAL_ERROR "--- stdout ---\n${out}--- stderr ---\n${err}")
endif()
