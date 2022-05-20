#pragma once

#include "perlinNoise.h"

// System Includes
#include <string>

// Maya General Includes
#include <maya/MGlobal.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDataHandle.h>

// Function Sets
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnAttribute.h>

// Proxies
#include <maya/MPxNode.h>



class ShakeNode: public MPxNode {

public:
	// Constructors
	ShakeNode(): MPxNode() {};

	// Destructor
	virtual ~ShakeNode() override;

	// Public Methods
	static void *creator() {return new ShakeNode();}
	static MStatus initialize();
	virtual MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// Node's attributes
	static const MString typeName;
	static const MTypeId typeId;

	// Node's input attributes
	static MObject enableAttr;
	static MObject inTimeAttr;
	static MObject weightAttr;
	static MObject seedAttr;
	static MObject frequencyAttr;
	static MObject strengthAttrX;
	static MObject strengthAttrY;
	static MObject strengthAttrZ;
	static MObject strengthAttr;
	static MObject fractalAttr;
	static MObject roughnessAttr;
	static MObject shakeAttr;

	// Node's output attributes
	static MObject outputAttrX;
	static MObject outputAttrY;
	static MObject outputAttrZ;
	static MObject outputAttr;
};
