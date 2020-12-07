import os

os.system("rm src/*/*.Zones*")
os.system("rm src/*/*DEC*")
os.system("rm src/*/*PAR*")
os.system("rm src/*/*SEQ*")

#POISEUILLE ALIGNED
os.system("cd src/poiseuille_pdf_align && trust -clean")

#POISEUILLE TILTED
os.system("cd src/poiseuille_pdf_tilt_11_dir && trust -clean")
os.system("cd src/poiseuille_pdf_tilt_11_hybrid && trust -clean")
os.system("cd src/poiseuille_pdf_tilt_11_multi_dir && trust -clean")
os.system("cd src/poiseuille_pdf_tilt_30_dir && trust -clean")
os.system("cd src/poiseuille_pdf_tilt_30_hybrid && trust -clean")
os.system("cd src/poiseuille_pdf_tilt_30_multi_dir && trust -clean")
os.system("cd src/poiseuille_pdf_tilt_45_dir && trust -clean")
os.system("cd src/poiseuille_pdf_tilt_45_hybrid && trust -clean")
os.system("cd src/poiseuille_pdf_tilt_45_multi_dir && trust -clean")

#TAYLOR COUETTE
os.system("cd src/taylor_couette_pdf_dir && trust -clean")
os.system("cd src/taylor_couette_pdf_hybrid && trust -clean")
os.system("cd src/taylor_couette_pdf_multi_dir && trust -clean")

#CYLINDER
os.system("cd src/cylinder_pdf_static_re_20_dir && trust -clean")
os.system("cd src/cylinder_pdf_static_re_20_hybrid && trust -clean")
os.system("cd src/cylinder_pdf_static_re_20_multi_dir && trust -clean")
os.system("cd src/cylinder_pdf_static_re_100_dir && trust -clean")
os.system("cd src/cylinder_pdf_static_re_100_hybrid && trust -clean")
os.system("cd src/cylinder_pdf_static_re_100_multi_dir && trust -clean")
os.system("cd src/cylinder_pdf_rotate_re_20_dir && trust -clean")
os.system("cd src/cylinder_pdf_rotate_re_20_hybrid && trust -clean")
os.system("cd src/cylinder_pdf_rotate_re_20_multi_dir && trust -clean")
os.system("cd src/cylinder_pdf_rotate_re_100_dir && trust -clean")
os.system("cd src/cylinder_pdf_rotate_re_100_hybrid && trust -clean")
os.system("cd src/cylinder_pdf_rotate_re_100_multi_dir && trust -clean")