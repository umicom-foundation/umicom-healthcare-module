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
#   Copyright (c) 2026 Umicom Foundation
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

# Attach the Framework-owned synthetic registry as an optional domain
# capability. The registry's source and public API live in Framework only.
# The suite places Healthcare at applications/healthcare; the standalone
# project places it at application. Both layouts are explicitly supported.
if(NOT TARGET Umicom::healthcare_core)
    set(_umicom_healthcare_core_module
        "${CMAKE_CURRENT_SOURCE_DIR}/../../framework/cmake/UmicomHealthcareCore.cmake")
    if(NOT EXISTS "${_umicom_healthcare_core_module}")
        set(_umicom_healthcare_core_module
            "${CMAKE_CURRENT_SOURCE_DIR}/../framework/cmake/UmicomHealthcareCore.cmake")
    endif()
    if(NOT EXISTS "${_umicom_healthcare_core_module}")
        message(FATAL_ERROR
            "Umicom Healthcare requires the Framework-owned Healthcare Core. "
            "Verify the Framework submodule in the parent checkout.")
    endif()
    include("${_umicom_healthcare_core_module}")
    unset(_umicom_healthcare_core_module)
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
    Umicom::healthcare_core
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


# The product-only regression uses the Framework patient registry directly.
# No medical state or workflow implementation is added to this module.
if(BUILD_TESTING)
    add_executable(healthcare-synthetic-journey-test
        tests/test_synthetic_journey.c)
    target_link_libraries(healthcare-synthetic-journey-test PRIVATE
        Umicom::healthcare_module)
    umicom_apply_warnings(healthcare-synthetic-journey-test)
    umicom_apply_sanitizers(healthcare-synthetic-journey-test)
    add_test(NAME healthcare.module.synthetic_journey
        COMMAND healthcare-synthetic-journey-test)
    set_tests_properties(healthcare.module.synthetic_journey PROPERTIES
        TIMEOUT 30 LABELS "healthcare;synthetic;regression")
endif()

# Verify product composition of Framework intake and administrative routing.
# No medical decision or routing implementation is duplicated in this module.
if(BUILD_TESTING)
    add_executable(healthcare-synthetic-flow-test
        tests/test_synthetic_flow.c)
    target_link_libraries(healthcare-synthetic-flow-test PRIVATE
        Umicom::healthcare_module)
    umicom_apply_warnings(healthcare-synthetic-flow-test)
    umicom_apply_sanitizers(healthcare-synthetic-flow-test)
    add_test(NAME healthcare.module.synthetic_flow
        COMMAND healthcare-synthetic-flow-test)
    set_tests_properties(healthcare.module.synthetic_flow PROPERTIES
        TIMEOUT 30 LABELS "healthcare;synthetic;regression")
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
