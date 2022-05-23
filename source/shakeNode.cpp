#include "shakeNode.h"



// Node's attributes
const MString ShakeNode::typeName("shakeNode");
const MTypeId ShakeNode::typeId(0x00122710);

// Node's input attributes
MObject ShakeNode::enableAttr;
MObject ShakeNode::inTimeAttr;
MObject ShakeNode::weightAttr;
MObject ShakeNode::seedAttr;
MObject ShakeNode::frequencyAttr;
MObject ShakeNode::strengthAttrX;
MObject ShakeNode::strengthAttrY;
MObject ShakeNode::strengthAttrZ;
MObject ShakeNode::strengthAttr;
MObject ShakeNode::fractalAttr;
MObject ShakeNode::roughnessAttr;
MObject ShakeNode::shakeAttr;
 
// Node's output attributes
MObject ShakeNode::outputAttrX;
MObject ShakeNode::outputAttrY;
MObject ShakeNode::outputAttrZ;
MObject ShakeNode::outputAttr;



ShakeNode::~ShakeNode() {
	/* ShakeNode Destructor */
}

MStatus ShakeNode::initialize() {
	/* Node initializer.

	This method initializes the node, and should be overridden in user-defined
	nodes.

	Returns:
		status code (MStatus): kSuccess if the operation was successful,
			kFailure if an error occured during the operation

	*/
	MStatus status;
	MFnNumericAttribute nAttr;
	MFnUnitAttribute uAttr;
	MFnCompoundAttribute cAttr;

	enableAttr = nAttr.create("enable", "ena", MFnNumericData::kBoolean, 1);
	nAttr.setKeyable(true);
	nAttr.setReadable(false);

	inTimeAttr = uAttr.create("inTime", "itm", MFnUnitAttribute::kTime);
	uAttr.setKeyable(true);
	uAttr.setReadable(false);

	weightAttr = nAttr.create("weight", "wgt", MFnNumericData::kDouble, 1.0);
	nAttr.setMin(0);
	nAttr.setMax(1);

	seedAttr = nAttr.create("seed", "sed", MFnNumericData::kInt, 21);
	nAttr.setMin(0);

	frequencyAttr = nAttr.create("frequency", "frq", MFnNumericData::kDouble, 1.0);
	nAttr.setMin(0.001);
	nAttr.setMax(10);

	strengthAttrX = nAttr.create("strengthX", "strX", MFnNumericData::kDouble, 10.0);
	strengthAttrY = nAttr.create("strengthY", "strY", MFnNumericData::kDouble, 10.0);
	strengthAttrZ = nAttr.create("strengthZ", "strZ", MFnNumericData::kDouble, 10.0);
	strengthAttr = nAttr.create("strength", "str", strengthAttrX, strengthAttrY, strengthAttrZ);

	fractalAttr = nAttr.create("fractalNoise", "frn", MFnNumericData::kDouble, 0.0);
	nAttr.setMin(0);
	nAttr.setMax(1);

	roughnessAttr = nAttr.create("roughness", "rgh", MFnNumericData::kDouble, 0.0);
	nAttr.setMin(0);
	nAttr.setMax(1);

	/* shakeAttr:
	-- shake
		 | -- weight
		 | -- seed
		 | -- frequency
		 | -- strength X Y Z
		 | -- fractal
		 | -- roughness
	*/
	shakeAttr = cAttr.create("shakeLayer", "shk");
	cAttr.addChild(weightAttr);
	cAttr.addChild(seedAttr);
	cAttr.addChild(frequencyAttr);
	cAttr.addChild(strengthAttr);
	cAttr.addChild(fractalAttr);
	cAttr.addChild(roughnessAttr);
	cAttr.setArray(true);
	cAttr.setKeyable(true);
	cAttr.setReadable(false);

	outputAttrX = nAttr.create("outputX", "outX", MFnNumericData::kDouble, 0.0);
	outputAttrY = nAttr.create("outputY", "outY", MFnNumericData::kDouble, 0.0);
	outputAttrZ = nAttr.create("outputZ", "outZ", MFnNumericData::kDouble, 0.0);
	outputAttr = nAttr.create("output", "out", outputAttrX, outputAttrY, outputAttrZ);
	nAttr.setWritable(false);

	addAttribute(enableAttr);
	addAttribute(inTimeAttr);
	addAttribute(shakeAttr);
	addAttribute(outputAttr);

	attributeAffects(enableAttr, outputAttr);
	attributeAffects(inTimeAttr, outputAttr);
	attributeAffects(shakeAttr, outputAttr);

	return MS::kSuccess;
}

MStatus ShakeNode::compute(const MPlug &plug, MDataBlock &dataBlock) {
	/* This method should be overridden in user defined nodes.

	Recompute the given output based on the nodes inputs. The plug represents the data
	value that needs to be recomputed, and the data block holds the storage for all of
	the node's attributes.

	The MDataBlock will provide smart handles for reading and writing this node's
	attribute values. Only these values should be used when performing computations.

	When evaluating the dependency graph, Maya will first call the compute method for
	this node. If the plug that is provided to the compute indicates that the attribute
	was defined by the Maya parent node, the compute method should return
	MS::kUnknownParameter. When this occurs, Maya will call the internal Maya node from
	which the user-defined node is derived to compute the plug's value.

	This means that a user defined node does not need to be concerned with computing
	inherited output attributes. However, if desired, these can be safely recomputed by
	this method to change the behaviour of the node.

	Args:
		plug (MPlug&): Plug representing the attribute that needs to be recomputed
		dataBlock (MDataBlock&): Data block containing storage for the node's attributes

	Returns:
		status code (MStatus): kSuccess if the operation was successful,
			kFailure if an error occured during the operation

	*/
	MStatus status;

	bool enable = dataBlock.inputValue(enableAttr, &status).asBool();
	if (enable == 0) {
		dataBlock.setClean(plug);
	} else {
		MArrayDataHandle shakeLayersDH = dataBlock.inputArrayValue(shakeAttr, &status);
		unsigned int numShakeLayers = shakeLayersDH.elementCount();
		if (numShakeLayers == 0) {
			dataBlock.setClean(plug);
		}
		else {
			double uiTime = dataBlock.inputValue(inTimeAttr, &status).asTime().asUnits(MTime::uiUnit());
			double resultX = 0, resultY = 0, resultZ = 0;
			PerlinNoise ipNoise;
			for (unsigned int i = 0; i < numShakeLayers; ++i) {
				shakeLayersDH.jumpToArrayElement(i);
				MDataHandle shakeLayerDH = shakeLayersDH.inputValue();
				double weight = shakeLayerDH.child(weightAttr).asDouble();
				if (weight != 0) {
					int seed = shakeLayerDH.child(seedAttr).asInt();
					double freq = shakeLayerDH.child(frequencyAttr).asDouble();
					MDataHandle strengthDH = shakeLayerDH.child(strengthAttr);
					double strX = strengthDH.child(strengthAttrX).asDouble();
					double strY = strengthDH.child(strengthAttrY).asDouble();
					double strZ = strengthDH.child(strengthAttrZ).asDouble();
					double fractal = shakeLayerDH.child(fractalAttr).asDouble();
					double rough = shakeLayerDH.child(roughnessAttr).asDouble();
					resultX += ipNoise.calculateNoise(weight, uiTime, seed + 13.0, freq, strX, fractal, rough);
					resultY += ipNoise.calculateNoise(weight, uiTime, seed + 578.0, freq, strY, fractal, rough);
					resultZ += ipNoise.calculateNoise(weight, uiTime, seed + 1511.0, freq, strZ, fractal, rough);
				}
			}
			MDataHandle outputDH = dataBlock.outputValue(outputAttr, &status);
			outputDH.set3Double(resultX, resultY, resultZ);
			outputDH.setClean();
		}
		dataBlock.setClean(plug);
	}

	return MS::kSuccess;
}
