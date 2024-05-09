install(
    TARGETS bitdoku_exe
    RUNTIME COMPONENT bitdoku_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
