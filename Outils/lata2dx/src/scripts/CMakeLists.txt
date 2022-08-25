#
# Install python scripts
#
if (loadercpp)
  if (loaderpython)
    install(FILES Lata_to_med.py   DESTINATION lib)
    install(FILES Lata_to_case.py   DESTINATION lib)
  endif()
endif()
