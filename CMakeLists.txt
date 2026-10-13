#-----------------------------------------------------------------------------
# Umicom Healthcare Module
# File: CMakeLists.txt
#
# PURPOSE:
#   Compose a deliberately non-clinical product identity over the canonical
#   Umicom Framework runtime. All reusable healthcare state, safety, policy,
#   workflow, persistence, and GUI widgets must be Framework-owned.
#
# STATUS:
#   Foundation scaffold only. No patient records, clinical decisions,
#   prescriptions or external fulfilment are implemented or permitted.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)

project(
    UmicomHealthcareModule
    VERSION 0.1.0
    DESCRIPTION "Thin Umicom Healthcare product composition"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(UMICOM_HEALTHCARE_BUILD_CONSOLE
       "Build the non-clinical Healthcare verification console"
       ON)

include(CTest)
include(GNUInstallDirs)

# The parent suite owns Framework construction. A separately installed SDK
# can provide the same public target to this independent module repository.
if(NOT TARGET Umicom::Framework)
    find_package(UmicomFramework CONFIG REQUIRED)
endif()
if(NOT TARGET Umicom::application)
    message(FATAL_ERROR
        "Umicom Healthcare requires Framework target Umicom::application")
endif()

# Standalone installations may not have the suite's warning/sanitizer helpers.
if(NOT COMMAND umicom_apply_warnings)
    function(umicom_apply_warnings target)
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4)
        else()
            target_compile_options(${target} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endfunction()
endif()
if(NOT COMMAND umicom_apply_sanitizers)
    function(umicom_apply_sanitizers target)
        # The owning superproject configures sanitizer policies, if enabled.
    endfunction()
endif()

# Do not place the patient's domain model inside this product library.
# Product source is limited to identity and composition of Framework services.
add_library(umicom_healthcare_module STATIC
    src/application.c
)
add_library(Umicom::healthcare_module ALIAS umicom_healthcare_module)

target_include_directories(umicom_healthcare_module PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(umicom_healthcare_module PUBLIC
    Umicom::Framework
    Umicom::application
)
umicom_apply_warnings(umicom_healthcare_module)
umicom_apply_sanitizers(umicom_healthcare_module)

if(UMICOM_HEALTHCARE_BUILD_CONSOLE)
    add_executable(umicom-healthcare-console src/console/main.c)
    target_link_libraries(umicom-healthcare-console PRIVATE
        Umicom::healthcare_module)
    umicom_apply_warnings(umicom-healthcare-console)
    umicom_apply_sanitizers(umicom-healthcare-console)
endif()

if(BUILD_TESTING)
    add_executable(healthcare-application-test tests/test_application.c)
    target_link_libraries(healthcare-application-test PRIVATE
        Umicom::healthcare_module)
    umicom_apply_warnings(healthcare-application-test)
    umicom_apply_sanitizers(healthcare-application-test)
    add_test(NAME healthcare.module.profile COMMAND healthcare-application-test)
    set_tests_properties(healthcare.module.profile PROPERTIES
        TIMEOUT 30 LABELS "healthcare;scaffold;regression")
endif()

install(TARGETS umicom_healthcare_module
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
if(TARGET umicom-healthcare-console)
    install(TARGETS umicom-healthcare-console
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES application.umicom.yaml README.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/applications/healthcare)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    install(FILES LICENSE
        DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/healthcare)
endif()

# The suite's UmicomSharedNativeApplications.cmake discovers this module's
# umicom_healthcare_module target and native_executable manifest property.
# It constructs a GTK4 preview host using Framework-owned UI components.
# It must not be interpreted as a clinically functional healthcare GUI.
message(STATUS "Umicom Healthcare scaffold version: ${PROJECT_VERSION}")
message(STATUS "Clinical functionality: disabled pending Framework implementation and clinical assurance")
