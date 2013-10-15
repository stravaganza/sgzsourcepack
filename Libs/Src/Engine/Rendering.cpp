// OpenGL rendering utilities

// ==[ File ]===================================================================================
//
//  -Name     : Rendering.cpp
//  -Proyect  : Engine
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : UtilGL::Rendering namespace implementation.
//
// =============================================================================================

/*

	25/01/2002  -  File created.


*/


// ==[ Headers ]================================================================================


#include "Engine.h"


namespace UtilGL
{

extern SContextData* _pContextData;

namespace Rendering
{


// ==[ Functions  ]=============================================================================


// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool DrawRect(UtilGL::Texturing::CTexture2D *pTexture,
//								float fPosX,   float fPosY,  float fSizeX, float fSizeY,
//								float fAngle,  const CVector4& v4Color,
//								int nSrcBlend, int nDstBlend)
//
//
//  - Purpose   : Draws a rect with the specified parameters. If pTexture is
//                non-NULL, the rect will be textured.
//                Position and size parameters go in viewport range (0.0f - 1.0f).
//                So fPosX = 0.5f means a horizontal position just in the middle
//                while fSizeY = 2.0f for example would mean an y size two times
//                larger than viewport's height.
//                fAngle is an optional rotation angle around the z axis, v4Color
//                specifies color (including 4th param = alpha = translucency)
//                and nSrcBlend/nDstBlend tell which blending mode to use.
//
// -----------------------------------------------------------------------------
bool DrawRect(	UtilGL::Texturing::CTexture2D *pTexture,
				float fPosX,  float fPosY,  float fSizeX, float fSizeY,
				float fAngle, const CVector4& v4Color, int nSrcBlend, int nDstBlend)
{
	assert(UtilGL::IsReady());

	// Transform

	CVector3 v1(fPosX,          fPosY,          0.5f);
	CVector3 v2(fPosX,          fPosY + fSizeY, 0.5f);
	CVector3 v3(fPosX + fSizeX, fPosY + fSizeY, 0.5f);
	CVector3 v4(fPosX + fSizeX, fPosY,          0.5f);

	assert(CWindowGL::GetActiveWindow());

	float fAspect = CWindowGL::GetActiveWindow()->GetAspect();

	if(!IS_ZERO(fAngle))
	{
		CMatrix mtxRotation;

		float fCenterX = fPosX + fSizeX * 0.5f, fCenterY = fPosY + fSizeY * 0.5f; 

		mtxRotation.Translate(-fCenterX, -fCenterY, 0.0f);
		mtxRotation.RotateZ(fAngle);
		mtxRotation.Scale(1.0f, fAspect, 1.0f);
		mtxRotation.Translate(fCenterX, fCenterY, 0.0f);

		mtxRotation.Transform(&v1);
		mtxRotation.Transform(&v2);
		mtxRotation.Transform(&v3);
		mtxRotation.Transform(&v4);
	}

	UtilGL::Transforming::NormViewportToLocal(&v1);
	UtilGL::Transforming::NormViewportToLocal(&v2);
	UtilGL::Transforming::NormViewportToLocal(&v3);
	UtilGL::Transforming::NormViewportToLocal(&v4);

	// Set renderstates

	UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::DISABLED);

	if(pTexture)
	{
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
		pTexture->SetActive();
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // TODO: REMOVE
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	}

	if(!((nSrcBlend == UtilGL::States::BLEND_ONE  && nDstBlend == UtilGL::States::BLEND_ZERO) ||
		 (nSrcBlend == UtilGL::States::BLEND_ZERO && nDstBlend == UtilGL::States::BLEND_ONE)))
	{
		UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::ENABLED);
		UtilGL::States::Set(UtilGL::States::SRCBLEND, nSrcBlend);
		UtilGL::States::Set(UtilGL::States::DSTBLEND, nDstBlend);
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::DISABLED);
	}

	// Render

	UtilGL::States::SetColor(v4Color);

	if(pTexture)
	{
		glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3fv(v1.Data());

		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(v2.Data());

		glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(v3.Data());

		glTexCoord2f(1.0f, 1.0f);
		glVertex3fv(v4.Data());

		glEnd();
	}
	else
	{
		glBegin(GL_QUADS);

		glVertex3fv(v1.Data());
		glVertex3fv(v2.Data());
		glVertex3fv(v3.Data());
		glVertex3fv(v4.Data());

		glEnd();
	}

	UtilGL::States::Set(UtilGL::States::BLENDING, UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,  UtilGL::States::ENABLED);

	return true;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool DrawCenteredQuad(UtilGL::Texturing::CTexture2D *pTexture,
//										float fCenterX, float fCenterY, float fWidth, float fHeight,
//										float fAngle,   const CVector4& v4Color,
//										int nSrcBlend,  int nDstBlend)
//
//
//  - Purpose   : Same as previous but specifing a center.
//
// -----------------------------------------------------------------------------
bool DrawCenteredQuad(	UtilGL::Texturing::CTexture2D *pTexture,
						float fCenterX, float fCenterY,          float fWidth,  float fHeight,
						float fAngle,   const CVector4& v4Color, int nSrcBlend, int nDstBlend)
{
	assert(CWindowGL::GetActiveWindow());

	return DrawRect(pTexture,
					fCenterX - fWidth * 0.5f, fCenterY - fHeight * 0.5f, fWidth, fHeight,
					fAngle, v4Color, nSrcBlend, nDstBlend);
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool DrawBillboards(	int   nNumBillboards,
//										UtilGL::Texturing::CTexture2D *pTexture,
//										const CVector3* pPositions,
//										const CVector4* pColors,
//										float fSize,
//										float fAngle = 0.0f)
//
//
//
//  - Purpose   : Draws a group of billboards (front facing quads, no matter
//                how the camera points them).
//                If NULL texture is indicated then mapping coords won't be
//                specified, and no texture routines will be used. Same goes
//                for color array.
//                A unique constant world space size is provided for all of them.
//
// -----------------------------------------------------------------------------
bool DrawBillboards(int   nNumBillboards,
					UtilGL::Texturing::CTexture2D *pTexture,
					const CVector3* pPositions,
					const CVector4* pColors,
					float fSize,
					float fAngle)
{
	assert(UtilGL::IsReady());

	static float ppfTexCoords[4][2] = { 0.0f, 1.0f,
										0.0f, 0.0f,
										1.0f, 0.0f,
										1.0f, 1.0f };

	// Get local axis in eye space, we'll use this two vectors to
	// build the particles

	CMatrix mtxWorld     = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_WORLD);
	CMatrix mtxView      = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_VIEW);
	CMatrix mtxWorldView = mtxView * mtxWorld;

	// Rotate if we have to

	if(!IS_ZERO(fAngle))
	{
		mtxWorldView.RotateZ(fAngle);
	}

	CVector3 v3X = mtxWorldView.XAxis() * 0.5f * fSize;
	CVector3 v3Y = mtxWorldView.YAxis() * 0.5f * fSize;

	// Compute addition vectors

	CVector3 v3_1 = - v3X + v3Y;
	CVector3 v3_2 = - v3X - v3Y;
	CVector3 v3_3 =   v3X - v3Y;
	CVector3 v3_4 =   v3X + v3Y;

	if(pTexture)
	{
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
		pTexture->SetActive();
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // TODO: REMOVE
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	}

	// Draw quads

	glBegin(GL_QUADS);

	for(int nCount = 0; nCount < nNumBillboards; nCount++)
	{
		if(pColors) UtilGL::States::SetColor(pColors[nCount]);

		if(pTexture) glTexCoord2fv(&ppfTexCoords[0][0]);
		glVertex3fv((pPositions[nCount] + v3_1).Data());

		if(pTexture) glTexCoord2fv(&ppfTexCoords[1][0]);
		glVertex3fv((pPositions[nCount] + v3_2).Data());

		if(pTexture) glTexCoord2fv(&ppfTexCoords[2][0]);
		glVertex3fv((pPositions[nCount] + v3_3).Data());

		if(pTexture) glTexCoord2fv(&ppfTexCoords[3][0]);
		glVertex3fv((pPositions[nCount] + v3_4).Data());
	}

	glEnd();

	return true;
}

// --[  Function  ]-------------------------------------------------------------
//
//  - prototype : bool DrawBillboards(	int   nNumBillboards,
//										UtilGL::Texturing::CTexture2D *pTexture,
//										const CVector3* pPositions,
//										const CVector4* pColors,
//										const float*    pfSizes)
//
//
//
//  - Purpose   : Same as previous but specifying a size for each billboard.
//
// -----------------------------------------------------------------------------
bool DrawBillboards(int   nNumBillboards,
					UtilGL::Texturing::CTexture2D *pTexture,
					const CVector3* pPositions,
					const CVector4* pColors,
					const float*    pfSizes)
{
	assert(UtilGL::IsReady());

	static float ppfTexCoords[4][2] = { 0.0f, 1.0f,
										0.0f, 0.0f,
										1.0f, 0.0f,
										1.0f, 1.0f };

	// Get local axis in eye space, we'll use this two vectors to
	// build the particles

	CMatrix mtxWorld     = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_WORLD);
	CMatrix mtxView      = UtilGL::Transforming::GetMatrix(UtilGL::Transforming::MATRIX_VIEW);
	CMatrix mtxWorldView = mtxView * mtxWorld;

	CVector3 v3X = mtxWorldView.XAxis() * 0.5f;
	CVector3 v3Y = mtxWorldView.YAxis() * 0.5f;

	if(pTexture)
	{
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
		pTexture->SetActive();
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // TODO: REMOVE
	}
	else
	{
		UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	}

	// Draw quads

	glBegin(GL_QUADS);

	CVector3 v3ScaledX, v3ScaledY, v3_1, v3_2, v3_3, v3_4;

	for(int nCount = 0; nCount < nNumBillboards; nCount++)
	{
		// Compute addition vectors per particle

		v3ScaledX = v3X * pfSizes[nCount];
		v3ScaledY = v3Y * pfSizes[nCount];

		v3_1 = - v3ScaledX + v3ScaledY;
		v3_2 = - v3ScaledX - v3ScaledY;
		v3_3 =   v3ScaledX - v3ScaledY;
		v3_4 =   v3ScaledX + v3ScaledY;

		if(pColors) UtilGL::States::SetColor(pColors[nCount]);

		if(pTexture) glTexCoord2fv(&ppfTexCoords[0][0]);
		glVertex3fv((pPositions[nCount] + v3_1).Data());

		if(pTexture) glTexCoord2fv(&ppfTexCoords[1][0]);
		glVertex3fv((pPositions[nCount] + v3_2).Data());

		if(pTexture) glTexCoord2fv(&ppfTexCoords[2][0]);
		glVertex3fv((pPositions[nCount] + v3_3).Data());

		if(pTexture) glTexCoord2fv(&ppfTexCoords[3][0]);
		glVertex3fv((pPositions[nCount] + v3_4).Data());
	}

	glEnd();

	return true;
}

bool ComputeBoxBlur(int	nRadiusPwr2, UtilGL::Texturing::CTexture2D* pTexture)
{
	if(!pTexture)
	{
		return false;
	}

	if(nRadiusPwr2 < 1)
	{
		nRadiusPwr2 = 1;
	}

	int nRadiusPwr = MYROUND(log10f(nRadiusPwr2) / log10f(2));
	int nRadius    = pow(2, nRadiusPwr);

	if(nRadius < 1) nRadius = 1;

	// Set States

	UtilGL::States::Set(UtilGL::States::LIGHTING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::ENABLED);
	UtilGL::States::Set(UtilGL::States::SRCBLEND,  UtilGL::States::BLEND_SRCALPHA);
	UtilGL::States::Set(UtilGL::States::DSTBLEND,  UtilGL::States::BLEND_INVSRCALPHA);

	// Transforming

	UtilGL::Transforming::PushMatrix(UtilGL::Transforming::MATRIX_WORLD);
	UtilGL::Transforming::PushMatrix(UtilGL::Transforming::MATRIX_VIEW);
	UtilGL::Transforming::PushMatrix(UtilGL::Transforming::MATRIX_PROJECTION);

	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_WORLD);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_VIEW);
	UtilGL::Transforming::ClearMatrix(UtilGL::Transforming::MATRIX_PROJECTION);

	glMatrixMode(GL_PROJECTION);
	glOrtho(0, pTexture->GetWidth(), pTexture->GetHeight(), 0, 0.0f, 10.0f);

	// Texturing

	UtilGL::States::SetColor(CVector4(1.0f, 1.0f, 1.0f, 0.66f));

	UtilGL::Texturing::SetCurrentUnit(0);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	pTexture->SetActive();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,  GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,  GL_PRIMARY_COLOR_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,  GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);

	UtilGL::Texturing::SetCurrentUnit(1);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::ENABLED);
	pTexture->SetActive();

	float pfConstantCol[4] = { 0.5f, 0.5f, 0.5f, 0.0f };
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, pfConstantCol);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,  GL_INTERPOLATE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,  GL_PREVIOUS_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,  GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_COLOR);

	// Blur loop

	int nCurRadius = 1;

	while(nRadius >= nCurRadius)
	{
		// First copy to texture

		pTexture->CopyFromFramebuffer(0, 0, 0, 0, pTexture->GetWidth(), pTexture->GetHeight());

		float fOffsetU = (float)nCurRadius / (float)pTexture->GetWidth();
		float fOffsetV = (float)nCurRadius / (float)pTexture->GetHeight();

		// Columns

		glBegin(GL_QUADS);

		glMultiTexCoord2f(GL_TEXTURE0_ARB, 0.0f - fOffsetU, 1.0f);
		glMultiTexCoord2f(GL_TEXTURE1_ARB, 0.0f + fOffsetU, 1.0f);
		glVertex3i(0, 0, -1);

		glMultiTexCoord2f(GL_TEXTURE0_ARB, 0.0f - fOffsetU, 0.0f);
		glMultiTexCoord2f(GL_TEXTURE1_ARB, 0.0f + fOffsetU, 0.0f);
		glVertex3i(0, pTexture->GetHeight(), -1);

		glMultiTexCoord2f(GL_TEXTURE0_ARB, 1.0f - fOffsetU, 0.0f);
		glMultiTexCoord2f(GL_TEXTURE1_ARB, 1.0f + fOffsetU, 0.0f);
		glVertex3i(pTexture->GetWidth(), pTexture->GetHeight(), -1);

		glMultiTexCoord2f(GL_TEXTURE0_ARB, 1.0f - fOffsetU, 1.0f);
		glMultiTexCoord2f(GL_TEXTURE1_ARB, 1.0f + fOffsetU, 1.0f);
		glVertex3i(pTexture->GetWidth(), 0, -1);

		glEnd();

		// Store accumulation

		pTexture->CopyFromFramebuffer(0, 0, 0, 0, pTexture->GetWidth(), pTexture->GetHeight());

		// Rows

		glBegin(GL_QUADS);

		glMultiTexCoord2f(GL_TEXTURE0_ARB, 0.0f, 1.0f + fOffsetV);
		glMultiTexCoord2f(GL_TEXTURE1_ARB, 0.0f, 1.0f - fOffsetV);
		glVertex3i(0, 0, -1);

		glMultiTexCoord2f(GL_TEXTURE0_ARB, 0.0f, 0.0f + fOffsetV);
		glMultiTexCoord2f(GL_TEXTURE1_ARB, 0.0f, 0.0f - fOffsetV);
		glVertex3i(0, pTexture->GetHeight(), -1);

		glMultiTexCoord2f(GL_TEXTURE0_ARB, 1.0f, 0.0f + fOffsetV);
		glMultiTexCoord2f(GL_TEXTURE1_ARB, 1.0f, 0.0f - fOffsetV);
		glVertex3i(pTexture->GetWidth(), pTexture->GetHeight(), -1);

		glMultiTexCoord2f(GL_TEXTURE0_ARB, 1.0f, 1.0f + fOffsetV);
		glMultiTexCoord2f(GL_TEXTURE1_ARB, 1.0f, 1.0f - fOffsetV);
		glVertex3i(pTexture->GetWidth(), 0, -1);

		glEnd();

		nCurRadius = nCurRadius * 2;
	}

	pTexture->CopyFromFramebuffer(0, 0, 0, 0, pTexture->GetWidth(), pTexture->GetHeight());

	// States

	UtilGL::States::Set(UtilGL::States::BLENDING,  UtilGL::States::DISABLED);
	UtilGL::States::Set(UtilGL::States::ZBUFFER,   UtilGL::States::ENABLED);

	UtilGL::Texturing::SetCurrentUnit(1);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	UtilGL::Texturing::SetCurrentUnit(0);
	UtilGL::States::Set(UtilGL::States::TEXTURE2D, UtilGL::States::DISABLED);
	
	// Transforming

	UtilGL::Transforming::PopMatrix(UtilGL::Transforming::MATRIX_PROJECTION);
	UtilGL::Transforming::PopMatrix(UtilGL::Transforming::MATRIX_VIEW);
	UtilGL::Transforming::PopMatrix(UtilGL::Transforming::MATRIX_WORLD);

	return true;
}

} // namespace Rendering

} // namespace UtilGL