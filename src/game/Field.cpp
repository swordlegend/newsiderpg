#include "Field.h"


int Field::m_nFieldSize_X = 0;

int Field::m_nWayMoveCharx = 0;
int Field::m_nWayMoveChary = 0;


//--------------------------------------------------------------------------
Field::Field(void* _sASpriteSet)
//--------------------------------------------------------------------------
{
	m_nWayMoveCharx = 0;

	s_ASpriteSet = (ASpriteSet*)_sASpriteSet;

	pLayerList = GL_NEW List2<BackLayer*>();
	pMiddleLayerList = GL_NEW List2<BackLayer*>();

	pEnvirSprList = GL_NEW List2<ASprite*>();

	pEvtWayList = GL_NEW List2<MAP_EVT_WAY*>();

	pKeyInputEvt = GL_NEW List2<VEvent*>();

	pDropItem = GL_NEW List2<ITEM_STRUCT*>();

	pDontgoRect = GL_NEW List2<DONT_GO_RECT*>();

	pEventRect = GL_NEW List2<EVENT_RECT*>();
	m_nLastZoneEvent = 0;

//	pMonsterGenRect = GL_NEW List2<MONSTER_REGEN_DATA*>();


	pWayAs = SUTIL_LoadSprite(PACK_SPRITE_UI, SPRITE_UI_BGOBJECT_UI);
//	m_pFieldUiDamageNumAsIns = GL_NEW ASpriteInstance(m_pFieldUiDamageNumAs, 0, 0, NULL);// 0번째 배열, 실사용시는 define 필요
//	m_pFieldUiDamageNumAs->SetBlendFrame(FRAME_DAMAGE_NUM_BLEND);


//	InitState(stageIdx);
}


//--------------------------------------------------------------------------
Field::~Field()
//--------------------------------------------------------------------------
{
	DeleteFieldImage();

//	for(int loop = 0; loop < SPRITE_MAP_MAX; loop++)
//	{
//		if(pFieldAs[loop])	{SUTIL_FreeSprite(pFieldAs[loop]);}
//	}

	InitList(pLayerList);
	while(NotEndList(pLayerList))
	{
		SAFE_DELETE(GetData(pLayerList));
		pLayerList->Delete();
	}
	SAFE_DELETE(pLayerList);

	InitList(pMiddleLayerList);
	while(NotEndList(pMiddleLayerList))
	{
		SAFE_DELETE(GetData(pMiddleLayerList));
		pMiddleLayerList->Delete();
	}
	SAFE_DELETE(pMiddleLayerList);

	SAFE_DELETE(pFrontLayer);

	//	리스트만 지워준다.
	InitList(pEnvirSprList);
	while(NotEndList(pEnvirSprList))
	{
		pEnvirSprList->Delete();
	}
	SAFE_DELETE(pEnvirSprList);

	Release_EVT_WAY();
	SAFE_DELETE(pEvtWayList);

	Release_EVT_INPUTKEY();
	SAFE_DELETE(pKeyInputEvt);

	InitList(pDropItem);
	while(NotEndList(pDropItem))
	{
		SUTIL_FreeSpriteInstance(GetData(pDropItem)->pInstance);
		SAFE_DELETE(GetData(pDropItem)->item);
		SAFE_DELETE(GetData(pDropItem));
		pDropItem->Delete();
	}
	SAFE_DELETE(pDropItem);

	SUTIL_FreeSprite(pWayAs);

	InitList(pDontgoRect);
	while(NotEndList(pDontgoRect))
	{
		SAFE_DELETE(GetData(pDontgoRect));
		pDontgoRect->Delete();
	}
	SAFE_DELETE(pDontgoRect);

	InitList(pEventRect);
	while(NotEndList(pEventRect))
	{
		SAFE_DELETE(GetData(pEventRect));
		pEventRect->Delete();
	}
	SAFE_DELETE(pEventRect);

//	InitList(pMonsterGenRect);
//	while(NotEndList(pMonsterGenRect))
//	{
//		SAFE_DELETE(GetData(pMonsterGenRect));
//		pMonsterGenRect->Delete();
//	}
//	SAFE_DELETE(pMonsterGenRect);
}


//--------------------------------------------------------------------------
void Field::Paint(bool isCinema)
//--------------------------------------------------------------------------
{
//	SUTIL_SetColor(0x000000);
//	SUTIL_FillRect(0,0,240,320);

	if(900 == m_nSaveStageNum)
	{
		SUTIL_SetColor(0x000000);
		SUTIL_FillRect(0,0,240,320);
	}
	else if(1000 <= m_nSaveStageNum)
	{
		PaintWorldMap();
	}
	else
	{
		InitList(pLayerList);
		while(NotEndList(pLayerList))
		{
			GetData(pLayerList)->Paint();
			MoveNext(pLayerList);
		}

		if(false == isCinema)
		{
			Paint_EVT_INPUTKEY();
			PaintDropItem();
			Paint_EVT_WAY();
			Paint_EVT_Rect();
		}
	}
}

//--------------------------------------------------------------------------
void Field::FrontPaint()
//--------------------------------------------------------------------------
{
	if(pFrontLayer)
	{
		pFrontLayer->Paint();
	}
}


//--------------------------------------------------------------------------
void Field::MiddlePaint()
//--------------------------------------------------------------------------
{
	if(900 > m_nSaveStageNum)
	{
		InitList(pMiddleLayerList);
		while(NotEndList(pMiddleLayerList))
		{
			GetData(pMiddleLayerList)->Paint();
			MoveNext(pMiddleLayerList);
		}
	}
}


//--------------------------------------------------------------------------
void Field::Process()
//--------------------------------------------------------------------------
{
	if(pFrontLayer)
	{
		pFrontLayer->SetAngle(m_nSrcCamAngle_X);
		//GetData(pLayerList)->Process();
		pFrontLayer->Process();
	}




	InitList(pLayerList);
	while(NotEndList(pLayerList))
	{
		GetData(pLayerList)->SetAngle(m_nSrcCamAngle_X);
		GetData(pLayerList)->Process();
		MoveNext(pLayerList);
	}

	InitList(pMiddleLayerList);
	while(NotEndList(pMiddleLayerList))
	{
		GetData(pMiddleLayerList)->SetAngle(m_nSrcCamAngle_X);
		GetData(pMiddleLayerList)->Process();
		MoveNext(pMiddleLayerList);
	}

	Process_EVT_INPUTKEY();

	ProcessDropItem();

	ProcessWorldMap();

//	 dbg(">>y Yellow >>a Aqua >>r Red >>g Green >>b Blue >>p  puple >>w  White \n" ); 
//	 dbg(">>>Y Yellow >>>a Aqua >>>r Red >>>g Green >>>b Blue >>>p  puple >>>w White \n"); 
//	 dbg(" >>a pause >>>b Test >>pause \n");

//	 dbg( "int %d char* %s char %c\n", 100, "가나다afadfas", 'A');
}


//--------------------------------------------------------------------------
void Field::GetMessage()
//--------------------------------------------------------------------------
{

}


////////////////////////////////////////////////////


//--------------------------------------------------------------------------
void Field::LoadMap(int nNextStageNum)
//--------------------------------------------------------------------------
{
	BackLayer* pBackLayer = NULL;

//	if(m_nSaveStageNum == nNextStageNum)	{return;}

	ReleaseMap(nNextStageNum);

	MoveHead(pLayerList);
	MoveHead(pMiddleLayerList);
	MoveHead(pEnvirSprList);

	//	필드정보
	m_nSrcCamAngle_X = 0;

	//	스프라이트 파일 로드

	if(nNextStageNum/100 != m_nSaveStageNum/100)
	{
		LoadFieldImage(nNextStageNum);
/*
		SUTIL_LoadAspritePack(PACK_SPRITE_MAP);
		switch(nNextStageNum/100)
		{
			case 1://	FOREST
			//-----------------------------------------------------------
			{
				LoadSprite(SPRITE_MAP_MAP_0);
				LoadSprite(SPRITE_MAP_MAP_1);
				LoadSprite(SPRITE_MAP_MAP_2);
				LoadSprite(SPRITE_MAP_MAP_3);
				break;
			}
			case 2://	CAVE
			//-----------------------------------------------------------
			{
				LoadSprite(SPRITE_MAP_MAP_4);
				LoadSprite(SPRITE_MAP_MAP_5);
				LoadSprite(SPRITE_MAP_MAP_6);
				break;
			}
			case 3://	TEMPLE
			//-----------------------------------------------------------
			{
				LoadSprite(SPRITE_MAP_MAP_16);
				LoadSprite(SPRITE_MAP_MAP_17);
				break;
			}
			case 4://	DEVILZONE
			//-----------------------------------------------------------
			{
				LoadSprite(SPRITE_MAP_MAP_22);
				LoadSprite(SPRITE_MAP_MAP_23);
				LoadSprite(SPRITE_MAP_MAP_24);
				LoadSprite(SPRITE_MAP_MAP_25);
				break;
			}
			case 100://	VILLAGE
			//-----------------------------------------------------------
			{
				LoadSprite(SPRITE_MAP_MAP_7);
				LoadSprite(SPRITE_MAP_MAP_8);
				LoadSprite(SPRITE_MAP_MAP_9);
				LoadSprite(SPRITE_MAP_MAP_10);
				LoadSprite(SPRITE_MAP_MAP_11);
				LoadSprite(SPRITE_MAP_MAP_12);
				LoadSprite(SPRITE_MAP_MAP_13);
				LoadSprite(SPRITE_MAP_MAP_14);
				LoadSprite(SPRITE_MAP_MAP_15);
				LoadSprite(SPRITE_MAP_MAP_16);
				break;
			}
		}
		SUTIL_ReleaseAspritePack();
*/
	}

	//	맵 정보를 갱신시킨다.
	m_nSaveStageNum = nNextStageNum;


	//	화면 출력 순서대로 등록시켜준다.
	switch(m_nSaveStageNum)
	{
		case 900://	검은 배경
		//----------------------------------------------------------------
		{
			break;
		}
		case 100:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKOUT1);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT2);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);

			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_100_EVT);
			break;
		}

		case 101:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKOUT2);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT3);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT1);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);

			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_101_EVT);
			break;
		}

		case 102:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKINT);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN2);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);

			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_102_EVT);
			break;
		}
		case 103:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKOUT2);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT3);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT1);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);

			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_103_EVT);
			break;
		}
		case 104:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKOUT2);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT3);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT2);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);

			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_104_EVT);
			break;
		}
		case 105:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKOUT2);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT3);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);

			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_105_EVT);
			break;
		}
		case 106:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKINT);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEINEXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN3);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONTEXIT);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);

			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_106_EVT);
			break;
		}
		case 107:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKINT);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN3);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN2);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONTEXIT);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);

			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_107_EVT);
			break;
		}
		case 108:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKINT);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEINEXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN2);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_108_EVT);
			break;
		}
		case 109:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKINT);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEINEXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN1);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_109_EVT);
			break;
		}
		case 110:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKOUT1);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT3);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT1);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONTEXIT);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);

			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_110_EVT);
			break;
		}
		case 111:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKINT);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN3);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONTEXIT);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_111_EVT);
			break;
		}
		case 112:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKINT);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEINEXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN1);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_112_EVT);
			break;
		}
		case 113:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKINT);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN3);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN1);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_113_EVT);
			break;
		}
		case 114:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKOUT1);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUTEXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEOUT3);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONTEXIT);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_114_EVT);
			break;
		}
		case 115:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKINT);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN3);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130	;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONTEXIT);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_115_EVT);
			break;
		}
		case 116:		//	FOREST
		//----------------------------------------------------------------
		{
				//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKINT);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN3);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_116_EVT);
			break;
		}
		case 117:		//	FOREST
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKINT);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEINEXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN3);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 130;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			Load_EVT(PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_MAP_117_EVT);
			break;
		}
		case 200:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON4EXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pLayerList->Insert_next(pBackLayer);

			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_200_EVT);
			break;
		}
		case 201:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON1);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONTEXIT);
			pLayerList->Insert_next(pBackLayer);


			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_201_EVT);
			break;
		}
		case 202:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON4EXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON3);
			pLayerList->Insert_next(pBackLayer);


			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_202_EVT);
			break;
		}
		case 203:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONTEXIT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_203_EVT);
			break;
		}
		case 204:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON3);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_204_EVT);
			break;
		}
		case 205:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON3);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON1);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_205_EVT);
			break;
		}
		case 206:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON4EXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON3);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_206_EVT);
			break;
		}
		case 207:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON3);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON1);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONTEXIT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_207_EVT);
			break;
		}
		case 208:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON4EXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON3);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON1);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONTEXIT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_208_EVT);
			break;
		}
		case 209:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON1);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_209_EVT);
			break;
		}
		case 210:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONTEXIT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_210_EVT);
			break;
		}
		case 211:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON3);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_211_EVT);
			break;
		}
		case 212:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON1);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_212_EVT);
			break;
		}
		case 213:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON4EXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON1);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONTEXIT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_213_EVT);
			break;
		}
		case 214:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON4EXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON1);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONTEXIT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_214_EVT);
			break;
		}
		case 215:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON4EXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_215_EVT);
			break;
		}
		case 216:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON4EXIT);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_216_EVT);
			break;
		}
		case 217:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON3);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONTEXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_217_EVT);
			break;
		}

		case 218:		//	CAVE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_BACK);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_STON3);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_FRONT);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_CAVE, DATA_MAP_CAVE_MAP_218_EVT);
			break;
		}
		case 300:		//	TEMPLE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_TEMPLE, DATA_MAP_TEMPLE_BACKOUT1);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_TEMPLE, DATA_MAP_TEMPLE_FIELDOUT);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 60;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_TEMPLE, DATA_MAP_TEMPLE_BACKOUTPILLAR8RED);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_TEMPLE, DATA_MAP_TEMPLE_OUTPILLAR2);
			pLayerList->Insert_next(pBackLayer);

			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 200;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_TEMPLE, DATA_MAP_TEMPLE_FRONT);

			Load_EVT(PACK_DATA_MAP_TEMPLE, DATA_MAP_TEMPLE_MAP_300_EVT);
			break;
		}
		//test
		case 301:		//	TEMPLE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_TEMPLE, DATA_MAP_TEMPLE_BACKOUT1);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_TEMPLE, DATA_MAP_TEMPLE_FIELDOUT);
			pLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 60;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_TEMPLE, DATA_MAP_TEMPLE_BACKOUTPILLAR8RED);
			pLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_TEMPLE, DATA_MAP_TEMPLE_OUTPILLAR2);
			pLayerList->Insert_next(pBackLayer);


			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 200;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_TEMPLE, DATA_MAP_TEMPLE_FRONT);

			Load_EVT(PACK_DATA_MAP_TEMPLE, DATA_MAP_TEMPLE_MAP_301_EVT);
			break;
		}
		case 400:		//	DEVILZONE
		//----------------------------------------------------------------
		{
			//	0번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_DEVILZONE, DATA_MAP_DEVILZONE_BACKSKYIN);
			pLayerList->Insert_next(pBackLayer);

			//	1번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_DEVILZONE, DATA_MAP_DEVILZONE_FIELD1);
			pMiddleLayerList->Insert_next(pBackLayer);

			//	2번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_DEVILZONE, DATA_MAP_DEVILZONE_TOP1);
			pMiddleLayerList->Insert_next(pBackLayer);

			//	3번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_DEVILZONE, DATA_MAP_DEVILZONE_FRONT1);
			pMiddleLayerList->Insert_next(pBackLayer);


			//	캐릭터 앞에 위치하는 레이어///////////////////////////////////////////////
//			pFrontLayer = GL_NEW BackLayer();
//			pFrontLayer->m_nBaseYLine = 0;
//			pFrontLayer->m_nMoveRate = 100;
//			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_DEVILZONE, DATA_MAP_DEVILZONE_FRONT1);

			Load_EVT(PACK_DATA_MAP_DEVILZONE, DATA_MAP_DEVILZONE_MAP_400_EVT);
			break;
		}
		case 800:
		//----------------------------------------------------------------
		{
			//	마을을 다녀왔던것으로 체크한다.
			m_nVillageUniqueIdx = 0;
			SAVELOAD_HaveBeenVillage[m_nVillageUniqueIdx] = true;
			
			// 3번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_SKY_NORMAL);
			pLayerList->Insert_next(pBackLayer);

			//	5번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_FIELD_NORMAL03);
			pLayerList->Insert_next(pBackLayer);

			//	4번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_TOWN1_1F);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_TOWN1_1F_EVT);
			break;
		}
		case 801:
		//----------------------------------------------------------------
		{
			//	마을을 다녀왔던것으로 체크한다.
			m_nVillageUniqueIdx = 0;
			SAVELOAD_HaveBeenVillage[m_nVillageUniqueIdx] = true;

			// 3번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_SKY_NORMAL);
			pLayerList->Insert_next(pBackLayer);

			//	5번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_FIELD_NORMAL03);
			pLayerList->Insert_next(pBackLayer);

			//	4번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_TOWN1_2F);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_TOWN1_2F_EVT);
			break;
		}
		case 802:
		//----------------------------------------------------------------
		{
			//	마을을 다녀왔던것으로 체크한다.
			m_nVillageUniqueIdx = 0;
			SAVELOAD_HaveBeenVillage[m_nVillageUniqueIdx] = true;

			// 3번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_SKY_NORMAL);
			pLayerList->Insert_next(pBackLayer);

			//	5번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_FIELD_NORMAL03);
			pLayerList->Insert_next(pBackLayer);

			//	4번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_TOWN1_3F);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_TOWN1_3F_EVT);
			break;
		}
		case 804:
		//----------------------------------------------------------------
		{
			//	마을을 다녀왔던것으로 체크한다.
			m_nVillageUniqueIdx = 1;
			SAVELOAD_HaveBeenVillage[m_nVillageUniqueIdx] = true;


			// 3번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 70;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_SKY_FOREST01);
			pLayerList->Insert_next(pBackLayer);

			//	5번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_FIELD_FOREST01);
			pLayerList->Insert_next(pBackLayer);

			//	4번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_TOWN2_1F);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_TOWN2_1F_EVT);
			break;
		}
		case 803:
		//----------------------------------------------------------------
		{
			//	마을을 다녀왔던것으로 체크한다.
			m_nVillageUniqueIdx = 1;
			SAVELOAD_HaveBeenVillage[m_nVillageUniqueIdx] = true;


			// 3번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 70;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_SKY_FOREST01);
			pLayerList->Insert_next(pBackLayer);

			//	5번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_FIELD_FOREST01);
			pLayerList->Insert_next(pBackLayer);

			//	4번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_TOWN2_2F);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_TOWN2_2F_EVT);
			break;
		}

		case 805:
		//----------------------------------------------------------------
		{
			//	마을을 다녀왔던것으로 체크한다.
			m_nVillageUniqueIdx = 1;
			SAVELOAD_HaveBeenVillage[m_nVillageUniqueIdx] = true;


			// 3번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 70;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_SKY_FOREST01);
			pLayerList->Insert_next(pBackLayer);

			//	5번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_FIELD_FOREST01);
			pLayerList->Insert_next(pBackLayer);

			//	4번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_TOWN2_3F);
			pLayerList->Insert_next(pBackLayer);

			Load_EVT(PACK_DATA_MAP_VILLAGE, DATA_MAP_VILLAGE_TOWN2_3F_EVT);
			break;
		}
	}

}


//--------------------------------------------------------------------------
void Field::ReleaseMap(int nNextStageNum)
//--------------------------------------------------------------------------
{
	//	같으면 이미지 재로딩을 하지 않는다.
	if(nNextStageNum/100 != m_nSaveStageNum/100)
	{
		DeleteFieldImage();

//		for(int loop = 0; loop < SPRITE_MAP_MAX; loop++)
//		{
//			if(pFieldAs[loop])	{SUTIL_FreeSprite(pFieldAs[loop]);}
//		}
	}

	InitList(pLayerList);
	while(NotEndList(pLayerList))
	{
		SAFE_DELETE(GetData(pLayerList));
		pLayerList->Delete();
	}
//	SAFE_DELETE(pLayerList);
	SAFE_DELETE(pFrontLayer);

	InitList(pMiddleLayerList);
	while(NotEndList(pMiddleLayerList))
	{
		SAFE_DELETE(GetData(pMiddleLayerList));
		pMiddleLayerList->Delete();
	}



	//	리스트만 지워준다.
	InitList(pEnvirSprList);
	while(NotEndList(pEnvirSprList))
	{
		pEnvirSprList->Delete();
	}
//	SAFE_DELETE(pEnvirSprList);
	//Release_EVT_WAY();

	InitList(pDropItem);
	while(NotEndList(pDropItem))
	{
		SUTIL_FreeSpriteInstance(GetData(pDropItem)->pInstance);
		SAFE_DELETE(GetData(pDropItem)->item)
		SAFE_DELETE(GetData(pDropItem));
		pDropItem->Delete();
	}

	InitList(pDontgoRect);
	while(NotEndList(pDontgoRect))
	{
		SAFE_DELETE(GetData(pDontgoRect));
		pDontgoRect->Delete();
	}

	InitList(pEventRect);
	while(NotEndList(pEventRect))
	{
		SAFE_DELETE(GetData(pEventRect));
		pEventRect->Delete();
	}

//	InitList(pMonsterGenRect);
//	while(NotEndList(pMonsterGenRect))
//	{
//		SAFE_DELETE(GetData(pMonsterGenRect));
//		pMonsterGenRect->Delete();
//	}



}


//--------------------------------------------------------------------------
void Field::LoadSprite(int nSpriteNum)
//--------------------------------------------------------------------------
{
	if(pFieldAs[nSpriteNum])	{return;}

	//	스프라이트 파일 로드
	//pFieldAs[nSpriteNum] = SUTIL_LoadSprite(PACK_SPRITE_MAP, nSpriteNum);
	pFieldAs[nSpriteNum] = SUTIL_LoadAspriteFromPack(PACK_SPRITE_MAP, nSpriteNum);
	

	//	블랜딩 등록
	int blendnum[SPRITE_MAP_MAX] = 
	{
		0, 0, FRAME_MAP_2_BLEND, 0,	FRAME_MAP_4_BLEND,
		FRAME_MAP_5_BLEND, 0, 0, FRAME_MAP_8_BLEND, FRAME_MAP_9_BLEND,
		0, 0, FRAME_MAP_12_BLEND, FRAME_MAP_13_BLEND, 0, 
		FRAME_MAP_15_BLEND, FRAME_MAP_16_BLEND, FRAME_MAP_17_BLEND, 0, FRAME_MAP_19_BLEND, 
		FRAME_MAP_20_BLEND, FRAME_MAP_21_BLEND, 0, 0, 0, 
		FRAME_MAP_25_BLEND, 0, 0, 0, 0, 
		0, 0, 0, 0, FRAME_MAP_34_BLEND, 
		FRAME_MAP_35_BLEND, 0,
	};

	if(0 != blendnum[nSpriteNum])
	{
		pFieldAs[nSpriteNum]->SetBlendFrame(blendnum[nSpriteNum]);
	}
		
	//	환경효과 등록
	MoveHead(pEnvirSprList);
	pEnvirSprList->Insert_next(pFieldAs[nSpriteNum]);	//	환경효과를 내는 스프라이트면 등록시켜준다.
}

/////////////////////////////////////////////////////


/*
//--------------------------------------------------------------------------
void Field::InitState(int nStateNum)
//--------------------------------------------------------------------------
{
	BackLayer* pBackLayer = NULL;
	MoveHead(pLayerList);

	MoveHead(pEnvirSprList);

	//	화면 출력 순서대로 등록시켜준다.
	switch(nStateNum)
	{
		case 1:
		{
			//	필드정보
			m_nFieldSize_X = 3000;
			m_nSrcCamAngle_X = 0;

			//	스프라이트 파일 로드
			pFieldAs[0] = SUTIL_LoadSprite(PACK_SPRITE_MAP, SPRITE_MAP_MAP_0);
			pFieldAs[1] = SUTIL_LoadSprite(PACK_SPRITE_MAP, SPRITE_MAP_MAP_1);
			pFieldAs[2] = SUTIL_LoadSprite(PACK_SPRITE_MAP, SPRITE_MAP_MAP_2);
			pFieldAs[3] = SUTIL_LoadSprite(PACK_SPRITE_MAP, SPRITE_MAP_MAP_3);

			pFieldAs[2]->SetBlendFrame(FRAME_MAP_2_BLEND);

			pEnvirSprList->Insert_next(pFieldAs[0]);			//	환경효과를 내는 스프라이트면 등록시켜준다.
			pEnvirSprList->Insert_next(pFieldAs[1]);			//	환경효과를 내는 스프라이트면 등록시켜준다.
			pEnvirSprList->Insert_next(pFieldAs[2]);			//	환경효과를 내는 스프라이트면 등록시켜준다.
			pEnvirSprList->Insert_next(pFieldAs[3]);			//	환경효과를 내는 스프라이트면 등록시켜준다.

			// 3번 레이어 로드 //////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 30;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_BACKINT);
			pLayerList->Insert_next(pBackLayer);

			//	5번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBackLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FIELD);
			pLayerList->Insert_next(pBackLayer);

			//	4번 레이어 로드 /////////////////////////////////////////////////////////
			pBackLayer = GL_NEW BackLayer();
			
			pBacmkLayer->m_nMoveRate = 100;
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN2);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN1);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN3);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEINEXIT);
			pBackLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_TREEIN3);
			pLayerList->Insert_next(pBackLayer);

			//	6번 레이어 로드 /////////////////////////////////////////////////////////
			//	캐릭터 앞에 위치하는 레이어
			pFrontLayer = GL_NEW BackLayer();
			pFrontLayer->m_nBaseYLine = 0;
			pFrontLayer->m_nMoveRate = 200;
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT1);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONTEXIT);
			pFrontLayer->LoadMapLayer(pFieldAs, PACK_DATA_MAP_FOREST, DATA_MAP_FOREST_FRONT2);
//			pLayerList->Insert_next(pBackLayer);
			break;



		}
	}
}
*/

//--------------------------------------------------------------------------
boolean Field::workPal(boolean Work, int Pal_Kind, int percent)
//--------------------------------------------------------------------------
{//SangHo - 필드는 다수의 팔렛트가 있을 가능성이 항상존재하므로 세부제어를 필드에 맏긴다

	//	환경효과
	boolean _Field_Work = Work;
	int _Field_Pal_Kind = Pal_Kind;
	int _Field_percent = percent;

	boolean _b_result = false;

	InitList(pEnvirSprList);
	while(NotEndList(pEnvirSprList))
	{
		if(Pal_Kind == 0)
		{
			if(!GetData(pEnvirSprList)->workPal(Work, Pal_Kind, percent) )
				_b_result = false;
		}
		MoveNext(pEnvirSprList);
	}
	return _b_result;
}



//--------------------------------------------------------------------------
void Field::SetCamera(int camx, int camy)
//--------------------------------------------------------------------------
{
//	m_nSrcCamAngle_X += ((((camx-120)-m_nSrcCamAngle_X) * 20) / 100);

	m_nSrcCamAngle_X = camx-120;

	if(0 > m_nSrcCamAngle_X)	{m_nSrcCamAngle_X = 0;}
	if(m_nFieldSize_X-SCREEN_WIDTH < m_nSrcCamAngle_X)	{m_nSrcCamAngle_X = m_nFieldSize_X-SCREEN_WIDTH;}
}


//--------------------------------------------------------------------------
void Field::Load_EVT(char* packName, int packIndex)
//--------------------------------------------------------------------------
{
	int tmpId = 0;
	int arrowpointx, arrowpointy, arrowtype = 0;

	//	쓰일 각종 리스트를 초기화시켜준다.
	Release_EVT_WAY();
	Release_EVT_INPUTKEY();
	//Npc::ReleaseAspriteNPC();
	MoveHead(pDontgoRect);
	MoveHead(pEventRect);
//	MoveHead(pMonsterGenRect);
	
	
	//	각종 데이타를 초기화시켜준다.
	MAP_EVT_WAY* pEvtWay = NULL;
	VEvent*		pEvtKeyInput = NULL;

	//	팩을 읽으면서 데이타를 분리시켜서 각각의 리스트에 넣어준다.
	SUTIL_Data_init(packName);		//<< 맵 이벤트 할 팩 로드
	SUTIL_Data_open(packIndex);

	while(1)
	{
		SUTIL_Data_readU8();											//	0x08		읽고 버린다.
		tmpId				= (short)SUTIL_Data_readU16();				//	Data0
		switch(tmpId)
		{
			case 1:
			//---------------------------------------------------------------
			{
				break;
			}
			case EVT_NEXT_MAP_POS:		//	맵에서 이동시 다음 맵을 정한다.
			//---------------------------------------------------------------
			{
				pEvtWay = GL_NEW MAP_EVT_WAY();
				pEvtWay->switchtime	 = 0;

				pEvtWay->sx			 = (short)SUTIL_Data_readU16();				//	x
				pEvtWay->sy			 = (short)SUTIL_Data_readU16();				//	y
				pEvtWay->dx			 = (short)SUTIL_Data_readU16();				//	인자1
				pEvtWay->dy			 = (short)SUTIL_Data_readU16();				//	인자2
				pEvtWay->type		 = (short)SUTIL_Data_readU16();				//	언제나:1 전투후:2
				pEvtWay->nextStage	 = (short)SUTIL_Data_readU16();				//	다음 움직일 스테이지
				pEvtWay->charx		 = (short)SUTIL_Data_readU16();				//	캐릭터 위치x
				pEvtWay->chary		 = (short)SUTIL_Data_readU16();				//	캐릭터 위치y

				pEvtWay->dx += pEvtWay->sx;										//	넓이를 좌표로 바꿔서 저장한다.
				pEvtWay->dy += pEvtWay->sy;

				//	화살표 셋팅
				arrowpointx = (pEvtWay->sx+pEvtWay->dx)/2;
				arrowpointy = (pEvtWay->sy+pEvtWay->dy)/2;

				//	에니메이션 셋팅
				arrowtype = ANIM_BG_OBJECT_ARROW_RIGHT;
				if(150 > arrowpointy)	{arrowtype = ANIM_BG_OBJECT_ARROW_UP;}
				if(250 < arrowpointy)	{arrowtype = ANIM_BG_OBJECT_ARROW_DOWN;}
				if(0 > arrowpointx)	{arrowtype = ANIM_BG_OBJECT_ARROW_LEFT;}

				switch(arrowtype)
				{
					case ANIM_BG_OBJECT_ARROW_DOWN:	{arrowpointy = pEvtWay->sy;		break;}
					case ANIM_BG_OBJECT_ARROW_UP:	{arrowpointy = pEvtWay->dy;		break;}
					case ANIM_BG_OBJECT_ARROW_LEFT:	{arrowpointx = pEvtWay->dx+20;	break;}
					case ANIM_BG_OBJECT_ARROW_RIGHT:{arrowpointx = pEvtWay->sx-20;	break;}
					default:	{break;}
				}

				pEvtWay->pArrawAsIns = GL_NEW ASpriteInstance(pWayAs, arrowpointx, arrowpointy, NULL);// 0번째 배열, 실사용시는 define 필요
				SUTIL_SetTypeAniAsprite(pEvtWay->pArrawAsIns, arrowtype);
				SUTIL_SetLoopAsprite(pEvtWay->pArrawAsIns, true);

				pEvtWayList->Insert_next(pEvtWay);
				continue;
			}
			case EVT_MAP_SIZE:			//	맵의 사이즈를 결정한다.
			//---------------------------------------------------------------
			{
				m_nFieldSize_X = (short)SUTIL_Data_readU16();	//	x는 맵의 크기이다.
				(short)SUTIL_Data_readU16();
				(short)SUTIL_Data_readU16();
				(short)SUTIL_Data_readU16();
				(short)SUTIL_Data_readU16();
				(short)SUTIL_Data_readU16();
				(short)SUTIL_Data_readU16();
				continue;
			}
			case EVT_DONT_GO_RECT:				//	EVT_DONT_GO_RECT
			//---------------------------------------------------------------
			{
				DONT_GO_RECT* pTmpRect = GL_NEW DONT_GO_RECT();

				pTmpRect->sx			 = (short)SUTIL_Data_readU16();				//	x
				pTmpRect->sy			 = (short)SUTIL_Data_readU16();				//	y
				pTmpRect->dx			 = (short)SUTIL_Data_readU16();				//	w
				pTmpRect->dy			 = (short)SUTIL_Data_readU16();				//	h
				SUTIL_Data_readU16();
				SUTIL_Data_readU16();
				SUTIL_Data_readU16();
				SUTIL_Data_readU16();

				pDontgoRect->Insert_next(pTmpRect);
				continue;
			}
			case EVT_NPC:				//	NPC
			//---------------------------------------------------------------
			{
				int x				 = (short)SUTIL_Data_readU16();				//	x
				int y				 = (short)SUTIL_Data_readU16();				//	y
				int imgidx			 = (short)SUTIL_Data_readU16();				//	인자1
				Npc* pTmpStoreNpc = GL_NEW Npc(imgidx, x, y);
				pTmpStoreNpc->m_nType = tmpId;
				pTmpStoreNpc->m_nSaveEvtCode = (short)SUTIL_Data_readU16();		//	인자2
				SUTIL_Data_readU16();				//	더미
				SUTIL_Data_readU16();				//	
				SUTIL_Data_readU16();				//	

				pKeyInputEvt->Insert_next(pTmpStoreNpc);
				continue;
			}
			case EVT_EVENT_RECT:				//	EVT_EVENT_RECT
			//---------------------------------------------------------------
			{
				EVENT_RECT* pTmpRect = GL_NEW EVENT_RECT();

				pTmpRect->sx			 = (short)SUTIL_Data_readU16();				//	x
				pTmpRect->sy			 = (short)SUTIL_Data_readU16();				//	y
				pTmpRect->dx			 = (short)SUTIL_Data_readU16();				//	w
				pTmpRect->dy			 = (short)SUTIL_Data_readU16();				//	h
				pTmpRect->eCode			 = (short)SUTIL_Data_readU16();
				pTmpRect->dummy[0]		 = (short)SUTIL_Data_readU16();
				pTmpRect->dummy[1]		 = (short)SUTIL_Data_readU16();
				pTmpRect->dummy[2]		 = (short)SUTIL_Data_readU16();

				pTmpRect->dx += pTmpRect->sx;
				pTmpRect->dy += pTmpRect->sy;

				pEventRect->Insert_next(pTmpRect);
				continue;
			}
//			case EVT_MON_GEN_RECT:				//	EVT_MON_GEN_RECT
//			//---------------------------------------------------------------
//			{
//				MONSTER_REGEN_DATA* pTmpData = GL_NEW MONSTER_REGEN_DATA();
//
//				pTmpData->x			 = (short)SUTIL_Data_readU16();				//	x
//				pTmpData->y			 = (short)SUTIL_Data_readU16();				//	y
//				pTmpData->nType		 = (short)SUTIL_Data_readU16();				//	w
//				(short)SUTIL_Data_readU16();				//	h
//				(short)SUTIL_Data_readU16();
//				(short)SUTIL_Data_readU16();
//				(short)SUTIL_Data_readU16();
//				pMonsterGenRect->Insert_next(pTmpData);
//				continue;
//			}

			//case EVT_BARREL:
			////---------------------------------------------------------------
			//{
			//	int x				 = (short)SUTIL_Data_readU16();				//	x
			//	int y				 = (short)SUTIL_Data_readU16();				//	y
			//	int dummy			 = (short)SUTIL_Data_readU16();				//	인자1
			//	CrushObj* pTmpObject = GL_NEW CrushObj(dummy, x, y);
			//	//pTmpObject->m_nType = tmpId;
			//	pTmpObject->m_nType = EVT_BARREL;
			//	pTmpObject->m_nSaveEvtCode = (short)SUTIL_Data_readU16();		//	인자2
			//	SUTIL_Data_readU16();				//	더미
			//	SUTIL_Data_readU16();				//	
			//	SUTIL_Data_readU16();				//	
			//
			//	pKeyInputEvt->Insert_next(pTmpObject);
			//	continue;
			//	break;
//			}
		}
		break;
	}

	SUTIL_Data_free();

	//	로드를 한후 이미지를 로드한다. (파일 인풋때문에)

	for(InitList(pKeyInputEvt);NotEndList(pKeyInputEvt);MoveNext(pKeyInputEvt))
	{
		switch(GetData(pKeyInputEvt)->m_nType )
		{
			case EVT_NPC:
			//--------------------------------------------------------------
			{
				((Npc*)GetData(pKeyInputEvt))->ReadyObject(s_ASpriteSet);
				break;
			}
//			case EVT_BARREL:
//			//--------------------------------------------------------------
//			{
//				((CrushObj*)GetData(pKeyInputEvt))->ReadyObject(s_ASpriteSet);
//				break;
//			}

			
		}
	}

}


//--------------------------------------------------------------------------
void Field::Release_EVT_WAY()
//--------------------------------------------------------------------------
{
	InitList(pEvtWayList);
	while(NotEndList(pEvtWayList))
	{
		SUTIL_FreeSpriteInstance(GetData(pEvtWayList)->pArrawAsIns);
		SAFE_DELETE(GetData(pEvtWayList));
		pEvtWayList->Delete();
	}
	MoveHead(pEvtWayList);
}

//--------------------------------------------------------------------------
int Field::Process_EVT_WAY(int battleState, int herodir, int herox1, int heroy1)
//--------------------------------------------------------------------------
{
	//	캐릭터의 위치
	m_nCharx = herox1;
	m_nChary = heroy1;

	switch(herodir)
	{
		case MAP_EVT_WAY_DIR_UP:	{heroy1-=40;	break;}
		case MAP_EVT_WAY_DIR_DOWN:	{heroy1+=40;	break;}
		case MAP_EVT_WAY_DIR_LEFT:	{herox1-=40;	break;}
		case MAP_EVT_WAY_DIR_RIGHT:	{herox1+=40;	break;}
	}

	//	화살표 에니메이션 업데이트
	for(InitList(pEvtWayList);NotEndList(pEvtWayList);MoveNext(pEvtWayList))
	{
		SUTIL_UpdateTimeAsprite(GetData(pEvtWayList)->pArrawAsIns);
	}

	for(InitList(pEvtWayList);NotEndList(pEvtWayList);MoveNext(pEvtWayList))
	{
		if(herox1 > GetData(pEvtWayList)->sx &&
			herox1 < GetData(pEvtWayList)->dx &&
			heroy1 > GetData(pEvtWayList)->sy &&
			heroy1 < GetData(pEvtWayList)->dy &&
			battleState >= GetData(pEvtWayList)->type )
		{
			GetData(pEvtWayList)->switchtime++;

			if(5 < GetData(pEvtWayList)->switchtime)
			{
//				if(100 > GetData(pEvtWayList)->charx)	//	방향은 오른쪽을 보고 있는다.
//				{
//					ADD_EVENT_MSG(EVT_SET_CAHRPOS, 0, GetData(pEvtWayList)->charx, GetData(pEvtWayList)->chary, SDIR_RIGHT);
//				}
//				else if(100 > (m_nFieldSize_X - GetData(pEvtWayList)->charx))	//	왼쪽
//				{
//					ADD_EVENT_MSG(EVT_SET_CAHRPOS, 0, GetData(pEvtWayList)->charx, GetData(pEvtWayList)->chary, SDIR_LEFT);
//				}

				m_nWayMoveCharx = GetData(pEvtWayList)->charx;
				m_nWayMoveChary = GetData(pEvtWayList)->chary;

				//	이벤트를 던지고 최소 시간을 딜레이준다. 안그럼 계속 이벤트를 던짐
				GetData(pEvtWayList)->switchtime = -20;

				return GetData(pEvtWayList)->nextStage;
			}
		}
		else
		{
			GetData(pEvtWayList)->switchtime = 0;
		}
	}

	return 0;
}

//--------------------------------------------------------------------------
void Field::Paint_EVT_WAY()
//--------------------------------------------------------------------------
{
	for(InitList(pEvtWayList);NotEndList(pEvtWayList);MoveNext(pEvtWayList))
	{
		GetData(pEvtWayList)->pArrawAsIns->CameraX = -m_nSrcCamAngle_X;
		SUTIL_PaintAsprite(GetData(pEvtWayList)->pArrawAsIns, S_NOT_INCLUDE_SORT);
	}
}

//--------------------------------------------------------------------------
void Field::Release_EVT_INPUTKEY()
//--------------------------------------------------------------------------
{
	InitList(pKeyInputEvt);
	while(NotEndList(pKeyInputEvt))
	{
		SAFE_DELETE(GetData(pKeyInputEvt));
		pKeyInputEvt->Delete();
	}
	MoveHead(pKeyInputEvt);
}


//--------------------------------------------------------------------------
int Field::Process_EVT_INPUTKEY()
//--------------------------------------------------------------------------
{
	for(InitList(pKeyInputEvt);NotEndList(pKeyInputEvt);MoveNext(pKeyInputEvt))
	{
		switch( GetData(pKeyInputEvt)->m_nType )
		{
			case EVT_NPC:
//			case EVT_BARREL:
			//-------------------------------------------------------
			{
				GetData(pKeyInputEvt)->SetAngle(m_nSrcCamAngle_X);
				GetData(pKeyInputEvt)->Process();
				break;
			}

		}
	}

	return 0;
}

//--------------------------------------------------------------------------
void Field::Paint_EVT_INPUTKEY()
//--------------------------------------------------------------------------
{
	for(InitList(pKeyInputEvt);NotEndList(pKeyInputEvt);MoveNext(pKeyInputEvt))
	{
		switch( GetData(pKeyInputEvt)->m_nType )
		{
			case EVT_NPC:
//			case EVT_BARREL:
			//-------------------------------------------------------
			{
				GetData(pKeyInputEvt)->Paint();
				break;
			}
		}
	}
}


//--------------------------------------------------------------------------
int Field::KeyPressEvent(int dir, int x1, int y1, int x2, int y2)
//--------------------------------------------------------------------------
{
	int evtcode = 0;

	for(InitList(pKeyInputEvt);NotEndList(pKeyInputEvt);MoveNext(pKeyInputEvt))
	{
		switch( GetData(pKeyInputEvt)->m_nType )
		{
			case EVT_NPC:
			//-------------------------------------------------------
			{
				evtcode = GetData(pKeyInputEvt)->CheckEvent(dir, x1,y1,x2,y2);
				if(evtcode)	{return evtcode;}
				break;
			}
		}
	}

	return 0;
}

//--------------------------------------------------------------------------
void Field::ProcessDropItem()
//--------------------------------------------------------------------------
{
	Position3D m_nPhysicsPos;

	for(InitList(pDropItem);NotEndList(pDropItem);MoveNext(pDropItem))
	{
		if(true == GetData(pDropItem)->bPossibleEat)	{continue;}

		//	물리좌표를 갱신받는다.
		m_nPhysicsPos = GetData(pDropItem)->m_Physics->process();

//		if(0 == m_nPhysicsPos.Sqr_GetLength() && 0 == GetData(pDropItem)->pInstance->m_posZ)
//		{
//			//	이동량이 없으므로 모든 값을 초기화시켜준다.
//			GetData(pDropItem)->m_Physics->initForce();
//			continue;
//		}

		GetData(pDropItem)->pInstance->m_posX += m_nPhysicsPos.x;
		GetData(pDropItem)->pInstance->m_posY += m_nPhysicsPos.y;
		GetData(pDropItem)->pInstance->m_posZ += m_nPhysicsPos.z;

		if(0 <= GetData(pDropItem)->pInstance->m_posZ)
		{
			GetData(pDropItem)->pInstance->m_posZ = 0;
			GetData(pDropItem)->m_Physics->SaveGrAccel.z = UP_GRV;
			GetData(pDropItem)->m_Physics->setRebound();

//			if(-1 < GetData(pDropItem)->m_Physics->SaveAccel.z)
//			{
//				GetData(pDropItem)->m_Physics->initForce();
//				GetData(pDropItem)->bPossibleEat = true;
//			}
		}

		if(0 == GetData(pDropItem)->m_Physics->SaveAccel.Sqr_GetLength())
		{
			GetData(pDropItem)->pInstance->m_posZ = 0;
			GetData(pDropItem)->m_Physics->initForce();
			GetData(pDropItem)->bPossibleEat = true;
		}
	}
}

//--------------------------------------------------------------------------
void Field::InsertDropItem(ItemBag _item, int x, int y)
//--------------------------------------------------------------------------
{
	MoveHead(pDropItem);
	ITEM_STRUCT* pItemStruct = GL_NEW ITEM_STRUCT();

	pItemStruct->m_Physics = GL_NEW Physics(LIGHT_WEIGHT);
	pItemStruct->m_Physics->initForce();

	Position3D Force;
	Force.Init(0,0,UP_GRV);
	pItemStruct->m_Physics->setGrForce(Force);
	Force.Init(SUTIL_GetRandom()%30-15, SUTIL_GetRandom()%10-5,-25-(SUTIL_GetRandom()%15));

	//	아이템이 필드를 나가는 부분 삭제
	if(220 > y && 0 > Force.y)						{Force.y *= (-1);}
	else if(240 < y && 0 < Force.y)					{Force.y *= (-1);}

	if(100 > x && 0 > Force.x)						{Force.x *= (-1);}
	else if(m_nFieldSize_X-100 < x && 0 < Force.x)	{Force.x *= (-1);}

	pItemStruct->m_Physics->setForce(Force);

	pItemStruct->bPossibleEat = false;
	pItemStruct->item = GL_NEW ItemBag();
	pItemStruct->item->Data0 = _item.Data0;
	pItemStruct->item->Data1 = _item.Data1;
	pItemStruct->item->Data2 = _item.Data2;
	pItemStruct->pInstance = GL_NEW ASpriteInstance(s_ASpriteSet->pItemAs, x, y, NULL);
	SUTIL_SetTypeModuleAsprite(pItemStruct->pInstance, PopupUi::itemICON(_item));
	pDropItem->Insert_next(pItemStruct);
}

//--------------------------------------------------------------------------
void Field::PaintDropItem()
//--------------------------------------------------------------------------
{
	for(InitList(pDropItem);NotEndList(pDropItem);MoveNext(pDropItem))
	{
		SUTIL_Paint_Frame(s_ASpriteSet->pShadowAs ,3 ,
			GetData(pDropItem)->pInstance->m_posX -m_nSrcCamAngle_X+1,
			GetData(pDropItem)->pInstance->m_posY-3, 0);

		GetData(pDropItem)->pInstance->CameraX = -m_nSrcCamAngle_X;
		SUTIL_PaintAsprite(GetData(pDropItem)->pInstance,S_NOT_INCLUDE_SORT);
	}
}


//--------------------------------------------------------------------------
ItemBag* Field::CheckDropItem(int x1, int y1)
//--------------------------------------------------------------------------
{
	ItemBag tmpItembag;
	tmpItembag.Data0 = 0;

	int x,y = 0;
	for(InitList(pDropItem);NotEndList(pDropItem);MoveNext(pDropItem))
	{
		x = GetData(pDropItem)->pInstance->m_posX - x1;
		y = GetData(pDropItem)->pInstance->m_posY - y1;
		if(SQR(20) < SQR(x) +SQR(y))	{continue;}

//		if(x1 > GetData(pDropItem)->pInstance->m_posX- GetData(pDropItem)->pInstance->m_stopAniX)	{continue;}
//		if(x2 < GetData(pDropItem)->pInstance->m_posX- GetData(pDropItem)->pInstance->m_stopAniX)	{continue;}
//		if(y1 > GetData(pDropItem)->pInstance->m_posX- GetData(pDropItem)->pInstance->m_stopAniX)	{continue;}
//		if(y2 < GetData(pDropItem)->pInstance->m_posX- GetData(pDropItem)->pInstance->m_stopAniX)	{continue;}

		//tmpItembag.Data0 = GetData(pDropItem)->item->Data0;
		//tmpItembag.Data1 = GetData(pDropItem)->item->Data1;
		//tmpItembag.Data2 = GetData(pDropItem)->item->Data2;

//		SUTIL_FreeSpriteInstance(GetData(pDropItem)->pInstance);
//		SAFE_DELETE(GetData(pDropItem)->item)
//		SAFE_DELETE(GetData(pDropItem));
//		pDropItem->Delete();

		return (GetData(pDropItem)->item);
	}

	return NULL;
}

//--------------------------------------------------------------------------
void Field::DeleteNowItem()
//--------------------------------------------------------------------------
{
	//	현재 리스트의 아이템을 지운다 조금 위험한 코드
	SAFE_DELETE(GetData(pDropItem)->m_Physics)
	SUTIL_FreeSpriteInstance(GetData(pDropItem)->pInstance);
	SAFE_DELETE(GetData(pDropItem)->item)
	SAFE_DELETE(GetData(pDropItem));
	pDropItem->Delete();
}


//--------------------------------------------------------------------------
void Field::DeleteFieldImage()
//--------------------------------------------------------------------------
{
	for(int loop = 0; loop < SPRITE_MAP_MAX; loop++)
	{
		if(pFieldAs[loop])	{SUTIL_FreeSprite(pFieldAs[loop]);}
	}		
}


//--------------------------------------------------------------------------
void Field::ReMatchFieldImage()
//--------------------------------------------------------------------------
{
	for(InitList(pLayerList);NotEndList(pLayerList);MoveNext(pLayerList))
	{
		GetData(pLayerList)->RematchImage(pFieldAs);
	}

	for(InitList(pMiddleLayerList);NotEndList(pMiddleLayerList);MoveNext(pMiddleLayerList))
	{
		GetData(pMiddleLayerList)->RematchImage(pFieldAs);
	}

	if(pFrontLayer)
	{
		pFrontLayer->RematchImage(pFieldAs);
	}
	

	//	환경효과를 다시 등록시킨다.
	InitList(pEnvirSprList);
	while(NotEndList(pEnvirSprList))
	{
		pEnvirSprList->Delete();
	}

	for(int loop = 0; loop < SPRITE_MAP_MAX; loop++)
	{
		if(pFieldAs[loop])
		{
			MoveHead(pEnvirSprList);
			pEnvirSprList->Insert_next(pFieldAs[loop]);	//	환경효과를 내는 스프라이트면 등록시켜준다.
		}
	}
}

//--------------------------------------------------------------------------
void Field::LoadFieldImage(int idx)
//--------------------------------------------------------------------------
{
//	if(nNextStageNum/100 != m_nSaveStageNum/100)
//	{
		SUTIL_LoadAspritePack(PACK_SPRITE_MAP);
		switch(idx/100)
		{
			case 1://	FOREST
			//-----------------------------------------------------------
			{
				LoadSprite(SPRITE_MAP_MAP_0);
				LoadSprite(SPRITE_MAP_MAP_1);
				LoadSprite(SPRITE_MAP_MAP_2);
				LoadSprite(SPRITE_MAP_MAP_3);
				break;
			}
			case 2://	CAVE
			//-----------------------------------------------------------
			{
				LoadSprite(SPRITE_MAP_MAP_4);
				LoadSprite(SPRITE_MAP_MAP_5);
				LoadSprite(SPRITE_MAP_MAP_6);
				break;
			}
			case 3://	TEMPLE
			//-----------------------------------------------------------
			{
				LoadSprite(SPRITE_MAP_MAP_16);
				LoadSprite(SPRITE_MAP_MAP_17);
				break;
			}
			case 4://	DEVILZONE
			//-----------------------------------------------------------
			{
				LoadSprite(SPRITE_MAP_MAP_22);
				LoadSprite(SPRITE_MAP_MAP_23);
				LoadSprite(SPRITE_MAP_MAP_24);
				LoadSprite(SPRITE_MAP_MAP_25);
				break;
			}
			case 8://	VILLAGE
			//-----------------------------------------------------------
			{
				LoadSprite(SPRITE_MAP_MAP_7);
				LoadSprite(SPRITE_MAP_MAP_8);
				LoadSprite(SPRITE_MAP_MAP_9);
				LoadSprite(SPRITE_MAP_MAP_10);
				LoadSprite(SPRITE_MAP_MAP_11);
				LoadSprite(SPRITE_MAP_MAP_12);
				LoadSprite(SPRITE_MAP_MAP_13);
				LoadSprite(SPRITE_MAP_MAP_14);
				LoadSprite(SPRITE_MAP_MAP_15);
				LoadSprite(SPRITE_MAP_MAP_16);
				LoadSprite(SPRITE_MAP_MAP_26);
				LoadSprite(SPRITE_MAP_MAP_27);
				LoadSprite(SPRITE_MAP_MAP_28);
				LoadSprite(SPRITE_MAP_MAP_36);

				break;
			}
			case 9://	검은 배경
			//-----------------------------------------------------------
			{
				break;
			}
			
		}
		SUTIL_ReleaseAspritePack();
//	}
}

//--------------------------------------------------------------------------
void Field::Paint_EVT_Rect()
//--------------------------------------------------------------------------
{
	for(InitList(pEventRect);NotEndList(pEventRect);MoveNext(pEventRect))
	{
		if(MAIN_QUEST_findOpen == GET_MAINQUEST_IDX() && FIELD_CALL_caveOpen == GetData(pEventRect)->eCode)
		{
			SUTIL_Paint_Frame(pWayAs, FRAME_BG_OBJECT_EVENT_ARROW ,
				((GetData(pEventRect)->dx+GetData(pEventRect)->sx)>>1) - m_nSrcCamAngle_X,
				((GetData(pEventRect)->dy+GetData(pEventRect)->sy)>>1) + m_nEventAddYPos, 0);
		}
	}
}


//--------------------------------------------------------------------------
int Field::Process_EVT_Rect(int herox1, int heroy1)
//--------------------------------------------------------------------------
{
	m_nEventArrowTimer = (m_nEventArrowTimer+1)%4;
	m_nEventAddYPos = 0;
	if(1 == m_nEventArrowTimer)			{m_nEventAddYPos = 2;}
	else if(2 == m_nEventArrowTimer)	{m_nEventAddYPos = 1;}

	for(InitList(pEventRect);NotEndList(pEventRect);MoveNext(pEventRect))
	{
		if(herox1 > GetData(pEventRect)->sx &&
			herox1 < GetData(pEventRect)->dx &&
			heroy1 > GetData(pEventRect)->sy &&
			heroy1 < GetData(pEventRect)->dy	 )
		{
			if(m_nLastZoneEvent == GetData(pEventRect)->eCode)
			{
				return 0;
			}
			else
			{
				m_nLastZoneEvent = GetData(pEventRect)->eCode;
				return m_nLastZoneEvent;
			}
		}
	}

	m_nLastZoneEvent = 0;
	return m_nLastZoneEvent;
}



//--------------------------------------------------------------------------
void Field::ProcessWorldMap()
//--------------------------------------------------------------------------
{
//	pCinema->Update_Cinematics();
}


//--------------------------------------------------------------------------
void Field::PaintWorldMap()
//--------------------------------------------------------------------------
{
	return;

	SUTIL_SetColor(0x000000);
	SUTIL_FillRect(0,0,240,320);

//	pCinema->Paint_Cinematics();

	switch(m_nSaveStageNum)
	{
		case 1000:
		//------------------------------------------------------------
		{
			_SUTIL->pFont->setColor(0xFFFFFF);
			_SUTIL->pFont->DrawText(_SUTIL->g, (char*)"1 : GO TO SMALL TOWN", 50, 100, 0);//Text
			_SUTIL->pFont->DrawText(_SUTIL->g, (char*)"2 : GO TO FOREST1", 50, 120, 0);//Text
			_SUTIL->pFont->DrawText(_SUTIL->g, (char*)"3 : GO TO CRYSTAL CAVE", 50, 141, 0);//Text
			_SUTIL->pFont->DrawText(_SUTIL->g, (char*)"4 : GO TO MIDDLE TOWN ROAD", 50, 160, 0);//Text
			break;
		}

		case 1001:
		//------------------------------------------------------------
		{
			_SUTIL->pFont->setColor(0xFFFFFF);
			_SUTIL->pFont->DrawText(_SUTIL->g, (char*)"1 : GO TO MIDDLE TOWN", 50, 100, 0);//Text
			_SUTIL->pFont->DrawText(_SUTIL->g, (char*)"2 : GO TO WEST FOREST", 50, 120, 0);//Text
			_SUTIL->pFont->DrawText(_SUTIL->g, (char*)"3 : GO TO EAST FOREST", 50, 140, 0);//Text
			_SUTIL->pFont->DrawText(_SUTIL->g, (char*)"4 : GO TO MIST CAVE", 50, 160, 0);//Text
			_SUTIL->pFont->DrawText(_SUTIL->g, (char*)"5 : GO TO SMALL TOWN ROAD", 50, 180, 0);//Text
			break;
		}

	}
//	SUTIL_PaintAsprite(m_pWorldMapAsIns, S_NOT_INCLUDE_SORT);

}

//--------------------------------------------------------------------------
bool Field::KeyEvent(int m_keyCode, int m_keyRepeat)
//--------------------------------------------------------------------------
{
/*
	switch(m_nSaveStageNum)
	{
		case 1000:
		//------------------------------------------------------------
		{
			switch(m_keyCode)
			{
				case MH_KEY_1:
				{
					ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT, 0, 10);
					ADD_EVENT_MSG(EVT_CHANGE_STAGE, 10, 801, 0);
					m_nWayMoveCharx = 50;
					m_nWayMoveChary = 240;
					return true;
				}
				case MH_KEY_2://	{AppMain::ResvNextState(MS_GAME_FIELD, 101, 1950);		break;}
				{
					ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT, 0, 10);
					ADD_EVENT_MSG(EVT_CHANGE_STAGE, 10, 101, 0);
					m_nWayMoveCharx = 1950;
					m_nWayMoveChary = 240;
					return true;
				}
				case MH_KEY_3://	{AppMain::ResvNextState(MS_GAME_FIELD, 200, 0);	break;}
				{
					ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT, 0, 10);
					ADD_EVENT_MSG(EVT_CHANGE_STAGE, 10, 200, 0);
					m_nWayMoveCharx = 50;
					m_nWayMoveChary = 240;
					return true;
				}

				case MH_KEY_4://	{AppMain::ResvNextState(MS_GAME_FIELD, 104, 0);		break;}
				{
					ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT, 0, 10);
					ADD_EVENT_MSG(EVT_CHANGE_STAGE, 10, 104, 0);
					m_nWayMoveCharx = 50;
					m_nWayMoveChary = 240;
					return true;
				}

			}

			break;
		}
		case 1001:
		//------------------------------------------------------------
		{
			switch(m_keyCode)
			{
				case MH_KEY_1://	{AppMain::ResvNextState(MS_GAME_FIELD, 803, 0);		break;}
				{
					ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT, 0, 10);
					ADD_EVENT_MSG(EVT_CHANGE_STAGE, 10, 803, 0);
					m_nWayMoveCharx = 50;
					m_nWayMoveChary = 240;
					return true;
				}

				case MH_KEY_2://	{AppMain::ResvNextState(MS_GAME_FIELD, 105, 450);		break;}
				{
					ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT, 0, 10);
					ADD_EVENT_MSG(EVT_CHANGE_STAGE, 10, 105, 0);
					m_nWayMoveCharx = 450;
					m_nWayMoveChary = 240;
					return true;
				}

				case MH_KEY_3://	{AppMain::ResvNextState(MS_GAME_FIELD, 113, 50);		break;}
				{
					ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT, 0, 10);
					ADD_EVENT_MSG(EVT_CHANGE_STAGE, 10, 113, 0);
					m_nWayMoveCharx = 50;
					m_nWayMoveChary = 240;
					return true;
				}

				case MH_KEY_4://	{AppMain::ResvNextState(MS_GAME_FIELD, 205, 50);		break;}
				{
					ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT, 0, 10);
					ADD_EVENT_MSG(EVT_CHANGE_STAGE, 10, 205, 0);
					m_nWayMoveCharx = 50;
					m_nWayMoveChary = 240;

					return true;
				}

				case MH_KEY_5://	{AppMain::ResvNextState(MS_GAME_FIELD, 104, 1450);		break;}
				{
					ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT, 0, 10);
					ADD_EVENT_MSG(EVT_CHANGE_STAGE, 10, 104, 0);
					m_nWayMoveCharx = 1450;
					m_nWayMoveChary = 240;
					return true;
				}

			}

			break;
		}
	}
*/
	return false;
}