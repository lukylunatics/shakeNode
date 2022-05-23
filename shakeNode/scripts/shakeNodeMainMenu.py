# Built-in imports
import logging

# Third-party imports
from maya import cmds
from maya import mel



menuName = "Shake"
logger = logging.getLogger(menuName) 



class MainMenu():
	"""Class for creating user-defined menu items in the maya main menu."""


	# GUI	
	menuItems = []


	def createMenuItems(self) -> bool:
		"""Adds custom menu items in the maya main menu.

		Must be overridden in the derived class.

		Returns:
			bool: True if the operation was successful, False if an	error occured during the
				operation.

		"""
		return False


	def displayOptionBox(self, *args, **kwargs):
		"""Displays the option box window for the command.
		
		Must be overridden in the derived class.

		"""
		pass


	def applyAndCloseButton(self, *args, **kwargs):
		"""Calls the doIt method and closes the option box window by saving the values."""
		self.doIt()
		mel.eval('saveOptionBoxSize')
		self.closeOptionBox()


	def closeOptionBox(self, *args, **kwargs):
		"""Closes the option box window."""
		mel.eval('hideOptionBox')


	def resetToDefaults(self, *args, **kwargs):
		"""Resets the settings to default ones.

		Must be overridden in the derived class.

		"""
		pass


	def getCreateCommandKwargs(self, *args, **kwargs) -> dict:
		"""Gets the moduleTemplate command arguments.

		The arguments are queried eiter from the option box widgets or the saved option 
		variables. If the widgets exist, their values will be saved to the option variables.

		Must be overridden in the derived class.

		Returns:
			dict: Dictionary of the kwargs to the moduleTemplate command.

		"""
		pass


	def doIt(self, *args, **kwargs) -> bool:
		"""Wrapper method for the main menu item.

		Must be overridden in the derived class.

		Returns:
			bool: True if the operation was successful, False if an	error occured during the operation.

		"""
		return False


	def deleteMenuItems(self) -> bool:
		"""Deletes custom menu items in the maya main menu.

		Returns:
			bool: True if the operation was successful, False if an	error occured during the
				operation.

		"""
		if self.menuItems:
			for item in self.menuItems:
				cmds.deleteUI(item, menuItem=True)

			self.menuItems.clear()

			logger.debug(f"Successfully deleted '{menuName}' item from main menu.")
			return True

		logger.debug(f"'{menuName}' menu item not found, nothing to delete.")
		return False



class ShakeNodeMainMenu(MainMenu):
	"""Derived class for the shakeNode plugin for creating user-defined menu items
	in the maya main menu.

	"""


	# Widgets
	_shakeNameWidget = 'shakeName'
	_shakeAttributeWidget = 'shakeAttribute'


	def createMenuItems(self):
		"""Wrapper method for the main menu item.

		Returns:
			bool: True if the operation was successful, False if an	error occured during the
				operation.

		"""
		if not self.menuItems:
			menu = "mainDeformMenu"
			mel.eval(f"ChaDeformationsMenu MayaWindow|{menu};")
			items = cmds.menu(menu, query=True, itemArray=True)
			measureItemIndex = None
			for index, item in enumerate(items):
				if item == "latticeItem":	measureItemIndex = index
			
			# Shake Node
			shakeNodeItem = cmds.menuItem(
				parent=menu,
				insertAfter=items[measureItemIndex-3],
				label=menuName,
				image="shakeNode.png",
				command=self.doIt,
				sourceType="python",
			)

			# Shake Node Option Box
			shakeNodeOptBox = cmds.menuItem(
				parent=menu,
				insertAfter=shakeNodeItem,
				command=self.displayOptionBox,
				optionBox=True,
			)

			self.menuItems.append(shakeNodeItem)
			self.menuItems.append(shakeNodeOptBox)

			return True

		logger.debug(f"'{menuName}' menu item already exists.")
		return False


	def displayOptionBox(self, *args, **kwargs):
		"""Displays the option box window for the command."""

		layout = mel.eval('getOptionBox')
		cmds.setParent(layout)
		cmds.columnLayout(adjustableColumn=True)

		mel.eval(f'setOptionBoxTitle("{menuName} Options")')
		mel.eval('setOptionBoxCommandName("shakeNode")')

		for widget in [self._shakeNameWidget, self._shakeAttributeWidget]:
			try: cmds.deletUI(widget, control=True)
			except:	pass
		
		# Shake Name
		shakeName = cmds.optionVar(query=self._shakeNameWidget)
		cmds.textFieldGrp(self._shakeNameWidget, label='Shake name', text=shakeName)

		# Shake Attribute
		shakeAttribute = cmds.optionVar(query=self._shakeAttributeWidget)
		cmds.textFieldGrp(self._shakeAttributeWidget, label='Shake attribute', text=shakeAttribute)
	
		# Action Buttons
		applyAndCloseButton = mel.eval('getOptionBoxApplyAndCloseBtn')
		cmds.button(applyAndCloseButton, edit=True, command=self.applyAndCloseButton)

		applyButton = mel.eval('getOptionBoxApplyBtn')
		cmds.button(applyButton, edit=True, command=self.doIt)

		closeButton = mel.eval('getOptionBoxCloseBtn')
		cmds.button(closeButton, edit=True, command=self.closeOptionBox)

		# Buttons in the menu only accepts MEL
		resetButton = mel.eval('getOptionBoxResetBtn')
		cmds.button(resetButton, edit=True,
		 	command='python("from shakeNodeMainMenu import ShakeNodeMainMenu; ShakeNodeMainMenu().resetToDefaults()")'
		)

		# Buttons in the menu only accepts MEL
		saveButton = mel.eval('getOptionBoxSaveBtn')
		cmds.button(saveButton,	edit=True,
		 	command='python("from shakeNodeMainMenu import ShakeNodeMainMenu; ShakeNodeMainMenu().getCreateCommandKwargs()")'
		)

		mel.eval('showOptionBox')


	def getCreateCommandKwargs(self, *args, **kwargs) -> dict:
		"""Gets the moduleTemplate command arguments.

		The arguments are queried eiter from the option box widgets or the saved option
		variables. If the widgets exist, their values will be saved to the option variables.

		Returns:
			dict: Dictionary of the kwargs to the moduleTemplate command.

		"""
		args = {}

		# Shake name
		if cmds.textFieldGrp(self._shakeNameWidget, exists=True):
			args['name'] = cmds.textFieldGrp(self._shakeNameWidget, query=True, text=True)
			cmds.optionVar(sv=(self._shakeNameWidget, args['name']))
		else:
			args['name'] = cmds.optionVar(query=self._shakeNameWidget) or 'shakeRot'

		# Shake attribute
		if cmds.textFieldGrp(self._shakeAttributeWidget, exists=True):
			args['attribute'] = cmds.textFieldGrp(self._shakeAttributeWidget, query=True, text=True)
			cmds.optionVar(sv=(self._shakeAttributeWidget, args['attribute']))
		else:
			args['attribute'] = cmds.optionVar(query=self._shakeAttributeWidget) or 'rotate'

		return args


	def resetToDefaults(self, *args, **kwargs):
		"""Resets the settings to default ones."""
		cmds.textFieldGrp(self._shakeNameWidget, edit=True, text="shakeRot")
		cmds.textFieldGrp(self._shakeAttributeWidget, edit=True, text="rotate")


	def doIt(self, *args, **kwargs) -> bool:
		"""Wrapper method for the main menu item.

		Returns:
			bool: True if the operation was successful, False if an	error occured during the operation.

		"""
		kwargs = self.getCreateCommandKwargs()
		if cmds.shake(**kwargs): return True

		return False
