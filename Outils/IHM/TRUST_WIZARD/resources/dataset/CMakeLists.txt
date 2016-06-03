
SET(_res_FILES
    data_ex1.json
    ex_pb_thermo_hydrau.json
    ex_pb_hydrau.json
    ex_pb_solid.json
#    ex_pb_solide2.json
    gauthier.json
    Maillage_test.med
 )

INSTALL(FILES ${_res_FILES} DESTINATION ${SALOME_TRUST_WIZARD_INSTALL_RES_DATA})
