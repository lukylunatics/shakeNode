# shakeNode
ShakeNode is a user-defined MPxNode dsegined to create convincing procedural noise on cameras and not only in Autodesk Maya.

![shakeNode loaded in Maya](/images/shakeNodeStill1.png)



# Video Tutorial:
[![Watch the video](/images/introSlate.png)](https://vimeo.com/669401036)



# How to install:

### Automated installation:
1. Unzip the archive.
2. Drag and drop the "dragDropInstaller.py" into the Maya 3d viewport.
3. Follow the steps on screen.

### Manual installation:
1. Close all running instances of Maya.
2. Go to your Maya modules directory (create the "modules" folder if it does not exist):
```
(Windows) /Users/<username>/Documents/maya/modules/
(Mac OS X) $HOME/Library/Preferences/Autodesk/maya/modules/
(Linux)	$HOME/maya/modules/
```
3. Copy the "shakeNode" folder and "shakeNode.mod" file in the modules folder.

4. Start Maya, the module will be loaded and the console should print out:
```
// Successfully imported plugin module 'shakeNode' v.1.0.0 //
```



# How to use:
After loading the plugin in Maya, type `shake` in the mel command line. Works with selection. You can as well provide a custom name with the -n flag and / or attribute with the -a flag. Some practical use examples:
#### Mel:
```
shake;
shake "pCube1";
shake -n "mainRotShake";
shake -n "positionShake" -a "translate" "pCube1";
```
#### Python:
```
from maya import cmds

cmds.shake()
cmds.shake("pCube1")
cmds.shake(n="mainRotShake")
cmds.shake("pCube1", n="positionShake", a="translate")
```

# Supported Maya versions and platforms:
```
Windows: Maya 2022, 2020
Linux:   Maya 2022
MacOS:   Coming Soon
```

# Release Notes:
```
Version 1.0.0

Initial release

```
