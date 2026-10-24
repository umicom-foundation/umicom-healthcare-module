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
    add_executable(umicom-healthcare-console src/console/main.c src/console/access_demo.c)
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

# Historical preview integration is retained below for review. The module-owned
# native target declared later now hosts Framework's synthetic access workspace;
# the generic preview remains available to other application modules.
# The suite's UmicomSharedNativeApplications.cmake discovers this module's
# umicom_healthcare_module target and native_executable manifest property.
# It constructs a GTK4 preview host using Framework-owned UI components.
# It must not be interpreted as a clinically functional healthcare GUI.
message(STATUS "Umicom Healthcare scaffold version: ${PROJECT_VERSION}")
message(STATUS "Clinical functionality: disabled pending Framework implementation and clinical assurance")

# This frontend belongs to the module and hosts Framework's synthetic workspace.
# The suite detects an existing native target and retains its own generic preview
# source for other products; no product-specific domain logic is copied here.
set(_umicom_healthcare_native_default OFF)
if(TARGET Umicom::healthcare_access_gtk4)
    set(_umicom_healthcare_native_default ON)
endif()
option(UMICOM_HEALTHCARE_BUILD_GTK4 "Build the synthetic Healthcare GTK workspace"
    ${_umicom_healthcare_native_default})
unset(_umicom_healthcare_native_default)
# A disabled module-owned frontend must not be replaced by the suite preview.
set_property(TARGET umicom_healthcare_module PROPERTY
    UMICOM_NATIVE_FRONTEND_DISABLED TRUE)
if(UMICOM_HEALTHCARE_BUILD_GTK4)
    set_property(TARGET umicom_healthcare_module PROPERTY
        UMICOM_NATIVE_FRONTEND_DISABLED FALSE)
    if(NOT TARGET Umicom::healthcare_access_gtk4)
        message(FATAL_ERROR "Healthcare GTK requires Framework healthcare_access_gtk4; enable Framework GTK support or disable UMICOM_HEALTHCARE_BUILD_GTK4")
    endif()
    add_executable(umicom-healthcare src/gtk/main.c)
    # Product composition reuses Framework-owned record and access surfaces.
    target_sources(umicom-healthcare PRIVATE src/gtk/workspace.c)
    if(TARGET Umicom::record_editor_gtk4)
        target_link_libraries(umicom-healthcare PRIVATE Umicom::record_editor_gtk4)
        target_compile_definitions(umicom-healthcare PRIVATE UMICOM_HEALTHCARE_RECORD_EDITOR_AVAILABLE=1)
    endif()
    target_link_libraries(umicom-healthcare PRIVATE
        Umicom::healthcare_module Umicom::healthcare_access_gtk4)
    if(TARGET Umicom::form_record_editor_gtk4 AND TARGET Umicom::healthcare_forms)
        target_link_libraries(umicom-healthcare PRIVATE Umicom::form_record_editor_gtk4 Umicom::healthcare_forms)
        target_compile_definitions(umicom-healthcare PRIVATE UMICOM_HEALTHCARE_FORM_EDITOR_AVAILABLE=1)
    endif()
    if(TARGET Umicom::form_record_import_gtk4 AND TARGET Umicom::healthcare_forms)
        target_sources(umicom-healthcare PRIVATE src/gtk/profile_transfer.c)
        target_link_libraries(umicom-healthcare PRIVATE Umicom::form_record_import_gtk4 Umicom::healthcare_forms)
        target_compile_definitions(umicom-healthcare PRIVATE UMICOM_HEALTHCARE_PROFILE_IMPORT_AVAILABLE=1)
    endif()
    umicom_apply_warnings(umicom-healthcare)
    umicom_apply_sanitizers(umicom-healthcare)
    install(TARGETS umicom-healthcare
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT Healthcare)
endif()

# Qualify native composition separately from portable domain exercises.
if(BUILD_TESTING AND TARGET umicom-healthcare)
    add_executable(healthcare-native-workspace-test tests/test_native_workspace.c src/gtk/workspace.c)
    target_include_directories(healthcare-native-workspace-test PRIVATE src/gtk)
    target_link_libraries(healthcare-native-workspace-test PRIVATE Umicom::healthcare_access_gtk4)
    if(TARGET Umicom::record_editor_gtk4)
        target_link_libraries(healthcare-native-workspace-test PRIVATE Umicom::record_editor_gtk4)
        target_compile_definitions(healthcare-native-workspace-test PRIVATE UMICOM_HEALTHCARE_RECORD_EDITOR_AVAILABLE=1)
    endif()
    if(TARGET Umicom::form_record_editor_gtk4 AND TARGET Umicom::healthcare_forms)
        target_link_libraries(healthcare-native-workspace-test PRIVATE Umicom::form_record_editor_gtk4 Umicom::healthcare_forms)
        target_compile_definitions(healthcare-native-workspace-test PRIVATE UMICOM_HEALTHCARE_FORM_EDITOR_AVAILABLE=1)
    endif()
    if(TARGET Umicom::form_record_import_gtk4 AND TARGET Umicom::healthcare_forms)
        target_sources(healthcare-native-workspace-test PRIVATE src/gtk/profile_transfer.c)
        target_link_libraries(healthcare-native-workspace-test PRIVATE Umicom::form_record_import_gtk4 Umicom::healthcare_forms)
        target_compile_definitions(healthcare-native-workspace-test PRIVATE UMICOM_HEALTHCARE_PROFILE_IMPORT_AVAILABLE=1)
    endif()
    umicom_apply_warnings(healthcare-native-workspace-test)
    umicom_apply_sanitizers(healthcare-native-workspace-test)
    add_test(NAME healthcare.module.native_workspace COMMAND healthcare-native-workspace-test)
    set_tests_properties(healthcare.module.native_workspace PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
        ENVIRONMENT "G_DEBUG=fatal-warnings" LABELS "healthcare;gtk4;composition;regression")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(healthcare-native-workspace-test)
    endif()
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(healthcare.module.native_workspace TARGETS healthcare-native-workspace-test)
    endif()
endif()

# Product qualification adds namespace/key checks and the host close policy.
# Imported and sibling Framework targets carry their capability in the public
# compile interface. A discovery variable belongs to the directory that called
# find_package and may be absent here. Retain the earlier gate for review.
if(FALSE)
    if(BUILD_TESTING AND TARGET umicom-healthcare AND TARGET Umicom::form_record_import_gtk4 AND TARGET Umicom::healthcare_forms AND SQLite3_FOUND)
    endif()
endif()
set(_healthcare_transfer_has_sqlite FALSE)
if(TARGET Umicom::data)
    get_target_property(_healthcare_transfer_data_definitions Umicom::data INTERFACE_COMPILE_DEFINITIONS)
    if("UMICOM_HAS_SQLITE=1" IN_LIST _healthcare_transfer_data_definitions)
        set(_healthcare_transfer_has_sqlite TRUE)
    endif()
endif()
if(BUILD_TESTING AND TARGET umicom-healthcare AND TARGET Umicom::form_record_import_gtk4 AND TARGET Umicom::healthcare_forms AND _healthcare_transfer_has_sqlite)
    add_executable(healthcare-profile-transfer-test tests/test_profile_transfer.c src/gtk/profile_transfer.c src/gtk/workspace.c)
    target_include_directories(healthcare-profile-transfer-test PRIVATE src/gtk)
    target_link_libraries(healthcare-profile-transfer-test PRIVATE Umicom::healthcare_access_gtk4 Umicom::form_record_import_gtk4 Umicom::healthcare_forms)
    target_compile_definitions(healthcare-profile-transfer-test PRIVATE UMICOM_HEALTHCARE_PROFILE_IMPORT_AVAILABLE=1)
    if(TARGET Umicom::record_editor_gtk4)
        target_link_libraries(healthcare-profile-transfer-test PRIVATE Umicom::record_editor_gtk4)
        target_compile_definitions(healthcare-profile-transfer-test PRIVATE UMICOM_HEALTHCARE_RECORD_EDITOR_AVAILABLE=1)
    endif()
    if(TARGET Umicom::form_record_editor_gtk4)
        target_link_libraries(healthcare-profile-transfer-test PRIVATE Umicom::form_record_editor_gtk4)
        target_compile_definitions(healthcare-profile-transfer-test PRIVATE UMICOM_HEALTHCARE_FORM_EDITOR_AVAILABLE=1)
    endif()
    umicom_apply_warnings(healthcare-profile-transfer-test)
    umicom_apply_sanitizers(healthcare-profile-transfer-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(healthcare-profile-transfer-test)
    endif()
    foreach(_healthcare_transfer_case IN ITEMS valid key namespace schema close owned)
        add_test(NAME healthcare.module.profile_transfer.${_healthcare_transfer_case}
            COMMAND healthcare-profile-transfer-test ${_healthcare_transfer_case} "${CMAKE_CURRENT_BINARY_DIR}/profile-transfer-fixtures")
        set_tests_properties(healthcare.module.profile_transfer.${_healthcare_transfer_case} PROPERTIES
            TIMEOUT 45 SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings" LABELS "healthcare;gtk4;synthetic;record-bundle;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(healthcare.module.profile_transfer.${_healthcare_transfer_case} TARGETS healthcare-profile-transfer-test)
        endif()
    endforeach()
    unset(_healthcare_transfer_case)
endif()

unset(_healthcare_transfer_has_sqlite)
unset(_healthcare_transfer_data_definitions)

# Shared maintenance and provenance stay optional for older installed SDKs.
# Every target compiling workspace.c receives the same capability definition.
if(TARGET Umicom::database_maintenance_gtk4 AND TARGET Umicom::record_transfer_inspector_gtk4)
    get_target_property(_healthcare_storage_definitions Umicom::data INTERFACE_COMPILE_DEFINITIONS)
    if(BUILD_TESTING AND TARGET umicom-healthcare AND "UMICOM_HAS_SQLITE=1" IN_LIST _healthcare_storage_definitions)
        add_executable(healthcare-storage-tools-test tests/test_storage_tools.c src/gtk/workspace.c)
        target_include_directories(healthcare-storage-tools-test PRIVATE src/gtk)
        target_link_libraries(healthcare-storage-tools-test PRIVATE Umicom::healthcare_access_gtk4)
        umicom_apply_warnings(healthcare-storage-tools-test)
        umicom_apply_sanitizers(healthcare-storage-tools-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(healthcare-storage-tools-test)
        endif()
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/healthcare-storage-fixtures")
        foreach(_healthcare_storage_case IN ITEMS composition acknowledgement unchecked-copy unchecked-receipt health backup recover receipt close retained-child revoke source-retained null missing-receipt)
            add_test(NAME healthcare.module.storage_tools.${_healthcare_storage_case}
                COMMAND healthcare-storage-tools-test ${_healthcare_storage_case} "${CMAKE_CURRENT_BINARY_DIR}/healthcare-storage-fixtures")
            set_tests_properties(healthcare.module.storage_tools.${_healthcare_storage_case} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
                ENVIRONMENT "G_DEBUG=fatal-warnings" LABELS "healthcare;gtk4;synthetic;backup;provenance;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(healthcare.module.storage_tools.${_healthcare_storage_case} TARGETS healthcare-storage-tools-test)
            endif()
        endforeach()
    endif()
    foreach(_healthcare_storage_target IN ITEMS umicom-healthcare healthcare-native-workspace-test healthcare-profile-transfer-test healthcare-storage-tools-test)
        if(TARGET ${_healthcare_storage_target})
            target_sources(${_healthcare_storage_target} PRIVATE src/gtk/storage_tools.c)
            target_link_libraries(${_healthcare_storage_target} PRIVATE Umicom::database_maintenance_gtk4 Umicom::record_transfer_inspector_gtk4)
            target_compile_definitions(${_healthcare_storage_target} PRIVATE UMICOM_HEALTHCARE_STORAGE_TOOLS_AVAILABLE=1)
        endif()
    endforeach()
endif()
install(FILES docs/synthetic-storage-tools.html DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/applications/healthcare/docs" COMPONENT Healthcare)
unset(_healthcare_storage_definitions)
unset(_healthcare_storage_case)
unset(_healthcare_storage_target)
