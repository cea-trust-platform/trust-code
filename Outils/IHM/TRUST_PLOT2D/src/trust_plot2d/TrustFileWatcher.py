import os

from pyqtside.QtCore import QTimer,QObject,  QFileSystemWatcher, QFile, QIODevice, Slot, SIGNAL

class TrustFileWatcher(QObject):
    def __init__(self, parent, textEdit, fv, extension):
        QObject.__init__(self, parent)
        self.textEdit = textEdit
        self._showFileView = fv
        self.__extension = extension
        self.__outputFile = None
        self.__outputFileWatcher = None
        self.__outputFileTimer = None

    def _getLongCaseName(self):
        cutn=self._showFileView._caseName.split(".data")[0]
        return os.path.join(self._showFileView._currDir,cutn)

    @Slot()
    def restart(self):
        """
        \brief restart output file watching
        \param aCaseOutputFile : trust output file (case.out)
        """
        aCaseOutputFile = self._getLongCaseName()+self.__extension
        self.stop()
        self.__outputFile = QFile(aCaseOutputFile)
        self.__outputFileWatcher = QFileSystemWatcher(self.parent())
        self.connect( self.__outputFileWatcher, SIGNAL("fileChanged( QString )"), self.__onSimulationFileChanged )

        # start timer to detect output file creation
        self.__outputFileTimer = QTimer( self )
        self.__outputFileTimer.timeout.connect( self.__checkOutputFileExists )
        self.__outputFileTimer.start(10)         # start for 10 milliseconds

    @Slot()
    def terminate(self):
        """
        \brief terminate output file watching
        """
        self.stop()
    @Slot()
    def stop(self):
        """
        \brief stop output file watching
        """
        if ( self.__outputFileWatcher is not None ):
            self.__onSimulationFileChanged(None) # on recupere le reste du fichier
            self.disconnect( self.__outputFileWatcher, SIGNAL("fileChanged( QString )"), self.__onSimulationFileChanged )
            aMonitorFileList = [str(name) for name in self.__outputFileWatcher.files()]
            for aMonitorFile in aMonitorFileList :
                self.__outputFileWatcher.removePath( aMonitorFile )
            if (  self.__outputFile.isOpen() ):
                self.__outputFile.close()
            self.__outputFileWatcher = None
            self.__outputFile = None
    @Slot()
    def __checkOutputFileExists(self):
        """
        \brief check output file cretion by trust
        """
        if os.path.exists( str(self.__outputFile.fileName()) ) :
            # output file has been created
            # Stop timer
            if self.__outputFileTimer:
                self.__outputFileTimer.stop()
            self.__outputFileTimer.timeout.disconnect( self.__checkOutputFileExists )
            del self.__outputFileTimer
            self.__outputFileTimer = None
            # start real output file watching
            self.__outputFileWatcher.addPath( str(self.__outputFile.fileName()) )
            self.textEdit.clear()
        else:
            self.__outputFileTimer.start(10)         # restart for 10 milliseconds
    @Slot()
    def __onSimulationFileChanged( self, thePath=None ):
        if ( self.__outputFile.exists() ):
            if ( not self.__outputFile.isOpen() ):
                self.__outputFile.open( QIODevice.ReadOnly | QIODevice.Text )
            try:
                aByteRead = self.__outputFile.bytesAvailable()
                simulationData = self.__outputFile.read( aByteRead )
                if ( len(simulationData) > 0 ):
                    self.textEdit.append(unicode(simulationData))
            except:
                print "ERROR reading simulation output file"
