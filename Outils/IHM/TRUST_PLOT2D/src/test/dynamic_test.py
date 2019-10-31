import unittest
from trust_plot2d.TrustFiles import BuildFromPath, SonPOINTFile, SonSEGFile, DTEVFile
import numpy as np
import os

class DynamicUnitTest(unittest.TestCase):
    BASE_DIR = "data"

    def __init__(self, methodName='runTest'):
        unittest.TestCase.__init__(self, methodName=methodName)
        # When running from the build/insall directory:
        if not os.path.isdir(self.BASE_DIR):
            rootDir = os.environ.get("TRUST_PLOT2D_ROOT_DIR")
            self.BASE_DIR = os.path.join(rootDir, "bin", "salome", "test")

    def touch(self, fname, times=None):
        """ Touch file """
        from time import sleep
        if os.path.isfile(fname):   s = "a"
        else:                       s = "wa"
        with file(fname, s):
            os.utime(fname, times)
        sleep(0.001)  # Python might run so fast that two consecutive touches produce an equal timestamp

    def almostEqualPointArr(self, l_ref, l_tst, prec=1.0e-8):
        if isinstance(l_ref, list):
            l_ref = np.array(l_ref)
        if isinstance(l_tst, list):
            l_tst = np.array(l_tst)
        self.assertEqual(l_ref.shape, l_tst.shape)
        self.assertTrue(abs(np.sum(l_tst-l_ref)) < prec)

    def setUp(self):
        import tempfile
        self.tmpDir = tempfile.mkdtemp(prefix="evol_test")

    def tearDown(self):
        import shutil
        shutil.rmtree(self.tmpDir, False)

    def testTrustFileStatus(self):
        from time import sleep
        refFile = os.path.join(self.tmpDir, "ref.stop")
        fPath = os.path.join(self.BASE_DIR, "long.dt_ev")
        self.touch(refFile)
        self.touch(fPath)
        f = BuildFromPath(fPath, refFile)
        self.assertTrue(isinstance(f, DTEVFile))
        self.assertEqual("reset", f._queryAndUpdateStatus())
        self.assertEqual("same", f._queryAndUpdateStatus())
        self.touch(refFile)
        self.assertEqual("reset", f._queryAndUpdateStatus())
        fPath = os.path.join(self.tmpDir, "long_INVALIDNAME.dt_ev")
        f = BuildFromPath(fPath, refFile)
        self.assertEqual("invalid", f._queryAndUpdateStatus())
        self.touch(fPath)
        self.assertEqual("reset", f._queryAndUpdateStatus())
        self.assertEqual("same", f._queryAndUpdateStatus())
        with open(fPath, "w") as fo:
            fo.write("toto")
        self.assertEqual("append", f._queryAndUpdateStatus())

    def testTrustFileSonPOINTFile_header(self):
        fPath = os.path.join(self.BASE_DIR, "long_SONDE_PRESSION.son")
        f = BuildFromPath(fPath, None)
        self.assertTrue(isinstance(f, SonPOINTFile))
        self.assertEqual("PRESSION", f._field)
        self.assertEqual(1, f._ncompo)
        self.assertEqual(2, f._dim)
        self.almostEqualPointArr([[0.13, 0.105], [0.13, 0.115]], f._points)
        fPath = os.path.join(self.BASE_DIR, "long_SONDE_PRESSION_3D.son")
        f = BuildFromPath(fPath, None)
        self.assertTrue(isinstance(f, SonPOINTFile))
        self.assertEqual("PRESSION", f._field)
        self.assertEqual(1, f._ncompo)
        self.assertEqual(3, f._dim)
        self.almostEqualPointArr([[0.13, 0.105,0.1], [0.13, 0.115,0.2]], f._points)
        fPath = os.path.join(self.BASE_DIR, "long_SONDE_VITESSE.son")
        f = BuildFromPath(fPath, None)
        self.assertTrue(isinstance(f, SonPOINTFile))
        self.assertEqual("VITESSE", f._field)
        self.assertEqual(2, f._ncompo)
        self.assertEqual(2, f._dim)
        self.almostEqualPointArr([[1.4e-01,1.05e-01],[1.4e-01, 1.15e-01]], f._points)
        # Test getEntries()
        entr = f.getEntries(); entr.sort()
        self.assertEqual(["VITESSE_X (X=0.14,Y=0.105)", "VITESSE_X (X=0.14,Y=0.115)", "VITESSE_Y (X=0.14,Y=0.105)", "VITESSE_Y (X=0.14,Y=0.115)"], entr)

    def testTrustFileSonSEGFile_header(self):
        fPath = os.path.join(self.BASE_DIR, "long_V.son")
        f = BuildFromPath(fPath, None)
        self.assertTrue(isinstance(f, SonSEGFile))
        self.assertEqual("VITESSE", f._field)
        self.assertEqual(2, f._ncompo)
        self.assertEqual(2, f._dim)
        ref_pts = [[1.30000000e-01, 0.00000000e+00], [1.30000000e-01, 2.02020202e-03], [1.30000000e-01, 4.04040404e-03],
                   [1.30000000e-01, 6.06060606e-03]]
        self.almostEqualPointArr(ref_pts, f._points)
        start, end = [[0.130000, 0.000000]], [[0.130000, 0.200000]]
        self.almostEqualPointArr(start, [f._start])
        self.almostEqualPointArr(end, [f._end])
        self.assertEqual("y", f._orient)
        # Test getEntries()
        entr = f.getEntriesSeg(); entr.sort()
        self.assertEqual(["VITESSE_X (SEG [X=0.13,Y=0] -> [X=0.13,Y=0.2])", "VITESSE_Y (SEG [X=0.13,Y=0] -> [X=0.13,Y=0.2])"], entr)
        # Test x axis value computation:
        f._computeXAxis()
        ref = np.array(ref_pts)[:, 1]
        self.assertTrue(abs(np.sum(f._xaxis-ref)) < 1.0e-8)
        # Cheat a bit and test curvilinear
        f._orient = "curvi."
        f._computeXAxis()
        self.assertTrue(abs(np.sum(f._xaxis-ref)) < 1.0e-8)
        l = f.getPointEntries("VITESSE_Y (SEG [X=0.13,Y=0] -> [X=0.13,Y=0.2])")
        self.assertNotEqual(-1, l.index("VITESSE_Y (X=0.13,Y=0)"))
        self.assertRaises(ValueError, l.index, "VITESSE_X (X=0.13,Y=0)")

    def testTrustFileDTEV_header(self):
        fPath = os.path.join(self.BASE_DIR, "long.dt_ev")
        fObj = BuildFromPath(fPath, None)
        entr = fObj.getEntries()
        val1, val2 = fObj.getValues("Ri=max|dV/dt|")
        ref_e = {'residu=max|Ri|', 'Ri=max|dV/dt|', 'dt_stab', 'facsec', 'temps', 'dt'}
        self.assertEqual(ref_e, set(entr))

    # Dynamic reading tests
    def testTrustFileDTEV_readOn(self):
        import shutil as sh
        fPath = os.path.join(self.tmpDir, "long.dt_ev")
        refPath = os.path.join(self.tmpDir, "long.stop")
        sh.copyfile(os.path.join(self.BASE_DIR, "long.dt_ev"), fPath)
        self.touch(refPath)
        self.touch(fPath)
        fObj = BuildFromPath(fPath, refPath)
        t1, y1 = fObj.getValues("Ri=max|dV/dt|")
        self.assertTrue(abs(np.sum(t1-np.array([0., 0.005, 0.01, 0.015, 0.02]))) < 1.0e-8)
        ref2 = np.array([26.42237,   17.83193,   12.52936,    9.493226,   8.615865])
        self.assertTrue(abs(np.sum(y1-ref2)) < 1.0e-4)
        t2, y2 = fObj.getValues("temps")
        self.assertTrue(abs(np.sum(t1-t2)) < 1.0e-12)
        self.assertTrue(abs(np.sum(t2-y2)) < 1.0e-12)
        t3, y3 = fObj.getNewValues("temps")
        self.assertEqual(0, t3.shape[0])
        self.assertEqual(0, y3.shape[0])
        t33, y33 = fObj.getNewValues("dt")
        self.assertTrue(t33 is None)   # getValues() must be called first
        self.assertTrue(y33 is None)
        # Reset
        self.touch(refPath)
        t3, y3 = fObj.getNewValues("temps")
        self.assertTrue(t3 is None)
        self.assertTrue(y3 is None)
        t4, y4 = fObj.getValues("Ri=max|dV/dt|")
        self.assertTrue(abs(np.sum(t4-t1)) < 1.0e-12)
        self.assertTrue(abs(np.sum(y4-y1)) < 1.0e-12)
        _, _ = fObj.getValues("temps")
        # partial append in the file
        with open(fPath) as fR: lns = fR.readlines()[-3:]
        with open(fPath, "a") as fR:
            fR.write(lns[0])
            fR.write(lns[1][:20])
        t5, y5 = fObj.getNewValues("Ri=max|dV/dt|")
        self.assertTrue(abs(np.sum(t5-np.array([0.01]))) < 1.0e-8)
        self.assertTrue(abs(np.sum(y5-np.array([12.52936]))) < 1.0e-4)
        t5b, y5b = fObj.getNewValues("temps")
        self.assertTrue(abs(np.sum(t5b-np.array([0.01]))) < 1.0e-8)
        self.assertTrue(abs(np.sum(y5b-np.array([0.01]))) < 1.0e-8)
        t5, y5 = fObj.getNewValues("Ri=max|dV/dt|")
        self.assertEqual(0, t5.shape[0]); self.assertEqual(0, y5.shape[0])
        with open(fPath, "a") as fR:    # continue writing
            fR.write(lns[1][20:])
            fR.write(lns[2])
        t5, y5 = fObj.getNewValues("Ri=max|dV/dt|")
        self.assertTrue(abs(np.sum(t5-np.array([0.015, 0.02]))) < 1.0e-8)
        self.assertTrue(abs(np.sum(y5-np.array([9.493226,   8.615865]))) < 1.0e-4)
        with open(fPath, "a") as fR:  # write again
            fR.write(lns[0])
        t6, y6 = fObj.getValues("Ri=max|dV/dt|")
        self.assertTrue(abs(np.sum(t6[-1]-np.array([0.01]))) < 1.0e-8)
        self.assertTrue(abs(np.sum(y6[-1]-np.array([12.52936]))) < 1.0e-4)
        # Now true reset of the file (writing a smaller one)
        with open(fPath) as fR: lns = fR.readlines()
        with open(fPath, "w") as fR:
            for l in range(6):
                fR.write(lns[l])
        e = fObj.getEntries()
        t2, y2 = fObj.getNewValues("Ri=max|dV/dt|")
        self.assertTrue(t2 is None)
        self.assertTrue(y2 is None)
        t2, y2 = fObj.getValues("Ri=max|dV/dt|")
        self.assertTrue(abs(np.sum(t2-t1)) < 1.0e-12)
        self.assertTrue(abs(np.sum(y2-y1)) < 1.0e-12)
        # Bug spotted by Gauthier:
        self.touch(refPath)
        fObj.getEntries()
        t7, v7 = fObj.getNewValues("Ri=max|dV/dt|")
        self.assertTrue(t7 is None and v7 is None)

    def testTrustFileSEG_readOn(self):
        # Lighter than testTrustFileDTEV_readOn, just to test reloadValues() really.
        import shutil as sh
        fPath = os.path.join(self.tmpDir, "long_V.son")
        refPath = os.path.join(self.tmpDir, "long.stop")
        sh.copyfile(os.path.join(self.BASE_DIR, "long_V.son"), fPath)
        self.touch(refPath)
        self.touch(fPath)
        fObj = BuildFromPath(fPath, refPath)
        ref_pts = np.array([[1.30000000e-01, 0.00000000e+00], [1.30000000e-01, 2.02020202e-03], [1.30000000e-01, 4.04040404e-03],
                   [1.30000000e-01, 6.06060606e-03]])
        seg_x, seg_y = "VITESSE_X (SEG [X=0.13,Y=0] -> [X=0.13,Y=0.2])", "VITESSE_Y (SEG [X=0.13,Y=0] -> [X=0.13,Y=0.2])"
        s1, v1 = fObj.getValues(seg_x)
        self.assertTrue(abs(np.sum(s1-ref_pts[:,1])) < 1.0e-8)
        s2, v2 = fObj.getValues(seg_y)
        self.assertTrue(abs(np.sum(s2-ref_pts[:,1])) < 1.0e-8)
        ref_v0 = np.array([1.05401193e+00, 0.00000000e+00, 1.05401193e+00, 5.69082130e-04, 1.05401193e+00, 1.13816426e-03,
                          1.05401193e+00, 1.70724639e-03])
        ref_v1 = np.array([1.04726372e+00, 0.00000000e+00, 1.04726372e+00, 6.16002242e-04, 1.04726372e+00, 1.23200448e-03,
                           1.04726372e+00, 1.84800673e-03])
        ref_vx0,ref_vy0 = ref_v0[::2], ref_v0[1::2]
        ref_vx1,ref_vy1 = ref_v1[::2], ref_v1[1::2]
        self.assertTrue(abs(np.sum(v1-ref_vx0)) < 1.0e-4)
        self.assertTrue(abs(np.sum(v2-ref_vy0)) < 1.0e-4)
        # partial append in the file
        with open(fPath) as fR: lns = fR.readlines()[-3:]
        with open(fPath, "a") as fR: fR.write(lns[0])
        s21, v21 = fObj.getNewValues(seg_x)
        self.assertTrue(s21 is None and v21 is None)  # a new seg is available and should be read with getValues()
        s22, v22 = fObj.getNewValues(seg_y)
        self.assertTrue(s22 is None and v22 is None)
        s3, v3 = fObj.getValues(seg_x)
        s4, v4 = fObj.getValues(seg_y)
        self.assertTrue(abs(np.sum(v3-ref_vx1)) < 1.0e-4)
        self.assertTrue(abs(np.sum(v4-ref_vy1)) < 1.0e-4)
        self.assertTrue(abs(np.sum(s4-ref_pts[:,1])) < 1.0e-4)
        s5, v5 = fObj.getNewValues(seg_x)
        self.assertEqual(0, s5.shape[0]); self.assertEqual(0, v5.shape[0])
        s55, v55 = fObj.getNewValues(seg_y)
        self.assertEqual(0, s55.shape[0]); self.assertEqual(0, v55.shape[0])
        s6, v6 = fObj.getValues(seg_y)
        self.assertTrue(abs(np.sum(v6-v4)) < 1.0e-4)
        self.assertTrue(abs(np.sum(s6-s4)) < 1.0e-4)
        # Test when querying SonSEG file as a point file:
        entr1, entr2 = "VITESSE_X (X=0.13,Y=0)", "VITESSE_Y (X=0.13,Y=0)"
        _, v7 = fObj.getValues(entr2)
        self.assertTrue(abs(np.sum(v7)) < 1.0e-12)  # all null in file
        s8, v8 = fObj.getNewValues(entr2)
        self.assertEqual(0, s8.shape[0])
        self.assertEqual(0, v8.shape[0])
        s9, v9 = fObj.getNewValues(entr1)
        self.assertTrue(s9 is None and v9 is None)
        self.touch(refPath)
        s8, v8 = fObj.getNewValues(entr1)
        self.assertTrue(s8 is None and v8 is None)
        s9, _ = fObj.getValues(entr1)
        self.assertEqual(7, s9.shape[0])
        s10, v10 = fObj.getNewValues(entr2)
        self.assertTrue(s10 is None and v10 is None)

if __name__ == "__main__":
    suite = unittest.TestSuite()
    suite.addTest(DynamicUnitTest('testTrustFileStatus'))
    suite.addTest(DynamicUnitTest('testTrustFileSonPOINTFile_header'))
    suite.addTest(DynamicUnitTest('testTrustFileSonSEGFile_header'))
    suite.addTest(DynamicUnitTest('testTrustFileDTEV_header'))
    suite.addTest(DynamicUnitTest('testTrustFileDTEV_readOn'))
    suite.addTest(DynamicUnitTest('testTrustFileSEG_readOn'))
    res = unittest.TextTestRunner().run(suite)
    if not res.wasSuccessful():
        import sys
        sys.exit(1)
