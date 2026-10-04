# End-to-end tests: run the built `calc` binary and check its stdout, stderr
# and exit code. Each test calls run_cli.cmake with:
#   ARGS      ;-separated command-line arguments (optional)
#   INPUT     text piped to stdin (optional)
#   STDOUT    regex stdout must match (optional)
#   STDERR    regex stderr must match (optional)
#   EXIT_CODE expected exit code (default 0)
function(calc_cli_test name)
  cmake_parse_arguments(T "" "INPUT;STDOUT;STDERR;EXIT_CODE" "ARGS" ${ARGN})
  if(NOT DEFINED T_EXIT_CODE)
    set(T_EXIT_CODE 0)
  endif()
  # Arguments travel as one |-separated string; run_cli.cmake splits them.
  string(REPLACE ";" "|" joined_args "${T_ARGS}")
  add_test(NAME cli.${name}
    COMMAND ${CMAKE_COMMAND}
      -DCALC=$<TARGET_FILE:calc>
      "-DARGS=${joined_args}"
      "-DINPUT=${T_INPUT}"
      "-DEXPECT_STDOUT=${T_STDOUT}"
      "-DEXPECT_STDERR=${T_STDERR}"
      -DEXPECT_EXIT_CODE=${T_EXIT_CODE}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/run_cli.cmake)
endfunction()

calc_cli_test(one_shot ARGS "2*(3+4)" STDOUT "^14\n$")
calc_cli_test(one_shot_fraction ARGS "1/4" STDOUT "^0.25\n$")
calc_cli_test(one_shot_negative ARGS "-2^2" STDOUT "^-4\n$")
calc_cli_test(one_shot_functions ARGS "sqrt(16) + max(1, 2)" STDOUT "^6\n$")
calc_cli_test(syntax_error ARGS "2 + )" STDERR "syntax error: unexpected '\\)'\n  2 \\+ \\)\n      \\^" EXIT_CODE 1)
calc_cli_test(division_by_zero ARGS "1/0" STDERR "division by zero" EXIT_CODE 1)
calc_cli_test(too_many_args ARGS "1;2" STDERR "expected one expression" EXIT_CODE 2)
calc_cli_test(version ARGS "--version" STDOUT "^calc ${PROJECT_VERSION}\n$")
calc_cli_test(help ARGS "--help" STDOUT "Usage: calc")
calc_cli_test(stdin_single INPUT "1+1\n" STDOUT "^2\n$")
calc_cli_test(stdin_variables INPUT "x = 3\nx * 2\n\nans + 1\n" STDOUT "^3\n6\n7\n$")
calc_cli_test(stdin_error_continues INPUT "1/0\n2+2\n" STDOUT "^4\n$" STDERR "division by zero" EXIT_CODE 1)
