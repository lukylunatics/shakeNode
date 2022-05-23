#pragma once

#include "shakeNode.h"

// System Includes
#include <string>

// Maya Api Includes
#include <maya/MPxNode.h>
#include <maya/MGlobal.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDataHandle.h>

// Function Sets
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnCompoundAttribute.h>



class ShakeNodeRot: public ShakeNode {

public:
	// Constructors
	ShakeNodeRot(): ShakeNode() {};

	// Destructor
	virtual ~ShakeNodeRot() override;

	// Public Methods
	static void *creator() {return new ShakeNodeRot();}
	static MStatus initialize();
	virtual MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

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

private:
	// Private Methods
	double radians(double degrees);

	// Private Data
	const double pi = 3.14159265358979323846;
};
