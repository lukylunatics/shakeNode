#pragma once

// System Includes
#include <string>

// Maya General Includes
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MSyntax.h>
#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MSelectionList.h>
#include <maya/MDGModifier.h>
#include <maya/MTime.h>
#include <maya/MAnimControl.h>
#include <maya/MString.h>

// Function Sets
#include <maya/MFnDependencyNode.h>

// Iterators
#include <maya/MItSelectionList.h>

// Proxies
#include <maya/MPxCommand.h>



class ShakeCommand: public MPxCommand {

public:
	// Constructors
	ShakeCommand(): MPxCommand(), _shakeName("Shake"), _shakeAttribute("rotate") {};

	// Destructor
	virtual ~ShakeCommand() override;

	// Public Methods
	static void *creator() {return new ShakeCommand();}
	virtual bool isUndoable() const {return true;}
	static MSyntax syntaxCreator();
	virtual MStatus doIt(const MArgList &argList);
	virtual MStatus redoIt();
	virtual MStatus undoIt();

	// Public Data
	static const char *commandName;

	// Command's Flags
	static const char *nameFlagShort;
	static const char *nameFlagLong;

	static const char *attributeFlagShort;
	static const char *attributeFlagLong;

	static const char *helpFlagShort;
	static const char *helpFlagLong;

private:
	// Private Methods
	MStatus gatherFlagArguments(const MArgList &argList);
	MStatus _getTime1Output();
	MStatus _validateNodes();
	MStatus _createShakeNode(std::string name, std::string output);
	MStatus _setupShake();

	// Private Data
	std::string _shakeName;
	std::string _shakeAttribute;

	MPlug _timeOutPlug;

	MObject _nodeObj;
	MPlug _nodeInPlug;

	MObject _shakeObj;
	MPlug _shakeInPlug;
	MPlug _shakeOutPlug;

	MSelectionList _selList;
	MDGModifier _dgMod;

friend void displayHelp();
};
