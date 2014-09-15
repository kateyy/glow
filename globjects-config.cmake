
# GLOBJECTS_FOUND

# GLOBJECTS_LIBRARIES
# GLOBJECTS_INCLUDES

# GLOBJECTS_LIBRARY
# GLOBJECTS_LIBRARY_RELEASE
# GLOBJECTS_LIBRARY_DEBUG
# GLOBJECTS_INCLUDE_DIR

include(FindPackageHandleStandardArgs)

if(CMAKE_CURRENT_LIST_FILE)
    get_filename_component(GLOBJECTS_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
endif()

file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" ENVPROGRAMFILES)
file(TO_CMAKE_PATH "$ENV{GLOBJECTS_DIR}" ENVGLOBJECTS_DIR)

set(GLOBJECTS_INCLUDES "")
set(GLOBJECTS_LIBRARIES "")

set(LIB_PATHS   
    ${GLOBJECTS_DIR}/build
    ${GLOBJECTS_DIR}/build/Release
    ${GLOBJECTS_DIR}/build/Debug
    ${GLOBJECTS_DIR}/build-release
    ${GLOBJECTS_DIR}/build-debug
    ${ENVGLOBJECTS_DIR}/lib
    ${GLOBJECTS_DIR}/lib
    ${ENVPROGRAMFILES}/globjects/lib
    /usr/lib
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    /usr/lib64
    /usr/local/lib64
    /sw/lib64
    /opt/local/lib64
)

macro (find LIB_NAME HEADER)
    set(HINT_PATHS ${ARGN})
    
    string(TOUPPER ${LIB_NAME} LIB_NAME_UPPER)

    find_path(${LIB_NAME_UPPER}_INCLUDE_DIR ${HEADER}
        ${ENVGLOBJECTS_DIR}/include
        ${ENVGLOBJECTS_DIR}/source/${LIB_NAME}/include
        ${GLOBJECTS_DIR}/include
        ${GLOBJECTS_DIR}/source/${LIB_NAME}/include
        ${ENVPROGRAMFILES}/globjects/include
        /usr/include
        /usr/local/include
        /sw/include
        /opt/local/include
        DOC "The directory where ${header} resides")

    find_library(${LIB_NAME_UPPER}_LIBRARY_RELEASE
        NAMES ${LIB_NAME}
        PATHS ${HINT_PATHS}
        DOC "The ${LIB_NAME} library")
    find_library(${LIB_NAME_UPPER}_LIBRARY_DEBUG
        NAMES ${LIB_NAME}d
        PATHS ${HINT_PATHS}
        DOC "The ${LIB_NAME} debug library")
    
    if(${LIB_NAME_UPPER}_LIBRARY_RELEASE AND ${LIB_NAME_UPPER}_LIBRARY_DEBUG)
        set(${LIB_NAME_UPPER}_LIBRARY "optimized" ${${LIB_NAME_UPPER}_LIBRARY_RELEASE} "debug" ${${LIB_NAME_UPPER}_LIBRARY_DEBUG})
    elseif(${LIB_NAME_UPPER}_LIBRARY_RELEASE)
        set(${LIB_NAME_UPPER}_LIBRARY ${${LIB_NAME_UPPER}_LIBRARY_RELEASE})
    elseif(${LIB_NAME_UPPER}_LIBRARY_DEBUG)
        set(${LIB_NAME_UPPER}_LIBRARY ${${LIB_NAME_UPPER}_LIBRARY_DEBUG})
    endif()

    # DEBUG
    # message("${LIB_NAME_UPPER}_INCLUDE_DIR     = ${${LIB_NAME_UPPER}_INCLUDE_DIR}")
    # message("${LIB_NAME_UPPER}_LIBRARY_RELEASE = ${${LIB_NAME_UPPER}_LIBRARY_RELEASE}")
    # message("${LIB_NAME_UPPER}_LIBRARY_DEBUG   = ${${LIB_NAME_UPPER}_LIBRARY_DEBUG}")
    # message("${LIB_NAME_UPPER}_LIBRARY         = ${${LIB_NAME_UPPER}_LIBRARY}")

    list(APPEND GLOBJECTS_INCLUDES ${${LIB_NAME_UPPER}_INCLUDE_DIR})
    list(APPEND GLOBJECTS_LIBRARIES ${${LIB_NAME_UPPER}_LIBRARY})
endmacro()

find(globjects globjects/globjects_api.h ${LIB_PATHS})

# add dependencies
find_package(glbinding REQUIRED)
list(APPEND GLOBJECTS_INCLUDES ${GLBINDING_INCLUDES})
list(APPEND GLOBJECTS_LIBRARIES ${GLBINDING_LIBRARIES})

# DEBUG
# message("GLOBJECTS_INCLUDES  = ${GLOBJECTS_INCLUDES}")
# message("GLOBJECTS_LIBRARIES = ${GLOBJECTS_LIBRARIES}")

find_package_handle_standard_args(GLOBJECTS DEFAULT_MSG GLOBJECTS_LIBRARIES GLOBJECTS_INCLUDES)
mark_as_advanced(GLOBJECTS_FOUND)
