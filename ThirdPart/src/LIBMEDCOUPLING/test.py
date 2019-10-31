import MEDLoader
a=MEDLoader.MEDFileData("Kernel_Post_MED/MED_docond_0000.med")
print(a.getMeshes()[0].getName())
