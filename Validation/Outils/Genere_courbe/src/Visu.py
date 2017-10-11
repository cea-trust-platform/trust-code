#classe decrivant une visu
#****************************************************************************
# Copyright (c) 2015 - 2016, CEA
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************

import sys, os

from Courbe import Courbe

from lib import GestionMessages
from lib import getNomFonction
from lib import extraireMotcleValeur,print_description
from lib import chaine2Ascii
from lib import _accoladeF


dico_plots={"pseudocolor":"Pseudocolor","pseudocolor_with_range":"Pseudocolor","vector":"Vector","blackvector":"Vector","blackvector_with_nb":"Vector","isovaleurs":"Contour","molecule":"Molecule","pseudocolor_with_opacity":"Pseudocolor","histogram":"Histogram"}
list_plot=dico_plots.keys()
list_plot.append('mesh')



def write_insert_text(ficPlot):
	ficPlot.write(r'''def insert_text(ox,oy,oz,size,text):
  # on cree un fichier sonde
  global sondetmp
  sondetmp+=1
  f=open("sondetmp%d.son"%sondetmp,"w")
  f.write("# sonde pour %s\n"%text)
  f.write("# Temps x= %s y= %s z= %s\n"%(ox,oy,oz))
  f.write("# Champ fictif\n# Type POINTS\n")
  f.write("%d %d\n"%(0.,0.))
  f.close()
  res=OpenDatabase("sondetmp%d.son"%sondetmp)
  if 0:
   ok=AddPlot("Mesh","MESH")
   if (ok==0): 1/0
   m=MeshAttributes()
   m.SetLegendFlag(0)
   m.SetPointSizePixels(10)
   SetPlotOptions(m)
   # DrawPlots()
  if 1:
   ok=AddPlot("Label","value")
   if (ok==0): 1/0
   L=LabelAttributes()
   L.SetFormatTemplate(text)
   L.SetHorizontalJustification(1)
   L.SetLegendFlag(0)
   L.SetTextHeight1(size)
   #print dir(L)
   #print L
   SetPlotOptions(L)
   # ok=DrawPlots()
   # if (ok==0): 1/0
  pass

''')
	pass

class Visu:
	'''Classe decrivant une visu telle qu elle devra etre tracee dans le rapport de validation de TRUST.'''

	#constructeur
	def __init__(self, verbose=0, output=''):
		'''Constructeur.'''
		if output=='':
			self.gestMsg = GestionMessages(verbose,'log')
		else:
			self.gestMsg = output
		self.verbose = verbose
		#initialisations
		self.titre     = 'Undefined'
		self.format = 'png'
		self.plot = []
		self.cycles= ""
		self.description = []
		self.inclureDescCourbes = 1
		self.width="15cm"
		self.origine="trio_u"
		self.nb_img_without_newline=-1
		pass

	def is_directname(self, plot):
		nb_args_after_field={"pseudocolor":0,"pseudocolor_with_range":2,"vector":-1,"blackvector":-1,"blackvector_with_nb":-2,"isovaleurs":0,"molecule":1,"pseudocolor_with_opacity":1,"histogram":3}
		nb_args=len(plot)
		nb_args_after=nb_args_after_field[plot[0]]
		if nb_args-3==abs(nb_args_after):
			directname=True
		elif nb_args-5==abs(nb_args_after):
			directname=False
		else:
			if nb_args_after<0: # le dernier argument est facultatif
				if nb_args-3==abs(nb_args_after)-1:
					directname=True
				elif  nb_args-5==abs(nb_args_after)-1:
					directname=False
				else:
					from string import join
					self.gestMsg.ecrire(GestionMessages._ERR, '1,3 4 or 5 parameters expected after %s and not %s'% (plot[0],join(plot)))
					pass
				pass
			else:
				from string import join
				self.gestMsg.ecrire(GestionMessages._ERR, '%d or %d parameters expected after %s and not %s'% (nb_args_after+2,nb_args_after+4,plot[0],join(plot)))
				pass
			pass
		return directname

	#lecture des parametres de la visu dans le fichier de parametres
	def printFichierParametres(self):
		dec='\t'
		print dec,"Visu {"
		dec='\t\t'
		if self.titre != 'Undefined' : print dec,"titre", self.titre
		print_description(self.description,dec)

		if self.format != 'png' : print dec,"format", self.format
		from string import join
		for plot  in self.plot: print dec,join(plot)
		if self.cycles != ""   : print dec,"cycles",self.cycles

		if self.inclureDescCourbes != 1 : print dec,"inclureDescCourbes", self.inclureDescCourbes
		if self.width != "15cm" : print dec,"width", self.width
		if self.origine != "trio_u" : print dec,"origine", self.origine
		if self.nb_img_without_newline!=-1: print dec, "nb_img_without_newline",self.nb_img_without_newline
		dec='\t'
		print dec,"}"
		pass
	def lireParametres(self, fichier):
		'''Lecture des parametres de la visu.'''
		self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
		fin = False
		list_mot=dico_plots.keys()
		list_mot.extend(['mesh','operator','operator_to_all','instruction_visit','normal3d','up3d','zoom3d','zoom2d','query','upaxis','inserttext' ])
		dico=[]
		for mot in list_mot: dico.append(mot)
		dico+=['titre','description','origine','width','origine','cycles','incluredesccourbes','nb_img_without_newline']
		while not fin:
			ligne = fichier.readline()
			if not ligne:
				self.gestMsg.ecrire(GestionMessages._ERR, 'Unexpected end of file. We expect for parameters definition of visualization.')
			ligne = ligne.strip()
			if len(ligne)>0 and ligne[0]!='#':
				motcle,valeur,motcle_lu = extraireMotcleValeur(fichier,ligne, self.gestMsg)
				valeur.strip()
				try:
					valeurT=valeur.split()
					valeurT=[ x.strip() for x in valeurT ]

					valeurT2=[motcle]
					for x in valeurT :valeurT2.append(x)
				except:
					pass

				if motcle==_accoladeF:
					fin = True
				elif motcle == 'nb_img_without_newline':
					self.nb_img_without_newline=int(valeur)
				elif motcle=='titre':
					self.titre = valeur

				elif motcle in list_mot:
					self.plot.append(valeurT2)
				elif motcle=='description':
					self.description.append(valeur)
				elif motcle=='width':
					self.width=valeur
				elif motcle=='origine':
					self.origine=valeur
				elif motcle=='cycles':
					self.cycles+=" "+valeur
				elif motcle=='incluredesccourbes':
					self.inclureDescCourbes = int(valeur)
				else:
					# self.gestMsg.ecrire(GestionMessages._ERR, 'Parameter %s not recognized. List of parameters for visualization: %s ' % (motcle_lu,dico),fichier=fichier)
					self.gestMsg.ecrire_usage(GestionMessages._ERR, 'Visu', dico,motcle_lu,fichier=fichier)
       				if motcle!=_accoladeF and not (motcle in dico): print "Missing something for ",motcle;1/0

	#generation du graphique correspondant a la visu
	def genererGraphe(self, dest, indice,debug_figure):
		'''Generation du graphique correspondant a la visu.'''
		list_data_base=[]
     		for x in self.plot:
			if not(x[1] in list_data_base) and(x[0] in list_plot):
				list_data_base.append(x[1])
				pass
			pass

		self.gestMsg.ecrire(GestionMessages._DEBOG, 'DEBUT %s.%s' % (self.__class__.__name__, getNomFonction()), niveau=15)
		nomFichierPlot = 'visit_%03d.py' % (indice)
		nomFichierPlotComplet = dest + '/'+nomFichierPlot
		ficPlot = open(nomFichierPlotComplet, 'w')
		ficPlot.write('#Fichier de plot correspondant a la visu "%s"\n' % chaine2Ascii(self.titre))
		ficPlot.write('SetDatabaseCorrelationOptions(3,2)\n')
		ficPlot.write('# on change le stride des vecteurs par defaut \n')
		ficPlot.write('vd=VectorAttributes()\nvd.SetUseStride(1)\nSetDefaultPlotOptions(vd)\n')
		ficPlot.write('# on retire le nom du user sur les images \n')
		ficPlot.write('annotation=GetAnnotationAttributes()\nannotation.SetUserInfoFlag(0)\nSetAnnotationAttributes(annotation)\n')
		ficPlot.write('try:\n  execfile(\'config_visit.py\')\n  print "user config loaded"\nexcept:\n  pass\n\nsondetmp=0\n\n')
		for database in list_data_base:

			ficPlot.write('res=OpenDatabase(\"%s\")\n' % (database))
			ficPlot.write('if res==0: print "can\'t read data file %s ";quit()\n'%(database))
			pass
		# a faire CreateDatabaseCorrelation("database",["FTD_all_VEF/post1.lata","FTD_all_VEF/post2.lata"],0) print GetDatabaseCorrelation("database")

		queries_str="\n"
		for plot in self.plot:
			if (plot[0]=="mesh"):
				ficPlot.write('ActivateDatabase(\"%s\")\n'%plot[1])

				if self.cycles:
					ficPlot.write('cycle0=%s\nif (cycle0<0): cycle0+=TimeSliderGetNStates()\n'%self.cycles.split()[0])
				else:
					ficPlot.write('cycle0=TimeSliderGetNStates()-1\n')

				ficPlot.write('SetTimeSliderState(cycle0)\n')
				ficPlot.write('ok=AddPlot(\"Mesh\",\"%s\")\n'%plot[2])
				ficPlot.write('if (ok==0): 1/0\n')

				if (len(plot)==4):
					color=plot[3]
					listcolor=["red","green","blue","black"]
					if color not in listcolor:
						from string import join
						self.gestMsg.ecrire(GestionMessages._ERR, 'color expected in %s ansd not %s'% (listcolor,color))
					ficPlot.write('red=(255,0,0,255);green=(0,255,0,255);black=(0,0,0,255);blue=(0,0,255,255)\n')
					ficPlot.write('m=MeshAttributes()\nm.SetMeshColorSource(1)\nm.SetMeshColor(%s)\nSetPlotOptions(m)\n'%color)


				# ficPlot.write('ok=DrawPlots()\n')
				# ficPlot.write('if (ok==0): 1/0\n')
				pass
			elif (plot[0] in dico_plots.keys()):
				directname=self.is_directname(plot)


				ficPlot.write('ActivateDatabase(\"%s\")\n'%plot[1])
				if self.cycles:
					ficPlot.write('cycle0=%s\nif (cycle0<0): cycle0+=TimeSliderGetNStates()\n'%self.cycles.split()[0])
				else:
					ficPlot.write('cycle0=TimeSliderGetNStates()-1\n')

				ficPlot.write('SetTimeSliderState(cycle0)\n')

				#if (plot[1].split('.')[-1]=="case"):
				if (directname):
					ficPlot.write('ok=AddPlot(\"%s\",\"%s\")\n'%(dico_plots[plot[0]],plot[2]))
					decal=2
				else:
					ficPlot.write('ok=AddPlot(\"%s\",\"%s_%s_%s\")\n'%(dico_plots[plot[0]],plot[3],plot[4],plot[2]))
					decal=4
					pass
				ficPlot.write('if (ok==0): 1/0\n')
				# ficPlot.write('ok=DrawPlots()\n')
				# ficPlot.write('if (ok==0): 1/0\n')
				if (plot[0]=="blackvector"):
					ficPlot.write('vb=VectorAttributes()\nvb.SetColorByMag(0)\nvb.SetUseLegend(0)\n')
					if (len(plot)==2+decal):
						ficPlot.write('vb.SetScale(%s)\n'%plot[decal+1])
						pass
					ficPlot.write('SetPlotOptions(vb)\n')
					pass
				if (plot[0]=="blackvector_with_nb"):
					ficPlot.write('vb=VectorAttributes()\nvb.SetColorByMag(0)\nvb.SetUseLegend(0)\n')
					ficPlot.write('vb.SetUseStride(0)\nvb.SetNVectors(%s)\n'%plot[decal+1])
					if (len(plot)==3+decal):
						ficPlot.write('vb.SetScale(%s)\n'%plot[decal+2])
						pass
					ficPlot.write('SetPlotOptions(vb)\n')
					pass
				if (plot[0]=="vector"):
					if (len(plot)==2+decal):
						ficPlot.write('vb=VectorAttributes()\n')
						ficPlot.write('vb.SetScale(%s)\n'%plot[decal+1])
						ficPlot.write('SetPlotOptions(vb)\n')
						pass
					pass
				if (plot[0]=="pseudocolor_with_opacity"):
					if (len(plot)!=2+decal):
						from string import join
						self.gestMsg.ecrire(GestionMessages._ERR, '3 or 5 parameters expected after %s and not %s'% (plot[0],join(plot)))
						pass
					ficPlot.write('pb=PseudocolorAttributes()\npb.SetOpacity(%s)\npb.SetOpacityType(2)\nSetPlotOptions(pb)\n'%plot[decal+1])

					pass
				if (plot[0]=="pseudocolor_with_range"):
					if (len(plot)!=decal+3):
						from string import join
						self.gestMsg.ecrire(GestionMessages._ERR, '6 parameters expected after %s and not %s'% (plot[0],join(plot)))
						pass
					ficPlot.write('pb=PseudocolorAttributes()\n')
					if (plot[decal+1].upper()!="MIN"):
						ficPlot.write('pb.SetMin(%s)\npb.SetMinFlag(1)\n'%plot[decal+1])
						pass
					if (plot[decal+2].upper()!="MAX"):
						ficPlot.write('pb.SetMax(%s)\npb.SetMaxFlag(1)\n'%plot[decal+2])
						pass
					ficPlot.write('SetPlotOptions(pb)\n')
					pass
				if (plot[0]=="histogram"):
					if (len(plot)!=decal+4):
						from string import join
						self.gestMsg.ecrire(GestionMessages._ERR, '5 or 7 parameters expected after %s and not %s'% (plot[0],join(plot)))
						pass
					ficPlot.write('pb=HistogramAttributes()\n')
					min_user=0
					if (plot[decal+1].upper()!="MIN"):
						ficPlot.write('#pb.specifyRange=1\n')
						ficPlot.write('pb.SetMinFlag(1)\n')
						ficPlot.write('pb.SetMin(%s)\n'%plot[decal+1])
						min_user=1
						pass
					max_user=0
					if (plot[decal+2].upper()!="MAX"):
						ficPlot.write('pb.SetMaxFlag(1)\n')
						ficPlot.write('pb.SetMax(%s)\n'%plot[decal+2])
						max_user=1
						pass
					if (min_user!=max_user):
						from string import join
						self.gestMsg.ecrire(GestionMessages._ERR, 'if you specify the min (or the max) you should specify also the max (or the min) in %s'% (join(plot)))
						pass
					ficPlot.write('pb.SetNumBins(%s)\n'%plot[decal+3])
					ficPlot.write('SetPlotOptions(pb)\n')
					pass
				if (plot[0]=="molecule"):
					if (len(plot)!=decal+2):
						from string import join
						self.gestMsg.ecrire(GestionMessages._ERR, '5 parameters expected after %s and not %s'% (plot[0],join(plot)))
						pass
					ficPlot.write('pb=MoleculeAttributes()\npb.SetRadiusFixed(%s)\nSetPlotOptions(pb)\n'%plot[decal+1])
					pass
			elif (plot[0]=='instruction_visit'):
				for i in range(1,len(plot)):
					ficPlot.write(plot[i]+" ")
				ficPlot.write('\n')
			elif (plot[0]=='normal3d') or (plot[0]=='up3d'):
				ficPlot.write('v=GetView3D()\n')
				if (plot[0]=='normal3d'):
					ficPlot.write('v.viewNormal')
				else:
					ficPlot.write('v.viewUp')
					pass
				ficPlot.write('=(%s,%s,%s)\n'%(plot[1],plot[2],plot[3]))
				ficPlot.write('SetView3D(v)\n');
			elif (plot[0]=="zoom3d"):
				if (len(plot)!=4):
					from string import join
					self.gestMsg.ecrire(GestionMessages._ERR, '3 parameters expected after %s and not %s'% (plot[0],join(plot)))
					pass
				ficPlot.write('v=GetView3D()\nv.SetImagePan(%s,%s)\nv.SetImageZoom(%s)\n'%(plot[1],plot[2],plot[3]))
				ficPlot.write('SetView3D(v)\n')
			elif (plot[0]=="zoom2d"):
				if (len(plot)!=5):
					from string import join
					self.gestMsg.ecrire(GestionMessages._ERR, '4 parameters expected after %s and not %s'% (plot[0],join(plot)))
					pass
				ficPlot.write('v=GetView2D()\nv.SetWindowCoords(%s,%s,%s,%s)\n'%(plot[1],plot[2],plot[3],plot[4]))
				ficPlot.write('SetView2D(v)\n')
			elif (plot[0]=='operator') or (plot[0]=='operator_to_all') :
				all=0
				if plot[0]=='operator_to_all': all=1
				type_op=plot[1].lower()
				list_base={"threeslice":3,"slice":6,"slice2d":6,"slice2d_x":3,"slice2d_y":3,"slice2d_z":3,"clip_1plane":6,"clip_2planes":12,"no_axes":0,"no_triad":0,"no_bounding_box":0,"no_databaseinfo":0,"no_legend":0}
				list=list_base.keys()
				if type_op not in list:
					self.gestMsg.ecrire(GestionMessages._ERR, 'Unknown keyword. We expect for an operator parameter, and not: %s . Expected possible keywords: %s' % (type_op,list))
				if len(plot)-2!=list_base[type_op]:
					self.gestMsg.ecrire(GestionMessages._ERR, 'We expect %d args for operator %s  and not: %s .' % (list_base[type_op],type_op,len(plot)-2))
				if type_op=='slice' or (type_op[:7]=='slice2d'):
					ficPlot.write('AddOperator(\"Slice\",%d)\n'%all)
					ficPlot.write('s=SliceAttributes()\n')
					ficPlot.write('s.SetOriginType(s.Point)\n')
					ficPlot.write('s.SetOriginPoint(%s,%s,%s)\n'%(plot[2],plot[3],plot[4]))
					ficPlot.write('s.SetAxisType(s.Arbitrary)\n')
					print type_op
					if (type_op=="slice2d_x"):
						ficPlot.write('s.SetNormal(-1,0,0)\ns.SetUpAxis(0,1,0)\n')
					elif (type_op=="slice2d_y"):
						ficPlot.write('s.SetNormal(0,-1,0)\ns.SetUpAxis(0,0,1)\n')
					elif (type_op=="slice2d_z"):
						ficPlot.write('s.SetNormal(0,0,1)\ns.SetUpAxis(0,1,0)\n')
					else:
						ficPlot.write('s.SetNormal(%s,%s,%s)\n'%(plot[5],plot[6],plot[7]))
						pass

					if type_op=='slice':
						ficPlot.write('s.SetProject2d(0)\n')
					elif type_op=='slice2d':
						ficPlot.write('s.SetProject2d(1)\n')
						V=None
						for p in self.plot:
							if (p[0]=='upaxis'):
								V=[float(p[1]),float(p[2]),float(p[3])]
								break
							pass
						if V is None:
							if (0): self.gestMsg.ecrire(GestionMessages._ERR, 'You have a slice2d with normal (%s,%s,%s) not equal axis, you must specify UpAxis.'%(plot[5],plot[6],plot[7]))
						else:
							ficPlot.write('s.SetUpAxis(%d,%d,%d)\n'%(V[0],V[1],V[2]))
							pass
						pass

					ficPlot.write('SetOperatorOptions(s,0,%d)\n'%all)
				elif type_op=='threeslice':
					ficPlot.write('AddOperator(\"ThreeSlice\",%d)\n'%all)
					ficPlot.write('s=ThreeSliceAttributes()\n')
					ficPlot.write('s.x,s.y,s.z=%s,%s,%s\n'%(plot[2],plot[3],plot[4]))
					ficPlot.write('SetOperatorOptions(s,0,%d)\n'%all)

				elif type_op=='clip_1plane' or type_op=='clip_2planes' :
					ficPlot.write('AddOperator(\"Clip\",%d)\n'%all)
					ficPlot.write('s=ClipAttributes()\n');
					ficPlot.write('s.plane1Origin=(%s,%s,%s)\n'%(plot[2],plot[3],plot[4]))
					ficPlot.write('s.plane1Normal=(%s,%s,%s)\n'%(plot[5],plot[6],plot[7]))

					if type_op=='clip_2planes' :
						ficPlot.write('s.plane2Origin=(%s,%s,%s)\n'%(plot[8],plot[9],plot[10]))
						ficPlot.write('s.plane2Normal=(%s,%s,%s)\n'%(plot[11],plot[12],plot[13]))
						ficPlot.write('s.SetPlane2Status(1)\n')

						pass
					ficPlot.write('SetOperatorOptions(s,0,%d)\n'%all)
				elif type_op=='no_axes':
					ficPlot.write('annotation=GetAnnotationAttributes()\n')
					ficPlot.write('try:\n annotation.GetAxes2D().SetVisible(0)\n')
					ficPlot.write(' annotation.GetAxes3D().SetVisible(0)\n')
					ficPlot.write(' annotation.GetAxes2D().SetVisible(0)\n')
					ficPlot.write('except:\n')
					ficPlot.write(' print \"option no_axes incompatible with this version of visit\"\n pass\n')
					ficPlot.write('SetAnnotationAttributes(annotation)\n')
				elif type_op=='no_triad':
					ficPlot.write('annotation=GetAnnotationAttributes()\n')
					ficPlot.write('try:\n annotation.GetAxes2D().SetVisible(0)\n')
					ficPlot.write(' annotation.GetAxes3D().SetTriadFlag(0)\n')
					ficPlot.write('except:\n')
					ficPlot.write(' print \"option no_triad incompatible with this version of visit\"\n pass\n')
					ficPlot.write('SetAnnotationAttributes(annotation)\n')
				elif type_op=='no_bounding_box':
					ficPlot.write('annotation=GetAnnotationAttributes()\n')

				        ficPlot.write('try: annotation.GetAxes3D().SetBboxFlag(0)\n')
					ficPlot.write('except:\n')
					ficPlot.write(' print \"option no_boundig_box incompatible with this version of visit\"\n pass\n')
					ficPlot.write('SetAnnotationAttributes(annotation)\n')
				elif type_op=='no_databaseinfo':
					ficPlot.write('annotation=GetAnnotationAttributes()\n')

				        ficPlot.write('annotation.SetDatabaseInfoFlag(0)\n')
					ficPlot.write('SetAnnotationAttributes(annotation)\n')
				elif type_op=='no_legend':
					if all==1:
						ficPlot.write('annotation=GetAnnotationAttributes()\n')

						ficPlot.write('annotation.SetLegendInfoFlag(0)\n')
						ficPlot.write('SetAnnotationAttributes(annotation)\n')
					else:
						raise Exception("operator legend not implemented, use operator_to_all legend")

					pass
				else:
					print "Missing code"
					1/0
					pass

				# ficPlot.write('ok=DrawPlots()\n')
				# ficPlot.write('if (ok==0): 1/0\n')
				pass
			elif (plot[0]=="query"):
				if (len(plot)!=3):
					from string import join
					self.gestMsg.ecrire(GestionMessages._ERR, '2 parameters expected after %s and not %s'% (plot[0],join(plot)))
					pass
				query=plot[1].replace('_',' ')
				# le Query(...,1) permet d'avoir le "bon" query c.a.d  restreint
				query_str='  ok=Query("%s",1)\n  if (ok==0): 1/0\n'%query+('  res=GetQueryOutputValue()\n  f=open("%s","a")\n  f.write(str(time)+" ")\n  if type(res)==tuple:\n   for ii in range(len(res)): f.write(str(res[ii])+" ")\n  else:\n   f.write(str(res))\n  f.write("\\n\")\n  f.close()\n'%plot[2])
				# remise a zero du fichier
				ficPlot.write('f=open("%s","w")\nf.close()\n'%plot[2])

				#print plot
				#1/0
				queries_str+=query_str
			elif (plot[0]=="inserttext"):
				if (len(plot)<6):
					self.gestMsg.ecrire(GestionMessages._ERR, '>=5 parameters (ox,oy,oz,taille,texte) expected after %s and not %s'% (plot[0],' '.join(plot[1:])))
					pass
				write_insert_text(ficPlot)

				ficPlot.write('insert_text(%s,%s,%s,%s,"%s")\n'%(plot[1],plot[2],plot[3],plot[4],' '.join(plot[5:])))

			elif (plot[0]=="upaxis"):
				1+1
				# on a deja fait le travail
			else:
				self.gestMsg.ecrire(GestionMessages._ERR, 'Unknown keyword. Error during interpretation for visualization of %s.' %(plot[0]))
				pass
			pass
		# fin ecriture de SetActiveTime de l'image et de la session
		ficPlot.write('ok=DrawPlots()\n')
		ficPlot.write('if (ok==0): 1/0\n')
		ficPlot.write('s=SaveWindowAttributes()\n')
		if self.format=='png':
			format=4
		else:
			print self.format +"not supported yet."
			1/0
		ficPlot.write('s.SetFormat('+str(format)+')\n')
		ficPlot.write('SetSaveWindowAttributes(s)\n')
		cycles=self.cycles.split()
		if (len(cycles)==0):
			ficPlot.write('cycles=[str(TimeSliderGetNStates()-1)]\n')
		else:
			ficPlot.write('cycles=%s\n'%str(cycles))
			#ficPlot.write('cycles=[%s'%self.cyles[0])
			#for x in cycles[1:] : ficPlot.write(x+',')
			#ficPlot.write(']\n')
		ficPlot.write('num=-1\nfor state in cycles:\n')
		ficPlot.write('  num+=1\n')
		# permet de faire [-1] comme en python !!
		ficPlot.write('  state_reel=eval(state)\n  if (state_reel<0): state_reel+=TimeSliderGetNStates()\n')
		ficPlot.write('  ok=SetTimeSliderState(state_reel)\n')
		ficPlot.write('  if (ok==0): 1/0\n')
		ficPlot.write('  Query("Time")\n  time=GetQueryOutputValue()\n')
		ficPlot.write(queries_str)
		ficPlot.write('  name=SaveWindow()\n')
		ficPlot.write('  print name\n')
		self.fichierGraphique='fic_%03d'%(indice)
		self.fichierGraphiqueComplet = dest + '/' + self.fichierGraphique
		ficPlot.write('''  cmd="mv \\\""+name+"\\\" ''')
		ficPlot.write(self.fichierGraphiqueComplet+'_%d.'+self.format+'\"%num\n')
		ficPlot.write('  import os\n  print cmd\n  os.system(cmd)\n')
		ficPlot.write('SaveSession(\"visit_%03d.session\")\n'%(indice))
		ficPlot.write('quit()\n')
		ficPlot.close()
		#print "ici", debug_figure,type(debug_figure),indice
		#debug_figure=None
		if (debug_figure==None) or (debug_figure==-1) or (debug_figure==indice):
			pipe = os.system('visit $TRUST_VISIT_NP -cli -s %s -nowin -noconfig ' % nomFichierPlotComplet)




	#---------------------------------------------
	# Methodes d'affichage des infos
	def afficherParametres(self):
		'''Affichage des parametres de la visu.'''
		self.gestMsg.ecrire(GestionMessages._INFO, '\tTitre=  %s' % self.titre)
		pass
	def modifie_pour_comparaison(self,old_path):
		'''modification pour ajouter courbes anciennes versions'''

		newvisus=[]
		if self.origine != 'trio_u':
			return newvisus
		for i in range(len(old_path)):
			old=old_path[i]
			ajout_str=str(i)
			if i==0: ajout_str=""
			from copy import copy,deepcopy
			newvisu=copy(self)
			newvisu.plot=deepcopy(self.plot)
			if newvisu.titre!="Undefined":
				newvisu.titre+=" old"+ajout_str
				pass

			for x in newvisu.plot:
				#if (( x[0]=="mesh")or( x[0]=="vector") or( x[0]=="blackvector")or( x[0]=="pseudocolor")   or( x[0]=="isovaleurs") ):
				if x[0] in list_plot:
					x[1]=old+x[1]
					pass
				if x[0]=="query":
					x[2]=old+x[2]
					pass
				pass

			newvisus.append(newvisu)

			newvisu_delta=copy(self)
			plots=deepcopy(self.plot)
			newvisu_delta.plot=[]
			if newvisu.titre!="Undefined":
				newvisu_delta.titre+=" delta"+ajout_str
				pass
			for x in plots:

				if ( x[0]=="mesh"):
					newvisu_delta.plot.append(x)
					newx=deepcopy(x)
					newx[1]=old+x[1]
					newvisu_delta.plot.append(newx)


				# elif (( x[0]=="vector") or(  x[0]=="blackvector") or( x[0]=="pseudocolor")   or( x[0]=="isovaleurs") ):
				elif (x[0] in dico_plots.keys()):
					type="DefineScalarExpression";
					if ( x[0]=="vector") or(  x[0]=="blackvector")or (  x[0]=="blackvector_with_nb"): type="DefineVectorExpression";
					if (self.is_directname(x)) :
						print "on ne fait pas de delta sur ",x
					else:
						var=x[3]+"_"+x[4]+"_"+x[2]
						newvisu_delta.plot.append(["instruction_visit",type+"(\"delta_"+var+"\",\""+var+"-pos_cmfe(<"+old+x[1]+"[0]id:"+var+">,"+x[2]+",1000.)\")"])
						# newvisu_delta.plot.append(["instruction_visit",type+"(\"delta_"+var+"\",\""+var+"-conn_cmfe(<"+old+x[1]+"[0]id:"+var+">,"+x[2]+")\")"])
						newx=deepcopy(x)
						newx[3]="delta_"+x[3]
						newvisu_delta.plot.append(newx)
						pass
				elif (x[0]=="query"):
					newx=deepcopy(x)
					newx[2]=x[2]+"_delta"+ajout_str
					newvisu_delta.plot.append(newx)
				else:
					newvisu_delta.plot.append(x)
					pass
				pass
			newvisus.append(newvisu_delta)
			pass
		print "on a ajoute",len(newvisus) ,"visu(s)"
		return newvisus
	pass


