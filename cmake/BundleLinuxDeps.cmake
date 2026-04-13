if (NOT DEFINED TARGET_FILE OR NOT DEFINED TARGET_DIR)
    message(FATAL_ERROR "TARGET_FILE y TARGET_DIR son requeridos")
endif()

set(LIB_OUTPUT_DIR "${TARGET_DIR}/lib")
file(REMOVE_RECURSE "${LIB_OUTPUT_DIR}")
file(MAKE_DIRECTORY "${LIB_OUTPUT_DIR}")

execute_process(
    COMMAND ldd "${TARGET_FILE}"
    RESULT_VARIABLE LDD_RESULT
    OUTPUT_VARIABLE LDD_OUTPUT
    ERROR_VARIABLE LDD_ERROR
)

if (NOT LDD_RESULT EQUAL 0)
    message(FATAL_ERROR "ldd fallo para ${TARGET_FILE}: ${LDD_ERROR}")
endif()

set(SKIP_LIB_NAMES
    linux-vdso.so.1
    libc.so.6
    libm.so.6
    libpthread.so.0
    libdl.so.2
    librt.so.1
    libutil.so.1
    libresolv.so.2
    ld-linux-x86-64.so.2
)

string(REPLACE "\n" ";" LDD_LINES "${LDD_OUTPUT}")
foreach(LDD_LINE IN LISTS LDD_LINES)
    string(STRIP "${LDD_LINE}" LDD_LINE)
    if (LDD_LINE STREQUAL "")
        continue()
    endif()

    unset(LIB_PATH)
    if (LDD_LINE MATCHES "=> ([^ ]+) ")
        set(LIB_PATH "${CMAKE_MATCH_1}")
    elseif (LDD_LINE MATCHES "^(/[^ ]+)")
        set(LIB_PATH "${CMAKE_MATCH_1}")
    endif()

    if (NOT DEFINED LIB_PATH)
        continue()
    endif()

    if (LIB_PATH STREQUAL "not")
        continue()
    endif()

    if (NOT EXISTS "${LIB_PATH}")
        continue()
    endif()

    get_filename_component(LIB_NAME "${LIB_PATH}" NAME)
    list(FIND SKIP_LIB_NAMES "${LIB_NAME}" SKIP_INDEX)
    if (NOT SKIP_INDEX EQUAL -1)
        continue()
    endif()

    file(REAL_PATH "${LIB_PATH}" LIB_REAL_PATH)
    if (NOT EXISTS "${LIB_REAL_PATH}")
        continue()
    endif()

    get_filename_component(LIB_REAL_NAME "${LIB_REAL_PATH}" NAME)
    file(COPY "${LIB_REAL_PATH}" DESTINATION "${LIB_OUTPUT_DIR}")

    if (NOT LIB_NAME STREQUAL LIB_REAL_NAME)
        configure_file("${LIB_REAL_PATH}" "${LIB_OUTPUT_DIR}/${LIB_NAME}" COPYONLY)
    endif()
endforeach()