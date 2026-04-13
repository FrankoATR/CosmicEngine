if (NOT DEFINED TARGET_FILE_NAME OR NOT DEFINED TARGET_DIR)
    message(FATAL_ERROR "TARGET_FILE_NAME y TARGET_DIR son requeridos")
endif()

set(LAUNCHER_PATH "${TARGET_DIR}/launch.sh")

set(LAUNCHER_EXECUTABLE_NAME "${TARGET_FILE_NAME}")
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/linux-launcher.in"
    "${LAUNCHER_PATH}"
    @ONLY
    NEWLINE_STYLE UNIX
)

execute_process(COMMAND chmod +x "${LAUNCHER_PATH}")