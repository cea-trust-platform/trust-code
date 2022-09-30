# -*- coding: utf-8 -*-
"""
Authors : Elie Saikali & Adrien Bruneton

  _____  _____         _____
 |_   _|/ ____|       / ____|
   | | | |      ___  | |      ___
   | | | |     / _ \ | |     / _ \
  _| |_| |____| (_) || |____| (_) |
 |_____|\_____|\___/  \_____|\___/


ICoCo python supervisor V2 : Play with the posrosity field
"""

class TRUSTICoCo():
    """
    TRUSTICoCoclass which implements the RunICoCo method
    """
    @classmethod
    def RunICoCo(self):
        """
        Execute ...
        """
        import medcoupling as mc
        import trusticoco as ti
        import time

        print ("Using ICoCo version",ti.ICOCO_VERSION)
        pbT = ti.ProblemTrio()
        pbT.name = "TRUST"
        pbT.setDataFile("Poreux_stab_VDF.data")
        pbT.initialize()

        porosity = mc.ICoCoMEDDoubleField()

        print("InputFieldsNames : ",pbT.getInputFieldsNames())
        print("OutputFieldsNames : ",pbT.getOutputFieldsNames())

        def run(pb):
            """
            Internal method for RunICoCo. It defines the time looping scheme.
            """
            stop = False # Does the Problem want to stop ?
            t = 0.0

            init = True
            while not stop:
                dt,stop = pbT.computeTimeStep()
                if stop: 
                	return

                pbT.initTimeStep(dt)
                
                pbT.getInputMEDDoubleFieldTemplate("POROSITY_INN", porosity)
                if init :
                	zmesh = porosity.getMCField().getMesh()
                	bary_cart = zmesh.computeCellCenterOfMass()
                	xx = bary_cart[:,0]
                	
                zfld = porosity.getMCField().getArray()

				# 1-0.5*(x>0.2)*(x<0.8)
                for i in range (zfld.getNumberOfTuples()):
                	if t < 25:
                		if xx[i] > 0.2 and xx[i] < 0.8:
                			zfld[i] = 0.5
                		else:
                			zfld[i] = 1
                	elif t < 75:
                		if xx[i] > 0.4 and xx[i] < 1.2:
                			zfld[i] = 0.5
                		else:
                			zfld[i] = 1
                	elif t < 150:
                		if xx[i] > 0.6 and xx[i] < 1.6:
                			zfld[i] = 0.5
                		else:
                			zfld[i] = 1
                	else:
                		if xx[i] > 1 and xx[i] < 1.6:
                			zfld[i] = 0.5
                		else:
                			zfld[i] = 1
                    
                pbT.setInputMEDDoubleField("POROSITY_INN",porosity )
                t = pbT.presentTime() # time av_totalanced

                # Main time loop:
                ok = pbT.solveTimeStep()
                t = pbT.presentTime() # time av_totalanced

                # iterate or stop ?
                if (not ok): # The resolution failed, try with a new time interval.
                    pbT.abortTimeStep()
                else: # The resolution was successful, validate and go to the next time step.
                    pbT.validateTimeStep()

                init = False
                print("=================================================================")

            stat = pbT.isStationary()
            if (stat):
                stop = True

        run(pbT)
        pbT.terminate()

if __name__ == "__main__":
    TRUSTICoCo().RunICoCo()
