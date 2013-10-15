#include "Plugins.h"
#include "FXSceneConstruction.h"


const std::string CFXSceneConstruction::CLASSNAME("Scene Construction");

REGISTER_PLUGIN(CPluginFactory, CFXSceneConstruction)


#define STR_SORT_RANDOMPIECE               "Random Piece"
#define STR_SORT_RANDOMGROUP_RANDOMPIECE   "Random Group + Random Piece"
#define STR_SORT_AXISDIR                   "Axis Direction"
#define STR_SORT_RANDOMGROUP_AXISDIR       "Random Group + Axis Direction"

#define STR_CREATION_ALPHA                 "Alpha"
#define STR_CREATION_ALPHA_ORTOGONALOFFSET "Alpha + Ortogonal Offset"
#define STR_CREATION_ALPHA_ORTOGONALANGLE  "Alpha + Ortogonal Angle"
#define STR_CREATION_ALPHA_RANDOMANGLE     "Alpha + Random Angle" 


CFXSceneConstruction::CFXSceneConstruction()
{
	DefineVar("Scene Start Time",    CVarFloat::CLASSNAME,  "Time from where the scene will start playing");
	DefineVar("Scene Speed",         CVarSpeed::CLASSNAME,  "Play speed. 1.0 = normal speed");
	DefineVar("Scene Camera",        CVarString::CLASSNAME, "Active camera name");
	DefineVar("Scene Render Mode",   CVarCombo::CLASSNAME,  "Render mode. Mostly for debugging purposes");
	DefineVar("Scene Loop Length",   CVarFloat::CLASSNAME,  "Loop Length (-1=no loop, 0=scene length, >0=user defined");
	DefineVar("Scene Loop Start",    CVarFloat::CLASSNAME,  "Loop Start Time");

	DefineVar("Group Prefix Length", CVarInt::CLASSNAME,    "Pieces can be grouped (f.e bricks of a same building) assigning a prefix to the object name. The prefix lenght is the number of characters in the prefix");
	DefineVar("Randomness (0-1)",    CVarFloat::CLASSNAME,  "Adds \"noise\" to some parameters like: Piece Build Time, Offset Length, Angle in Ortogonal Angle build mode");
	DefineVar("Seed",                CVarInt::CLASSNAME,    "Every seed gives different results. If you don't like a result try with different seed values");
	DefineVar("Construction (0-1)",  CVarFloat::CLASSNAME,  "Does the animation. 0.0 means totally deconstructed while 1.0 means totally constructed. Use the inbetween interval to animate.");
	DefineVar("Piece Build Time",    CVarFloat::CLASSNAME,  "The time it takes to a piece to be built, ranging from 0.0 to 1.0");
	DefineVar("Offset Length",       CVarFloat::CLASSNAME,  "When using Alpha+Ortogonal Offset creation mode, this tells how far pieces start from the end position");
	DefineVar("Axis Direction",      CVarCombo::CLASSNAME,  "When using some Axis Direction sort mode, this tells which axis direction is used to sort all pieces");
	DefineVar("Group Interval (0-1)",CVarFloat::CLASSNAME,  "When using some group sort mode, this tells the time separation between groups. 0.0 means they start all at the same time while 1.0 means they start one after each other");
	DefineVar("Skip Prefix",         CVarString::CLASSNAME, "If a prefix is specified all objects with a name starting with it will not be constructed");

	DefineVar("Type Of Sort",		 CVarCombo::CLASSNAME,  "Tells which sort method to use to construct the pieces");
	DefineVar("Type Of Creation",	 CVarCombo::CLASSNAME,  "Tells how pieces are animated to do the final construction");

	DefineResourceReference("Scene", CResourceScene::CLASSNAME);

	// Set combo options (the first added becomes the default value)

	((CVarCombo*)GetVar("Scene Render Mode"))->AddOption("Full");
	((CVarCombo*)GetVar("Scene Render Mode"))->AddOption("Solid");
	((CVarCombo*)GetVar("Scene Render Mode"))->AddOption("Wire");

	((CVarCombo*)GetVar("Axis Direction"))->AddOption("-X Axis");
	((CVarCombo*)GetVar("Axis Direction"))->AddOption("+X Axis");
	((CVarCombo*)GetVar("Axis Direction"))->AddOption("-Y Axis");
	((CVarCombo*)GetVar("Axis Direction"))->AddOption("+Y Axis");
	((CVarCombo*)GetVar("Axis Direction"))->AddOption("-Z Axis");
	((CVarCombo*)GetVar("Axis Direction"))->AddOption("+Z Axis");

	((CVarCombo*)GetVar("Type Of Sort"))->AddOption(STR_SORT_RANDOMPIECE);
	((CVarCombo*)GetVar("Type Of Sort"))->AddOption(STR_SORT_RANDOMGROUP_RANDOMPIECE);
	((CVarCombo*)GetVar("Type Of Sort"))->AddOption(STR_SORT_AXISDIR);
	((CVarCombo*)GetVar("Type Of Sort"))->AddOption(STR_SORT_RANDOMGROUP_AXISDIR);

	((CVarCombo*)GetVar("Type Of Creation"))->AddOption(STR_CREATION_ALPHA);
	((CVarCombo*)GetVar("Type Of Creation"))->AddOption(STR_CREATION_ALPHA_ORTOGONALOFFSET);
	((CVarCombo*)GetVar("Type Of Creation"))->AddOption(STR_CREATION_ALPHA_ORTOGONALANGLE);
	((CVarCombo*)GetVar("Type Of Creation"))->AddOption(STR_CREATION_ALPHA_RANDOMANGLE);

	// Set the rest of default values

	((CVarFloat*)GetVar("Scene Start Time" ))->LockConstant(true); // must be constant
	((CVarFloat*)GetVar("Scene Start Time" ))->SetConstant(0.0f);
	((CVarSpeed*)GetVar("Scene Speed"      ))->SetConstant(1.0f);

	((CVarInt*)GetVar("Group Prefix Length"))->LockConstant(true); // must be constant
	((CVarInt*)GetVar("Group Prefix Length"))->SetConstant(0);
	((CVarFloat*)GetVar("Randomness (0-1)"))->LockConstant(true); // must be constant
	((CVarFloat*)GetVar("Randomness (0-1)"))->SetConstant(0);
	((CVarInt*)GetVar("Seed"))->LockConstant(true); // must be constant
	((CVarInt*)GetVar("Seed"))->SetConstant(0);
	((CVarFloat*)GetVar("Construction (0-1)"))->SetConstant(1.0f);
	((CVarFloat*)GetVar("Piece Build Time"))->LockConstant(true);
	((CVarFloat*)GetVar("Piece Build Time"))->SetConstant(0.1f);
	((CVarFloat*)GetVar("Offset Length"))->SetConstant(10.0f);
	((CVarFloat*)GetVar("Group Interval (0-1)"))->LockConstant(true);
	((CVarFloat*)GetVar("Group Interval (0-1)"))->SetConstant(1.0f);

	((CVarFloat*)GetVar("Scene Loop Start" ))->SetConstant(0.0f);
	((CVarFloat*)GetVar("Scene Loop Length"))->SetConstant(-1.0f);

	GetVar("Group Prefix Length" )->ReloadFXOnChange(this);
	GetVar("Randomness (0-1)"    )->ReloadFXOnChange(this);
	GetVar("Seed"                )->ReloadFXOnChange(this);
	GetVar("Piece Build Time"    )->ReloadFXOnChange(this);
	GetVar("Type Of Sort"        )->ReloadFXOnChange(this);
	GetVar("Type Of Creation"    )->ReloadFXOnChange(this);
	GetVar("Offset Length"       )->ReloadFXOnChange(this);
	GetVar("Axis Direction"      )->ReloadFXOnChange(this);
	GetVar("Group Interval (0-1)")->ReloadFXOnChange(this);
	GetVar("Skip Prefix"         )->ReloadFXOnChange(this);

	// Define additional transformation variables

	DefineAddTransformVars(this);

	m_fAnimStart = m_fAnimEnd = 0.0f;
	m_pResScene  = NULL;
}

CFXSceneConstruction::~CFXSceneConstruction()
{
	Free();
}

bool CFXSceneConstruction::ExecuteCommand(const std::string& strCommand)
{
	CInputMemory    input;
	CAdvancedParser parser;

	input.Open(const_cast<char*>(strCommand.data()), strCommand.length());
	parser.Init(&input);

	bool bResult = parser.ReadKeyString("#Camera", '=', &m_strCamera, CParser::SEARCH_INCURLINE);

	if(bResult) ((CVarString*)GetVar("Scene Camera"))->SetString(m_strCamera);

	return bResult;
}

bool CFXSceneConstruction::LoadData(CResourceList* pResourceList)
{
	if(!(m_pResScene = this->FindResource(pResourceList, "Scene", CResourceScene::CLASSNAME)))
	{
		FXLoadError("Can't find scene resource");
		return false;
	}

	m_pResScene->NotifyDependency(this);

	// Get camera name

	CVarString::CValueString valueCamera;
	EvaluateVar("Scene Camera", 0.0f, &valueCamera);
	m_strCamera = valueCamera.GetString();

	CSceneView* pSceneView = const_cast<CSceneView*>(((CResourceScene*)m_pResScene)->GetSceneView());

	if(pSceneView)
	{
		// Put scene to starttime (to optimize particles, mainly)

		CVarFloat::CValueFloat   valueStartTime;
		CVarSpeed::CValueSpeed   valueSpeed;
		CVarInt::CValueInt       valuePrefixLength;
		CVarFloat::CValueFloat   valueConstruction;
		CVarFloat::CValueFloat   valuePieceBuildTime;
		CVarFloat::CValueFloat   valueOffsetLength;
		CVarFloat::CValueFloat   valueRandomness;
		CVarInt::CValueInt       valueSeed;
		CVarCombo::CValueCombo   valueSortType;
		CVarCombo::CValueCombo   valueCreationType;
		CVarCombo::CValueCombo   valueAxisDirection;
		CVarFloat::CValueFloat   valueGroupInterval;
		CVarString::CValueString valueSkipPrefix;

		EvaluateVar("Scene Start Time",     0.0f, &valueStartTime);
		EvaluateVar("Scene Speed",          valueStartTime.GetValue(), &valueSpeed);
		EvaluateVar("Group Prefix Length",  0.0f, &valuePrefixLength);
		EvaluateVar("Construction (0-1)",   0.0f, &valueConstruction);
		EvaluateVar("Piece Build Time",     0.0f, &valuePieceBuildTime);
		EvaluateVar("Offset Length",        0.0f, &valueOffsetLength);
		EvaluateVar("Randomness (0-1)",     0.0f, &valueRandomness);
		EvaluateVar("Seed",				    0.0f, &valueSeed);
		EvaluateVar("Type Of Sort",         0.0f, &valueSortType);
		EvaluateVar("Type Of Creation",     0.0f, &valueCreationType);
		EvaluateVar("Axis Direction",       0.0f, &valueAxisDirection);
		EvaluateVar("Group Interval (0-1)", 0.0f, &valueGroupInterval);
		EvaluateVar("Skip Prefix",          0.0f, &valueSkipPrefix);

		float fTime = valueStartTime.GetValue() + valueSpeed.Integrate(0.0f, valueStartTime.GetValue());

		CNode* pNode = (CNode*)pSceneView->GetRootNode();

		if(pNode)
		{
			CTransformVisitor transformVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);
			transformVisitor.Reset();
			transformVisitor.SetEvalTime(fTime);
			pNode->AcceptVisitor(&transformVisitor);
			transformVisitor.DoPendingTransformations();

			// Process objects

			CEnumVisitor enumVisitor(CNodeVisitor::TRAVERSAL_DOWNWARDS);
			pNode->AcceptVisitor(&enumVisitor);

			srand(valueSeed.GetValue());

			int i;
			std::map<std::string, int> mapPrefixes;

			for(i = 0; i < enumVisitor.GetNumObjects(); i++)
			{
				CObject* pObject = enumVisitor.GetObject(i);

				if(pObject)
				{
					// First check if we have to skip this object:

					if(valueSkipPrefix.GetString().size() > 0)
					{
						if(pObject->GetNodeName().length() >= valueSkipPrefix.GetString().length())
						{
							if(valueSkipPrefix.GetString() == pObject->GetNodeName().substr(0, valueSkipPrefix.GetString().length()))
							{
								continue;
							}
						}
					}

					// Check if it's a new group

					std::string strPrefix = pObject->GetNodeName();

					if(pObject->GetNodeName().length() > valuePrefixLength.GetValue())
					{
						strPrefix = pObject->GetNodeName().substr(0, valuePrefixLength.GetValue());
					}

					// Create new prefix entry if needed

					if(mapPrefixes.find(strPrefix) != mapPrefixes.end())
					{
						mapPrefixes[strPrefix] = mapPrefixes.size();
					}

					// Create new structure

					SOffset newOffset;

					newOffset.originalTM      = pObject->GetAbsoluteTM();
					newOffset.fOriginalAlpha  = pObject->GetAlpha();
					newOffset.fDuration       = valuePieceBuildTime.GetValue() + ComputeRand(-valueRandomness.GetValue(), valueRandomness.GetValue()) * valuePieceBuildTime.GetValue();
					newOffset.fAlpha          = 0.0f;
					newOffset.nGroup          = mapPrefixes[strPrefix];

					if(newOffset.fDuration < 0.001f) newOffset.fDuration = 0.001f;

					m_mapOffsets[pObject]     = newOffset;
				}
			}

			// Compute some data to sort elements

			MAPOFFSETS::iterator it;

			float fGroupIntervalTime; // time between one group and the start of the other
			float fGroupDuration;     // construction duration of each group

			float fDiv = 1.0f / mapPrefixes.size();
			fGroupIntervalTime = fDiv * valueGroupInterval.GetValue();
			fGroupDuration = 1.0f - (fGroupIntervalTime * (mapPrefixes.size() - 1));

			std::vector<float> vecGroupStarts;
			std::vector<float> vecGroupRandomStarts;
			std::vector<float> vecDistances;

			for(i = 0; i < mapPrefixes.size(); i++)
			{
				vecGroupStarts.push_back(i * fGroupIntervalTime);
			}

			for(i = 0; i < mapPrefixes.size(); i++)
			{
				int elem = rand() % vecGroupStarts.size();
				vecGroupRandomStarts.push_back(vecGroupStarts[elem]);
				vecGroupStarts.erase(vecGroupStarts.begin() + elem);
			}

			float fMinDist = FLOAT_MAX, fMaxDist = FLOAT_MIN;

			for(it = m_mapOffsets.begin(); it != m_mapOffsets.end(); ++it)
			{
				CVector3 v3Pos = it->first->GetWorldMassCenter();

				if(valueAxisDirection.GetValue() == "-X Axis")
				{
					vecDistances.push_back(-v3Pos.X());
				}
				else if(valueAxisDirection.GetValue() == "+X Axis")
				{
					vecDistances.push_back(+v3Pos.X());
				}
				else if(valueAxisDirection.GetValue() == "-Y Axis")
				{
					vecDistances.push_back(-v3Pos.Z());
				}
				else if(valueAxisDirection.GetValue() == "+Y Axis")
				{
					vecDistances.push_back(+v3Pos.Z());
				}
				else if(valueAxisDirection.GetValue() == "-Z Axis")
				{
					vecDistances.push_back(-v3Pos.Y());
				}
				else if(valueAxisDirection.GetValue() == "+Z Axis")
				{
					vecDistances.push_back(+v3Pos.Y());
				}

				if(vecDistances.back() < fMinDist) fMinDist = vecDistances.back();
				if(vecDistances.back() > fMaxDist) fMaxDist = vecDistances.back();
			}

			for(i = 0; i < vecDistances.size(); i++)
			{
				float fLength     = fMaxDist - fMinDist;
				float fNormalized = (vecDistances[i] - fMinDist) / fLength;
				vecDistances[i]   = fNormalized;
			}

			// Compute rest of parameters

			srand(valueSeed.GetValue());

			for(i = 0, it = m_mapOffsets.begin(); it != m_mapOffsets.end(); ++it, i++)
			{
				CObject* pObject = (CObject*)it->first;
				SOffset* pOffset = &it->second;

				pOffset->v3Start.Set(0.0f, 0.0f, 0.0f);

				if(valueCreationType.GetValue() == STR_CREATION_ALPHA)
				{
					pOffset->bHasQuat = false;
				}
				else if(valueCreationType.GetValue() == STR_CREATION_ALPHA_ORTOGONALOFFSET)
				{
					pOffset->bHasQuat = false;
					CVector3 v3Offset;

					float fOffset = rand() % 2 ? valueOffsetLength.GetValue() : -valueOffsetLength.GetValue();

					int axis = rand() % 3;
					if(axis == 0) v3Offset = pObject->GetAbsoluteTM().XAxis();
					if(axis == 1) v3Offset = pObject->GetAbsoluteTM().YAxis();
					if(axis == 2) v3Offset = pObject->GetAbsoluteTM().ZAxis();

					v3Offset.Normalize();
					v3Offset = v3Offset * fOffset;
					pOffset->v3Start = v3Offset;
				}
				else if(valueCreationType.GetValue() == STR_CREATION_ALPHA_ORTOGONALANGLE)
				{
					pOffset->bHasQuat = true;

					float fAngle = rand() % 2 ? 90.0f : -90.0f;
					fAngle = fAngle + ComputeRand(-valueRandomness.GetValue() * 90.0f, valueRandomness.GetValue() * 90.0f);

					CMatrix randomRot;

					int axis = rand() % 3;
					if(axis == 0) randomRot.Rotate(fAngle, 1.0f, 0.0f, 0.0f);
					if(axis == 1) randomRot.Rotate(fAngle, 0.0f, 1.0f, 0.0f);
					if(axis == 2) randomRot.Rotate(fAngle, 0.0f, 0.0f, 1.0f);

					pOffset->quat = randomRot.Quaternion();
				}
				else if(valueCreationType.GetValue() == STR_CREATION_ALPHA_RANDOMANGLE)
				{
					pOffset->bHasQuat = true;
					CMatrix randomRot;
					randomRot.Rotate(ComputeRand(0.0f, 360.0f), 1.0f, 0.0f, 0.0f);
					randomRot.Rotate(ComputeRand(0.0f, 360.0f), 0.0f, 1.0f, 0.0f);
					randomRot.Rotate(ComputeRand(0.0f, 360.0f), 0.0f, 0.0f, 1.0f);
					pOffset->quat = randomRot.Quaternion();
				}

				/////////

				if(valueSortType.GetValue() == STR_SORT_RANDOMPIECE)
				{
					pOffset->fStartTime = ComputeRand(0.0f, 1.0f - pOffset->fDuration);
				}
				else if(valueSortType.GetValue() == STR_SORT_RANDOMGROUP_RANDOMPIECE)
				{
					pOffset->fStartTime = vecGroupStarts[pOffset->nGroup] + (ComputeRand(0.0f, 1.0f) * fGroupDuration);
				}
				else if(valueSortType.GetValue() == STR_SORT_AXISDIR)
				{
					pOffset->fStartTime = vecDistances[i];
				}
				else if(valueSortType.GetValue() == STR_SORT_RANDOMGROUP_AXISDIR)
				{
					pOffset->fStartTime = vecGroupStarts[pOffset->nGroup] + (vecDistances[i] * fGroupDuration);
				}

				//

				if((pOffset->fStartTime + pOffset->fDuration) > 1.0f)
				{
					pOffset->fStartTime = 1.0f - pOffset->fDuration;
				}
			}
		}

		m_fAnimStart = pSceneView->GetAnimationStart();
		m_fAnimEnd   = pSceneView->GetAnimationEnd();
	}

	return true;
}

bool CFXSceneConstruction::Free()
{
	if(m_pResScene)
	{
		m_pResScene->RemoveDependency(this);
		m_pResScene = NULL;
	}

	m_mapOffsets.clear();
	m_strCamera.erase();
	return true;
}

bool CFXSceneConstruction::DoFrame(CDemo* pDemo, float fEffectTime, float fDemoTime)
{
	assert(pDemo);

	if(!m_pResScene)
	{
		FXRuntimeError("Scene resource not available");
		return false;
	}

	CSceneView* pSceneView = (CSceneView*)(((CResourceScene*)m_pResScene)->GetSceneView());

	if(!pSceneView)
	{
		FXRuntimeError("NULL pSceneView");
		return false;
	}

	CVarFloat::CValueFloat   valueStartTime;
	CVarSpeed::CValueSpeed   valueSpeed;
	CVarCombo::CValueCombo   valueRenderMode;
	CVarString::CValueString valueCamera;
	CVarFloat::CValueFloat   valueConstruction;
	CVarFloat::CValueFloat   valueLoopLength;
	CVarFloat::CValueFloat   valueLoopStart;

	EvaluateVar("Scene Start Time",     fEffectTime, &valueStartTime);
	EvaluateVar("Scene Speed",          fEffectTime, &valueSpeed);
	EvaluateVar("Scene Render Mode",    fEffectTime, &valueRenderMode);
	EvaluateVar("Scene Camera",         fEffectTime, &valueCamera);
	EvaluateVar("Scene Loop Length",    fEffectTime, &valueLoopLength);
	EvaluateVar("Scene Loop Start",     fEffectTime, &valueLoopStart);
	EvaluateVar("Construction (0-1)",   fEffectTime, &valueConstruction);

	float fConstructionValue = valueConstruction.GetValue();

	if(fConstructionValue < 0.0f) fConstructionValue = 0.0f;
	if(fConstructionValue > 1.0f) fConstructionValue = 1.0f;

	if(valueCamera.GetString() != m_strCamera)
	{
		// Camera string changed at runtime
		m_strCamera = valueCamera.GetString();
	}

	float fTime = valueStartTime.GetValue() + valueSpeed.Integrate(0.0f, fEffectTime);

	if(IS_ZERO(valueLoopLength.GetValue()))
	{
		ComputeLoop(valueLoopStart.GetValue(), m_fAnimEnd, &fTime);
	}
	else if(valueLoopLength.GetValue() > 0.0f)
	{
		ComputeLoop(valueLoopStart.GetValue(), valueLoopLength.GetValue(), &fTime);
	}

	// Transform

	CMatrix  mtxAddLocal, mtxAddWorld;
	CVector3 v3AddCamPos, v3AddCamAngles;
	EvaluateAddTransformVars(this, fEffectTime, &mtxAddLocal, &mtxAddWorld, &v3AddCamPos, &v3AddCamAngles);

	pSceneView->SetActiveCamera(m_strCamera);
	pSceneView->Transform(pDemo, fTime, mtxAddLocal, mtxAddWorld, v3AddCamPos, v3AddCamAngles);

	// Compute movement

	MAPOFFSETS::iterator it;

	for(it = m_mapOffsets.begin(); it != m_mapOffsets.end(); ++it)
	{
		SOffset* pOffset = &it->second;
		CObject* pObject = (CObject*)it->first;

		it->first->SetPreRenderCallback (PreRender,  pOffset);
		it->first->SetPostRenderCallback(PostRender, pOffset);

		// Set a translucency value before callback. Otherwise objects won't be sorted properly before render

		float fPieceTime = (fConstructionValue - pOffset->fStartTime) / pOffset->fDuration;
		pOffset->fTime   = fPieceTime;

		pObject->SetAlpha(0.5f);
	}

	// Render

	CRenderVisitor::EMode eRenderMode = CRenderVisitor::MODE_FULLDETAIL;

	     if(valueRenderMode.GetValue() == "Full")  eRenderMode = CRenderVisitor::MODE_FULLDETAIL;
	else if(valueRenderMode.GetValue() == "Solid") eRenderMode = CRenderVisitor::MODE_SOLID;
	else if(valueRenderMode.GetValue() == "Wire")  eRenderMode = CRenderVisitor::MODE_WIREFRAME;

	bool bResult = pSceneView->Render(pDemo, fTime, eRenderMode, false);

	for(it = m_mapOffsets.begin(); it != m_mapOffsets.end(); ++it)
	{
		it->first->SetPreRenderCallback (NULL, NULL);
		it->first->SetPostRenderCallback(NULL, NULL);
	}

	return bResult;
}

void CFXSceneConstruction::PreRender(CRenderable* pRenderable, void* pData)
{
	SOffset* pOffset = (SOffset*)pData;
	CObject* pObject = (CObject*)pRenderable;

	float fTime  = pOffset->fTime;

	if(fTime < 0.0f) fTime = 0.0f;
	if(fTime > 1.0f) fTime = 1.0f;

	CVector3 v3Pos   = pObject->GetAbsoluteTM().Position() + pOffset->v3Start.LERP(CVector3(0.0f, 0.0f, 0.0f), fTime);
	CMatrix  matrix  = pObject->GetAbsoluteTM();
	matrix.SetPosition(v3Pos);

	if(pOffset->bHasQuat)
	{
		CMatrix rot = (pOffset->quat.SLERP(CQuaternion(), fTime)).Matrix();
		matrix = matrix * rot;
	}

	pObject->SetAbsoluteTM(matrix);
	pObject->SetAlpha(ComputeLERP(pOffset->fAlpha, pOffset->fOriginalAlpha, fTime));
}

void CFXSceneConstruction::PostRender(CRenderable* pRenderable, void* pData)
{
	SOffset* pOffset = (SOffset*)pData;
	CObject* pObject = (CObject*)pRenderable;

	pObject->SetAlpha		(pOffset->fOriginalAlpha);
	pObject->SetAbsoluteTM	(pOffset->originalTM);
}



