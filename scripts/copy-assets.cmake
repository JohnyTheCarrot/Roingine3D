if (NOT DEFINED ASSETS_SRC)
    message(FATAL_ERROR "ASSETS_SRC not defined")
endif ()
if (NOT DEFINED ASSETS_DST)
    message(FATAL_ERROR "ASSETS_DST not defined")
endif ()
if (NOT DEFINED HASH_FILE)
    message(FATAL_ERROR "HASH_FILE not defined")
endif ()

function(print_status MSG)
    message(STATUS "${MSG}")
endfunction()

set(HASH_FILE_NEW "${HASH_FILE}.new")

if (EXISTS ${HASH_FILE})
    # Compare the two hash files
    file(READ ${HASH_FILE} old_hash_content)
    file(READ ${HASH_FILE_NEW} new_hash_content)
    if (old_hash_content STREQUAL new_hash_content)
        print_status("Assets hash unchanged. Skipping copy.")
        return()
    endif ()
else ()
    print_status("No existing hash file found. Will copy assets.")
endif ()

print_status("Copying assets")

execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SRC} ${ASSETS_DST}
)

execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${HASH_FILE_NEW} ${HASH_FILE}
)
