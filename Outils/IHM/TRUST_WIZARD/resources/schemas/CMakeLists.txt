
SET(_res_FILES
    bound_cond_def.json
    bc_thermo_hydrau.json
    bc_hydrau.json
    bc_solid.json
    domain.json
    equations.json
    medium.json
    thermo_hydrau_pb.json
    hydrau_pb.json
    solid_pb.json
    schema_composed.json
    schema_leaf.json
    simple_schema2.json
    trust_schema.json
)

INSTALL(FILES ${_res_FILES} DESTINATION ${SALOME_TRUST_WIZARD_INSTALL_RES_DATA})
