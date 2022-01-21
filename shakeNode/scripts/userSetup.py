from maya import cmds



pluginName = "shakeNode"



cmds.loadPlugin(pluginName)
print(
	"// Successfully imported plugin module '{}' v.{} //".format(
	cmds.pluginInfo(pluginName, query=True, name=True),
	cmds.pluginInfo(pluginName, query=True, version=True))
)

