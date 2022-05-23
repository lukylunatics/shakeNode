# Built-in imports
import logging

# Third-Party imports
from maya import cmds



moduleName = "shakeNode"
logger = logging.getLogger(f"{moduleName} Startup")



cmds.loadPlugin(moduleName)

logger.info(
	"Successfully imported plugin module '{}' v.{}".format(
	cmds.pluginInfo(moduleName, query=True, name=True),
	cmds.pluginInfo(moduleName, query=True, version=True))
)

