import unittest

class trusticoco_test(unittest.TestCase):

    def test_simple(self):
        """ Test import of modules and one function in each """
        import medcoupling as mc
        import trusticoco_V2 as ti
        pbT = ti.ProblemTrio()
        pbT.setDataFile("test_conduc.data")

        f = ti.MEDField()   # @TODO@ should be mc.MEDField() in the future
        f.setName("toto")
        self.assertEqual(f.getName(), "toto")

    def test_small_run(self):
        """ Execute a small run """
        import trusticoco_V2 as ti
        import medcoupling as mc

        pbT = ti.ProblemTrio()
        pbT.name = "TRUST"
        pbT.setDataFile("test_conduc.data")
        pbT.initialize()

        def run(pb, tmax):
            # pb.initialize()
            stop = False
            t= 0
            while (t < tmax) and (not stop):
                dt, stop = pb.computeTimeStep()
                if stop:
                    return
                pb.initTimeStep(dt)
                # List of available fields:
                print("out",pb.getOutputFieldsNames())
                print("in",pb.getInputFieldsNames())

                #
                # Play with fields typed as MEDField:
                #
                fl_asmf = ti.MEDField()        ## @TODO@ should be mc.MEDField() ...
                pb.getOutputMEDFieldAsMF("PVOL_ELEM_dom", fl_asmf)
                print(fl_asmf.getName())
                toto_asmf = ti.MEDField()        ## @TODO@ should be mc.MEDField() ...
                pb.getInputMEDFieldTemplateAsMF("pvol", toto_asmf)
                totomc1 = toto_asmf.getField()
                print(totomc1, toto_asmf.getField())
                totomc1 += 1    # From MEDCoupling
                pb.setInputMEDFieldAsMF("pvol",toto_asmf)

                #
                # Play with fields typed directly as MEDCouplingFieldDouble (nicer in Python ...)
                #
                fl_asmc = pb.getOutputMEDField("PVOL_ELEM_dom")
                print(fl_asmc.getName())
                totomc2 = pb.getInputMEDFieldTemplate("pvol")
                print(totomc2)
                totomc2 += 1
                #pb.setInputMEDField("pvol",totomc2) ## @TODO@ NOT POSSIBLE (yet!)
                totomf2 = ti.MEDField(totomc2)       ## @TODO@ should be mc.MEDField() ...
                pb.setInputMEDFieldAsMF("pvol",totomf2)

                # Main time loop:
                ok = pb.solveTimeStep()
                if ok:
                    pb.validateTimeStep()
                    t = pb.presentTime()
                    if pb.isStationary():
                        print("pb stationary -> stop")
                        stop = True
                else:
                    pb.abortTimeStep()
                    dt2, stop2 = pb.computeTimeStep()
                    print("pb abortTimeStep", dt, dt2)
                    assert dt != dt2

        run(pbT,8.e-5)
        pbT.terminate()

if __name__ == "__main__":
    unittest.main()
