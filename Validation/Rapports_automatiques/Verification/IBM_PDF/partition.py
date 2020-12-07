import os


#POISEUILLE ALIGNED
os.system("cd src/poiseuille_pdf_align && trust -partition poiseuille_pdf_align.data")

#POISEUILLE TILTED
os.system("cd src/poiseuille_pdf_tilt_11_dir && trust -partition poiseuille_pdf_tilt_11_dir.data")
os.system("cd src/poiseuille_pdf_tilt_11_hybrid && trust -partition poiseuille_pdf_tilt_11_hybrid.data")
os.system("cd src/poiseuille_pdf_tilt_11_multi_dir && trust -partition poiseuille_pdf_tilt_11_multi_dir.data")
os.system("cd src/poiseuille_pdf_tilt_30_dir && trust -partition poiseuille_pdf_tilt_30_dir.data")
os.system("cd src/poiseuille_pdf_tilt_30_hybrid && trust -partition poiseuille_pdf_tilt_30_hybrid.data")
os.system("cd src/poiseuille_pdf_tilt_30_multi_dir && trust -partition poiseuille_pdf_tilt_30_multi_dir.data")
os.system("cd src/poiseuille_pdf_tilt_45_dir && trust -partition poiseuille_pdf_tilt_45_dir.data")
os.system("cd src/poiseuille_pdf_tilt_45_hybrid && trust -partition poiseuille_pdf_tilt_45_hybrid.data")
os.system("cd src/poiseuille_pdf_tilt_45_multi_dir && trust -partition poiseuille_pdf_tilt_45_multi_dir.data")

#TAYLOR COUETTE
os.system("cd src/taylor_couette_pdf_dir && trust -partition taylor_couette_pdf_dir.data")
os.system("cd src/taylor_couette_pdf_hybrid && trust -partition taylor_couette_pdf_hybrid.data")
os.system("cd src/taylor_couette_pdf_multi_dir && trust -partition taylor_couette_pdf_multi_dir.data")

#CYLINDER
os.system("cd src/cylinder_pdf_static_re_20_dir && trust -partition cylinder_pdf_static_re_20_dir.data")
os.system("cd src/cylinder_pdf_static_re_20_hybrid && trust -partition cylinder_pdf_static_re_20_hybrid.data")
os.system("cd src/cylinder_pdf_static_re_20_multi_dir && trust -partition cylinder_pdf_static_re_20_multi_dir.data")
os.system("cd src/cylinder_pdf_static_re_100_dir && trust -partition cylinder_pdf_static_re_100_dir.data")
os.system("cd src/cylinder_pdf_static_re_100_hybrid && trust -partition cylinder_pdf_static_re_100_hybrid.data")
os.system("cd src/cylinder_pdf_static_re_100_multi_dir && trust -partition cylinder_pdf_static_re_100_multi_dir.data")
os.system("cd src/cylinder_pdf_rotate_re_20_dir && trust -partition cylinder_pdf_rotate_re_20_dir.data")
os.system("cd src/cylinder_pdf_rotate_re_20_hybrid && trust -partition cylinder_pdf_rotate_re_20_hybrid.data")
os.system("cd src/cylinder_pdf_rotate_re_20_multi_dir && trust -partition cylinder_pdf_rotate_re_20_multi_dir.data")
os.system("cd src/cylinder_pdf_rotate_re_100_dir && trust -partition cylinder_pdf_rotate_re_100_dir.data")
os.system("cd src/cylinder_pdf_rotate_re_100_hybrid && trust -partition cylinder_pdf_rotate_re_100_hybrid.data")
os.system("cd src/cylinder_pdf_rotate_re_100_multi_dir && trust -partition cylinder_pdf_rotate_re_100_multi_dir.data")