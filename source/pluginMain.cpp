#include "shakeNode.h"
#include "shakeNodeRot.h"
#include "shakeCommand.h"

// Function Sets
#include <maya/MFnPlugin.h>



MStatus initializePlugin(MObject obj) {
	MStatus status;
	MFnPlugin pluginFn(obj, "Lunatics", "1.0.1", "Any");

	status = pluginFn.registerNode(
		ShakeNode::typeName,
		ShakeNode::typeId,
		ShakeNode::creator,
		ShakeNode::initialize,
		MPxNode::kDependNode
	);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = pluginFn.registerNode(
		ShakeNodeRot::typeName,
		ShakeNodeRot::typeId,
		ShakeNodeRot::creator,
		ShakeNodeRot::initialize,
		MPxNode::kDependNode
	);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = pluginFn.registerCommand(
		ShakeCommand::commandName,
		ShakeCommand::creator,
		ShakeCommand::syntaxCreator
	);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (MGlobal::mayaState() == MGlobal::kInteractive) {
		MGlobal::executePythonCommandOnIdle("from shakeNodeMainMenu import ShakeNodeMainMenu");
		MGlobal::executePythonCommandOnIdle("ShakeNodeMainMenu().createMenuItems()");
	}

	return status;
}


MStatus uninitializePlugin(MObject obj) {
	MStatus status;
	MFnPlugin pluginFn(obj);

	status = pluginFn.deregisterCommand(ShakeCommand::commandName);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = pluginFn.deregisterNode(ShakeNodeRot::typeId);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = pluginFn.deregisterNode(ShakeNode::typeId);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (MGlobal::mayaState() == MGlobal::kInteractive) {
		MGlobal::executePythonCommandOnIdle("ShakeNodeMainMenu().deleteMenuItems()");
	}

	return status;
}
