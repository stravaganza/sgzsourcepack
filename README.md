STRAVAGANZA SOURCEPACK
======================

## INTRODUCTION
This pack contains the whole soucecode used by Stravaganza in all its productions since 2003: Euskal 11 Invitation, Cafre, Decadentia, r8080 and Arise. Older demos are not included. They include the full sourcecode of all libraries, demos and tools (besides the Demo Editor which is being ported to .NET, and some tools being currently developed).
Why? Just because they will be more useful in your hands than in mi HD. I don't give a shit about rippers, and I'm pretty sure nobody will be interested in ripping my code anyway. 
I consider sourcecode reading absolutely boring but some people might get some nice ideas from it.

Projects included:

* Libraries
 * BaseLib
 * 3D Engine
 * DemoSystem
 * Plugins
* Tools
 * DemoPlayer
 * 3DStudioMAX Tools
 * DemoEditor  (not included)
 * VoteCounter (not included)
 * VJ System   (not included)
* Demos
 * Euskal 11 Invitation
 * Cafre
 * Decadentia
 * r08080
 * Arise

Workspace files for VC6 and VC7 are given.
       

## BUILD INSTRUCTIONS
### Libraries
Open the VC6 (.dsw) or VC7 (.sln) workspace file located at \Libs\Src. Simply choose Build->Batch Build or build the Plugins project. This will generate 3 static libraries (BaseLib, 3DEngine and DemoSystem) and a dll (Plugins) with its import library.

### Tools
Open the VC6/VC7 workspace located at \Tools\Src\ and open the MaxTools settings window. Configure the maxsdk include path and the maxsdk library path. Run Build->Batch Build. This will generate the necessary tools/plugins in \Tools\Bin\ If you want to use the 3DStudioMAX plugin just place the .dlu file in max's plugin folder. 
FreeImage.dll needs to be placed in the root MAX dir (Copy from Libs\External\Dll). 
The plugin is accessed from the utility panel.

### Demos
After building the libraries, open the workspace located at \Prods\Src and build all projects.
To run all demos you need to include the binaries and the plugin dll generated before. 
Both Freeimage.dll and Bass.dll from \Libs\Extenal have to be included in the exe directories.
