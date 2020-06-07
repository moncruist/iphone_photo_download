#[=======================================================================[.rst:
FindLibMTP
------------

Find libmtp headers and library.

Imported Targets
^^^^^^^^^^^^^^^^

``LibMTP::LibMTP``
  The libmtp library, if found.

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables in your project:

``LibMTP_FOUND``
  true if (the requested version of) libmtp is available.
``LibMTP_VERSION``
  the version of libmtp.
``LibMTP_LIBRARIES``
  the libraries to link against to use libmtp.
``LibMTP_INCLUDE_DIRS``
  where to find the libmtp headers.
``LibMTP_COMPILE_OPTIONS``
  this should be passed to target_compile_options(), if the
  target is not used for linking

#]=======================================================================]
find_package(pkg-config QUIET)
pkg_check_modules(PC_LIBMTP QUIET libmtp)

set(LibMTP_COMPILE_OPTIONS ${PC_LIBMTP_CFLAGS_OTHER})
set(LibMTP_VERSION ${PC_LIBMTP_VERSION})

find_path(LibMTP_INCLUDE_DIR
  NAMES
    libmtp.h
  HINTS
    ${PC_LIBMTP_INCLUDE_DIRS})

find_library(LibMTP_LIBRARY
  NAMES
    mtp
  HINTS
    ${PC_LIBMTP_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibMTP
  FOUND_VAR
    LibMTP_FOUND
  REQUIRED_VARS
    LibMTP_LIBRARY
    LibMTP_INCLUDE_DIR
  VERSION_VAR
    LibMTP_VERSION)

if(LibMTP_FOUND AND NOT TARGET LibMTP::LibMTP)
  add_library(LibMTP::LibMTP UNKNOWN IMPORTED)
  set_target_properties(LibMTP::LibMTP PROPERTIES
    IMPORTED_LOCATION "${LibMTP_LIBRARY}"
    INTERFACE_COMPILE_OPTIONS "${LibMTP_COMPILE_OPTIONS}"
    INTERFACE_INCLUDE_DIRECTORIES "${LibMTP_INCLUDE_DIR}")
endif()

mark_as_advanced(LibMTP_LIBRARY LibMTP_INCLUDE_DIR)

if(LibMTP_FOUND)
  set(LibMTP_LIBRARIES ${LibMTP_LIBRARY})
  set(LibMTP_INCLUDE_DIRS ${LibMTP_INCLUDE_DIR})
endif()