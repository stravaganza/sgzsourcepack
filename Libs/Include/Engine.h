#ifndef __ENGINE_H
#define __ENGINE_H

// Standard headers
#include <windows.h>
#include <windowsx.h>
#include <memory.h>
#include <vfw.h>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>

// Lev Povalahev's OpenGL extension utilities (includes also standard OpenGL header).
#include <extgl.h>
#include <GL\glu.h>

// BaseLib
#include <BaseLib.h>

// ID's
#include <IDs.h>

// GeoMath
#include <GeoMath.h>

// Animation
#include <Animation.h>
#include <AnimPos.h>
#include <AnimRot.h>
#include <AnimScl.h>
#include <AnimRotSampled.h>
#include <AnimPosSampled.h>
#include <AnimSclSampled.h>

// OpenGL Util
#include <Extensions.h>
#include <RenderContext.h>
#include <Texturing.h>
#include <Rendering.h>
#include <States.h>
#include <Transforming.h>
#include <UtilGL.h>

// NodeVisitors
#include <NodeVisitor.h>
#include <LightingVisitor.h>
#include <RenderVisitor.h>
#include <TransformVisitor.h>
#include <EnumVisitor.h>

// Misc
#include <PerlinNoise.h>

// SceneGraph
#include <Node.h>
#include <Renderable.h>
#include <Camera.h>
#include <Object.h>
#include <BoundingVolume.h>
#include <BoundingBox.h>
#include <Helper.h>
#include <Light.h>
#include <Scene.h>

// Shaders
#include <Shader.h>
#include <ShaderList.h>
#include <ShaderStandard.h>

// Advanced parser
#include <AdvancedParser.h>

// Special FX
#include <LensFlare.h>
#include <ParticleSystem.h>
#include <ParticleSystemStd.h>

// Sceneview
#include <SceneView.h>
#include <WindowGL.h>
#include <AviRenderer.h>

#endif