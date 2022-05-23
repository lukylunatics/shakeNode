# Built-in imports
import os
import sys
import logging
import platform
import subprocess as sp
from typing import List

# Third-party imports
from maya import cmds
from maya.app.startup import basic
from PySide2 import QtCore as qtc



logger = logging.getLogger("Drag and Drop Installer")



class DragDropInstaller():
	"""Class for drag and drop install deployments of modules in Autodesk Maya.

	"""


	# Class variable
	moduleName = ""
	dependencies = ""

	# System
	platformName = platform.platform()

	# Python
	mayaPy = f"\"{os.environ['MAYA_LOCATION']}/bin/mayapy\""
	mayaVersion = int(cmds.about(version=True))

	# Paths
	draggedFromPath = os.path.dirname(__file__).replace("\\", "/")  # In maya 2023 it returns "\\"
	defaultModulesPath = f"{os.environ['MAYA_APP_DIR']}/modules"
	defaultScriptsPath = f"{os.environ['MAYA_APP_DIR']}/scripts"

	# Files
	installationFiles = []
	existingFiles = [
		f"{defaultModulesPath}/{moduleName}",
		f"{defaultModulesPath}/{moduleName}.mod"
	]


	def __str__(self) -> str:
		return f"Drag and drop installer for '{self.moduleName}' Copyright © 2022 Lunatics"


	def validatePythonVersion(self, requiredVersion=3) -> bool:
		"""Validate the required python version.

		Args:
			requiredVersion (int): Required python version for the validation to be successfull.

		Returns:
			bool: If the python version is lower than the requiredVersion, it will return False,
				otherwise True

		"""
		mayaPythonVersion = int(os.environ['MAYA_PYTHON_VERSION'])

		logStr = f": Required python version is '{requiredVersion}' or higher, running '{mayaPythonVersion}'."

		if mayaPythonVersion >= requiredVersion:
			logger.info(f"PASSED {logStr}")
			return True

		logger.critical(f"FAILED {logStr}")
		return False


	def validateInstallationFiles(self) -> bool:
		"""Checks if all required installation files exist in source directory.

		Returns:
			bool: True if the operation was successful, False if an	error occured during the operation.

		"""
		missingFiles = []
		for file in self.installationFiles:
			if not qtc.QFileInfo(file).exists(): missingFiles.append(file)

		if len(missingFiles) == 0:
			logger.info("PASSED : Find all installation files.")
			return True

		logger.critical(f"FAILED : Found missing installation files {missingFiles}.")
		return False


	def isPluginLoaded(self, pluginName) -> bool:
		"""Checks if the specified plugin is loaded.

		Args:
			pluginName (str): Name of the plugin to be queried.

		Returns:
			bool: True if the plugin is loaded, False otherwise.

		"""
		return cmds.pluginInfo(pluginName, query=True, loaded=True)


	def isSceneModified(self) -> bool:
		"""Checks is the currently open scene was modified.
		
		Returns:
			bool: If the scene was modified True will be returned, otherwise False.
		
		"""
		return cmds.file(query=True, modified=True)


	def unloadPlugin(self, pluginName, force=True) -> bool:
		"""Unloads the specified plugin.

		Args:
			pluginName (str): Name of the plugin to unload.
			force (bool): Use it if the plugin is currently being used in the scene. It will load a new
				clean scene and unload the plugin.

		Returns:
			bool: True if the plugin is was successfully unload, otherwise False.		

		"""
		if force: cmds.file(new=True, force=True)

		cmds.unloadPlugin(pluginName, force=True)

		return True


	def removeExistingVersion(self) -> bool:
		"""Removes all files from existing installation.

		If files from existing installation have been found, they will be removed.

		"""
		# finished = False
		for path in self.existingFiles:
			fileInfo = qtc.QFileInfo(path)
			if fileInfo.exists():
				if fileInfo.isDir(): qtc.QDir(path).removeRecursively()
				if fileInfo.isFile(): qtc.QFile(path).remove()

		logger.info("PASSED : Remove existing installation files if they exist.")


	def validateFileInfo(self, path) -> qtc.QFileInfo or False:
		"""Validate the specified file.

		Args:
			path (str): Path to the object.

		Returns:
			fileInfo (QFileInfo): If the specified object exists it will be returned,	if it does not
			exist False will be returned instead.

		"""
		fileInfo = qtc.QFileInfo(path)
		if fileInfo.exists():
			logger.debug(f"'{path}' file or directory exists.")
			return fileInfo

		logger.warning(f"'{path}' file or directory does not exists.")
		return False


	def installDependencies(self, dependencies) -> bool:
		"""Install required dependecies by calling the mayapy pip package manager.

		TODO add support for multipile dependencies? for loop if it is a list

		Args:
			dependencies (str or list): Name od the package to install.

		Returns:
			bool: True if the operation was successful, False if an	error occured during the operation.

		"""
		command = f"{self.mayaPy} -m pip install --target={self.moduleScriptsPath} {dependencies}"

		if self.platformName == "Darwin" or self.platformName == "Linux":
			command = f"sudo ./{command}"

		try:
			output = sp.check_output(command, shell=True, stderr=sp.STDOUT, universal_newlines=True)
			logger.info(f"Pip install output:\n{output}")
		except sp.CalledProcessError:
			logger.critical("FAILED : Dependecies could not be installed")
			return False

		logger.info("PASSED : Install dependencies.")
		return True


	def createDirectory(self, path) -> bool:
		"""Creates the specified directory if it does not already exist.

		Args: 
			path (string): The path for the directory to be created.

		Returns:
			bool: True if the operation was successful, False if an	error occured during the operation.

		"""
		directory = qtc.QDir(path)
		if not directory.exists():
			directory.mkpath(directory.absolutePath())
			logger.debug(f"Directory '{directory.absolutePath()}' was successfully created.")
		else:
			logger.debug(f"Directory '{directory.absolutePath()}' already exists.")

		return True


	def getFilesInDirectory(self,
		path,
		nameFilters=[],
		filters=qtc.QDir.Files,
		includeSubDirectories=qtc.QDirIterator.Subdirectories,
	) -> List[qtc.QFileInfo] or False:
		"""Returns a list with files contained in the specified directory.

		Args: 
			path (string): Path to the directory.
			nameFilters (list): A list with name filters e.g. ['ellie*'], ['*.fbx'].
			filters (QDir.Flag): NoFilter, Files.
			includeSubDirectories (QDirIterator.IteratorFlag): Whether or not search in
				sub-directories, Subdirectories - true, NoIteratorFlags - false.

		Returns:
			fileInfoList (List[QFileInfo] or False): List with QFileInfo objects that the directory
			contains,	if the list is empty or the directory does not exis it will	return False.

		"""
		fileInfo = qtc.QFileInfo(path)
		if fileInfo.exists() and fileInfo.isDir():
			fileInfoList = []
			dirIter = qtc.QDirIterator(path, nameFilters, filters, includeSubDirectories)
			while dirIter.hasNext():
				dirIter.next()
				fileInfoList.append(dirIter.fileInfo())
		
			if len(fileInfoList) != 0:
				return fileInfoList
	
			logger.warning(f"'{path}' contains no files with those {nameFilters} name filters.")

		else:	logger.warning(f"Entry: '{path}' does not exist or is not a directory - nothing to return.")

		return False


	def copy(self, source, destination, overwrite=True) -> bool:
		"""Copies the source file / directory to the destination.

		Args:
			source (str): Source file or directory path.
			destination (str): Destination directory path.
			overwrite (bool): If destination file / directory exists, it will be overwritten.

		Returns:
			bool: True if the operation was successful, False if an	error occured during the
				operation.

		"""
		fileInfo = self.validateFileInfo(source)
		if not fileInfo: return False

		# Input can be a directory or a file
		finished = False
		if fileInfo.isDir():
			fileObjs = self.getFilesInDirectory(source)
			if len(fileObjs) != 0:
				for fileObj in fileObjs:
					destinationFilePath = fileObj.filePath().replace(source, destination)
					destinationFile = qtc.QFile(destinationFilePath)
					if overwrite and destinationFile.exists(): destinationFile.remove()

					destinationDir = qtc.QFileInfo(destinationFilePath).dir()
					self.createDirectory(destinationDir)
					finished = qtc.QFile(fileObj.filePath()).copy(destinationFilePath)
			else:
				logger.debug('Did not find any files to copy in the given directory')

		if fileInfo.isFile():
			self.createDirectory(destination)
			destinationFilePath = f'{destination}/{fileInfo.fileName()}'
			destinationFile = qtc.QFile(destinationFilePath)
			if overwrite and destinationFile.exists(): destinationFile.remove()
	
			finished = qtc.QFile(fileInfo.filePath()).copy(destinationFilePath)

		if finished: return True

		return False


	def copyInstallationFiles(self):
		"""Copies the installation files to the destination.

		Performs the actual installation by first copying the .mod file and then the moduleName folder
		with all its content.

		"""
		self.copy(
			source=self.installationFiles[-1],
			destination=self.defaultModulesPath
		)
		self.copy(
			source=f"{self.draggedFromPath}/{self.moduleName}",
			destination=f"{self.defaultModulesPath}/{self.moduleName}"
		)


	def setupPlugin(self) -> bool:
		"""Sequence of methods for unloading the plugin if one is already loaded.

		"""
		# In case this is a plugin module check if the plugin is already loaded and try to unload it
		if self.isPluginLoaded(self.moduleName):
			if self.isSceneModified():
				# Create a new scene in order to be able to unload the plugin if it is being used
				input = self.createDialog(
					message=(
						f"The installer has detected that the {self.moduleName} is already installed and loaded."
						"\nIn order to continue a new clean must be opened."
						"\nDo you want to save the current scene?"
					),
					title=f"{self.moduleName} Installer",
					buttons=["Save", "Don't Save"],
					cancelButton="Later"
				)
				if input == "Save": cmds.SaveScene()

			# If plugin was successfully unloaded or was not loaded at all, installation can continue
			self.unloadPlugin(self.moduleName)
			if not self.isPluginLoaded(self.moduleName):
				logger.info("PASSED : Plugin was successfully unloaded and installation can continue.")
			# If could not unload the plugin call for manual installation by user
			else:
				logger.critical(
					f"FAILED : Could not unload {self.moduleName}. Please close maya and manualy copy the:"
					f"\n{self.moduleName} and {self.moduleName}.mod to your maya modules folder."
				)
				return False
		
		return True


	def install(self) -> bool:
		"""Perform the actuall installation.

		Returns:
			bool: True if the operation was successful, False if an	error occured during theoperation.

		"""
		# If running in maya 2022 check if it is run in python 3
		if self.mayaVersion == 2022:
			if not self.validatePythonVersion(): return False

		# Check if all necessery files are unzipped and in the intallation source folder
		if not self.validateInstallationFiles(): return False

		# Create installation dialog asking if installation should begin
		input = self.createDialog(
			message=(f"This will install {self.moduleName} in:\n{self.defaultModulesPath}"),
			title=f"{self.moduleName} Installer"
		)
		# Begin installation
		if input == "Install":
			logger.info("PASSED : Ask user if installation should proceed.")

			# Plugin setup
			self.setupPlugin()

			self.removeExistingVersion()

			self.copyInstallationFiles()

			# install dependencies if there are any
			if self.dependencies:	
				if not self.installDependencies(self.dependencies):
					self.removeExistingVersion()
					return False

			self.postInstallation()

			# Ask to restart maya and whether or not you want to save the current scene
			input = self.createDialog(
				message=(f"Installation complete - please restart Maya!"),
				title=f"{self.moduleName} Installer",
				buttons=["Restart", "Later"],
				cancelButton="Later"
			)
			if input == "Restart":
				logger.info("PASSED : Restarting Maya.")
				cmds.quit()
			else:
				logger.info(f"PASSED : Successfully installed '{self.moduleName}' - please restart Maya!")

			return True

		# Installation was cancelled by user
		else:
			logger.info(f"Installation of {self.moduleName} has been cancelled.")

		return False


	def postInstallation(self):
		"""Post installation procedures.

		Removes existing module entries from sys.path and sys.modules
		Appends the new module path to sys.path
		Reloads the newly installed module

		Returns:
			bool: True if the operation was successful, False if an	error occured during the
				operation.

		"""
		self.removeModuleEntriesFromSys(self.moduleName)

		sys.path.append(self.moduleScriptsPath)

		cmds.loadModule(scan=True)
		cmds.loadModule(allModules=True)

		basic.executeUserSetup()


	def removeModuleEntriesFromSys(self, moduleName):
		"""Removes all entries containing 'moduleName' from sys.path and sys.modules.

		Args:
			moduleName (str): Name of the module to be removed.

		Returns:
			bool: True if the operation was successful, False if an	error occured during the
				operation.

		"""
		for path in sys.path:
			if moduleName in path:
				sys.path.remove(path)
				logger.debug(f"'{moduleName}' was found and removed from sys.path entries.")
				break
		
		for module in sys.modules:
			if moduleName in module:
				sys.modules.pop(moduleName)
				logger.debug(f"'{moduleName}' was found and removed from sys.modules entries.")
				break


	def createDialog(self,
		message="Default Message",
		title="Default Title",
		icon="question",
		buttons=["Install", "Cancel"],
		cancelButton="Cancel"
	) -> str:
		"""Convinience wrapper method for creating confirm dialogs.

		Returns:
			str: Input from user as string e.g. "Install" or "Cancel".

		"""
		return cmds.confirmDialog(
			title=title,
			message=message,
			icon=icon,
			button=buttons,
			cancelButton=cancelButton,
			dismissString=cancelButton
		)




class ShakeNodeInstaller(DragDropInstaller):
	"""DragDropInstall derived class for installing the moduleTemplate."""


	# Class variable
	moduleName = "shakeNode"

	# Python
	moduleScriptsPath = f"{DragDropInstaller.defaultModulesPath}/{moduleName}/scripts"

	# Files
	installationFiles = [
		f"{DragDropInstaller.draggedFromPath}/{moduleName}/plug-ins/windows/2023/{moduleName}.mll",
		f"{DragDropInstaller.draggedFromPath}/{moduleName}/plug-ins/windows/2022/{moduleName}.mll",
		f"{DragDropInstaller.draggedFromPath}/{moduleName}/plug-ins/linux/2023/{moduleName}.so",
		f"{DragDropInstaller.draggedFromPath}/{moduleName}/plug-ins/linux/2022/{moduleName}.so",
		f"{DragDropInstaller.draggedFromPath}/{moduleName}/scripts/AE{moduleName}Template.mel",
		f"{DragDropInstaller.draggedFromPath}/{moduleName}/scripts/AE{moduleName}RotTemplate.mel",
		f"{DragDropInstaller.draggedFromPath}/{moduleName}/scripts/userSetup.py",
		f"{DragDropInstaller.draggedFromPath}/{moduleName}/scripts/{moduleName}MainMenu.py",
		f"{DragDropInstaller.draggedFromPath}/{moduleName}/icons/{moduleName}.png",
		f"{DragDropInstaller.draggedFromPath}/{moduleName}/icons/{moduleName}Rot.png",
		f"{DragDropInstaller.draggedFromPath}/{moduleName}/icons/out_{moduleName}.png",
		f"{DragDropInstaller.draggedFromPath}/{moduleName}/icons/out_{moduleName}Rot.png",
		f"{DragDropInstaller.draggedFromPath}/{moduleName}.mod"
	]



def onMayaDroppedPythonFile(*args, **kwargs):
	"""Main function that runs when dragging the file into Maya.

	Installation is performed by copying the module to the user preferences and creating
	a module file.

	"""
	ShakeNodeInstaller().install()
