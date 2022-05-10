"""

Morad Ben Tayeb
CEA Saclay - DM2S/STMF/LGLS
Mars 2021 - Stage 6 mois

We provide here a python package that can be used to deeply analyze the .med files.


"""

import medcoupling as mc
import numpy as np
import scipy.stats as stats
import matplotlib.pyplot as plt
from .trust_statistical_tools_base import StatisticalPostProcessing


class IJK_PostProcessing(StatisticalPostProcessing):

    _EPS = 1e-12

    def __init__(self, fileName, meshName="DOM"):
        """

        Initialization of a class instance.

        Parameters
        ----------
        fileName : str
            Name of the .med file.
        meshName : str 
            Name of the mesh.
        
        Returns
        ------
        None

        """
        self.fileName = fileName
        self.meshName = meshName

        # Will contain mesh discretization
        # Assumes that there is only one
        # mesh per .med file.
        # Will store the Cartesian mesh
        # Get mesh from med file
        self.uMesh = mc.ReadUMeshFromFile(fileName, meshName, 0)
        self.xx, self.yy, self.zz, self.cMesh = self._constructCMesh(self.uMesh)

        # Constants computed from mesh
        self._bary_cart = self.cMesh.computeCellCenterOfMass()
        self.c, self.cI = self.uMesh.getCellsContainingPoints(self._bary_cart, IJK_PostProcessing._EPS)

        # Will contain dimensions of IJK mesh
        # (i.e. number of cubes in each directions)
        self.x_ind = len(self.xx) - 1
        self.y_ind = len(self.yy) - 1
        self.z_ind = len(self.zz) - 1

        self.l_x = np.array([(self.xx[i] + self.xx[i + 1]) * 0.5 for i in range(self.x_ind)])
        self.l_y = np.array([(self.yy[i] + self.yy[i + 1]) * 0.5 for i in range(self.y_ind)])
        self.l_z = np.array([(self.zz[i] + self.zz[i + 1]) * 0.5 for i in range(self.z_ind)])

    def _constructCMesh(self, myMesh):
        """

        We sort nodes and construct Cartesian Mesh.

        Parameters
        ----------
        myMesh : medcoupling.MEDCouplingUMesh object
            Object containing information about the unstructured mesh

        Returns
        ------
        uniqX : array
            X coordinates
        uniqY : array
            Y coordinates
        uniqZ : array
            Z coordinates
        mcart : medcoupling.MEDCouplingUMesh object
            Object containing information about the cartesian mesh

        """
        coo = myMesh.getCoords()
        cooX, cooY, cooZ = coo[:, 0], coo[:, 1], coo[:, 2].deepCopy()
        coo2 = coo[:, 0:3]

        # in the x-direction, nothing special is to be done
        # we search the common tuples and we sort the array

        myMesh.setCoords(coo2)
        # c : list of common tuples
        # cI : list of indexes to navigate in c
        c, cI = cooX.findCommonTuples(IJK_PostProcessing._EPS)
        uniqX = cooX[c[cI[:-1]]]
        uniqX.sort()

        # y-direction
        c, cI = cooY.findCommonTuples(IJK_PostProcessing._EPS)
        uniqY = cooY[c[cI[:-1]]]
        uniqY.sort()

        # z-direction
        c, cI = cooZ.findCommonTuples(IJK_PostProcessing._EPS)
        uniqZ = cooZ[c[cI[:-1]]]
        uniqZ.sort()

        mcart = mc.MEDCouplingCMesh("cart")
        mcart.setCoords(uniqX, uniqY, uniqZ)

        return (uniqX.toNumPyArray(), uniqY.toNumPyArray(), uniqZ.toNumPyArray(), mcart)

    def _mapDataToCMesh(self, myUField):
        """

        Map the unstructured data onto a defined structured support.

        Parameters
        ----------
        myUField : medcoupling.MEDCouplingFieldDouble object
            Object containing information about the field.

        Returns
        -------
        fcart : medcoupling.MEDCouplingFieldDouble object
            Field applied to cartesian mesh.

        """
        # All is done here
        grp_size = self.cI.deltaShiftIndex()
        assert grp_size.isUniform(1)
        pts_ids = self.c[self.cI[:-1]]

        arr = myUField.getArray()  # DataArrayDouble containing field values
        ordered_arr = arr[pts_ids]

        fcart = mc.MEDCouplingFieldDouble(mc.ON_CELLS, mc.ONE_TIME)
        fcart.setName(self.uMesh.getName())
        fcart.setMesh(self.cMesh)
        fcart.setArray(ordered_arr)
        # fcart.writeVTK("test")

        return fcart

    def _fromUToC(self, fieldName, frame):
        """

        Apply field to cartesian mesh.

        Parameters
        ----------
        fieldName : str 
            Name of the field.
        frame : int 
            Selected frame.

        Returns
        -------
        field : medcoupling.MEDCouplingFieldDouble object
            Field applied to cartesian mesh.

        """

        field = self.getOriginalData(fieldName, frame=frame)

        return self._mapDataToCMesh(field)

    def _getFieldNames(self):
        """

        Get field names on mesh.

        """
        return mc.GetFieldNamesOnMesh(mc.ON_CELLS, self.fileName, self.meshName)

    def _getIndexFromCoor(self, X, coor):
        """

        Find index of closest value ` coor ` in ` X `. 

        """

        l = len(X)

        if coor < X[0]:
            ind = 0
        elif coor > X[-1]:
            ind = l - 1
        else:
            for i in range(l - 1):
                if X[i] > coor:
                    ind = i
                    break

            if abs(X[ind] - coor) > abs(X[ind - 1] - coor):
                ind = ind - 1

        return ind

    def _surfaceMean(self, field, normal, component=0):
        """
        
        Method to comupte the average on a surface.

        """

        scalar_field = field[:, component]

        if normal == "X":
            avg = []
            x_axis = self.l_x
            for ind in range(self.x_ind):
                avg.append(self._get2DSliceFromIndex(scalar_field, ind, normal).getAverageValue())

        if normal == "Y":
            avg = []
            x_axis = self.l_y
            for ind in range(self.y_ind):
                avg.append(self._get2DSliceFromIndex(scalar_field, ind, normal).getAverageValue())

        if normal == "Z":
            avg = []
            x_axis = self.l_z
            for ind in range(self.z_ind):
                avg.append(self._get2DSliceFromIndex(scalar_field, ind, normal).getAverageValue())

        return (x_axis, avg)

    def writeVTK(self, fieldName, frame=0, name=None):
        """
        
        Write in VTK format.

        """
        f = self._fromUToC(fieldName=fieldName, frame=frame)
        if name == None:
            name = f.getName()
        f.writeVTK(name)

        return ()

    def setEpsilon(self, newEps):
        """
        
        Set a new value for epsilon.

        Parameters
        ----------
        newEps : float
            New value for epsilon.

        Returns
        -------
        None
        
        """
        IJK_PostProcessing._EPS = newEps

        return ()

    def getIterations(self, fieldName):
        """
        
        Get iterations.
        
        """
        return [i[2] for i in mc.GetAllFieldIterations(self.fileName, fieldName)]

    def getOriginalData(self, fieldName, frame=0):
        """

        Get original data.

        Parameters
        ----------
        fieldName : str
            Name of the field.
        frame : int 
            Selected frame
        
        Returns
        -------
        myField : medcoupling.MEDCouplingFieldDouble object
            Field in cartesian mesh

        """
        lst_dt = mc.GetAllFieldIterations(self.fileName, fieldName)
        #  # If more time step defined, take the last one
        myField = mc.ReadFieldCell(self.fileName, self.meshName, 0, fieldName, frame, lst_dt[-1][1])
        # myField.writeVTK("myField.vtu")

        return myField

    def getFrames(self, fieldName):
        """
        
        Get the total number of frames.

        Parameters
        ----------
        fieldName : str 
            Name of the field.
        
        Returns
        -------
        numFrames : int 
            Number of frames.
        
        """
        return len(mc.GetAllFieldIterations(self.fileName, fieldName)) - 1

    def plotSurface(self, field, coor, normal="X", component=0):
        """
        Plot 2D heatmap.
        Parameters
        ----------
        field : array
            3D field.
        coor : float 
            Coordinate
        normal : str 
            Normal to the surface you want to display.
        Returns
        -------
        None
        """

        if normal == "X":
            l_x = self.zz
            l_y = self.yy
            plt.xlabel("Z-Axis")
            plt.ylabel("Y-Axis")
            ind = self._getIndexFromCoor(self.l_x, coor)
            surfaceField = [[field.getValueOnPos(ind, j, k)[component] for k in range(self.z_ind)] for j in range(self.y_ind)]
        if normal == "Y":
            l_x = self.xx
            l_y = self.zz
            plt.xlabel("X-Axis")
            plt.ylabel("Z-Axis")
            ind = self._getIndexFromCoor(self.l_y, coor)
            surfaceField = [[field.getValueOnPos(i, ind, k)[component] for k in range(self.z_ind)] for i in range(self.x_ind)]
        if normal == "Z":
            l_x = self.yy
            l_y = self.xx
            plt.xlabel("Y-Axis")
            plt.ylabel("X-Axis")
            ind = self._getIndexFromCoor(self.l_z, coor)
            surfaceField = [[field.getValueOnPos(i, j, ind)[component] for j in range(self.y_ind)] for i in range(self.x_ind)]
        plt.pcolormesh(l_x, l_y, surfaceField)
        plt.colorbar()
        plt.show()

        return ()

    def _getIndexFromCoor(self, X, coor):
        """
        Find index of closest value ` coor ` in ` X `. 
        """

        l = len(X)

        if coor < X[0]:
            ind = 0
        elif coor > X[-1]:
            ind = l - 1
        else:
            for i in range(l - 1):
                if X[i] > coor:
                    ind = i
                    break

            if abs(X[ind] - coor) > abs(X[ind - 1] - coor):
                ind = ind - 1

        return ind

    def getField(self, fieldName, frame=0):
        """
        
        Get field

        """
        return self._fromUToC(fieldName=fieldName, frame=frame)

    def _get2DSliceFromIndex(self, field, ind, normal="X"):
        """

        Get a 2D slice of a field.

        """
        nx = self.x_ind
        ny = self.y_ind
        nz = self.z_ind
        if normal == "X":
            L = [ind + j * nx + k * nx * ny for k in range(nz) for j in range(ny)]
        if normal == "Y":
            L = [i + ind * nx + k * nx * ny for k in range(nz) for i in range(nx)]
        if normal == "Z":
            L = [i + j * nx + ind * nx * ny for j in range(nx) for i in range(nx)]

        return field[L]

    def get2DSlice(self, field, coor, normal="X"):
        """
        
        Get a 2D slice of a field

        """
        nx = self.x_ind
        ny = self.y_ind
        nz = self.z_ind
        if normal == "X":
            ind = self._getIndexFromCoor(self.l_x, coor)
            L = [ind + j * nx + k * nx * ny for j in range(ny) for k in range(nz)]
        if normal == "Y":
            ind = self._getIndexFromCoor(self.l_y, coor)
            L = [i + ind * nx + k * nx * ny for k in range(nz) for i in range(nx)]
        if normal == "Z":
            ind = self._getIndexFromCoor(self.l_z, coor)
            L = [i + j * nx + ind * nx * ny for j in range(nx) for i in range(nx)]

        return field[L]

    def getLine(self, field, coor1, coor2, direction="X", component=0):
        """

        Plot a 1D lineout.

        Parameters
        ----------
        field : array
            3D scalar field.
        coor1 : float
            Coordinate.
        coor2 : float 
            Coordinate.
        direction : str
            Direction of the line.

        Returns
        -------
        line : array
            1D array, x-coordinate.
        lineField : array
            1D array, values.

        """

        if direction == "X":
            line = self.l_x
            ind1 = self._getIndexFromCoor(self.l_y, coor1)
            ind2 = self._getIndexFromCoor(self.l_z, coor2)
            lineField = [field.getValueOnPos(i, ind1, ind2)[component] for i in range(len(line))]
        if direction == "Y":
            line = self.l_y
            ind1 = self._getIndexFromCoor(self.l_x, coor1)
            ind2 = self._getIndexFromCoor(self.l_z, coor2)
            lineField = [field.getValueOnPos(ind1, i, ind2)[component] for i in range(len(line))]
        if direction == "Z":
            line = self.l_z
            ind1 = self._getIndexFromCoor(self.l_x, coor1)
            ind2 = self._getIndexFromCoor(self.l_y, coor2)
            lineField = [field.getValueOnPos(ind1, ind2, i)[component] for i in range(len(line))]

        return (line, lineField)

    def getCoordinates(self):
        """
        
        Return arrays of coordinates.
        
        """
        return (self.l_x, self.l_y, self.l_z)

    def compute2DMean(self, field, normal="X", component=0):
        """

        Computes mean on each 2D face which normal is "X", "Y" or "Z".

        """
        return self._surfaceMean(field, normal, component)

    def plot(self, x1, y1, label1="", linestyle1="-", linewidth1=0.5, **kwargs):
        """

        To display figure.

        """

        save_path = kwargs.get("save_path")

        if save_path == None:
            save_path = "medPostProcessing/"

        kwargs["save_path"] = save_path

        StatisticalPostProcessing._plot(self, x1, y1, label1, linestyle1, linewidth1, **kwargs)

        return ()

    def spatialAutoCorrelation(self, X, Y, max_lag=None, fit=False):
        """

        Computes spatial signal autocorrelation.

        """
        dist, autocorr = StatisticalPostProcessing._computeAutocorrelation(self, X, Y, max_lag)
        res = [dist, autocorr]
        if fit:
            Xfit, Yfit = StatisticalPostProcessing._fitParabola(self, res[0], res[1])
            res = res + [Xfit, Yfit]
        return res

    def spatialEnergySpectrum(self, X, Y):
        """

        Computes spatial energy spectrum with Welch method.

        """
        lambd, power = StatisticalPostProcessing._computeWelch(self, X, Y)
        return (lambd, power)

    def powerSpectrum2D(self, field):
        """
        
        Computes 2D power spectrum density.
        Warning, the field must be a NumPy array.

        """
        nx = len(field)
        ny = len(field[0])

        fourier_image = np.fft.fftn(field)
        fourier_amplitudes = np.abs(fourier_image) ** 2

        kfreqx = np.fft.fftfreq(nx) * nx
        kfreqy = np.fft.fftfreq(ny) * ny
        kfreq2D = np.meshgrid(kfreqx, kfreqy)
        knrm = np.sqrt(kfreq2D[0] ** 2 + kfreq2D[1] ** 2)

        knrm = knrm.flatten()
        fourier_amplitudes = fourier_amplitudes.flatten()

        kbins = np.arange(0.5, max(nx, ny) // 2 + 1, 1.0)
        kvals = 0.5 * (kbins[1:] + kbins[:-1])
        Abins, _, _ = stats.binned_statistic(knrm, fourier_amplitudes, statistic="mean", bins=kbins)
        Abins *= np.pi * (kbins[1:] ** 2 - kbins[:-1] ** 2)

        return (kvals, Abins)

    def powerSpectrum3D(self, field):
        """
        
        Computes 3D power spectrum density.
        Warning, the field must be a NumPy array.

        """
        nx = self.x_ind
        ny = self.y_ind
        nz = self.z_ind

        fourier_image = np.fft.fftn(field)
        fourier_amplitudes = np.abs(fourier_image) ** 2

        kfreqx = np.fft.fftfreq(nx) * nx
        kfreqy = np.fft.fftfreq(ny) * ny
        kfreqz = np.fft.fftfreq(nz) * nz
        kfreq3D = np.meshgrid(kfreqx, kfreqy, kfreqz)
        knrm = np.sqrt(kfreq3D[0] ** 2 + kfreq3D[1] ** 2 + kfreq3D[2] ** 2)

        knrm = knrm.flatten()
        fourier_amplitudes = fourier_amplitudes.flatten()

        kbins = np.arange(0.5, max(nx, ny, nz) // 2 + 1, 1.0)
        kvals = 0.5 * (kbins[1:] + kbins[:-1])
        Abins, _, _ = stats.binned_statistic(knrm, fourier_amplitudes, statistic="mean", bins=kbins)
        Abins *= 4.0 * np.pi / 3.0 * (kbins[1:] ** 3 - kbins[:-1] ** 3)

        return (kvals, Abins)

    def toNumPy(self, field, component=0):
        """
        
        Convert from medcoupling to numpy array.

        """

        f = field[:, component].getArray().toNumPyArray()

        return f

    def computeMean(self, fieldName, startFrame=0, endFrame=-1):
        """
        
        Computes spatial mean.
        
        """
        if endFrame == -1:
            endFrame = self.getFrames(fieldName)
        for frame in range(startFrame, endFrame + 1):
            if frame == startFrame:
                field = self._fromUToC(fieldName=fieldName, frame=frame)
            else:
                field += self._fromUToC(fieldName=fieldName, frame=frame)

        return field / (endFrame - startFrame + 1)

    def computeFluctuation(self, fieldName, atFrame, startFrame=0, endFrame=-1):
        """
        
        Computes spatial fluctuation.
        
        """
        meanField = self.computeMean(fieldName, startFrame, endFrame)
        field = self._fromUToC(fieldName=fieldName, frame=atFrame)

        return field - meanField
