#include "GFieldBattle.h"




//--------------------------------------------------------------------------
GFieldBattle::GFieldBattle()
//--------------------------------------------------------------------------
{
	isHoldingProcess = true;
	isHoldingPaint = true;
	isHoldingKey = true;
}


//--------------------------------------------------------------------------
GFieldBattle::~GFieldBattle()
//--------------------------------------------------------------------------
{
	//	이벤트 메세지를 죽인다.
	Message_FreeInstance();
	EventManager_FreeInstance();
	Quest_FreeInstance();
}


//--------------------------------------------------------------------------
void GFieldBattle::Initialize(int nDummy1, int nDummy2)		//	nDummy1:필드 번호,  nDummy2:안쓴다.
//--------------------------------------------------------------------------
{
	ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT,0,10);
	ADD_EVENT_MSG(EVT_INIT_GAME, 10, 0, nDummy1, nDummy2);

}

//--------------------------------------------------------------------------
void GFieldBattle::LoadFirstData(int timer, int nDummy1, int nDummy2)
//--------------------------------------------------------------------------
{
	switch(timer)
	{
	case 0:
		//--------------------------------------------------------------------------
		{
			//	이벤트를 초기화시켜준다.
			INIT_MESSAGE();
			INIT_QUEST();
			INIT_EVENT();
			break;
		}
	case 1:
		//--------------------------------------------------------------------------
		{
			//	그림자 효과
			s_ASpriteSet.pShadowAs = SUTIL_LoadSprite(PACK_SPRITE_UI, SPRITE_UI_SHADOW);
			s_ASpriteSet.pShadowAs->SetBlendFrame(FRAME_SHADOW_BLEND);

			s_ASpriteSet.pDebuffAs = SUTIL_LoadSprite(PACK_SPRITE_UI, SPRITE_UI_FIELDEFT_UI);
			s_ASpriteSet.pDebuffAs->SetBlendFrame(FRAME_WEFFECT_BLEND);

			s_ASpriteSet.pFieldUiAs = SUTIL_LoadSprite(PACK_SPRITE_UI, SPRITE_UI_FIELD_UI);
			s_ASpriteSet.pFieldUiAs->SetBlendFrame(FRAME_UI_BLEND);

			s_ASpriteSet.pItemAs = SUTIL_LoadSprite(PACK_SPRITE_UI, SPRITE_UI_ITEM_UI);
			//s_ASpriteSet.pFieldUiAs->SetBlendFrame(FRAME_UI_BLEND);
			break;
		}
	case 2:
		//--------------------------------------------------------------------------
		{
			//	주인공
			hero = GL_NEW Character(SEX_WOMAN ,SDIR_RIGHT, (void*) &s_ASpriteSet);//인스턴스 포인터전달, 방향지정 및 초기화

			if(0 != nDummy2)
			{
				hero->InitCharPos(nDummy2, 240);
				if(300 < nDummy2)		{hero->_b_LookRight = SDIR_LEFT;	SUTIL_SetDirAsprite(hero->_ins_Hero, SDIR_LEFT);}
				else					{hero->_b_LookRight = SDIR_RIGHT;	SUTIL_SetDirAsprite(hero->_ins_Hero, SDIR_RIGHT);}
			}
			break;
		}
	case 3:
		//--------------------------------------------------------------------------
		{
			//	필드 UI
			pFieldUi = GL_NEW FieldUi((void*) &s_ASpriteSet);

			//	팝업 UI
			pPopupUi = GL_NEW PopupUi((void*) &s_ASpriteSet,hero->_spr_Hero);

			//	충돌체크 데이타
			m_CharInfo = (S_CHARINFO*)MALLOC(sizeof(S_CHARINFO));
			break;
		}
	case 4:
		//--------------------------------------------------------------------------
		{
			//	배경
			pField = GL_NEW Field((void*) &s_ASpriteSet);
			break;
		}
	case 5:
		//--------------------------------------------------------------------------
		{
			//	씨네마
			pCinema = GL_NEW cCinematic((void*) &s_ASpriteSet);

			//	몬스터
			//	Monster* tmpMonster;
			pMonList = GL_NEW List2<Monster*>();
//			pObjectList = GL_NEW List2<Monster*>();
			pMonSkillList = GL_NEW List2<S_MONSKILL*>();
			pMonMsgList = GL_NEW List2<S_MESSAGE*>();
			break;
		}
	case 6:
		//--------------------------------------------------------------------------
		{
			//-------------------Sound Load
			SUTIL_Sound_Load(EFFECT_SWORD_A	);
			SUTIL_Sound_Load(EFFECT_SWORD_B	);
			SUTIL_Sound_Load(EFFECT_SWORD_C	);
			SUTIL_Sound_Load(EFFECT_SWORD_D	);
			SUTIL_Sound_Load(EFFECT_GUN_A	);
			SUTIL_Sound_Load(EFFECT_GUN_B	);
			SUTIL_Sound_Load(EFFECT_ORB_A	);
			SUTIL_Sound_Load(EFFECT_ORB_B	);
			SUTIL_Sound_Load(EFFECT_ORB_C	);
			SUTIL_Sound_Load(BGM_TEST_A		);
			break;
		}
		//		case 7:
		//		//--------------------------------------------------------------------------
		//		{
		//			//Resv_ChangeStage(nDummy1);
		//	ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT,0,10);
		//	ADD_EVENT_MSG(EVT_CHANGE_STAGE, 10, nDummy1);
		//			ADD_EVENT_MSG(EVT_CHANGE_STAGE, 0, nDummy1);
		//
		//			break;
		//		}
	}
}
/////////////////////////////////////////////////////////////////////////////////

////--------------------------------------------------------------------------
//void GFieldBattle::Resv_ChangeStage(int nNextStage)
////--------------------------------------------------------------------------
//{
//	m_nNextStage = nNextStage;
//	m_nStageLoadStep = 0;
//	b_ChangeStage = true;
//}

//--------------------------------------------------------------------------
void GFieldBattle::LoadStage(int m_nNextStage, int step)
//--------------------------------------------------------------------------
{
	switch(step)
	{
	case 0:
		//----------------------------------------------------------
		{
			//	몬스터 정보
			//			nGenCnt = 0;
			ReleaseStage();
			break;
		}
	case 1:
		//----------------------------------------------------------
		{
			//	필드 정보
			pField->LoadMap(m_nNextStage);

			//	캐릭터에게 필드 아이디를 넘겨준다.(마을/필드 구분위해서)
			hero->m_nFieldId = m_nNextStage;

			//	이벤트에서 넘겨줄수 있다. (캐릭터 위치 셋팅)

			if(0 != pField->m_nWayMoveCharx)
			{
				//int temp = s_Camera.cX - hero->_ins_Hero->m_posX - hero->_ins_Hero->m_stopAniX;

				if(100 > pField->m_nWayMoveCharx)	//	방향은 오른쪽을 보고 있는다.
				{
					hero->InitCharPos(pField->m_nWayMoveCharx, pField->m_nWayMoveChary, SDIR_RIGHT);
				}
				else if(100 > (pField->m_nFieldSize_X - pField->m_nWayMoveCharx))	//	왼쪽
				{
					hero->InitCharPos(pField->m_nWayMoveCharx, pField->m_nWayMoveChary, SDIR_LEFT);
				}
				else	// 처리하지 않는다.
				{
					hero->InitCharPos(pField->m_nWayMoveCharx, pField->m_nWayMoveChary);
				}

				//hero->InitCharPos(pField->m_nWayMoveCharx, pField->m_nWayMoveChary);
				hero->_move_Order = HERO_STOP;

				pField->m_nWayMoveCharx = 0;

				//s_Camera.cX = temp;
				//	카메라 앵글 초기화 조정
				s_Camera.cX = 0;
				SetCameraMove();
				//pField->SetCamera(pField->m_nSrcCamAngle_X);
			}
			break;
		}
	case 2:
		//----------------------------------------------------------
		{
			switch(m_nNextStage)
			{
				//				case 1000:
				//				case 1001:
				//				//---------------------------------------------------------------
				//				{
				//					AppMain::ResvNextState(MS_GAME_WORLDMAP, m_nNextStage, 0);
				//					return;
				//				}

			case 100:
			case 101:
			case 102:
			case 103:
			case 104:
			case 105:
			case 106:
			case 107:
			case 108:
			case 109:
			case 110:
			case 111:
			case 112:
			case 113:
			case 114:
			case 115:
			case 116:
			case 117:
			case 200:
			case 201:
			case 202:
			case 203:
			case 204:
			case 205:
			case 206:
			case 207:
			case 208:
			case 209:
			case 210:
			case 211:
			case 212:
			case 213:
			case 214:
			case 215:
			case 216:
			case 217:
				//---------------------------------------------------------------
				{
					//	몬스터 정보
					MakeMonsterSeed(m_nNextStage);
					//					for(int loop = 0; loop < nMonTypeCnt; loop++)
					//					{
					//						LoadMonsterSprite( nMonDataIdx[loop] );
					//					}
					AddFirstMonSetting();

					if(MAIN_QUEST_openDoor >= GET_MAINQUEST_IDX())
					{
						if(201 == m_nNextStage)
						{
							Monster* pTmpMonster = NULL;
							pTmpMonster = AddMonster(1, MON_IDX_STONE_BIG, 0, 0, GRADE_NORMAL, 590, 270, 590, 270, NOT_REGEN, 0);
							pTmpMonster->pMonAsIns->m_pal = 3;												  
							pTmpMonster = AddMonster(1, MON_IDX_STONE_BIG, 0, 0, GRADE_NORMAL, 590, 270, 590, 270, NOT_REGEN, 0);
							pTmpMonster->pMonAsIns->m_pal = 3;												  
							pTmpMonster = AddMonster(1, MON_IDX_STONE_BIG, 0, 0, GRADE_NORMAL, 640, 280, 640, 280, NOT_REGEN, 0);
							pTmpMonster->pMonAsIns->m_pal = 3;												  
							pTmpMonster = AddMonster(1, MON_IDX_STONE_BIG, 0, 0, GRADE_NORMAL, 570, 290, 570, 290, NOT_REGEN, 0);
							pTmpMonster->pMonAsIns->m_pal = 3;
						}
					}

					if(MAIN_QUEST_openDoor == GET_MAINQUEST_IDX())
					{
						//	이벤트시 통을 만들어준다.
						if(201 == m_nNextStage || 202 == m_nNextStage || 203 == m_nNextStage)
						{
							int xrnd = 0;
							int yrnd = 0;
							int rnd = 3+SUTIL_GetRandom()%5;
							for(int loop = 0; loop < rnd; loop++)
							{
								xrnd = (SUTIL_GetRandom()%pField->m_nFieldSize_X);
								yrnd = 170+(SUTIL_GetRandom()%100);

								AddMonster(1, MON_IDX_BARREL, 0, 0, GRADE_NORMAL, xrnd, yrnd, xrnd, yrnd, NOT_REGEN, 0);
							}
						}
					}

					if(MAIN_QUEST_findDiary == GET_MAINQUEST_IDX())
					{
						//	이벤트시 다이어리 아이템을 만들어준다.
						if(203 == m_nNextStage)
						{
							if(pPopupUi->GET_Count(ITEM_CALL_diary) == 0)
							{
								//	아이템을 만든다.

								ItemBag tmpBag;
								tmpBag.Data0 = ITEM_CALL_diary;
								tmpBag.Data1 = 1;
								tmpBag.Data2 = 0;

								pField->InsertDropItem(tmpBag, 800, 230);
							}
						}
					}
					break;
				}
/*호로스
				case 400:
				//---------------------------------------------------------------
				{
					MakeMonsterSeed(m_nNextStage);

					SUTIL_LoadAspritePack(PACK_SPRITE_MON);
					LoadMonsterSprite(MON_IDX_HOROS);
					LoadMonsterSprite(MON_IDX_BUG);
					SUTIL_ReleaseAspritePack();

					//	소환되는 쫄(중간중간에 소환)
					AddMonster(1, MON_IDX_HOROS, 0, 0, GRADE_SPECIAL, 300, 180, 300, 180, NOT_REGEN, 0);
					break;
				}
*/
				

/*마지막보스
				case 400:
				//---------------------------------------------------------------
				{
					MakeMonsterSeed(m_nNextStage);

					SUTIL_LoadAspritePack(PACK_SPRITE_MON);
					
					LoadMonsterSprite(MON_IDX_DRILL);
					LoadMonsterSprite(MON_IDX_BOSS5_DEVIL);

					LoadMonsterSprite(MON_IDX_BIGDOG1);
					LoadMonsterSprite(MON_IDX_BIGDOG2);
					LoadMonsterSprite(MON_IDX_BIGDOG3);
					LoadMonsterSprite(MON_IDX_BIGDOG4);
					LoadMonsterSprite(MON_IDX_BIGDOG5);

					LoadMonsterSprite(MON_IDX_BIGDOG6);
					LoadMonsterSprite(MON_IDX_BIGDOG7);
					LoadMonsterSprite(MON_IDX_BIGDOG8);
					LoadMonsterSprite(MON_IDX_BIGDOG9);
					LoadMonsterSprite(MON_IDX_BIGDOG10);


					

					SUTIL_ReleaseAspritePack();

					BossMon5_1* pTmpMon = (BossMon5_1*)AddMonster(1, MON_IDX_BIGDOG1, 0, 0, GRADE_SPECIAL, 230, 215-20, 230, 215-20, NOT_REGEN, 0);
					pTmpMon->pChildBody[0] = AddMonster(1, MON_IDX_BIGDOG2, 0, 0, GRADE_SPECIAL, 230, 215-10, 230, 215-10, NOT_REGEN, 0);
					pTmpMon->pChildBody[1] = AddMonster(1, MON_IDX_BIGDOG3, 0, 0, GRADE_SPECIAL, 230, 215+10, 230, 215+10, NOT_REGEN, 0);
					pTmpMon->pChildBody[2] = AddMonster(1, MON_IDX_BIGDOG4, 0, 0, GRADE_SPECIAL, 230, 215+20, 230, 215+20, NOT_REGEN, 0);
					pTmpMon->pChildBody[3] = AddMonster(1, MON_IDX_BIGDOG5, 0, 0, GRADE_SPECIAL, 230,	 110, 230,	  110, NOT_REGEN, 0);
					pTmpMon->pChildBody[4] = AddMonster(1, MON_IDX_BIGDOG6, 0, 0, GRADE_SPECIAL,   0,	-200,   0,	 -200, NOT_REGEN, 0);
					pTmpMon->pChildBody[5] = AddMonster(1, MON_IDX_BIGDOG7, 0, 0, GRADE_SPECIAL,   0,	-200,   0,	 -200, NOT_REGEN, 0);
					pTmpMon->pChildBody[6] = AddMonster(1, MON_IDX_BIGDOG8, 0, 0, GRADE_SPECIAL,   0,	-200,   0,	 -200, NOT_REGEN, 0);
					pTmpMon->pChildBody[7] = AddMonster(1, MON_IDX_BIGDOG9, 0, 0, GRADE_SPECIAL,   0,	-200,   0,	 -200, NOT_REGEN, 0);
					pTmpMon->pChildBody[8] = AddMonster(1, MON_IDX_BIGDOG10, 0, 0, GRADE_SPECIAL,  0,	-200,   0,	 -200, NOT_REGEN, 0);
					break;
				}
*/

//대비존스
				case 400:
				//---------------------------------------------------------------
				{
					MakeMonsterSeed(m_nNextStage);

					SUTIL_LoadAspritePack(PACK_SPRITE_MON);
					LoadMonsterSprite(MON_IDX_DEVIJOHNS);
					SUTIL_ReleaseAspritePack();

					//	소환되는 쫄(중간중간에 소환)
					AddMonster(1, MON_IDX_DEVIJOHNS, 0, 0, GRADE_SPECIAL, 180, 220, 180, 220, NOT_REGEN, 0);
					break;
				}




// 				case 400:
// 				//---------------------------------------------------------------
// 				{
// 					MakeMonsterSeed(m_nNextStage);
// 
// 					SUTIL_LoadAspritePack(PACK_SPRITE_MON);
// 					LoadMonsterSprite(MON_IDX_BIG_DRAGON1);
// 					LoadMonsterSprite(MON_IDX_BIG_DRAGON2);
// 					LoadMonsterSprite(MON_IDX_BIG_DRAGON3);
// 
// 					SUTIL_ReleaseAspritePack();
// 
// 					BossBigDragon1* pTmpMon = (BossBigDragon1*)AddMonster(1, MON_IDX_BIG_DRAGON1, 0, 0, GRADE_SPECIAL, 320, 210, 320, 210, NOT_REGEN, 0);
// 					pTmpMon->SetDirection(SDIR_LEFT);
// 					pTmpMon->pChildBody[0] = AddMonster(1, MON_IDX_BIG_DRAGON2, 0, 0, GRADE_SPECIAL, 0, 210, 0, 210, NOT_REGEN, 0);
// 					pTmpMon->SetDirection(SDIR_LEFT);
// 					pTmpMon->pChildBody[1] = AddMonster(1, MON_IDX_BIG_DRAGON3, 0, 0, GRADE_SPECIAL, 235-120, 173, 235-120, 173, NOT_REGEN, 0);
// 					break;
// 				}

			}
			break;
		}
	}

}


//--------------------------------------------------------------------------
void GFieldBattle::ReleaseStage()
//--------------------------------------------------------------------------
{
	//	CLEAR_EVENT_MSG();

	//	static 변수 초기화
	Monster::UniqueIdx = 0;

	//	이미지 초기화
	for(int loop = 0; loop < SPRITE_MON_MAX; loop++)
	{
		if(NULL != m_MonAs[loop])
		{
			SUTIL_FreeSprite(m_MonAs[loop]);
		}
	}

	InitList(pMonList);
	while(NotEndList(pMonList))
	{
		SAFE_DELETE(GetData(pMonList));
		pMonList->Delete();
	}
	//	SAFE_DELETE(pMonList)

//	InitList(pObjectList);
//	while(NotEndList(pObjectList))
//	{
//		SAFE_DELETE(GetData(pObjectList));
//		pObjectList->Delete();
//	}

	InitList(pMonSkillList);
	while(NotEndList(pMonSkillList))
	{
		SUTIL_FreeSpriteInstance(GetData(pMonSkillList)->pMonSkillAsIns);
		SAFE_DELETE(GetData(pMonSkillList));
		pMonSkillList->Delete();
	}
	//	SAFE_DELETE(pMonSkillList)

	//	message
	InitList(pMonMsgList);
	while(NotEndList(pMonMsgList))
	{
		SAFE_DELETE(GetData(pMonMsgList));
		pMonMsgList->Delete();
	}
	//	SAFE_DELETE(pMonMsgList)

	//	필드 UI
	//	SAFE_DELETE(pFieldUi);

	//	기타
	SAFE_DELETE(pMonData);
	SAFE_DELETE(m_nSeedData);
	SAFE_DELETE(m_nSeedDataGrade);
}

///////////////////////////////////////////////////////////////////////////////


//--------------------------------------------------------------------------
void GFieldBattle::MakeMonsterSeed(int nStageNum)
//--------------------------------------------------------------------------
{
	int loop = 0;
	int nRealFieldNum = 0;

	while(NU != FIELD_MONSTER_INFO[nRealFieldNum][0])
	{
		if(nStageNum == FIELD_MONSTER_INFO[nRealFieldNum][GEN_FIELD_NUM])	{break;}
		nRealFieldNum++;
	}

	if(NU == FIELD_MONSTER_INFO[nRealFieldNum][0])	{return;}

	m_nMonLevel = FIELD_MONSTER_INFO[nRealFieldNum][GEN_LEVEL];
	m_nMonLevelRnd = FIELD_MONSTER_INFO[nRealFieldNum][GEN_ADD_RND_LEVEL]+1;

	pField->m_nRegenDistance = FIELD_MONSTER_INFO[nRealFieldNum][GEN_DISTANCE];

	//	몬스터 등급 설정
	nMonGradeCnt[GRADE_RARE]	= FIELD_MONSTER_INFO[nRealFieldNum][GEN_RARE_GRADE];
	nMonGradeCnt[GRADE_UNIQUE]	= FIELD_MONSTER_INFO[nRealFieldNum][GEN_UNIQUE_GRADE];
	nMonGradeCnt[GRADE_SPECIAL]	= FIELD_MONSTER_INFO[nRealFieldNum][GEN_SPECIAL_GRADE];

	//	몬스터 설정
	nMonTypeCnt = 0;
	for(loop = 0; loop < GEN_MON_COUNT; loop++)
	{
		if(NU != FIELD_MONSTER_INFO[nRealFieldNum][GEN_MON1_IDX+loop*4])	{nMonTypeCnt++;}
	}

	//	nMonTypeCnt = 3;
	pMonData = GL_NEW STAGE_MON_DATA [nMonTypeCnt];

	for(loop = 0; loop < nMonTypeCnt; loop++)
	{
		pMonData[loop].nMonIdx = FIELD_MONSTER_INFO[nRealFieldNum][GEN_MON1_IDX+loop*4];
		pMonData[loop].nNameIdx = FIELD_MONSTER_INFO[nRealFieldNum][GEN_MON1_NAME+loop*4];
		pMonData[loop].nPtn = FIELD_MONSTER_INFO[nRealFieldNum][GEN_MON1_PTN+loop*4];
		pMonData[loop].nCount = FIELD_MONSTER_INFO[nRealFieldNum][GEN_MON1_COUNT+loop*4];
	}

	//	몬스터 총 갯수
	m_nMonSeedCnt = 0;
	for(int loop = 0; loop < nMonTypeCnt; loop++)
	{
		m_nMonSeedCnt += pMonData[loop].nCount;
	}

	//	몬스터가 없다.
	if(!m_nMonSeedCnt)	{return;}
	/////////////////////////////////////////////////////////////////////////////////////////////

	//	몬스터 데이타 구성
	m_nSeedData	= GL_NEW int[m_nMonSeedCnt];

	loop = 0;
	int loop2 = 0;
	int addcnt = 0;

	for(loop = 0; loop < nMonTypeCnt; loop++)
	{
		for(loop2 = 0; loop2 < pMonData[loop].nCount; loop2++)
		{
			m_nSeedData[loop2+addcnt] = loop;
		}
		addcnt += loop2;
	}

	//	시드 스왑(10번)
	int src, dst, tmp;
	for(loop = 0; loop < 10; loop++)
	{
		src = SUTIL_GetRandom()%(m_nMonSeedCnt);
		dst = SUTIL_GetRandom()%(m_nMonSeedCnt);

		tmp = m_nSeedData[src];
		m_nSeedData[src] = m_nSeedData[dst];
		m_nSeedData[dst] = tmp;
	}

	//	등급 결정 테이블
	int GradeDiceTable[5][4] = {{0, 38, 15, 5},	{0, 36, 16, 6},	{0, 34, 17, 7},	{0, 34, 17, 8},	{0, 33, 17, 9}};

	//	등급결정을 해야됨
	m_nSeedDataGrade = GL_NEW int [m_nMonSeedCnt];

	int TableIdx = 4;
	if(70 < hero->s_Status.LEVEL)		{TableIdx = 0;}
	else if(60 < hero->s_Status.LEVEL)	{TableIdx = 1;}
	else if(40 < hero->s_Status.LEVEL)	{TableIdx = 2;}
	else if(20 < hero->s_Status.LEVEL)	{TableIdx = 3;}

	//	등급 확인
	int Dice = 0;
	for(loop = GRADE_RARE; loop <= GRADE_SPECIAL; loop++)
	{
		while(0 < nMonGradeCnt[loop])
		{
			Dice = SUTIL_GetRandom()%100;
			if(GradeDiceTable[TableIdx][loop] > Dice)
			{
				m_nSeedDataGrade[Dice%m_nMonSeedCnt] = loop;
			}
			nMonGradeCnt[loop]--;
		}
	}

	//	몬스터의 이미지를 로드시킨다.
	SUTIL_LoadAspritePack(PACK_SPRITE_MON);
	for(loop = 0; loop < nMonTypeCnt; loop++)
	{
		LoadMonsterSprite(pMonData[loop].nMonIdx);
	}
	SUTIL_ReleaseAspritePack();
}


//--------------------------------------------------------------------------
void GFieldBattle::LoadMonsterSprite(int monidx)
//--------------------------------------------------------------------------
{
	//		pMonData[loop].nMonIdx = FIELD_MONSTER_INFO[nRealFieldNum][GEN_MON1_IDX+loop*4];

	//	일반 몬스터 //////////////////////////////////////////////////

	int loop2 = 0;
	int addcnt = 0;

	switch(monidx)
	{
		case MON_IDX_RAPTER:		{loop2 = SPRITE_MON_T_REX;	addcnt = FRAME_MON01_BLEND;		break;}
		case MON_IDX_GOLEM:			{loop2 = SPRITE_MON_GOLEM;	addcnt = FRAME_MON02_BLEND;		break;}
		case MON_IDX_SLIME:			{loop2 = SPRITE_MON_SLIME;	addcnt = FRAME_MON03_BLEND;		break;}
		case MON_IDX_WATER_ELE:		{loop2 = SPRITE_MON_ELEMENT;	addcnt = FRAME_MON04_BLEND;		break;}
		case MON_IDX_COBOLT:		{loop2 = SPRITE_MON_COBOLT;	addcnt = FRAME_MON05_BLEND;		break;}
		case MON_IDX_THUNDERBIRD:	{loop2 = SPRITE_MON_BIRD;	addcnt = FRAME_MON06_BLEND;		break;}
		case MON_IDX_TANK:			{loop2 = SPRITE_MON_TANK;	addcnt = FRAME_MON07_BLEND;		break;}
		case MON_IDX_PUMPKIN:		{loop2 = SPRITE_MON_PUMPKIN;	addcnt = FRAME_MON08_BLEND;		break;}
		case MON_IDX_SKELETON:		{loop2 = SPRITE_MON_SKELETON;	addcnt = FRAME_MON09_BLEND;		break;}
		case MON_IDX_GUNNER:		{loop2 = SPRITE_MON_GUNNER;	addcnt = FRAME_MON10_BLEND;		break;}
		case MON_IDX_ELF_FIRE:		{loop2 = SPRITE_MON_ELF;	addcnt = FRAME_MON11_BLEND;		break;}
		case MON_IDX_ELF_ICE:		{loop2 = SPRITE_MON_ELF;	addcnt = FRAME_MON11_BLEND;		break;}
		case MON_IDX_ELF_SILENCE:	{loop2 = SPRITE_MON_ELF;	addcnt = FRAME_MON11_BLEND;		break;}
		case MON_IDX_TRICERATOPS:	{loop2 = SPRITE_MON_TRICERATOPS;	addcnt = FRAME_MON12_BLEND;		break;}
		case MON_IDX_REDDRAGON:		{loop2 = SPRITE_MON_REDDRAGON;	addcnt = FRAME_MON13_BLEND;		break;}
		case MON_IDX_BOSS5_DEVIL:
		case MON_IDX_DEVIL:			{loop2 = SPRITE_MON_DEVIL;	addcnt = FRAME_MON14_BLEND;		break;}
		case MON_IDX_HUMAN_SWORD:	{loop2 = SPRITE_MON_HUMAN;	addcnt = FRAME_MON15_BLEND;		break;}
		case MON_IDX_HUMAN_MAGE:	{loop2 = SPRITE_MON_HUMAN;	addcnt = FRAME_MON15_BLEND;		break;}
		case MON_IDX_FLYTRAP:		{loop2 = SPRITE_MON_FLYTRAP;	addcnt = FRAME_MON16_BLEND;		break;}
		case MON_IDX_CRAB:			{loop2 = SPRITE_MON_CRAB;	addcnt = FRAME_MON17_BLEND;		break;}
		case MON_IDX_BUG:			{loop2 = SPRITE_MON_BUG;	addcnt = FRAME_MON18_BLEND;		break;}
		case MON_IDX_DRILL:			{loop2 = SPRITE_MON_DIRLL;	addcnt = FRAME_MON19_BLEND;		break;}
									

		case MON_IDX_HOROS:			{loop2 = SPRITE_MON_BOSS_2;	addcnt = FRAME_BOSS_2_BLEND;	break;}
		case MON_IDX_BIGDOG1:
		case MON_IDX_BIGDOG2:
		case MON_IDX_BIGDOG3:
		case MON_IDX_BIGDOG4:
		case MON_IDX_BIGDOG5:		{loop2 = SPRITE_MON_BOSS_5;	addcnt = FRAME_BOSS_5_BLEND;	break;}
		case MON_IDX_BIGDOG6:
		case MON_IDX_BIGDOG7:
		case MON_IDX_BIGDOG8:
		case MON_IDX_BIGDOG9:
		case MON_IDX_BIGDOG10:		{loop2 = SPRITE_MON_BOSS_6;	addcnt = FRAME_BOSS_6_BLEND;	break;}

		case MON_IDX_DEVIJOHNS:		{loop2 = SPRITE_MON_BOSS_7;	addcnt = FRAME_BOSS_7_BLEND;	break;}

		case MON_IDX_BIG_DRAGON1:
		case MON_IDX_BIG_DRAGON2:
		case MON_IDX_BIG_DRAGON3:	{loop2 = SPRITE_MON_BOSS_1;	addcnt = FRAME_BOSS_1_BLEND;	break;}


									



//		case MON_IDX_BARREL:		{loop2 = SPRITE_MON_BARREL;	addcnt = 0;						break;}
//		case MON_IDX_STONE_BIG:		{loop2 = SPRITE_MON_STONE_BIG;	addcnt = 0;					break;}
//		case MON_IDX_BOX:			{loop2 = SPRITE_MON_STONE_BIG;	addcnt = 0;					break;}
//		case MON_IDX_KNIGHT:		{loop2 = SPRITE_MON_STONE_BIG;	addcnt = 0;					break;}
//		case MON_IDX_TEMPLE_CROCK_SMALL:	{loop2 = SPRITE_MON_STONE_BIG;	addcnt = 0;			break;}
//		case MON_IDX_TEMPLE_CROCK_BIG:		{loop2 = SPRITE_MON_STONE_BIG;	addcnt = 0;			break;}
//		case MON_IDX_TEMPLE_STONEBOX_SMALL:	{loop2 = SPRITE_MON_STONE_BIG;	addcnt = 0;			break;}
//		case MON_IDX_TEMPLE_STONEBOX_BIG:	{loop2 = SPRITE_MON_STONE_BIG;	addcnt = 0;			break;}
		default:					{															break;}
	}

	SUTIL_FreeSprite(m_MonAs[loop2]);
	m_MonAs[loop2] = SUTIL_LoadAspriteFromPack(PACK_SPRITE_MON, loop2);

	m_MonAs[loop2]->SetBlendFrame(addcnt);
}

//--------------------------------------------------------------------------
Monster* GFieldBattle::AddMonster(int addType, int monidx, int nameidx, int ptnIdx, int gradeDataidx, int x, int y, int startx, int starty, int regenidx, int z)
//--------------------------------------------------------------------------
{
	Monster* tmpMonster = NULL;

	//	일반 몬스터 //////////////////////////////////////////////////
	int namestartidx = 0;
	int ptnstartidx = 0;

	switch(monidx)
	{
		case MON_IDX_RAPTER:		{tmpMonster = GL_NEW Mon_RAPTER();		break;}
		case MON_IDX_GOLEM:			{tmpMonster = GL_NEW Mon_GOLEM();		break;}
		case MON_IDX_SLIME:			{tmpMonster = GL_NEW Mon_SLIME();		break;}
		case MON_IDX_WATER_ELE:		{tmpMonster = GL_NEW Mon_WATER_EFE();	break;}
		case MON_IDX_COBOLT:		{tmpMonster = GL_NEW Mon_COBOLT();		break;}
		case MON_IDX_THUNDERBIRD:	{tmpMonster = GL_NEW Mon_THUNDERBIRD();	break;}
		case MON_IDX_TANK:			{tmpMonster = GL_NEW Mon_TANK();		break;}
		case MON_IDX_PUMPKIN:		{tmpMonster = GL_NEW Mon_PUMPKIN();		break;}
		case MON_IDX_SKELETON:		{tmpMonster = GL_NEW Mon_SKELETON();	break;}
		case MON_IDX_GUNNER:		{tmpMonster = GL_NEW Mon_GUNNER();		break;}
		case MON_IDX_ELF_FIRE:		{tmpMonster = GL_NEW Mon_ELF_FIRE();	break;}
		case MON_IDX_ELF_ICE:		{tmpMonster = GL_NEW Mon_ELF_ICE();		break;}
		case MON_IDX_ELF_SILENCE:	{tmpMonster = GL_NEW Mon_ELF_SILENCE();	break;}
		case MON_IDX_TRICERATOPS:	{tmpMonster = GL_NEW Mon_TRICERATOPS();	break;}
		case MON_IDX_REDDRAGON:		{tmpMonster = GL_NEW Mon_REDDRAGON();	break;}
		case MON_IDX_DEVIL:			{tmpMonster = GL_NEW Mon_DEVIL();		break;}
		case MON_IDX_HUMAN_SWORD:	{tmpMonster = GL_NEW Mon_HUMAN_SWORD();	break;}
		case MON_IDX_HUMAN_MAGE:	{tmpMonster = GL_NEW Mon_HUMAN_MAGE();	break;}
		case MON_IDX_FLYTRAP:		{tmpMonster = GL_NEW Mon_FLYTRAP();		break;}
		case MON_IDX_CRAB:			{tmpMonster = GL_NEW Mon_CRAB();		break;}
		default:					{										break;}
	}

	if(tmpMonster)
	{
		tmpMonster->RegistUsingBaseData(pMonSkillList, pMonMsgList, &s_ASpriteSet);

		//	컬러 결정
		//int coloridx = nameidx * tmpMonster->m_nSpriteIdx;
		int coloridx = nameidx%3;


		//	속성 결정
		int elementidx = nameidx%3;

		//	속성 결정
		//int levelidx = hero->s_Status.LEVEL;
		int levelidx = m_nMonLevel;
		
		if(m_nMonLevelRnd)
		{
			levelidx += SUTIL_GetRandom()%(m_nMonLevelRnd);
		}

		tmpMonster->SetMonData(coloridx, elementidx, (namestartidx+(monidx*9)+nameidx), (ptnstartidx+(monidx*2)+ptnIdx), gradeDataidx, levelidx);
		tmpMonster->SetPosition(x, y, z);
		tmpMonster->SetRegenType(addType, regenidx, startx, starty);
		MoveTail(pMonList);
		pMonList->Insert_prev(tmpMonster);
		return tmpMonster;
	}

	//	추가 오브젝트 //////////////////////////////////////////////////
	namestartidx += MON_IDX_COUNT_NORMAL_MON*9;	//	인덱스 보정
	ptnstartidx += MON_IDX_COUNT_NORMAL_MON*2;

	switch(monidx)
	{
		case MON_IDX_BARREL:				{tmpMonster = GL_NEW Barrel();					break;}
		case MON_IDX_STONE_BIG:				{tmpMonster = GL_NEW Stone_big();				break;}
		case MON_IDX_BOX:					{tmpMonster = GL_NEW Box();						break;}
		case MON_IDX_KNIGHT:				{tmpMonster = GL_NEW Knight();					break;}
		case MON_IDX_TEMPLE_CROCK_SMALL:	{tmpMonster = GL_NEW Temple_crock_small();		break;}
		case MON_IDX_TEMPLE_CROCK_BIG:		{tmpMonster = GL_NEW Temple_crock_big();		break;}
		case MON_IDX_TEMPLE_STONEBOX_SMALL:	{tmpMonster = GL_NEW Temple_stonebox_small();	break;}
		case MON_IDX_TEMPLE_STONEBOX_BIG:	{tmpMonster = GL_NEW Temple_stonebox_big();		break;}
		case MON_IDX_BUG:					{tmpMonster = GL_NEW Mon_BUG();					break;}
		case MON_IDX_DRILL:					{tmpMonster = GL_NEW Mon_DRILL();				break;}
		case MON_IDX_BOSS5_DEVIL:			{tmpMonster = GL_NEW Mon_BOSS5_DEVIL();			break;}

											
	}

	if(tmpMonster)
	{
		tmpMonster->RegistUsingBaseData(pMonSkillList, pMonMsgList, &s_ASpriteSet);

//		//	컬러 결정
//		int coloridx = 0;

//		//	속성 결정
//		int elementidx = 0;

//		//	레벨 결정
//		int levelidx = m_nMonLevel;

//		tmpMonster->SetMonData(coloridx, elementidx, nameidx, ptnIdx, gradeDataidx, levelidx);

		tmpMonster->SetMonData(0, MON_ELEMENTAL_NON, (namestartidx + (monidx-MON_IDX_BARREL)),
														(ptnstartidx + (monidx-MON_IDX_BARREL)), GRADE_NORMAL, m_nMonLevel);
		tmpMonster->SetPosition(x, y, z);
		tmpMonster->SetRegenType(addType, regenidx, startx, starty);
		MoveTail(pMonList);
		pMonList->Insert_prev(tmpMonster);

		return tmpMonster;
	}


	//	보스 몬스터 //////////////////////////////////////////////////
	namestartidx += (MON_IDX_COUNT_OBJECT);	//	인덱스 보정
	ptnstartidx += (MON_IDX_COUNT_OBJECT);

	switch(monidx)
	{
		case MON_IDX_HOROS:					{tmpMonster = GL_NEW BossMon2_1();					break;}
		case MON_IDX_BIGDOG1:				{tmpMonster = GL_NEW BossMon5_1();					break;}
		case MON_IDX_BIGDOG2:				{tmpMonster = GL_NEW BossMon5_2();					break;}
		case MON_IDX_BIGDOG3:				{tmpMonster = GL_NEW BossMon5_3();					break;}
		case MON_IDX_BIGDOG4:				{tmpMonster = GL_NEW BossMon5_4();					break;}
		case MON_IDX_BIGDOG5:				{tmpMonster = GL_NEW BossMon5_5();					break;}
		case MON_IDX_BIGDOG6:				{tmpMonster = GL_NEW BossMon5_6();					break;}

		case MON_IDX_BIGDOG7:				{tmpMonster = GL_NEW BossMon5_7();					break;}
		case MON_IDX_BIGDOG8:				{tmpMonster = GL_NEW BossMon5_8();					break;}
		case MON_IDX_BIGDOG9:				{tmpMonster = GL_NEW BossMon5_9();					break;}
		case MON_IDX_BIGDOG10:				{tmpMonster = GL_NEW BossMon5_10();					break;}

		case MON_IDX_DEVIJOHNS:				{tmpMonster = GL_NEW Mon_DEVIJOHNS();				break;}

		case MON_IDX_BIG_DRAGON1:			{tmpMonster = GL_NEW BossBigDragon1();				break;}
		case MON_IDX_BIG_DRAGON2:			{tmpMonster = GL_NEW BossBigDragon2();				break;}
		case MON_IDX_BIG_DRAGON3:			{tmpMonster = GL_NEW BossBigDragon3();				break;}
											
	}

	if(tmpMonster)
	{
		tmpMonster->RegistUsingBaseData(pMonSkillList, pMonMsgList, &s_ASpriteSet);

		//	컬러 결정
		//int coloridx = nameidx * tmpMonster->m_nSpriteIdx;
		int coloridx = 0;

		//	속성 결정
		int elementidx = 0;

		//	속성 결정 (보스 레벨을 랜덤으로 하면 한 몸통의 몬스터들의 피가 달라진다.)
		int levelidx = m_nMonLevel;

//		if(m_nMonLevelRnd)	{levelidx += SUTIL_GetRandom()%(m_nMonLevelRnd);}

		tmpMonster->SetMonData(coloridx, elementidx, nameidx, ptnIdx, gradeDataidx, levelidx);
		tmpMonster->SetPosition(x, y, z);
		tmpMonster->SetRegenType(addType, regenidx, startx, starty);
		MoveTail(pMonList);
		pMonList->Insert_prev(tmpMonster);
		return tmpMonster;
	}

	return NULL;

	/*
	case MON_IDX_BOSS2_1:		{tmpMonster = GL_NEW BossMon2_1();		break;}
	//		case MON_IDX_BOSS3_1:		{tmpMonster = GL_NEW BossMon3_1();		break;}
	//		case MON_IDX_BOSS3_2:		{tmpMonster = GL_NEW BossMon3_2();		break;}
	case MON_IDX_BOSS5_1:		{tmpMonster = GL_NEW BossMon5_1();		break;}
	case MON_IDX_BOSS5_2:		{tmpMonster = GL_NEW BossMon5_2();		break;}
	case MON_IDX_BOSS5_3:		{tmpMonster = GL_NEW BossMon5_3();		break;}
	case MON_IDX_BOSS5_4:		{tmpMonster = GL_NEW BossMon5_4();		break;}
	case MON_IDX_BOSS5_5:		{tmpMonster = GL_NEW BossMon5_5();		break;}
	//	case MON_IDX_BOSS6_1:		{tmpMonster = GL_NEW BossMon6_1();		break;}
	default:	{return NULL;}
	}

	*/
}



//--------------------------------------------------------------------------
void GFieldBattle::AddRegenMonSetting(int defPosx)
//--------------------------------------------------------------------------
{
	if(0 >= m_nMonSeedCnt)	{return;}
	m_nMonSeedCnt--;

	int x = SUTIL_GetRandom()%(pField->m_nRegenDistance);
	x += defPosx;

	int y = 185 + SUTIL_GetRandom()%70;

	//add
	AddMonster(1,
		pMonData[m_nSeedData[m_nMonSeedCnt]].nMonIdx,
		pMonData[m_nSeedData[m_nMonSeedCnt]].nNameIdx,
		pMonData[m_nSeedData[m_nMonSeedCnt]].nPtn,
		m_nSeedDataGrade[m_nMonSeedCnt],
		x, y, x, y, defPosx, 0);
}


//--------------------------------------------------------------------------
void GFieldBattle::AddFirstMonSetting()
//--------------------------------------------------------------------------
{
	//	이값을 추가해주면 여기서부터 한마리 만들어지게 한다.
	int x, y;
	int nRegenTmp = 0;

	while(pField->m_nFieldSize_X > nRegenTmp)
	{
		if(0 >= m_nMonSeedCnt)	{break;}

		AddRegenMonSetting(nRegenTmp);
		nRegenTmp += pField->m_nRegenDistance;
	}
}

/*
//--------------------------------------------------------------------------
void GFieldBattle::AddRegenMonSetting()
//--------------------------------------------------------------------------
{
if(1 > m_nMonSeedCnt)	{return;}

int MonIdx = 0;
int RegenDice = 0;

//	리젠 영역 결정
while(1)
{
RegenDice = SUTIL_GetRandom()%nRegenCnt;
//		if(1 > m_nRegenType[RegenDice])		{continue;}		//	리젠 영역이 아니라면
if(1 == m_nIsRegen[RegenDice])	{continue;}
break;
}

//	리젠 영역 및 리젠 타입 설정
int mapsize = pField->m_nFieldSize_X;
int mapstart = pField->m_nSrcCamAngle_X;
int xTmp = m_nRegenX[RegenDice];
int yTmp = m_nRegenY[RegenDice];
int regenXTmp = xTmp;
int regenYTmp = yTmp;
int addType = 0;

if(xTmp > mapstart && xTmp < (mapstart+SCREEN_WIDTH))		//	화면 안이라면
{
addType = 3;
}
else
{
addType = (SUTIL_GetRandom()%2)+2;
}

if(addType == 2)		//	접근하면서 시작한다. 접근거리를 계산한다.
{
regenXTmp = (xTmp+m_CharInfo->m_nPos.x)/2;
}
else if(addType == 3)		//	근처에서 재생되어 전투모드로 들어온다.
{
if(xTmp < m_CharInfo->m_nPos.x)
{
xTmp = m_CharInfo->m_nPos.x-SCREEN_WIDTH;
}
else
{
xTmp = m_CharInfo->m_nPos.x+SCREEN_WIDTH;
}

if(0 > xTmp)		{xTmp = m_CharInfo->m_nPos.x+SCREEN_WIDTH;}
if(mapsize < xTmp)	{xTmp = m_CharInfo->m_nPos.x-SCREEN_WIDTH;}

regenXTmp = xTmp;
}

//	몬스터의 갯수를 줄여준다.
m_nMonSeedCnt--;

//	몬스터 결정
//	현재 리젠중인 상태인걸 알려준다.
m_nIsRegen[RegenDice] = 1;

AddMonster(addType, m_nSeedData[m_nMonSeedCnt], m_nSeedDataGrade[m_nMonSeedCnt],
xTmp, yTmp,	regenXTmp, regenYTmp, RegenDice);

GetData(pMonList)->Process(m_CharInfo);
}
*/



//--------------------------------------------------------------------------
void GFieldBattle::Release()
//--------------------------------------------------------------------------
{	
	//	그림자 효과
	SUTIL_FreeSprite(s_ASpriteSet.pShadowAs);
	SUTIL_FreeSprite(s_ASpriteSet.pDebuffAs);
	SUTIL_FreeSprite(s_ASpriteSet.pFieldUiAs);
	SUTIL_FreeSprite(s_ASpriteSet.pItemAs);

	//	주인공
	SAFE_DEL(hero);

	// 배경
	//	/*SUTIL_FreeSprite*/SAFE_DELETE(pFieldAs);
	//	/*SUTIL_FreeSpriteInstance*/SAFE_DELETE(pFieldAsins);
	SAFE_DELETE(pField)

		//	몬스터

		SAFE_DELETE(m_CharInfo);

	for(int loop = 0; loop < SPRITE_MON_MAX; loop++)
	{
		if(NULL != m_MonAs[loop])
		{
			SUTIL_FreeSprite(m_MonAs[loop]);
		}
	}

	if(pMonList)
	{
		MoveHead(pMonList);
		MoveNext(pMonList);
		while(NotEndList(pMonList))
		{
			SAFE_DELETE(GetData(pMonList));
			pMonList->Delete();
			//		MoveNext(pMonList);
		}
	}
	SAFE_DELETE(pMonList)

		if(pMonSkillList)
		{
			MoveHead(pMonSkillList);
			MoveNext(pMonSkillList);
			while(NotEndList(pMonSkillList))
			{
				SUTIL_FreeSpriteInstance(GetData(pMonSkillList)->pMonSkillAsIns);
				SAFE_DELETE(GetData(pMonSkillList));
				pMonSkillList->Delete();
				//	MoveNext(pMonSkillList);
			}
		}
		SAFE_DELETE(pMonSkillList)

			//	message
			if(pMonMsgList)
			{
				MoveHead(pMonMsgList);
				MoveNext(pMonMsgList);
				while(NotEndList(pMonMsgList))
				{
					SAFE_DELETE(GetData(pMonMsgList));
					pMonMsgList->Delete();
				}
			}
			SAFE_DELETE(pMonMsgList)



				//	필드 UI
				SAFE_DELETE(pFieldUi);

			//	기타
			SAFE_DELETE(pMonData);
			SAFE_DELETE(m_nSeedData);
			SAFE_DELETE(m_nSeedDataGrade);

			for(int i=0; i<MAX_SOUND; i++)
			{
				SUTIL_Sound_Free(i);
			}

			//	씨네마
			if(pCinema)
			{
				pCinema->Release_Cinematics();
			}
			SAFE_DELETE(pCinema);
}

//--------------------------------------------------------------------------
void GFieldBattle::Process()
//--------------------------------------------------------------------------
{
	if(b_WorldMap){return;}//월드맵 그리기
		
	

	if(UpdatePrologue())	{return;}

	//	이벤트는 분석한다.
	Analysis_Event();
	if(isHoldingProcess)	{return;}



	if(true == pCinema->m_IsPlayCinema)
	{
		pCinema->Update_Cinematics();
		return;
	}

	int loop = 0;

	if(b_PopupUi)
	{
		return;
	}
	else if(hero->_m_actNum == HERO_DOWNED_2 && hero->s_Status.LIFE<=0){//주인공 사망
		b_PopupUi = true;
		pPopupUi->GameOver = true;
		pField->workPal(false,0,0);
	}

	//	if(b_ChangeStage)
	//	{
	//		LoadStage();
	//		return;
	//	}

	ProcessTimerEvent();

	//	메세지를 분석한다.
	Analysis_Mon_Message();

	//	//	주인공의 정보를 몬스터에게 넘겨주기위한 데이터를 만든다.
	//	m_CharInfo->m_nPos.x = hero->_ins_Hero->m_posX - hero->_ins_Hero->m_stopAniX;
	//	m_CharInfo->m_nPos.y = hero->_ins_Hero->m_posY;
	//	m_CharInfo->m_nPos.z = hero->_ins_Hero->m_posZ - hero->_ins_Hero->m_stopAniZ;
	//	m_CharInfo->m_Direction = hero->_b_LookRight;

	//	//	몬스터가 3마리 이하면 추가시켜준다.
	//	if(nGenCnt > GetNodeCount(pMonList))
	//	{
	//		AddMonster(SUTIL_GetRandom()%6);
	//		AddMonster(0);
	//		AddRegenMonSetting();
	//	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////// 충돌체크	//////////////////////////////////////////////////////
	////////////////////////////////////////////////////// 충돌체크	//////////////////////////////////////////////////////
	LIST__START(pMonList)//히어로가 몬스터를때리는경우
//		if(MON_AC_DIE != GetData(pMonList)->m_ActState &&
//			MON_ATK_DEAD_ATTACK != GetData(pMonList)->m_ActState &&
//			MON_AC_DIE_AFTER != GetData(pMonList)->m_ActState)
		if(!(GetData(pMonList)->m_nFeature & FE_DONT_TOUCH))
		{
			if(Contact_Check(hero->_ins_Hero, GetData(pMonList)->pMonAsIns,NULL,20))
			{

				pFieldUi->AddCombo();	//	nams
				DamageSand_Hero(hero, GetData(pMonList));//주인공 평타
			}

			if(hero->s_Skill_Set.OVER_SkillEffect)
			{
				if(Contact_Check(hero->_ins_Skill[hero->s_Skill_Set.Num][0], GetData(pMonList)->pMonAsIns,NULL, GetData(pMonList)->m_nBodySize))
				{
					pFieldUi->AddCombo();	//	nams
					DamageSand_Hero(hero, GetData(pMonList));//주인공 스킬
				}
			}

			for(int i =0;i<3;i++){
				if(hero->s_Knife_Eff[i].act && hero->s_Knife_Eff[i].LVup_Eff_Ins)
					if(Contact_Check(hero->s_Knife_Eff[i].LVup_Eff_Ins, GetData(pMonList)->pMonAsIns,NULL,GetData(pMonList)->m_nBodySize))//충췍!!
					{
						pFieldUi->AddCombo();	//	nams
						DamageSand_Hero(hero, GetData(pMonList));//칼 버프 체크
					}
			}

			for(int i =0 ;i<4 ; i++){
				if(hero->s_Bullet_Eff[i].act && hero->s_Bullet_Eff[i].LVup_Eff_Ins)
				{
					if(Contact_Check(hero->s_Bullet_Eff[i].LVup_Eff_Ins, GetData(pMonList)->pMonAsIns,NULL,GetData(pMonList)->m_nBodySize))//충췍!!
					{
						pFieldUi->AddCombo();	//	nams
						DamageSand_Hero(hero, GetData(pMonList));//장풍 체크
						if(i == 0 || i == 1) hero->s_Bullet_Eff[i].LVupEff_Num = 100;//장풍사라짐
					}
				}
			}
		}
		LIST____END(pMonList)

			if(!(Exception_Num & PROCESS_MON_NOT)){//몬스터가 히어로를 때리는경우
				LIST__START(pMonList)

					if(Contact_Check(GetData(pMonList)->pMonAsIns,hero->_ins_Hero,NULL,14))//충췍!!
					{
						DamageSand_Mon(GetData(pMonList),hero);//몬스터 평타
						hero->RCV_Debuff( GetData(pMonList)->SND_Debuff(0) );
					}
					LIST____END(pMonList)

						LIST__START(pMonSkillList)
						if(1 > GetData(pMonSkillList)->Delay)
						{
							if(0 < GetData(pMonSkillList)->damagetime)
							{
								GetData(pMonSkillList)->pMonSkillAsIns->m_posX += (GetData(pMonSkillList)->Movex);
								GetData(pMonSkillList)->pMonSkillAsIns->m_posY += (GetData(pMonSkillList)->Movey);
								GetData(pMonSkillList)->pMonSkillAsIns->m_posZ += (GetData(pMonSkillList)->Movez);

								if(Contact_Check(GetData(pMonSkillList)->pMonSkillAsIns,hero->_ins_Hero,NULL,14))//충췍!!
								{
									GetData(pMonSkillList)->damagetime--;
									DamageSand_Mon((Monster*)(GetData(pMonSkillList)->who),hero);//몬스터 스킬 
									if(SKILL_REMOVE == GetData(pMonSkillList)->type)GetData(pMonSkillList)->lifeTime = 0;
								}
							}
						}
						LIST____END(pMonSkillList)

			}
			////////////////////////////////////////////////////// 충돌체크	//////////////////////////////////////////////////////
			////////////////////////////////////////////////////// 충돌체크	//////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			//	주인공
			hero->Process();

			//	주인공의 정보를 몬스터에게 넘겨주기위한 데이터를 만든다.
			m_CharInfo->m_nPos.x = hero->_ins_Hero->m_posX;
			m_CharInfo->m_nPos.y = hero->_ins_Hero->m_posY;
			m_CharInfo->m_nPos.z = hero->_ins_Hero->m_posZ;
			m_CharInfo->m_Direction = hero->_b_LookRight;

			//	몬스터
			if(!(Exception_Num & PROCESS_MON_NOT)){
				MoveHead(pMonList);
				MoveNext(pMonList);
				while(NotEndList(pMonList))
				{
					GetData(pMonList)->Process(m_CharInfo);

					//	몬스터가 죽었을 경우
					if(MON_AC_DIE_AFTER == GetData(pMonList)->m_ActState && 0 == GetData(pMonList)->m_nSkillCount)
					{
						if(NOT_REGEN != GetData(pMonList)->m_nRegenIdx)
						{
							if(0 < m_nMonSeedCnt)
							{
								ADD_EVENT_MSG(EVT_ADD_MONSTER, 20, GetData(pMonList)->m_nRegenIdx);
							}
							//					m_nIsRegen[GetData(pMonList)->m_nRegenIdx] = 0;	//	재생 영역을 초기화시켜준다.
						}
						hero->Set_Exp(GetData(pMonList)->SND_Exp(hero->s_Status.LEVEL) );

						if(!(GetData(pMonList)->m_nFeature & FE_DONT_HAVE_ITEM))
						{
							ItemBag tmp = pPopupUi->MakeItem(GetData(pMonList)->m_nMonIdx , GetData(pMonList)->m_nLevel, hero->s_Status.SEX, pField->m_nSaveStageNum);
							if(0 != tmp.ITEM_KIND)
							{
								pField->InsertDropItem(tmp, GetData(pMonList)->pMonAsIns->m_posX, GetData(pMonList)->pMonAsIns->m_posY+15);
								//pField->InsertDropItem(tmp, 100, 200);
							}
						}

						//	팝업 UI
						SAFE_DELETE(GetData(pMonList));
						pMonList->Delete();
						continue;
					}

					MoveNext(pMonList);
				}

				//	몬스터 스킬
				MoveHead(pMonSkillList);
				MoveNext(pMonSkillList);
				while(NotEndList(pMonSkillList))
				{
					if(1 > GetData(pMonSkillList)->lifeTime)
					{
						SUTIL_FreeSpriteInstance(GetData(pMonSkillList)->pMonSkillAsIns);
						((Monster*)(GetData(pMonSkillList)->who))->m_nSkillCount--;
						SAFE_DELETE(GetData(pMonSkillList));
						pMonSkillList->Delete();
						continue;
					}

					if(1 > GetData(pMonSkillList)->Delay)
					{
						SUTIL_UpdateTimeAsprite(GetData(pMonSkillList)->pMonSkillAsIns);
						GetData(pMonSkillList)->lifeTime--;
					}
					else	{GetData(pMonSkillList)->Delay--;}

					MoveNext(pMonSkillList);
				}
			}

			SetCameraMove();
			pField->Process();
			EventProcess();

			//	필드 UI
			pFieldUi->Process();

			//	화면 스크롤 셋팅

			int camera_x = -(pField->m_nSrcCamAngle_X);
			hero->_ins_Hero->setCamera(camera_x,0);
			for(int xx = 0;xx<5;xx++){//이펙트에 카메라좌표 적용
				if(hero->s_Skill.Equip_A[xx]>=0){
					hero->_ins_Skill[xx][0]->setCamera(camera_x,0);
					hero->_ins_Skill[xx][1]->setCamera(camera_x,0);
				}
			}

			//	몬스터
			MoveHead(pMonList);
			MoveNext(pMonList);
			while(NotEndList(pMonList))
			{
				GetData(pMonList)->pMonAsIns->setCamera(camera_x,0);
				MoveNext(pMonList);
			}

			//	몬스터 스킬
			MoveHead(pMonSkillList);
			MoveNext(pMonSkillList);
			while(NotEndList(pMonSkillList))
			{
				GetData(pMonSkillList)->pMonSkillAsIns->setCamera(camera_x,0);
				MoveNext(pMonSkillList);
			}

			//hero->_m_mapX = ( pField->getRelativityX());  // 이함수를 이용하여 구한 좌표를 캐릭터나 몬스터를 그릴때 더해준다 
			//pField->getMapSizeX();  //맵의 길이를 리턴한다 
			//	필드 이펙트
			MoveHead(pFieldUi->pEftList);
			MoveNext(pFieldUi->pEftList);
			while(NotEndList(pFieldUi->pEftList))
			{
				GetData(pFieldUi->pEftList)->pSkillEftAsIns->setCamera(camera_x,0);
				MoveNext(pFieldUi->pEftList);
			}

			pFieldUi->m_pFieldUiDamageNumAsIns->setCamera(camera_x,0);

			//모든 프로세스가 종료된 후에 주인공케릭터의 좌표를 보정한다.
			MoveHead(pMonList);
			MoveNext(pMonList);
			while(NotEndList(pMonList))
			{
//				if((MON_AC_DIE != GetData(pMonList)->m_ActState) &&
//					(MON_ATK_DEAD_ATTACK != GetData(pMonList)->m_ActState) &&
//					(MON_AC_DIE_AFTER != GetData(pMonList)->m_ActState) &&
//					(true == GetData(pMonList)->m_bIsCheckBody) )

				if(!(GetData(pMonList)->m_nFeature & FE_DONT_TOUCH))
				{
					if(!hero->s_Skill_Set.OVER_SkillEffect)
					{
						Contact_Body_Check(hero->_ins_Hero, GetData(pMonList)->pMonAsIns,10,10,true);//충돌시 좌표보정
					}
				}
				MoveNext(pMonList);
			}


			MoveHead(pMonList);
			MoveNext(pMonList);
			int x_max = pField->m_nFieldSize_X;
			while(NotEndList(pMonList))
			{
//				if((MON_AC_DIE != GetData(pMonList)->m_ActState) &&
//					MON_ATK_DEAD_ATTACK != GetData(pMonList)->m_ActState &&
//					(MON_AC_DIE_AFTER != GetData(pMonList)->m_ActState))
				if(!(GetData(pMonList)->m_nFeature & FE_DONT_AREA_CHECK))
				{
//					if(!hero->s_Skill_Set.OVER_SkillEffect)
//					{
						Area_Check(GetData(pMonList)->pMonAsIns,x_max);//충돌시 좌표보정

//						if(1)
//						{
//							int a = GetData(pMonList)->pMonAsIns->m_posX - GetData(pMonList)->pMonAsIns->m_stopAniX;
//							int b = GetData(pMonList)->m_nPos.x
//						}

						//	몬스터는 좌표가 따로 관리된다.
//						GetData(pMonList)->m_nPos.x = GetData(pMonList)->pMonAsIns->m_posX;
//					}
				}
				MoveNext(pMonList);
			}

			///////////////////////////////////////////////////////////////////////////
			//	필드 충돌체크

			///////////////////////////////////////////////////////////////////////////

//			for(InitList(pField->pLayerList);NotEndList(pField->pLayerList);MoveNext(pField->pLayerList))
//			{
//				for(InitList(GetData(pField->pLayerList)->pObjectList);
//					NotEndList(GetData(pField->pLayerList)->pObjectList);
//					MoveNext(GetData(pField->pLayerList)->pObjectList))
//				{
//					Contact_Body_Check(hero->_ins_Hero, GetData(GetData(pField->pLayerList)->pObjectList)->pAsIns,10,10, true);//충돌시 좌표보정
//				}
//			}

///////////////////////////////////////////////////////////////////////////////////////////////////////

			if(1)
			{
				Monster* pTmpMonster = NULL;
				for(InitList(pMonList);NotEndList(pMonList);MoveNext(pMonList))
				{
					pMonList->SaveCurrentState();
					pTmpMonster = GetData(pMonList);

					if(!(pTmpMonster->m_nFeature & (FE_DONT_TOUCH|FE_DONT_HAVE_DOWN)))
					{
						for(InitList(pMonList);NotEndList(pMonList);MoveNext(pMonList))
						{
							if(GetData(pMonList) != pTmpMonster)
							{
								Contact_Body_Check(pTmpMonster->pMonAsIns, GetData(pMonList)->pMonAsIns,10,10,false);//충돌시 좌표보정
							}
						}
					}

					pMonList->RestoreCurrentState();
				}
			}
///////////////////////////////////////////////////////////////////////////////////////////////////////




			//	맵에 대한 충돌체크
			for(InitList(pField->pDontgoRect);NotEndList(pField->pDontgoRect);MoveNext(pField->pDontgoRect))
			{
				Contact_Map_Check(hero->_ins_Hero, 10,
					GetData(pField->pDontgoRect)->sx,  GetData(pField->pDontgoRect)->sy,
					GetData(pField->pDontgoRect)->dx,  GetData(pField->pDontgoRect)->dy, true);
			}

			//	NPC
			for(InitList(pField->pKeyInputEvt);NotEndList(pField->pKeyInputEvt);MoveNext(pField->pKeyInputEvt))
			{
				switch( GetData(pField->pKeyInputEvt)->m_nType )
				{
				case EVT_NPC:
					//-------------------------------------------------------
					{
						Npc* tmp = (Npc*)GetData(pField->pKeyInputEvt);
						Contact_Body_Check(hero->_ins_Hero, tmp->m_Asins, 10, 10, true);//충돌시 좌표보정
						break;
					}
				}
			}

			int Hero_XY = Area_Check(hero->_ins_Hero,x_max);


			if(hero->s_Skill_Set.act){
				if(hero->s_Skill_Set.act && hero->s_Skill_Set.Skill_ID[hero->s_Skill_Set.Num] == 11){//예외스킬
					hero->_ins_Skill[hero->s_Skill_Set.Num][1]->m_posX = hero->_ins_Skill[hero->s_Skill_Set.Num][0]->m_posX = hero->_ins_Hero->m_posX;
					hero->_ins_Skill[hero->s_Skill_Set.Num][1]->m_posY = hero->_ins_Skill[hero->s_Skill_Set.Num][0]->m_posY = hero->_ins_Hero->m_posY; 
				}else{
					hero->_ins_Skill[hero->s_Skill_Set.Num][1]->m_posX += (Hero_XY%100000);
					hero->_ins_Skill[hero->s_Skill_Set.Num][0]->m_posX += (Hero_XY%100000);
					hero->_ins_Skill[hero->s_Skill_Set.Num][1]->m_posY += (Hero_XY/100000);
					hero->_ins_Skill[hero->s_Skill_Set.Num][0]->m_posY += (Hero_XY/100000);
				}
			}


			if(SUTIL_SOUND_GetState() == -1){//쿨타임이 지난뒤에 BGM출력
				if(m_BGM_Cooltime >SOUND_COOLTIME){
					//m_BGM_Cooltime=0; SUTIL_Sound_Play(BGM_TEST_A,true);
				}else{
					m_BGM_Cooltime ++;
				}
			}else{
				m_BGM_Cooltime=0;
			}
}


//--------------------------------------------------------------------------
void GFieldBattle::Paint_ChangeScrollEvent(int type)
//--------------------------------------------------------------------------
{
	switch(type)
	{
	case 0:
		{
			_SUTIL->g->blandBlur(); 
			break;
		}
	case 1:
		{
			_SUTIL->g->blandBlur(); 
			break;
		}
	}
}


//--------------------------------------------------------------------------
void GFieldBattle::Paint()
//--------------------------------------------------------------------------
{
	
	if(b_WorldMap){//월드맵 그리기
		
		pWorldMapUi->Paint();
		if(pWorldMapUi->b_ViewWorldMap == false){//월드맵 창이 닫히면
			b_WorldMap = false;
			SAFE_DELETE(pWorldMapUi);
		}
		return;
	}

	if(PaintPrologue())	{return;}

	m_nLodingtime = (m_nLodingtime+1)%3;

	if(0 < m_nFadeOutTime)
	{
		m_nFadeOutTime--;
		Paint_ChangeScrollEvent(0);


		PAINT_LODING();
		//		SUTIL_SetColor(0xff0000);
		//		SUTIL_FillRect(30*m_nLodingtime,20,10,10);

		return;
	}

	//	SUTIL_SetColor(0xff0000);
	//	SUTIL_FillRect(30*m_nLodingtime,20,10,10);

	PAINT_LODING();

	if(isHoldingPaint)	{return;}

	//	SUTIL_SetColor(0x000000);
	//	SUTIL_FillRect(0,0,240,320);
	//SUTIL_PaintAsprite(__ins_Hero);

	//	주인공



	if(true == pCinema->m_IsPlayCinema)
	{
		pCinema->Paint_Cinematics();

		//	화면을 갱신시켜준다.
		SUTIL_UpdateSprite();

		if(pFieldUi->s_TalkBox.View){//대화창이뜰때
			pFieldUi->PaintTalkBox();
		}

		pPopupUi->PaintMessage();//이벤트가 부여되었을때

		//아이템 습득 리스트
		pPopupUi->PaintItemList();

		//	화면 전환시 자연스럽게 움직여준다.
		if(0 < m_nFadeInTime)
		{
			m_nFadeInTime--;

			for(int loop = 0; loop < (m_nFadeInTime); loop++)
			{
				Paint_ChangeScrollEvent(1);
			}
		}

#ifdef LOOK_Debug
		PaintDebug();
#endif

		return;
	}
	//hero->Paint();SUTIL_UpdateSprite();return;

	Paint_Exception_Check();//그리기에 대해서 몇가지 특수한 상황을 처리한다
	//	배경
	pField->Paint();

	if(BOSS_STAND_FACESIDE != BossMon5_1::m_nBossStep)
	{
		pField->MiddlePaint();
	}


	//	필드의 작은 맵화면이라면 리턴시킨다.
	if(1000 <= pField->m_nSaveStageNum)
	{
		//	화면 전환시 자연스럽게 움직여준다.
		if(0 < m_nFadeInTime)
		{
			m_nFadeInTime--;

			for(int loop = 0; loop < (m_nFadeInTime); loop++)
			{
				Paint_ChangeScrollEvent(1);
			}
		}
		return;
	}

	if(!(Exception_Num & PAINT_HERO_NOT)){
		hero->Paint();
	}

	//	몬스터
	if(!(Exception_Num & PAINT_MON_NOT)){
		MoveHead(pMonList);
		MoveNext(pMonList);
		while(NotEndList(pMonList))
		{
			GetData(pMonList)->Paint();
			MoveNext(pMonList);
		}


		//	몬스터 스킬
		MoveHead(pMonSkillList);
		MoveNext(pMonSkillList);
		while(NotEndList(pMonSkillList))
		{
			if(1 > GetData(pMonSkillList)->Delay)
			{
				SUTIL_PaintAsprite(GetData(pMonSkillList)->pMonSkillAsIns,S_INCLUDE_SORT);
			}
			MoveNext(pMonSkillList);
		}
	}

	//	필드를 나누어서 부른다.(boss5 예외처리)
	if(BOSS_STAND_FACESIDE == BossMon5_1::m_nBossStep)
	{
		pField->MiddlePaint();
	}
	

	//그리기 갱신
	SUTIL_UpdateSprite();

	if(!(Exception_Num & PAINT_FIELD_FRONT_NOT)){
		//	배경2 주인공 앞부분 위치

		pField->FrontPaint();
	}


	//	필드 UI ( 그리기가 끝난후 위에 덮어준다.)
	pFieldUi->Paint();

	pFieldUi->PaintCharInfo(hero->s_Status.LEVEL,
		hero->s_Status.LIFE,
		hero->s_Status.LIFE_MAX,
		hero->s_Status.MANA,
		hero->s_Status.MANA_MAX,
		hero->s_Status.ELEMENTAL );

	//	미니맵 임시
	PaintMiniMap();

	//아이템 습득 리스트
	pPopupUi->PaintItemList();

	//pFieldUi->SetTalkCinema(0);
	if(pFieldUi->s_TalkBox.View){//대화창이뜰때
		pFieldUi->PaintTalkBox();
	}else{
		pFieldUi->PaintSkillInfo(&(hero->s_Skill_Set.Skill_ID[0]),
			&(hero->s_Skill_Set.Cool_TimeMax[0]),
			&(hero->s_Skill_Set.Cool_TimeNow[0]),
			hero->s_Status.MANA,
			&(hero->s_Skill_Set.Need_Mana[0]));
		pFieldUi->PaintExpInfo(hero->s_Status.EXP,
			hero->s_Status.EXP_MAX);
	}


	if(b_PopupUi){//CLR 팝업이 뜰때 
		_SUTIL->g->blandBlur(); 
		pPopupUi->Paint();//
	}

	pPopupUi->PaintAreaMessage();//지역명을 표시할때
	pPopupUi->PaintMessage();//가방 공간부족할때


	//	화면 전환시 자연스럽게 움직여준다.
	if(0 < m_nFadeInTime)
	{
		m_nFadeInTime--;

		for(int loop = 0; loop < (m_nFadeInTime); loop++)
		{
			Paint_ChangeScrollEvent(1);
		}
	}


#ifdef LOOK_Debug
	PaintDebug();
#endif
}


#ifdef LOOK_Debug
void GFieldBattle::PaintDebug()
//--------------------------------------------------------------------------
{
	///////////////////각종 게임 상태 출력부//////////////////////
	///////////////////각종 게임 상태 출력부//////////////////////
	///////////////////각종 게임 상태 출력부//////////////////////
#define X_POSITION 150
#define Y_POSITION 29

#ifdef LOOK_USE_MEMORY
	char str[30] ; 
	int h = Y_POSITION+10;
	int a = 0;
	_SUTIL->g->SetColor( 0xffffff );
	a = MC_knlGetTotalMemory();
	_SUTIL->g->DrawRect(X_POSITION+0, h+0, a/(100*1024), 5);

	_SUTIL->g->SetColor( 0xff0000 );
	a = MC_knlGetTotalMemory() - MC_knlGetFreeMemory();
	_SUTIL->g->FillRect(X_POSITION+1, h+1, a/(100*1024), 4);

	_SUTIL->g->SetColor( 0x00ff00 );
	//a = MC_knlGetFreeMemory();
	//g->FillRect(1, 1, a/(100*1024), 2);
	_SUTIL->g->FillRect(X_POSITION+33, h+2, 1, 2);

	_SUTIL->g->SetColor( 0xFFffFF );
	_SUTIL->g->FillRect(X_POSITION+10, h+6, 1, 1);
	_SUTIL->g->FillRect(X_POSITION+20, h+6, 1, 1);
	_SUTIL->g->FillRect(X_POSITION+30, h+6, 1, 1);
	_SUTIL->g->FillRect(X_POSITION+40, h+6, 1, 1);
	_SUTIL->g->FillRect(X_POSITION+50, h+6, 1, 2);


	_SUTIL->pFont->setColor(0xff0000);
	_SUTIL->pFont->setOutline(true);
	_SUTIL->pFont->setOutlineColor(0x000000);

	SPRINTF(str, "USE_M : %d", a);
	_SUTIL->pFont->DrawText(_SUTIL->g, str,X_POSITION+ 0 , Y_POSITION, 0);
	_SUTIL->pFont->setColor(0x00FF00); 
	_SUTIL->pFont->DrawText(_SUTIL->g, ".",X_POSITION+ 66 , Y_POSITION-8 , 0); 
	_SUTIL->pFont->DrawText(_SUTIL->g, ".",X_POSITION+ 51 , Y_POSITION-8 , 0); 
#endif

#ifdef LOOK_FRM_PER_SEC
	long TimeNow = MC_knlCurrentTime();
	int tempTime = 10000 / ((TimeNow+1) - TimeLast);
	TimeLast = MC_knlCurrentTime();

	_SUTIL->pFont->setColor(0xff7777); 

	SPRINTF(str, "frm^2/^0sec : %d.%d", tempTime/10,tempTime%10); 
	_SUTIL->pFont->DrawText(_SUTIL->g, str,X_POSITION+ 0, Y_POSITION+20, 0);

	MEMSET(str, 0x00, 30);
	SPRINTF(str, "stage : %d", pField->m_nSaveStageNum); 
	_SUTIL->pFont->DrawText(_SUTIL->g, str,X_POSITION+ 0, Y_POSITION+30, 0);

	MEMSET(str, 0x00, 30);
	SPRINTF(str, "MCpos : %d.%d", m_CharInfo->m_nPos.x ,m_CharInfo->m_nPos.y); 
	_SUTIL->pFont->DrawText(_SUTIL->g, str,X_POSITION+ 0, Y_POSITION+40, 0);
#endif


	//////////////////////////////////////////////////////////////////
	//	MoveHead(_SUTIL->pDrawList);
	//	MoveNext(_SUTIL->pDrawList);
	//	while(NotEndList(_SUTIL->pDrawList))
	//	{
	//		if(NULL  ==  GetData(_SUTIL->pDrawList)->m_rect)
	//		{
	//			int a = 0;
	//		}
	//		MoveNext(_SUTIL->pDrawList);
	//	}
	//
	//////////////////////////////////////////////////////////////////


#ifdef LOOK_EVENT_RECT
	_SUTIL->g->DrawRect(hero->s_Event_Rect.X1-pField->m_nSrcCamAngle_X,hero->s_Event_Rect.Y1,
		hero->s_Event_Rect.W,hero->s_Event_Rect.H);

	/////////////////////////////////////////////////////////////////
	for(InitList(pField->pEvtWayList);NotEndList(pField->pEvtWayList);MoveNext(pField->pEvtWayList))
	{
		_SUTIL->g->DrawRect(GetData(pField->pEvtWayList)->sx-pField->m_nSrcCamAngle_X, GetData(pField->pEvtWayList)->sy,
			GetData(pField->pEvtWayList)->dx-GetData(pField->pEvtWayList)->sx,
			GetData(pField->pEvtWayList)->dy-GetData(pField->pEvtWayList)->sy);
	}
	/////////////////////////////////////////////////////////////////

#endif

#ifdef	LOOK_DONT_GO_RECT
	_SUTIL->g->SetColor( 0x00ff00 );
	for(InitList(pField->pDontgoRect);NotEndList(pField->pDontgoRect);MoveNext(pField->pDontgoRect))
	{

		_SUTIL->g->DrawRect(GetData(pField->pDontgoRect)->sx - pField->m_nSrcCamAngle_X,
			GetData(pField->pDontgoRect)->sy,
			GetData(pField->pDontgoRect)->dx,
			GetData(pField->pDontgoRect)->dy);
	}

#endif
	//	TEST
	//	_SUTIL->pFont->setOutline(TRUE);
	//	if(10001 == pField->m_nSaveStageNum || 10000 == pField->m_nSaveStageNum || 10002 == pField->m_nSaveStageNum)
	//	{
	//		_SUTIL->pFont->setColor(0xFFFFFF);
	//		_SUTIL->pFont->setOutlineColor(0x000000);
	//		_SUTIL->pFont->DrawText(_SUTIL->g, (char*)"SMALL TOWN", 10, 45, 0);//Text
	//	}
	//	
	//	if(10003 == pField->m_nSaveStageNum || 10004 == pField->m_nSaveStageNum || 10005 == pField->m_nSaveStageNum)
	//	{
	//		_SUTIL->pFont->setColor(0xFFFFFF);
	//		_SUTIL->pFont->setOutlineColor(0x000000);
	//		_SUTIL->pFont->DrawText(_SUTIL->g, (char*)"MIDDLE TOWN", 10, 45, 0);//Text
	//	}
	//	_SUTIL->pFont->setOutline(FALSE);
}
#endif

void GFieldBattle::GetHomingXY(Homing* p_Homing){

	int Xx = 0 , Yy = 0 , Cc = 0;//중간좌표
	int CitaTemp = 0;

	p_Homing->Cita2 = GetAngle(p_Homing->X2 - p_Homing->X1, p_Homing->Y2 - p_Homing->Y1);//현재각도 구하기
	if(ABS(p_Homing->Cita1 - p_Homing->Cita2) >180){//둔각일 경우 작은각에 360도를 더해서 평균값을 구할수있게한다
		if(p_Homing->Cita1 < p_Homing->Cita2)
			p_Homing->Cita1+=360;
		else
			p_Homing->Cita2+=360;

		//	CitaTemp = (p_Homing->Cita1 + p_Homing->Cita2)/2;
		//}else{//둔각은 180도 반대쪽 각을 체크해야한다
		//	CitaTemp = ((p_Homing->Cita1 + p_Homing->Cita2)/2 +180 )%360;
	}
	CitaTemp = (p_Homing->Cita1+p_Homing->Cita2)/2;

	if(p_Homing->TurnCita < ABS(p_Homing->Cita1 - CitaTemp)){//최대 턴 각도 보다 많이 휘어지면 보정한다
		if(p_Homing->Cita1 > CitaTemp)
			p_Homing->Cita1-=p_Homing->TurnCita;
		else
			p_Homing->Cita1+=p_Homing->TurnCita;

	}else{
		p_Homing->Cita1 = CitaTemp;
	}

	p_Homing->Cita1 = p_Homing->Cita1%360;

	p_Homing->X1 = p_Homing->X1 + p_Homing->R*SIN(p_Homing->Cita1)/1000;
	p_Homing->Y1 = p_Homing->Y1 + p_Homing->R*(-COS(p_Homing->Cita1))/1000; 

}

int GFieldBattle::GetAngle(int rx,int ry){
	if(rx==0) return(ry<0?0:180);
	if(ry==0) return(rx<0?270:90);

	int X = 0, Y = 0, AddAngle =0;
	if (rx > 0){
		if (ry > 0){
			//2사분면
			X = rx;
			Y = ry;
			AddAngle = 90;
		}else{
			//1사분면
			X = -ry;
			Y = rx;
			AddAngle = 0;
		}
	}else{
		if (ry > 0){
			//3사분면
			X = ry;
			Y = -rx;
			AddAngle = 180;
		}else{
			//4사분면
			X = -rx;
			Y = -ry;
			AddAngle = 270;
		}
	}


	//2사분면 기준
	if (X > Y){//분모가 크다면
		int dig = (Y*1000)/X;
		for(int i = 0 , h = 0,h2 =2000 ; i<=45 ;i++){
			h = dig - TAN(i);
			//h = ABS(h);
			if(h<=0)
				return AddAngle+i;
		}
	}else{//분자가 크다면 90도에서 역좌표
		int dig = (X*1000)/Y;
		for(int i = 0 , h = 0,h2 =2000 ; i<=45 ;i++){
			h = dig - TAN(i);
			//h = ABS(h);
			if(h<=0)
				return AddAngle + 90 - i;
		}


	}

	return 0;
}




void GFieldBattle::drawOverlab()
{
	if( overlabCnt == OVERLAB_STOP)
		return; 
	int screenArea = SCREEN_WIDTH*SCREEN_HEIGHT; 
	short* backBuf = (short*)_SUTIL->g->m_image->GetPixelData(); 

	if( copyBackBuf == NULL)
	{
		copyBackBuf = (short*)MALLOC(sizeof(short)*screenArea); 
		memcpy(copyBackBuf, backBuf, sizeof(short)*screenArea);
		overlabCnt = 0; 
	}

	overlabCnt+=1; 
	int overlabCntRevers = OVERLAB_BLAND_MAX - overlabCnt;
	if( overlabCnt > OVERLAB_BLAND_MAX)
	{	overlabCnt = OVERLAB_STOP; 
	SAFE_DEL(copyBackBuf);
	return; 
	}

	for( int i =0; i< screenArea; i++)
	{	 *(backBuf+i) = ((((*(backBuf+i)&0xF81f)*overlabCnt + (*(copyBackBuf+i)&0xF81f)*overlabCntRevers)>> OVERLAB_BLAND_MOVE_BIT) & 0xF81f) 
	| ((((*(backBuf+i)&0x7E0)*overlabCnt + (*(copyBackBuf+i)&0x7E0)*overlabCntRevers) >> OVERLAB_BLAND_MOVE_BIT) & 0x7E0);
	}
}

void GFieldBattle::setGamma(int gammaCase)
{	
	int screenArea = SCREEN_WIDTH*SCREEN_HEIGHT; 
	short* backBuf = (short*)_SUTIL->g->m_image->GetPixelData(); 

	short xorBit[] = 
	{
		0xffff,	//all revers
		0xF800,	//R		red
		0x7E0,  //G		green
		0x1F,   //B		blue
		0xF81f,	//RB	pink
		0xFFE0,	//RG	yellow
		0x7FF,  //GB	aqua
	}; 

	for( int i =0; i< screenArea; i++)
		*(backBuf+i) = xorBit[gammaCase]^(*(backBuf+i));

}
//--------------------------------------------------------------------------
void GFieldBattle::KeyEvent(int m_keyCode, int m_keyRepeat)
//--------------------------------------------------------------------------
{
	if(b_WorldMap){
		pWorldMapUi->KeyEvent(m_keyCode, m_keyRepeat);
		return;
	}

	if(isHoldingKey)	{return;}

	if(true == pField->KeyEvent(m_keyCode, m_keyRepeat))	{return;}

	///////////////////////////////////게임중 텍스트창 호출///////////////////////////////////////
	if(pFieldUi->s_TalkBox.View){//s_TalkBox가 뜰때 
		pFieldUi->KeyTalkBox(m_keyCode, m_keyRepeat);
		hero->KeyEvent(0,0);//케릭터의 행동을 정지시켜주기위한 용도
		return;
	}
	///////////////////////////////////게임중 텍스트창 호출///////////////////////////////////////

	//	씨네마중이면 리턴
	if(true == pCinema->m_IsPlayCinema)	{return;}

	///////////////////////////////////게임중 팝업 호출///////////////////////////////////////
	if(m_keyCode == MH_KEY_CLEAR && !m_keyRepeat && !b_PopupUi){
		if(hero->_move_Order == HERO_STOP){
			b_PopupUi = true;
			if(hero->s_Knife_Eff[3].act)hero->s_Knife_Eff[3].LVup_ActionEnd = true;//검버프는꼭 해지해야한다
		}
	}else if(b_PopupUi){
		b_PopupUi = pPopupUi->KeyEvent(m_keyCode, m_keyRepeat);

		if(!b_PopupUi){//팝업창이 닫힐때
			if(pPopupUi->SkillChange_A){
				hero->Init_Skill();
				pPopupUi->SkillChange_A=false;
			}
		}
	}else
		///////////////////////////////////게임중 팝업 호출///////////////////////////////////////


	{

		switch(m_keyCode)
		{
		case MH_KEY_5:
		case MH_KEY_SELECT:
			{
				//	이벤트에서 키에 관련된 일은 이곳에서 처리

				//	이벤트가 일어나면 그냥 키를 끝낸다.
				if(!m_keyRepeat)
				{
					if(KeyEventProcess())	{return;}
				}

				//ItemBag tmpBag = pField->CheckDropItem(m_CharInfo->m_nPos.x, m_CharInfo->m_nPos.y);
				ItemBag* tmpBag = pField->CheckDropItem(hero->_ins_Hero->m_posX,
					hero->_ins_Hero->m_posY);

				if(tmpBag){
					if(0 != tmpBag->ITEM_KIND)
					{
						if(true == pPopupUi->GET_item(tmpBag))
						{
							m_keyCode = 0;
							if(tmpBag->ITEM_ID>=8000){//아이템 이벤트 호출
								ADD_EVENT_MSG(EVT_GAME_KEYEVENT, 40, tmpBag->ITEM_ID);//Analysis_Event , CheckEvent
							}
							pField->DeleteNowItem();
						}else{
							pPopupUi->AddMessage(CLRMENU_Q5);
						}
					}
				}

				break;
			}
		case MH_KEY_SEND:
			{
				//Resv_ChangeStage(101);

				//SetEvent(4,0);
				//pField->m_nSrcCamAngle_X -= 3;
				//pMonList->CurNode()->ResvAction(MON_ATK_FIREBALL);

				//if(false == pCinema->s_cinematicsPause)
				//{
				//	pCinema->s_cinematicsPause = true;
				//}
				//else
				//{
				//	pCinema->s_cinematicsPause = false;
				//}
				//ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT, 0, 10);
				//ADD_EVENT_MSG(EVT_CHANGE_STAGE, 10, 100);
//				GET_MAINQUEST_IDX() = MAIN_QUEST_openDoor;

//				if(ischeck)
//				{
//					ischeck = false;
//				}
//				else
//				{
//					ischeck = true;
//				}

				//					ADD_EVENT_MSG(EVT_CHANGE_STAGE, 10, 100);



				//					LoadPrologue(DATA_TALK_OPENING, 0);


				break;
			}
		case MH_KEY_SOFT1:
			//Resv_ChangeStage(100);
			//ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT, 0, 10);
			//ADD_EVENT_MSG(EVT_START_CINEMA, 10, 5);

			//				SET_EVENT(1);
			//				PARSER_EVENT();
			break;
		case MH_KEY_SOFT2:
			{
				//pFieldUi->SetTalkCinema(0,1);//대사창 띄우기
				/*INIT_EVENT();
				SET_EVENT(2);
				PARSER_EVENT();*/
				//					pFieldUi->SetTalkCinema(0);//대사창 띄우기
				//
				//					
				//
				//					MoveHead(pMonList);
				//					MoveNext(pMonList);
				//					while(NotEndList(pMonList))
				//					{
				//						GetData(pMonList)->RCV_Debuff(DEBUF_POISON );
				//						MoveNext(pMonList);
				//					}
				break;
			}

		case MH_KEY_SIDE_UP:
			SUTIL_SOUND_SetVol(SUTIL_SOUND_GetVol()+10);
			break;
		case MH_KEY_SIDE_DOWN:
			SUTIL_SOUND_SetVol(SUTIL_SOUND_GetVol()-10);
			break;

			//case MH_KEY_SIDE_UP:
			//	break;
			//case MH_KEY_SIDE_DOWN:
			//	break;
		}




		hero->KeyEvent(m_keyCode, m_keyRepeat);
		Throw_Check();// 주인공의 잡기


	}


}

bool GFieldBattle::Contact_Check(ASpriteInstance* p_Attacker,ASpriteInstance* p_Defender,int attGap,int defGap)
{//충돌 체크 메소드 - Instance좌표에 Aspriteㅁ좌표의 "Flag 값에 따라" 합산한다, 따라서 별도의 계산이나 Flag 보정 전처리는 필요없다




	int att_Frame_Index = p_Attacker->GetAnimFrame();// Attacker 애니메이션의 프레임에 고유 Index 값을 얻어온다
	int def_Frame_Index = p_Defender->GetAnimFrame();// Defender 애니메이션의 프레임에 고유 Index 값을 얻어온다


	int aa = p_Attacker->m_sprite->_frames_rects_start[att_Frame_Index+1] - p_Attacker->m_sprite->_frames_rects_start[att_Frame_Index];
	int bb = p_Defender->m_sprite->_frames_rects_start[def_Frame_Index+1] - p_Defender->m_sprite->_frames_rects_start[def_Frame_Index];

	if( aa < 2 || bb < 1) //무결성체크 - 어택판정이 없거나 || 맞는판정이없거나
		return false;//판정하지 않는다 - def_Rect은 1번째에 기록되고, att_Rect 은 2번째부터 기록되어 있다, 둘중하나라도 없다면 F리턴


	//if( RECT_NUM(p_Attacker, att_Frame_Index) < 2 || RECT_NUM(p_Defender, def_Frame_Index) < 1) //무결성체크 - 어택판정이 없거나 || 맞는판정이없거나
	//	return false;//판정하지 않는다 - def_Rect은 1번째에 기록되고, att_Rect 은 2번째부터 기록되어 있다, 둘중하나라도 없다면 F리턴
	//
	short X_a, Y_a, X_d, Y_d;//충돌영역 XYWH
	byte W_a, H_a, W_d, H_d;//충돌영역 XYWH



	int att_Rect_Index = (p_Attacker->m_sprite->_frames_rects_start[att_Frame_Index]+1)*4;// Attacker 프레임의 시작지점 배열인덱스
	int def_Rect_Index = p_Defender->m_sprite->_frames_rects_start[def_Frame_Index]*4;// Defender 프레임의 시작지점 배열인덱스


	if(attGap==0){//attGap로 값이 넘어온게 없다면 새로 값을 가져와서 판단한다
		switch(WH(p_Attacker, att_Rect_Index+2)){
			case 0:attGap = 14;break;
			case 1:attGap = 24;break;
			case 2:attGap = 8;break;
			case 3:attGap = 36;break;
			case 4:attGap = 46;break;
			case 5:attGap = 68;break;
			case 6:attGap = 250;break;
		}
	}
	if(	(p_Attacker->m_posY +attGap/2	<	p_Defender->m_posY -defGap/2	) || 
		(p_Attacker->m_posY -attGap/2	>	p_Defender->m_posY +defGap/2	) )
		return false;//판정하지 않는다 - Y값이 겹치지 않으므로


	short ac_X_Att = p_Attacker->m_posX;						//Absolute coordinates 
	short ac_Y_Att = p_Attacker->m_posZ;					//Absolute coordinates 
	short ac_X_Def = p_Defender->m_posX;						//Absolute coordinates 
	short ac_Y_Def = p_Defender->m_posZ;					//Absolute coordinates 



	X_d =XY(p_Defender, def_Rect_Index  );
	Y_d =XY(p_Defender, def_Rect_Index+1);
	W_d =WH(p_Defender, def_Rect_Index+2);
	H_d =WH(p_Defender, def_Rect_Index+3);
	if(p_Defender->m_stopFlag & FLAG_FLIP_X) X_d = -(X_d+W_d);// 수비 애니 반전이면



	for(int xx = 0 ; xx <= (RECT_NUM(p_Attacker, att_Frame_Index)-2) ; xx++ , att_Rect_Index+=4){//공격측은 Rect가 1개 이상일 수 도 있다 - (RECT_NUM(p_Attacker, att_Frame_Index)-2):공격측 충돌영역 갯수

		if(xx==0){//x충격량,z충격량 저장하기 - 충돌영역의 2번째 Rect는 정보값을 저장한다
			//x충격량 - 맞는대상이 +일때는 밀려나가고 -일때는 당겨온다 단, 주인공의 뒤에 몬스터가 있다면 부호는 반대로 잡아준다
			s_Contact.x =		XY(p_Attacker, att_Rect_Index  );
			s_Contact.z =		XY(p_Attacker, att_Rect_Index+1);
			s_Contact.D_index = WH(p_Attacker, att_Rect_Index+3);

			//☆☆☆공격자 인스턴스 절대좌표와 수비자 인스턴스 절대좌표를 비교하여 X방향을 뒤집는다 
			//☆☆☆공격자가 보는 방향은 따지지 않는다
			if(ac_X_Att > ac_X_Def)s_Contact.x = -s_Contact.x;// 공격자 보다 더 뒤에있는 피격자는 물리X방향이 반대가 된다

		}else{

			/*X_a=XYWH(p_Attacker, att_Rect_Index  );
			Y_a=XYWH(p_Attacker, att_Rect_Index+1);
			W_a=XYWH(p_Attacker, att_Rect_Index+2);
			H_a=XYWH(p_Attacker, att_Rect_Index+3);*/

			X_a=p_Attacker->m_sprite->_frames_rects[att_Rect_Index];
			Y_a=p_Attacker->m_sprite->_frames_rects[att_Rect_Index+1];
			W_a=(byte)p_Attacker->m_sprite->_frames_rects[att_Rect_Index+2];
			H_a=(byte)p_Attacker->m_sprite->_frames_rects[att_Rect_Index+3];

			if(p_Attacker->m_stopFlag & FLAG_FLIP_X) X_a = -(X_a+W_a);// 공격 애니 반전이면
			if(Y_a ==0){
				int zz = true;
			}	


			if(
				(ac_X_Att+ X_a+W_a	> ac_X_Def+ X_d		) && 
				(ac_X_Att+ X_a		< ac_X_Def+ X_d+W_d	) && 
				(ac_Y_Att+ Y_a+H_a	> ac_Y_Def+ Y_d		) && 
				(ac_Y_Att+ Y_a		< ac_Y_Def+ Y_d+H_d	) ){//충돌판단 공식

					s_Contact.crash_x=(MAX(ac_X_Att+ X_a, ac_X_Def+ X_d) + MIN(ac_X_Att+ X_a+W_a, ac_X_Def+ X_d+W_d))>>1;//X충격 이펙트 지점 
					s_Contact.crash_z=(MAX(ac_Y_Att+ Y_a, ac_Y_Def+ Y_d) + MIN(ac_Y_Att+ Y_a+H_a, ac_Y_Def+ Y_d+H_d))>>1;//Z충격 이펙트 지점
					return true;//충돌 판단
			}
		}
	}

	return false;
}



bool GFieldBattle::Contact_Body_Check(ASpriteInstance* p_Attacker,ASpriteInstance* p_Defender,int attGap,int defGap,boolean HERO)
{//충돌 체크 메소드 - Instance좌표에 Asprite좌표의 "Flag 값에 따라" 합산한다, 따라서 별도의 계산이나 Flag 보정 전처리는 필요없다

	//	충돌체크가 없다면 리턴
	if(!p_Attacker->m_sprite->_frames_rects_start)	{return false;}
	if(!p_Defender->m_sprite->_frames_rects_start)	{return false;}

	//몸통간 안전거리 -5픽셀 추후구현 이부분이 작으면 작을수록 접근대상에 대해 뚫을 가능성이 커진다
	int Safe_range = 10;
	int tmpXZ[2];
	short X_a,W_a,	X_d,W_d;//충격체와 피격체의  XYWH

	//충격체 전좌표
	short Att_acX_before = p_Attacker->m_lastX;								//before Absolute coordinates 
	short Att_acY_before = p_Attacker->m_lastY;								//before Absolute coordinates 


	//충격체 현좌표
	short Att_acX = 0;
	short Att_acY = 0;
	int att_Frame_Index = 0;
	int att_Rect_Index = 0;
	if(0 == p_Attacker->m_nState)	//	에니메이션
	{
		p_Attacker->Get_AFrameXZ(&tmpXZ[0]);//이번에 그려질 Ani 의 x,z 좌표를 얻어온다
		Att_acX = tmpXZ[0] ;								//Absolute coordinates 
		Att_acY = p_Attacker->m_posY;				//Absolute coordinates 
		att_Frame_Index = p_Attacker->GetAnimFrame();// Attacker 애니메이션의 프레임에 고유 Index 값을 얻어온다
		att_Rect_Index = p_Attacker->m_sprite->_frames_rects_start[att_Frame_Index]*4;// AttackerBODY 프레임의 시작지점 배열인덱스
	}
	else if(1 == p_Attacker->m_nState)	//	프레임
	{
		Att_acX = p_Attacker->m_posX;								//Absolute coordinates 
		Att_acY = p_Attacker->m_posY;				//Absolute coordinates 
		att_Frame_Index = p_Attacker->m_nCrtFrame;
		att_Rect_Index = p_Attacker->m_sprite->_frames_rects_start[att_Frame_Index]*4;// AttackerBODY 프레임의 시작지점 배열인덱스
	}

	//피격체
	short Def_acX = 0;
	short Def_acY = 0;
	int def_Frame_Index = 0;
	int def_Rect_Index = 0;
	if(0 == p_Defender->m_nState)	//	에니메이션
	{
		p_Defender->Get_AFrameXZ(&tmpXZ[0]);//이번에 그려질 Ani 의 x,z 좌표를 얻어온다
		Def_acX = tmpXZ[0] ;								//Absolute coordinates 
		Def_acY = p_Defender->m_posY;				//Absolute coordinates 
		def_Frame_Index = p_Defender->GetAnimFrame();// Defender 애니메이션의 프레임에 고유 Index 값을 얻어온다	
		def_Rect_Index = p_Defender->m_sprite->_frames_rects_start[def_Frame_Index]*4;// DefenderBODY 프레임의 시작지점 배열인덱스
	}
	else if(1 == p_Defender->m_nState)	//	프레임
	{
		Def_acX = p_Defender->m_posX;								//Absolute coordinates 
		Def_acY = p_Defender->m_posY;				//Absolute coordinates 
		def_Frame_Index = p_Defender->m_nCrtFrame;
		def_Rect_Index = p_Defender->m_sprite->_frames_rects_start[def_Frame_Index]*4;// AttackerBODY 프레임의 시작지점 배열인덱스
	}

	//★.1////////////	현좌표와 피격체 중에 한쪽이라도 몸통이 없다면 충돌체크는 하지않는다.
	if( RECT_NUM(p_Attacker, att_Frame_Index) < 1 || RECT_NUM(p_Defender, def_Frame_Index) < 1) //무결성체크 - 양쪽중에 한쪽이라도 몸통이 없다면
		return false;//판정하지 않는다

	X_a =XY(p_Attacker, att_Rect_Index  );
	W_a =WH(p_Attacker, att_Rect_Index+2);
	if(p_Attacker->m_stopFlag & FLAG_FLIP_X) X_a = -(X_a+W_a);// 공격 애니 반전이면

	X_d =XY(p_Defender, def_Rect_Index  );
	W_d =WH(p_Defender, def_Rect_Index+2);
	if(p_Defender->m_stopFlag & FLAG_FLIP_X) X_d = -(X_d+W_d);// 수비 애니 반전이면


	//★.2////////////	전좌표가 영역 안에 있다면 충돌체크는 하지않는다.
	if	((Att_acY_before +attGap/2	>	Def_acY -defGap/2	) && 
		(Att_acY_before -attGap/2		<	Def_acY +defGap/2	)){//충격체 전좌표와 피격체의 Y 겹침

			if((Att_acX_before + (X_a+W_a)	>	Def_acX + (X_d) + Safe_range	) && 
				(Att_acX_before + (X_a) + Safe_range	<	Def_acX + (X_d+W_d)	)){
					return false;////전좌표가 이미 겹쳐 있을때는 충돌체크가 가동하지 않는다
			}

	}

	//★.3////////////	X합계영역, Y합계영역 안에 피격체가 존재하지않으면 충돌체크는 하지않는다.
	if ((MIN(Att_acX_before+(X_a), Att_acX+(X_a))			>= Def_acX + (X_d+W_d)	)||
		(MAX(Att_acX_before+(X_a+W_a), Att_acX+(X_a+W_a))	<= Def_acX + (X_d)		)||
		(MIN(Att_acY_before-attGap/2, Att_acY-attGap/2)		>= Def_acY + attGap/2	)||
		(MAX(Att_acY_before+attGap/2, Att_acY+attGap/2)		<= Def_acY - attGap/2	))
		return false;//어느한쪽이라도 벗어나면  충돌체크가 가동하지 않는다




	int overX_Length = 0;
	if(HERO && Att_acX_before == Att_acX){
	}else
		if(Att_acX_before < Def_acX){//★왼쪽으로 되돌아올 경우
			//if	(Att_acX	+ (X_a+W_a)	>	Def_acX +(X_d)		)//이동 좌표 사이에 장애물이 있다.
			if(Att_acX_before < Att_acX)//오른쪽으로 이동한것이 맞는가? - 겹쳐지다가 벗어난경우 걸릴수도 있음
				overX_Length = - ((Att_acX	+ (X_a+W_a)) - (Def_acX + (X_d)));
		}else{//★오른쪽으로 되돌아올 경우
			//if	(Att_acX	+ (X_a)		<	Def_acX +(X_d+W_d)	)//이동 좌표 사이에 장애물이 있다.
			if(Att_acX_before > Att_acX)//왼쪽으로 이동한것이 맞는가? - 겹쳐지다가 벗어난경우 걸릴수도 있음
				overX_Length = + ((Def_acX +(X_d+W_d)) - (Att_acX + (X_a)));
		}

		int overY_Length = 0;
		if(HERO && Att_acY_before == Att_acY){
		}else
			if(Att_acY_before < Def_acY){//★위로 되돌아올 경우
				//if	(Att_acY	+ attGap/2	>	Def_acY - defGap/2	)//이동 좌표 사이에 장애물이 있다.
				overY_Length = - ((Att_acY	+ attGap/2) - (Def_acY - defGap/2));

			}else{//★아래로 되돌아올 경우
				//if	(Att_acY	- attGap/2	<	Def_acY + defGap/2	)//이동 좌표 사이에 장애물이 있다.
				overY_Length = + ((Def_acY + defGap/2) - (Att_acY - attGap/2));
			}


			if(HERO){
				if( abs(overX_Length) > 0 ){// y축의 이동량이 크다면 x 를 반영한다.
					p_Attacker->m_posX += overX_Length;
					return true;
				}
				if( abs(overY_Length) > 0 ){// y축의 이동량이 크다면 x 를 반영한다.
					p_Attacker->m_posY += overY_Length;
					return true;
				}
			}else{//몬스터의 경우 배경과 비교할때 쓰임
				if( ABS(overX_Length) < ABS(overY_Length) ){// Y축의 이동량이 크다면 X 를 반영한다.
					p_Attacker->m_posX += overX_Length;
					return true;
				}else{// Y축의 이동량이 크다면 X 를 반영한다.
					p_Attacker->m_posY += overY_Length;
					return true;
				}
			}

			return true;
}



bool GFieldBattle::Contact_Map_Check(ASpriteInstance* p_Attacker,int attGap,int X,int Y,int W,int H, boolean HERO)
{//충돌 체크 메소드 - Instance좌표에 Asprite좌표의 "Flag 값에 따라" 합산한다, 따라서 별도의 계산이나 Flag 보정 전처리는 필요없다

	//	충돌체크가 없다면 리턴
	if(!p_Attacker->m_sprite->_frames_rects_start)	{return false;}

	//몸통간 안전거리 -5픽셀 추후구현 이부분이 작으면 작을수록 접근대상에 대해 뚫을 가능성이 커진다
	int Safe_range = 10;
	int tmpXZ[2];
	short X_a,W_a,	X_d,W_d;//충격체와 피격체의  XYWH

	//충격체 전좌표
	short Att_acX_before = p_Attacker->m_lastX;								//before Absolute coordinates 
	short Att_acY_before = p_Attacker->m_lastY;								//before Absolute coordinates 

	//충격체 현좌표
	short Att_acX = 0;
	short Att_acY = 0;
	int att_Frame_Index = 0;
	int att_Rect_Index = 0;
	if(0 == p_Attacker->m_nState)	//	에니메이션
	{
		p_Attacker->Get_AFrameXZ(&tmpXZ[0]);//이번에 그려질 Ani 의 x,z 좌표를 얻어온다
		Att_acX = tmpXZ[0] ;								//Absolute coordinates 
		Att_acY = p_Attacker->m_posY;				//Absolute coordinates 
		att_Frame_Index = p_Attacker->GetAnimFrame();// Attacker 애니메이션의 프레임에 고유 Index 값을 얻어온다
		att_Rect_Index = p_Attacker->m_sprite->_frames_rects_start[att_Frame_Index]*4;// AttackerBODY 프레임의 시작지점 배열인덱스
	}
	else if(1 == p_Attacker->m_nState)	//	프레임
	{
		Att_acX = p_Attacker->m_posX;								//Absolute coordinates 
		Att_acY = p_Attacker->m_posY;				//Absolute coordinates 
		att_Frame_Index = p_Attacker->m_nCrtFrame;
		att_Rect_Index = p_Attacker->m_sprite->_frames_rects_start[att_Frame_Index]*4;// AttackerBODY 프레임의 시작지점 배열인덱스
	}

	//피격체
	//short Def_acX = 0;
	//short Def_acY = 0;
	//int def_Frame_Index = 0;
	//int def_Rect_Index = 0;
	//if(0 == p_Defender->m_nState)	//	에니메이션
	//{
	//	p_Defender->Get_AFrameXZ(&tmpXZ[0]);//이번에 그려질 Ani 의 x,z 좌표를 얻어온다
	//	Def_acX = tmpXZ[0] ;								//Absolute coordinates 
	//	Def_acY = p_Defender->m_posY;				//Absolute coordinates 
	//	def_Frame_Index = p_Defender->GetAnimFrame();// Defender 애니메이션의 프레임에 고유 Index 값을 얻어온다	
	//	def_Rect_Index = p_Defender->m_sprite->_frames_rects_start[def_Frame_Index]*4;// DefenderBODY 프레임의 시작지점 배열인덱스
	//}
	//else if(1 == p_Defender->m_nState)	//	프레임
	//{
	//	Def_acX = p_Defender->m_posX;								//Absolute coordinates 
	//	Def_acY = p_Defender->m_posY;				//Absolute coordinates 
	//	def_Frame_Index = p_Defender->m_nCrtFrame;
	//	def_Rect_Index = p_Defender->m_sprite->_frames_rects_start[def_Frame_Index]*4;// AttackerBODY 프레임의 시작지점 배열인덱스
	//}

	//★.1////////////	현좌표와 피격체 중에 한쪽이라도 몸통이 없다면 충돌체크는 하지않는다.
	if( RECT_NUM(p_Attacker, att_Frame_Index) < 1) //무결성체크 - 공격자가 몸통이 없다면
		return false;//판정하지 않는다

	X_a =XY(p_Attacker, att_Rect_Index  );
	W_a =WH(p_Attacker, att_Rect_Index+2);
	if(p_Attacker->m_stopFlag & FLAG_FLIP_X) X_a = -(X_a+W_a);// 공격 애니 반전이면

	//X_d =XY(p_Defender, def_Rect_Index  );
	//W_d =WH(p_Defender, def_Rect_Index+2);
	//if(p_Defender->m_stopFlag & FLAG_FLIP_X) X_d = -(X_d+W_d);// 수비 애니 반전이면


	//★.2////////////	전좌표가 영역 안에 있다면 충돌체크는 하지않는다.
	//if	((Att_acY_before +attGap/2	>	Def_acY -defGap/2	) && 
	//	(Att_acY_before -attGap/2		<	Def_acY +defGap/2	)){//충격체 전좌표와 피격체의 Y 겹침

	//		if((Att_acX_before + (X_a+W_a)	>	Def_acX + (X_d) + Safe_range	) && 
	//			(Att_acX_before + (X_a) + Safe_range	<	Def_acX + (X_d+W_d)	)){
	//				return false;////전좌표가 이미 겹쳐 있을때는 충돌체크가 가동하지 않는다
	//		}

	//}

	//★.3////////////	X합계영역, Y합계영역 안에 피격체가 존재하지않으면 충돌체크는 하지않는다.
	//bool a1= (MIN(Att_acX_before+(X_a), Att_acX+(X_a))			>= X+W	);
	//bool a2= (MAX(Att_acX_before+(X_a+W_a), Att_acX+(X_a+W_a))	<= X		);
	//bool a3= (MIN(Att_acY_before-attGap/2, Att_acY-attGap/2)		>= Y+H	);
	//bool a4= (MAX(Att_acY_before+attGap/2, Att_acY+attGap/2)		<= Y	);

	if ((MIN(Att_acX_before+(X_a), Att_acX+(X_a))			>= X+W	)||
		(MAX(Att_acX_before+(X_a+W_a), Att_acX+(X_a+W_a))	<= X	)||
		(MIN(Att_acY_before-attGap/2, Att_acY-attGap/2)		>= Y+H	)||
		(MAX(Att_acY_before+attGap/2, Att_acY+attGap/2)		<= Y	))
		return false;//어느한쪽이라도 벗어나면  충돌체크가 가동하지 않는다




	int overX_Length = 0;
	if(HERO && Att_acX_before == Att_acX){
	}else
		if(Att_acX_before < X){//★왼쪽으로 되돌아올 경우
			if(Att_acX_before < Att_acX)//오른쪽으로 이동한것이 맞는가? - 겹쳐지다가 벗어난경우 걸릴수도 있음
				overX_Length = - ((Att_acX	+ (X_a+W_a)) - (X));
		}else{//★오른쪽으로 되돌아올 경우
			if(Att_acX_before > Att_acX)//왼쪽으로 이동한것이 맞는가? - 겹쳐지다가 벗어난경우 걸릴수도 있음
				overX_Length = + ((X+W) - (Att_acX + (X_a)));

		}

		int overY_Length = 0;
		if(HERO && Att_acY_before == Att_acY){
		}else
			if(Att_acY_before < Y){//★위로 되돌아올 경우
				overY_Length = - ((Att_acY	+ attGap/2) - (Y));

			}else{//★아래로 되돌아올 경우
				overY_Length = + ((Y+H) - (Att_acY - attGap/2));
			}


			if(HERO){
				if( abs(overX_Length) > 0 ){// y축의 이동량이 크다면 x 를 반영한다.
					p_Attacker->m_posX += overX_Length;
					return true;
				}
				if( abs(overY_Length) > 0 ){// y축의 이동량이 크다면 x 를 반영한다.
					p_Attacker->m_posY += overY_Length;
					return true;
				}
			}else{//몬스터의 경우 배경과 비교할때 쓰임
				if( ABS(overX_Length) < ABS(overY_Length) ){// Y축의 이동량이 크다면 X 를 반영한다.
					p_Attacker->m_posX += overX_Length;
					return true;
				}else{// Y축의 이동량이 크다면 X 를 반영한다.
					p_Attacker->m_posY += overY_Length;
					return true;
				}
			}

			return true;
}

int GFieldBattle::Area_Check(ASpriteInstance* Instance,int x_MAX)
{//맵의 x범위 안에 존재하는지 체크해서 밖으로 나갔을때 안으로 끌어들여오는 부분
	//몸통간 안전거리 -5픽셀 추후구현 이부분이 작으면 작을수록 접근대상에 대해 뚫을 가능성이 커진다
	int Safe_range = 35;

	int x_NOW = Instance->m_posX;


	int modify_X = 0;//보정 값을 X 에 반영한다
	int modify_Y = 0;
	if(x_NOW > x_MAX-Safe_range){
//		Instance->m_posX -= (x_NOW -  (x_MAX-Safe_range));
		modify_X = (x_MAX-Safe_range) - Instance->m_posX;
		Instance->m_posX = (x_MAX-Safe_range);
	}

	if(x_NOW < 0+Safe_range){
//		Instance->m_posX -= (x_NOW - (0+Safe_range));
		modify_X = (Safe_range) - Instance->m_posX;
		Instance->m_posX = (Safe_range);
	}

	return modify_Y*100000 + modify_X;
}
void GFieldBattle::Paint_Exception_Check()
{
	Exception_Num = 0; //초기화

	if(hero->s_Skill_Set.act && hero->s_Skill_Set.Skill_ID[hero->s_Skill_Set.Num] == 19)
	{//뇌룡
		switch(hero->_ins_Hero->m_nCrtFrame){
			case 1:
				pField->workPal(true,0,-5);
				break;
			case 2:
				pField->workPal(true,0,-10);
				break;
			case 9:
				pField->workPal(true,0,-5);
				break;
			case 10:
				pField->workPal(false,0,0);
				break;
		}

	}
	if(hero->s_Skill_Set.act && hero->s_Skill_Set.Skill_ID[hero->s_Skill_Set.Num] == 3)
	{//공간베기

		int ani_time = hero->_ins_Hero->m_nCrtAnimation;
		switch(hero->_ins_Hero->m_nCrtFrame){
			case 5:
				pField->workPal(true,0,-10);
				break;
			case 6:
				pField->workPal(true,0,-20);
				break;
			case 49:
				switch(ani_time){
			case 0:
				pField->workPal(true,0,20);
				break;
			case 1:
				pField->workPal(true,0,10);
				break;
			case 2:
				pField->workPal(false,0,0);
				break;
				}
		}


		if(hero->_ins_Hero->m_nCrtFrame == 18 || hero->_ins_Hero->m_nCrtFrame == 48){//전체 공격
			MoveHead(pMonList);
			MoveNext(pMonList);
			Position3D power;
			int tmp = 0;

			while(NotEndList(pMonList))
			{
				if((MON_AC_DIE != GetData(pMonList)->m_ActState) &&
					MON_ATK_DEAD_ATTACK != GetData(pMonList)->m_ActState &&
					(MON_AC_DIE_AFTER != GetData(pMonList)->m_ActState))
				{

					if(hero->_ins_Hero->m_nCrtFrame == 18){
						s_Contact.x = 1;
						s_Contact.y = 0;
						s_Contact.z = 0;
					}else{
						s_Contact.x = 0;
						s_Contact.y = 0;
						s_Contact.z = -50;
					}
					s_Contact.D_index = 56;//공간 참격 인덱스
					DamageSand_Hero(hero, GetData(pMonList));//주인공의 공격


				}

				MoveNext(pMonList);
			}
		}else{
			Exception_Num+=PROCESS_MON_NOT;//몬스터 정지
		}

	}
	if(hero->s_Skill_Set.act && hero->s_Skill_Set.Skill_ID[hero->s_Skill_Set.Num] == 18)
	{//블랙홀
		int ani_time = hero->_ins_Hero->m_nCrtAnimation;

		switch(hero->_ins_Hero->m_nCrtFrame){
			case 6:
				switch(ani_time%4){
			case 0:
				pField->workPal(true,0,-3);
				break;
			case 1:
				pField->workPal(true,0,-7);
				break;
			case 2:
				pField->workPal(true,0,-3);
				break;
			case 3:
				pField->workPal(false,0,0);
				break;


				}
				break;
			case 7:
				pField->workPal(false,0,0);
				break;


		}
		//if(hero->_ins_Hero->m_nCrtFrame < 7){//빨아당기기
		//	MoveHead(pMonList);
		//	MoveNext(pMonList);
		//	while(NotEndList(pMonList))
		//	{
		//		if(MON_AC_DIE != GetData(pMonList)->m_ActState && MON_AC_DIE_AFTER != GetData(pMonList)->m_ActState)
		//		{

		//			//MON_AC_BLACKHOLE

		//		}
		//		MoveNext(pMonList);
		//	}	
		//}

	}

	if(hero->s_Skill_Set.act && hero->s_Skill_Set.Skill_ID[hero->s_Skill_Set.Num] == 0)
	{//히어로 검버프 시전했을때
		int ani_time = hero->_ins_Hero->m_nCrtAnimation;

		switch(hero->_ins_Hero->m_nCrtFrame){
			case 16:
				switch(ani_time){
			case 0:
				pField->workPal(true,0,31);
				Exception_Num+=PAINT_MON_NOT+PAINT_HERO_NOT;

				break;
			case 1:
				pField->workPal(true,0,15);
				break;
				}
				break;
			case 17:
				pField->workPal(false,0,0);
				break;



		}
	}
	if(hero->s_Knife_Eff[3].act && hero->s_Knife_Eff[3].LVup_Eff_Ins)
	{//히어로 검버프 중일때

		if(hero->s_Skill_Set.act && hero->s_Skill_Set.Skill_ID[hero->s_Skill_Set.Num] == 11)
			hero->s_Knife_Eff[3].LVup_ActionEnd = true;//우주빔 발동시에는 해제한다

		switch(hero->s_Knife_Eff[3].LVup_Eff_Ins->m_nCrtModule){
			case ANIM_WOMAN_S_S1_A_S_SKILL1_BUFF_E_BACK://챠지
				{
					int RangeLengthMAX = 200;
					int RangeLengthMIN = 100;

					int pMonsIns_Num = 0;
					class Monster* pMonsIns[5]; 

					MoveHead(pMonList);
					MoveNext(pMonList);
					while(NotEndList(pMonList))
					{
						if((MON_AC_DIE != GetData(pMonList)->m_ActState) &&
							MON_ATK_DEAD_ATTACK != GetData(pMonList)->m_ActState &&
							(MON_AC_DIE_AFTER != GetData(pMonList)->m_ActState))
						{

							int tempX = GetData(pMonList)->pMonAsIns->m_posX - (hero->_ins_Hero->m_posX);
							int tempY = GetData(pMonList)->pMonAsIns->m_posY - (hero->_ins_Hero->m_posY);


							if((RangeLengthMAX> ABS(tempX)+ABS(tempY)) && (RangeLengthMIN< ABS(tempX)+ABS(tempY))){//발사거리에 존재하면 리스트에 추가
								pMonsIns[pMonsIns_Num] = GetData(pMonList);//포인터 저장 
								pMonsIns_Num++;
								if(pMonsIns_Num > 4)
									break;//몬스터 풀이 가득 찼다

							}

						}
						MoveNext(pMonList);
					}		



					if(pMonsIns_Num>0){//타킷 몬스터가 1마리 이상 있으면
						s_Homing[0].pMons = pMonsIns[RND(0,pMonsIns_Num-1)];//몬스터 포인터 넘기기
						s_Homing[1].pMons = pMonsIns[RND(0,pMonsIns_Num-1)];//몬스터 포인터 넘기기
						s_Homing[2].pMons = pMonsIns[RND(0,pMonsIns_Num-1)];//몬스터 포인터 넘기기

						hero->s_Knife_Eff[3].LVup_Eff_Ins->SetAnim(ANIM_WOMAN_S_S1_A_S_SKILL1_ATT_E_BACK);
						hero->s_Knife_Eff[3].LVup_Eff_Ins->m_bLoop = false;

						hero->s_Knife_Eff[4].LVup_Eff_Ins->SetAnim(ANIM_WOMAN_S_S1_A_S_SKILL1_ATT_E);
						hero->s_Knife_Eff[4].LVup_Eff_Ins->m_bLoop = false;
					}
				}

				break;
			case ANIM_WOMAN_S_S1_A_S_SKILL1_ATT_E_BACK://발사
				switch(hero->s_Knife_Eff[3].LVup_Eff_Ins->m_nCrtFrame){
			case 4:
				hero->s_Knife_Eff[0].act = true;

				s_Homing[0].X1 =(hero->s_Knife_Eff[3].LVup_Eff_Ins->m_flags? -51:+51) + (hero->_ins_Hero->m_posX);
				s_Homing[0].Y1 =-6 + (hero->_ins_Hero->m_posY);
				s_Homing[0].X2 = s_Homing[0].pMons->pMonAsIns->m_posX;
				s_Homing[0].Y2 = s_Homing[0].pMons->pMonAsIns->m_posY;
				s_Homing[0].Cita1=(hero->s_Knife_Eff[3].LVup_Eff_Ins->m_flags? 240:120);

				s_Homing[0].TurnCita = 5;
				s_Homing[0].R = 15;
				break;
			case 5:
				hero->s_Knife_Eff[1].act = true;

				s_Homing[1].X1 =(hero->s_Knife_Eff[3].LVup_Eff_Ins->m_flags? -61:+61) + (hero->_ins_Hero->m_posX);
				s_Homing[1].Y1 =-19 + (hero->_ins_Hero->m_posY);
				s_Homing[1].X2 = s_Homing[1].pMons->pMonAsIns->m_posX;
				s_Homing[1].Y2 = s_Homing[1].pMons->pMonAsIns->m_posY;
				s_Homing[1].Cita1=(hero->s_Knife_Eff[3].LVup_Eff_Ins->m_flags? 300:60);

				s_Homing[1].TurnCita = 5;
				s_Homing[1].R = 15;
				break;
			case 6:
				hero->s_Knife_Eff[2].act = true;

				s_Homing[2].X1 =(hero->s_Knife_Eff[3].LVup_Eff_Ins->m_flags? -71:+71) + (hero->_ins_Hero->m_posX);
				s_Homing[2].Y1 =+28 + (hero->_ins_Hero->m_posY);
				s_Homing[2].X2 = s_Homing[2].pMons->pMonAsIns->m_posX;
				s_Homing[2].Y2 = s_Homing[2].pMons->pMonAsIns->m_posY;
				s_Homing[2].Cita1=(hero->s_Knife_Eff[3].LVup_Eff_Ins->m_flags? 210:150);

				s_Homing[2].TurnCita = 5;
				s_Homing[2].R = 15;
				break;

				}

				for(int i =0;i<3;i++){
					if(hero->s_Knife_Eff[i].act && hero->s_Knife_Eff[i].LVup_Eff_Ins!=NULL){//호밍 시작




						if(ABS(s_Homing[i].X1 - s_Homing[i].X2) + ABS(s_Homing[i].Y1 - s_Homing[i].Y2) < s_Homing[i].R){
							hero->s_Knife_Eff[i].LVup_ActionEnd = !hero->s_Knife_Eff[i].LVup_Eff_Ins->UpdateSpriteAnim();//Effect 에니메이션 업데이트 실행
							if(hero->s_Knife_Eff[i].LVup_ActionEnd ){
								int zz = 0;
							}
							if(hero->s_Knife_Eff[i].LVup_Eff_Ins->m_bLoop){
								//hero->s_Knife_Eff[i].LVup_ActionEnd=true;
								hero->s_Knife_Eff[i].LVup_Eff_Ins->SetAnim(ANIM_WOMAN_S_S1_A_S_SKILL1_HIT1+i);
								hero->s_Knife_Eff[i].LVup_Eff_Ins->m_posX = s_Homing[i].X2;
								hero->s_Knife_Eff[i].LVup_Eff_Ins->m_posY = s_Homing[i].Y2+1;
								hero->s_Knife_Eff[i].LVup_Eff_Ins->m_bLoop = false;

								//s_Homing[i].R=0;
							}


						}else{

							if(s_Homing[i].pMons){
								s_Homing[i].X2 = s_Homing[i].pMons->pMonAsIns->m_posX;
								s_Homing[i].Y2 = s_Homing[i].pMons->pMonAsIns->m_posY;
							}

							GetHomingXY(&s_Homing[i]);
							hero->s_Knife_Eff[i].LVup_Eff_Ins->m_posX = s_Homing[i].X1;
							hero->s_Knife_Eff[i].LVup_Eff_Ins->m_posY = s_Homing[i].Y1;

							int xxx = ANIM_WOMAN_S_S1_DAGGER+((s_Homing[i].Cita1+15)%360) /30;
							if(xxx != hero->s_Knife_Eff[i].LVup_Eff_Ins->m_nCrtModule)
								hero->s_Knife_Eff[i].LVup_Eff_Ins->SetAnim(xxx);

							if(s_Homing[i].TurnCita <90)s_Homing[i].TurnCita *= 2;
							if(s_Homing[i].R<30)s_Homing[i].R += 1;

						}
					}
				}

				break;
		}
	}





	if(hero->s_Skill_Set.act && hero->s_Skill_Set.Skill_ID[hero->s_Skill_Set.Num] == 11)
	{//우주빔


		int ani_time = hero->_ins_Hero->m_nCrtAnimation;

		switch(hero->_ins_Hero->m_nCrtFrame){
			case 2:if(ani_time==0)pField->workPal(true,0,-5);
				break;
			case 3:if(ani_time==0)pField->workPal(true,0,-17);
				break;
			case 4:
				if(ani_time==0){
					pField->workPal(true,0,-31);
					BAKUP_HERO_Y = hero->_ins_Hero->m_posY;
					hero->_ins_Hero->m_posY = SCREEN_HEIGHT/2;
				}
				break;
			case 37:
				s_Camera.CameraAdd = (ani_time%2 ?-1 : +1)*RND(1,3);//카메라 흔들기
				switch(ani_time){
			case 0:
				hero->_ins_Hero->m_posY  = BAKUP_HERO_Y;
				pField->workPal(true,0,0);
				break;
			case 11:
				pField->workPal(true,0,15);
				for(int xx = 0;xx<SPRITE_MON_MAX;xx++) if(m_MonAs[xx])m_MonAs[xx]->workPal(true,0,15);
				break;
			case 12:
				pField->workPal(true,0,21);
				for(int xx = 0;xx<SPRITE_MON_MAX;xx++) if(m_MonAs[xx])m_MonAs[xx]->workPal(true,0,31);
				break;
			case 14:
				pField->workPal(true,0,20);
				for(int xx = 0;xx<SPRITE_MON_MAX;xx++) if(m_MonAs[xx])m_MonAs[xx]->workPal(true,0,17);
				break;
			case 15:
				pField->workPal(true,0,10);
				for(int xx = 0;xx<SPRITE_MON_MAX;xx++) if(m_MonAs[xx])m_MonAs[xx]->workPal(true,0,5);
				{
					MoveHead(pMonList);
					MoveNext(pMonList);
					Position3D power;
					int tmp = 0;
					while(NotEndList(pMonList))
					{
						if((MON_AC_DIE != GetData(pMonList)->m_ActState) &&
							MON_ATK_DEAD_ATTACK != GetData(pMonList)->m_ActState &&
							(MON_AC_DIE_AFTER != GetData(pMonList)->m_ActState))
						{

							s_Contact.x = 0;
							s_Contact.y = 0;
							s_Contact.z = -50;
							s_Contact.D_index = 72;//안드로메다 인덱스
							DamageSand_Hero(hero, GetData(pMonList));//주인공의 공격

						}

						MoveNext(pMonList);
					}
				}

				break;
			case 16:
				pField->workPal(false,0,0);
				for(int xx = 0;xx<SPRITE_MON_MAX;xx++) if(m_MonAs[xx])m_MonAs[xx]->workPal(false,0,0);
				break;

				}

				break;
		}
		if(hero->_ins_Hero->m_nCrtFrame >= 4 && hero->_ins_Hero->m_nCrtFrame <= 36){
			Exception_Num+=PAINT_MON_NOT+PAINT_FIELD_FRONT_NOT;
		}

	}

	if(CRI_SHINING){//크리티컬 화면 번쩍임
		CRI_SHINING--;
		pField->workPal(CRI_SHINING,0,7*CRI_SHINING);
	}

}

//--------------------------------------------------------------------------
void GFieldBattle::Throw_Check()
//--------------------------------------------------------------------------
{
	if(hero->s_Throw.ThrowPossible==true)
	{//주인공이 잡기가 가능할 타이밍일때 확인작업을 한다
		hero->s_Throw.ThrowPossible=false;//던지기 체크가 끝나면 던지기가능 상태는 바로해제 한다
		MoveHead(pMonList);
		MoveNext(pMonList);
		while(NotEndList(pMonList))
		{
			if(GetData(pMonList)->SetHolding(hero->s_Throw.ThrowNum))
			{
				hero->s_Throw.act = true;

				return;
			}

			MoveNext(pMonList);
		}
	}
}



//--------------------------------------------------------------------------
void GFieldBattle::Analysis_Mon_Message()
//--------------------------------------------------------------------------
{
	int msg = 0;
	//	message
	MoveHead(pMonMsgList);
	MoveNext(pMonMsgList);
	while(NotEndList(pMonMsgList))
	{
		if(0 < GetData(pMonMsgList)->m_nDelay)
		{
			GetData(pMonMsgList)->m_nDelay--;
			MoveNext(pMonMsgList);
			continue;
		}

		switch(GetData(pMonMsgList)->m_MsgType)
		{
		case MSG_CHANGE_STATE:
			//------------------------------------------------------------
			{
				int dst = GetData(pMonMsgList)->param[0];
				int changeState = GetData(pMonMsgList)->param[1];

				//	대상을 검색한다.
				MoveHead(pMonList);
				MoveNext(pMonList);
				while(NotEndList(pMonList))
				{
					if(dst == GetData(pMonList)->m_nMonIdx)	//	같은 타입이라면
					{
						GetData(pMonList)->ResvAction(changeState, 0);
						break;
					}
					MoveNext(pMonList);
				}
				break;
			}
		case MSG_SUMMONE_BABY:
			//------------------------------------------------------------
			{
				//				AddMonster(1, GetData(pMonMsgList)->param[0], GRADE_SPECIAL,
				//								GetData(pMonMsgList)->param[1], GetData(pMonMsgList)->param[2],
				//								GetData(pMonMsgList)->param[1], GetData(pMonMsgList)->param[2], 0,
				//								GetData(pMonMsgList)->param[3]);

				//	소환하다말았음
				//	딜레이 시간 체크
				break;
			}
		case MSG_SUMMONE_BUGS:
			//------------------------------------------------------------
			{
				//TEST
				if(15 < GetNodeCount(pMonList))	{break;}

				int posx[] = {100,  -100,  50,  50, -50, -50};
				int posy[] = {  0,     0, -30,  30, -30,  30};

				for(int loop = 0; loop < 6; loop++)
				{
					AddMonster(1, MON_IDX_BUG, 0, 0, GRADE_NORMAL,
						GetData(pMonMsgList)->param[0]+posx[loop], GetData(pMonMsgList)->param[1]+posy[loop],
						GetData(pMonMsgList)->param[0]+posx[loop], GetData(pMonMsgList)->param[1]+posy[loop],
						NOT_REGEN);
					GetData(pMonList)->Process(m_CharInfo);
				}

				//	소환하다말았음
				//	딜레이 시간 체크
				break;
			}
		case MSG_DE_SUMMONE_BUGS:
			//------------------------------------------------------------
			{
				//	대상을 검색한다.
				MoveHead(pMonList);
				MoveNext(pMonList);
				while(NotEndList(pMonList))
				{
					//	모든 몬스터의 피를 없애준다. 
					GetData(pMonList)->m_Stet.m_Hp = 0;

					if(MON_IDX_HOROS != GetData(pMonList)->m_PtnIdx)
					{
						//						if(MON_AC_STAND == GetData(pMonList)->m_ActState ||
						//							MON_AC_MOVE == GetData(pMonList)->m_ActState ||
						//							MON_AC_RUN == GetData(pMonList)->m_ActState)
						//						{
						GetData(pMonList)->ResvAction(MON_AC_DIE, 1);
						//						}

					}

					MoveNext(pMonList);
				}

				break;
			}
		case MSG_BOSS_CHANGESTATE:
			//------------------------------------------------------------
			{
				for(InitList(pMonList);NotEndList(pMonList);MoveNext(pMonList))
				{
					if(GetData(pMonList)->m_nUniqueIdx == GetData(pMonMsgList)->param[0])
					{
						GetData(pMonList)->ResvAction(GetData(pMonMsgList)->param[1], 0);
					}
					
/*

					if(GetData(pMonList)->m_nMonIdx == MON_IDX_BIGDOG1)
					{
						GetData(pMonList)->m_nAiNextState = GetData(pMonMsgList)->param[0];

						if(MON_AI_ATTACK_DONTMOVE == GetData(pMonMsgList)->param[0])
						{
							GetData(pMonList)->m_nUseAtkNum = GetData(pMonMsgList)->param[1];
							GetData(pMonList)->m_bIsSuccessAttack = false;

							GetData(pMonList)->m_Attack[GetData(pMonList)->m_nUseAtkNum].CoolTime =
								GetData(pMonList)->m_Attack[GetData(pMonList)->m_nUseAtkNum].MaxCoolTime;
						}
						else if(MON_AI_READY_TO_HIT == GetData(pMonMsgList)->param[0])
						{
							GetData(pMonList)->m_nDstTime = 20;
						}
					}
*/
				}
				break;
			}
			case MSG_BOSS5_SUMMON_WALL:
			//------------------------------------------------------------
			{
				AddMonster(1, MON_IDX_DRILL, 0, 0, GRADE_NORMAL,
							GetData(pMonMsgList)->param[0], GetData(pMonMsgList)->param[1],
							GetData(pMonMsgList)->param[0], GetData(pMonMsgList)->param[1],	NOT_REGEN);
						GetData(pMonList)->Process(m_CharInfo);
/*
				//	width
				if(0 == GetData(pMonMsgList)->param[0])
				{
					for(int loop = 0; loop < 20; loop++)
					{
						AddMonster(1, MON_IDX_DRILL, 0, 0, GRADE_NORMAL,

							15 + loop*20, GetData(pMonMsgList)->param[1]-10+SUTIL_GetRandom()%20,
							15 + loop*20, GetData(pMonMsgList)->param[1]-10+SUTIL_GetRandom()%20,
							NOT_REGEN);
						GetData(pMonList)->Process(m_CharInfo);
					}
				}
				//	height
				else
				{
					for(int loop = 0; loop < 6; loop++)
					{
						AddMonster(1, MON_IDX_DRILL, 0, 0, GRADE_NORMAL,
							GetData(pMonMsgList)->param[1]-10+SUTIL_GetRandom()%20, 180 + loop*20,
							GetData(pMonMsgList)->param[1]-10+SUTIL_GetRandom()%20, 180 + loop*20,
							NOT_REGEN);
						GetData(pMonList)->Process(m_CharInfo);
					}
				}
*/
				break;
			}
			case MSG_BOSS5_SUMMON_DEVIL:
			//------------------------------------------------------------
			{
				AddMonster(1, MON_IDX_BOSS5_DEVIL, 0, 0, GRADE_NORMAL,
							GetData(pMonMsgList)->param[0], GetData(pMonMsgList)->param[1],
							GetData(pMonMsgList)->param[0], GetData(pMonMsgList)->param[1],	NOT_REGEN);
						GetData(pMonList)->Process(m_CharInfo);


				break;
			}

			default:
			//------------------------------------------------------------
			{
				MoveNext(pMonMsgList);
				continue;
			}
		}

		//	메세지를 지워준다.
		SAFE_DELETE(GetData(pMonMsgList));
		pMonMsgList->Delete();
	}
}





#define RADER_POS_X	(7+74)
#define RADER_POS_Y	(16)
#define RADER_POS_W	(62 - 4)
#define RADER_POS_H	(16 - 4)
#define Y_START	(157)
#define Y_END	(255)
void GFieldBattle::PaintMiniMap(){
	////	미니맵
	SUTIL_Paint_Frame(pFieldUi->s_ASpriteSet->pFieldUiAs ,FRAME_UI_RADER, RADER_POS_X, RADER_POS_Y,0);

	int mapsize = pField->m_nFieldSize_X;

	MoveHead(pMonList);
	MoveNext(pMonList);
	while(NotEndList(pMonList))
	{
		////몬스터 (통이나 돌은 맞으면 안됨)
		if(0 == (GetData(pMonList)->m_nFeature & FE_DONT_DRAW_MAP))
		{
			SUTIL_Paint_Frame(pFieldUi->s_ASpriteSet->pFieldUiAs ,
				FRAME_UI_RADER_ENEMY,
				RADER_POS_X + GetData(pMonList)->pMonAsIns->m_posX*RADER_POS_W/mapsize +2,
				RADER_POS_Y + (GetData(pMonList)->pMonAsIns->m_posY - Y_START)*RADER_POS_H/(Y_END - Y_START) +1,
				0);
		}
		MoveNext(pMonList);
	}

	////	주인공	
	SUTIL_Paint_Frame(pFieldUi->s_ASpriteSet->pFieldUiAs ,
		FRAME_UI_RADER_MC,
		RADER_POS_X + m_CharInfo->m_nPos.x * RADER_POS_W/mapsize +2,
		RADER_POS_Y + (m_CharInfo->m_nPos.y - Y_START) *RADER_POS_H/(Y_END - Y_START) +1,
		0);

}

//--------------------------------------------------------------------------
void GFieldBattle::SetCameraMove()
//--------------------------------------------------------------------------
{
	if(s_Camera.cX == 0)
	{
		s_Camera.cX = hero->_ins_Hero->m_posX;
	}

	int tempX = hero->_ins_Hero->m_posX;
	int heroX = tempX;

	int scroll_x=0;

	int scroll_speed=0;
	switch(hero->_move_Order){
		case HERO_DASH_RIGHT:
		case HERO_DASH_LEFT:
		case HERO_DASH_STOP:
			/*s_Camera.stop_Frame=0;
			s_Camera.CameraAdd=0;
			tempX = s_Camera.cX;
			scroll_x = CAMERA_DASH_SCROLL;*/
			scroll_speed = 65;
			break;
		default:

			scroll_speed = 80;
			break;


			//case	HERO_STOP:
			//case 	HERO_ATTACK1	:
			//case 	HERO_ATTACK1END	:
			//case 	HERO_ATTACK2	:
			//case 	HERO_ATTACK2END	:
			//case 	HERO_ATTACK3	:
			//case 	HERO_ATTACK3END	:
			//case 	HERO_ATTACK4	:
			//	scroll_x = CAMERA_MAX_MOVE;

			//	s_Camera.stop_Frame++;
			//	if(CAMERA_DELAY < s_Camera.stop_Frame){//일정시간이상 대기하면 시야확보
			//		tempX += (hero->_ins_Hero->m_flags ? -CAMERA_MAX_SPACE : +CAMERA_MAX_SPACE);
			//	}else{
			//		tempX = s_Camera.cX;
			//	}
			//	break;
			//default:
			//	s_Camera.stop_Frame=0;
			//	tempX = s_Camera.cX;
			//	scroll_x = CAMERA_MAX_MOVE;
			//	break;

	}

	scroll_x = CAMERA_MAX_MOVE;

	//s_Camera.stop_Frame++;
	//if(CAMERA_DELAY < s_Camera.stop_Frame){//일정시간이상 대기하면 시야확보
	tempX += (hero->_ins_Hero->m_flags ? -CAMERA_MAX_SPACE : +CAMERA_MAX_SPACE);
	/*}else{
	tempX = s_Camera.cX;
	}*/




	if(tempX > heroX){
		if(tempX - heroX > scroll_x) 
			tempX -= (tempX - heroX) - scroll_x;
		else
			tempX += ((((s_Camera.cX)-tempX) * scroll_speed) / 100);
	}else{
		if(heroX - tempX > scroll_x) 
			tempX += (heroX - tempX) - scroll_x;
		else
			tempX += ((((s_Camera.cX)-tempX) * scroll_speed) / 100);
	}


	//부드러운 스크롤
	//tempX += ((((s_Camera.cX)-tempX) * 80) / 100);

	pField->SetCamera(tempX+s_Camera.CameraAdd);
	s_Camera.cX = tempX;
}


//--------------------------------------------------------------------------
void GFieldBattle::DamageSand_Hero(Character* hero, Monster* mon)
//--------------------------------------------------------------------------
{
	int tmp = 0;
	power.Init(s_Contact.x,s_Contact.y,s_Contact.z);

	if(mon->ReceiveAttack(power))
	{
		tmp = hero->SND_Damage(mon->m_nLevel,
			mon->m_nElementIdx,   
			mon->m_Stet.m_Defance,
			s_Contact.D_index);
		//CRITICAL_HERO
			if(hero->Critical && !CRI_SHINING)CRI_SHINING=3;
		mon->RCV_Damage(tmp);
		if(tmp){
			pFieldUi->InsertdamageNum(s_Contact.crash_x , mon->pMonAsIns->m_posY-mon->m_Stet.m_Height , mon->pMonAsIns->m_posZ, tmp, (hero->Critical?MON_CRI_NUM:MON_NUM));
			pFieldUi->Insert_CrashEffect(s_Contact.crash_x, mon->pMonAsIns->m_posY, s_Contact.crash_z, (hero->Critical?ANIM_WEFFECT_A_CRITICAL_1:ANIM_WEFFECT_A_EFF_HIT1));
			mon->RCV_Debuff( hero->SND_Debuff(0) );
		}


		//	추가 데미지가 있는지 확인하고 같이 감산해서 넘긴다.
		//	이유는 이부분의 갱신의 실제 데미지보다 빠르기때문에 그렇게 함
		int temphp = mon->m_Stet.m_Hp-Monster::m_nExtraDamage;
		if(0 > temphp)	{temphp = 0;}

		pFieldUi->InsertMonsterInfo(mon->m_nUniqueIdx,
			mon->m_nMonName,
			mon->m_nLevel,
			temphp,//mon->m_Stet.m_Hp-Monster::m_nExtraDamage,
			mon->m_Stet.m_MaxHp,
			mon->m_nElementIdx);
	}
}

//--------------------------------------------------------------------------
void GFieldBattle::DamageSand_Mon(Monster* mon , Character* hero)
//--------------------------------------------------------------------------
{

	if(AVOID_HERO){//주인공이 회피에 성공했을경우
		pFieldUi->InsertdamageNum(s_Contact.crash_x, hero->_ins_Hero->m_posY, hero->m_HEADTOP, 0, HERO_NUM);
	}else{
		int tmp = 0;

		tmp = mon->SND_Damage(hero->s_Status.LEVEL,
			hero->s_Status.ELEMENTAL,
			hero->s_Status.DEFENSE_PER,
			s_Contact.D_index);
		

			bool cri = false;
			if((RND(1,100)<= (mon->m_Stet.m_Cri)) && (RND(1,100) > hero->Get_Skill(SKILL_P_G_highAgi))) {tmp*=2;cri=true;}


			if(hero->_move_Order == HERO_CHARGE && hero->_b_JabNum == JAB_KNIGHT){//주인공이 가드에 성공했을경우
				tmp = (tmp*3)/10;
				pFieldUi->InsertEffect(s_Contact.crash_x, hero->_ins_Hero->m_posY, s_Contact.crash_z, ANIM_WEFFECT_A_HIT_DEFENSE);	
			}else{
				pFieldUi->Insert_CrashEffect(s_Contact.crash_x, hero->_ins_Hero->m_posY, s_Contact.crash_z, ANIM_WEFFECT_A_EFF_HITMC);	
			}
			if(RND(1,100) <= hero->Get_Skill(SKILL_P_S_ironWall)) {tmp/=2;}//패시브

			hero->RCV_Damage(tmp);
			pFieldUi->InsertdamageNum(s_Contact.crash_x, hero->_ins_Hero->m_posY, hero->m_HEADTOP, tmp, (cri?HERO_CRI_NUM:HERO_NUM));

			/////////////////////////////////////////////////////

			power.Init(s_Contact.x,s_Contact.y,s_Contact.z);
			hero->ReceiveAttack(power);

	}
	////////////////////////////////////////////////////
}


////--------------------------------------------------------------------------
//void GFieldBattle::SetEvent(int nEvtCode, int dummy1)
////--------------------------------------------------------------------------
//{	
//	if(!nEvtCode)	{return;}
//
//	switch(nEvtCode)
//	{
//		case 1:		//	맵이동 이벤트
//		//---------------------------------------------------------------
//		{
//			Resv_ChangeStage(dummy1);
//			break;
//		}
//		case 3:		//	지역 이동 이벤트
//		//---------------------------------------------------------------
//		{
//			Resv_ChangeStage(dummy1);
//			break;
//		}
//		case 2:		//	상점 npc
//		//---------------------------------------------------------------
//		{
//			//Resv_ChangeStage(dummy1);
//			int a = 90;
//			b_PopupUi = true;
//			break;
//		}
//		case 4:		//	씨네마 이벤트
//		//---------------------------------------------------------------
//		{
//			switch(dummy1)
//			{
//				case 0:
//				//------------------------------------------------------------
//				{
//					pCinema->Load_Cinematics(PACK_DATA_CINEMA, DATA_CINEMA_SCENE1, 1, pField);
//					break;
//				}
//				case 1:
//				//------------------------------------------------------------
//				{
//					hero->Set_MustAction(pCinema->m_ASpriteIns[0][1]->m_posX,
//											pCinema->m_ASpriteIns[0][1]->m_posY,
//											0, 0);
//
//					pCinema->Release_Cinematics();
////					m_nRestoreStageCount = 7;
//					
//					break;
//				}
//			}
//		}
//	}
//}


//--------------------------------------------------------------------------
void GFieldBattle::EventProcess()
//--------------------------------------------------------------------------
{
	//	imsi
	if(1000 <= pField->m_nSaveStageNum)												{return;}

	//	필드 이동관련 이벤트
	int result = pField->Process_EVT_WAY(MAP_EVT_WAY_TYPE_ALL, hero->s_Event_Rect.Direction, m_CharInfo->m_nPos.x, m_CharInfo->m_nPos.y);
	//	int result = pField->Process_EVT_WAY(MAP_EVT_WAY_TYPE_ALL, hero->s_Event_Rect.X1, hero->s_Event_Rect.Y1,
	//											hero->s_Event_Rect.X2, hero->s_Event_Rect.Y2);


	//	이동상태일 때만 이동 이벤트를 가능하게 한다.
	if(HERO_WALK_LEFT > hero->_move_Order || HERO_WALK_DOWN < hero->_move_Order)	{return;}

	if(WORLD_MAP_STAGE_NUM <= result){//월드맵 호출

		if(b_WorldMap){return;}

		b_WorldMap = true;
		pWorldMapUi = GL_NEW WorldmapUI(pField->m_nVillageUniqueIdx);
		return;
	}	
		
	//	돌이 있으면 넘어갈수 없다.
	if(MAIN_QUEST_openDoor >= GET_MAINQUEST_IDX())
	{
		if(200 == result && 201 == pField->m_nSaveStageNum)							{return;}
	}

	if(0 != result)
	{
		ADD_EVENT_MSG(EVT_SCREEN_FADE_OUT, 0, 10);
		ADD_EVENT_MSG(EVT_CHANGE_STAGE, 10, result);
	}

	//	이벤트에 대한 충돌체크
	result = pField->Process_EVT_Rect(m_CharInfo->m_nPos.x, m_CharInfo->m_nPos.y);

	if(0 != result)
	{
		ADD_EVENT_MSG(EVT_GAME_KEYEVENT, 0, result);
	}
}


//--------------------------------------------------------------------------
bool GFieldBattle::KeyEventProcess()
//--------------------------------------------------------------------------
{
	//	필드에서 키버튼 체크시 이벤트 체크이벤트 체크
	int evtcode = pField->KeyPressEvent(hero->s_Event_Rect.Direction,
		hero->s_Event_Rect.X1,hero->s_Event_Rect.Y1,
		hero->s_Event_Rect.X2,hero->s_Event_Rect.Y2);
	if(evtcode)
	{
		ADD_EVENT_MSG(EVT_GAME_KEYEVENT, 0, evtcode);
		return true;
	}

	return false;
}




//--------------------------------------------------------------------------
void GFieldBattle::Analysis_Event()
//--------------------------------------------------------------------------
{	
	bool bIsCrearMsg = false;;

	List2< S_EVENT_MSG* > *pMsgList = GET_EVENT_MSG_STRUCT();

	InitList( pMsgList );

	while(NotEndList( pMsgList ))
	{
		if(0 < GetData( pMsgList )->m_nDelay )
		{
			int delay = (GetData( pMsgList )->m_nDelay--);
			MoveNext( pMsgList );
			continue;
		}

		//	메세지를 분석한다.
		switch( GetData(pMsgList)->m_MsgType )
		{
		case EVT_SCREEN_FADE_OUT:
			//--------------------------------------------------------
			{
				SetFadeOut(GetData( pMsgList )->param[0]);
				break;
			}
		case EVT_SCREEN_FADE_IN:
			//--------------------------------------------------------
			{
				SetFadeIn(GetData( pMsgList )->param[0]);
				break;
			}
		case EVT_INIT_GAME:
			//--------------------------------------------------------
			{
				if(0 == GetData(pMsgList)->param[0])
				{
					LOAD_LOADING();
				}

				LoadFirstData(GetData( pMsgList )->param[0], GetData( pMsgList )->param[1], GetData( pMsgList )->param[2]);
				GetData( pMsgList )->param[0]++;

				if(7 > GetData( pMsgList )->param[0])
				{
					ADD_EVENT_MSG(EVT_INIT_GAME, 10, GetData( pMsgList )->param[0], GetData( pMsgList )->param[1], GetData( pMsgList )->param[2]);
				}
				else
				{
					ADD_EVENT_MSG(EVT_CHANGE_STAGE, 0, GetData( pMsgList )->param[1], 0, 1);
				}
				break;
			}
		case EVT_CHANGE_STAGE:
			//--------------------------------------------------------
			{
				//				if(1000 <= GetData( pMsgList )->param[0])
				//				{
				//					//큰 마을 이동
				//					AppMain::ResvNextState(MS_GAME_WORLDMAP, GetData( pMsgList )->param[0], 0);
				//					isHoldingProcess = true;
				//					isHoldingPaint = true;
				//					isHoldingKey = true;
				//				}
				//				else
				//				{
				//	퀘스트에 따라서 맵의 번호를 달리해준다.

				//	param2 는 처음 들어올땐 1로 들어온다.
				if(0 == GetData(pMsgList)->param[1] && 0 == GetData(pMsgList)->param[2])
				{
					//	메세지를 날려준다.
					bIsCrearMsg = true;
					if(!GET_EVENT_STATE())	{LOAD_LOADING();}
				}

				//	일반 맵 이동
				LoadStage(GetData(pMsgList)->param[0], GetData(pMsgList)->param[1]);
				GetData(pMsgList)->param[1]++;

				isHoldingProcess = true;
				isHoldingPaint = true;
				isHoldingKey = true;

				//	순차적으로 로딩을 시켜준다.
				if(3 > GetData(pMsgList)->param[1])
				{
					ADD_EVENT_MSG(EVT_CHANGE_STAGE, 0, GetData(pMsgList)->param[0], GetData(pMsgList)->param[1]);
				}
				else
				{
					UpdatePARSER_EVENT();


					/*
					//	이벤트를 체크한다.
					switch(PARSER_EVENT())
					{
					case EVT_START_CINEMA:
					{
					RELEASE_LOADING();
					break;
					}
					case EVT_CHANGE_STAGE:
					{
					break;
					}
					case EVT_NULL:
					case EVT_TALK_STARTEVT:
					{
					RELEASE_LOADING();
					SetFadeIn(5);

					isHoldingProcess = false;
					isHoldingPaint = false;
					isHoldingKey = false;
					break;
					}
					case EVT_WATCH:
					{
					isHoldingPaint = false;
					isHoldingProcess = false;
					isHoldingKey = false;
					break;
					}
					}
					*/
				}
				//				}

				break;
			}
		case EVT_START_CINEMA:
			//--------------------------------------------------------
			{
				isHoldingPaint = false;
				isHoldingProcess = false;
				isHoldingKey = false;
				RELEASE_LOADING();

				if(0 != GetData( pMsgList )->param[1])
				{
					pField->DeleteFieldImage();
					Field* ppField = GL_NEW Field((void*) &s_ASpriteSet);
					ppField->LoadMap(GetData( pMsgList )->param[1]);
					pCinema->Load_Cinematics(PACK_DATA_CINEMA, GetData( pMsgList )->param[0], 0, ppField);
				}
				else
				{
					//	현재 필드를 넘겨준다.
					pCinema->Load_Cinematics(PACK_DATA_CINEMA, GetData( pMsgList )->param[0], 0, pField);
				}

				SetFadeIn(5);
				break;
			}
		case EVT_END_CINEMA:
			//--------------------------------------------------------
			{
				isHoldingPaint = true;
				isHoldingProcess = true;
				isHoldingKey = true;

				pCinema->Release_Cinematics();

				if(pField != pCinema->pField)
				{
					SAFE_DELETE(pCinema->pField);
					pField->LoadFieldImage(pField->m_nSaveStageNum);
					pField->ReMatchFieldImage();
				}

				UpdatePARSER_EVENT();
				/*
				//	이벤트를 체크한다.
				switch(PARSER_EVENT())
				{
				case EVT_NULL:
				case EVT_TALK_STARTEVT:
				{
				isHoldingPaint = false;
				isHoldingProcess = false;
				isHoldingKey = false;
				SetFadeIn(5);
				break;
				}
				case EVT_WATCH:
				{
				isHoldingPaint = false;
				isHoldingProcess = false;
				isHoldingKey = false;
				break;
				}
				default:
				{
				isHoldingPaint = true;
				isHoldingProcess = true;
				isHoldingKey = true;
				break;
				}

				}
				*/
				break;
			}
		case EVT_CINEMA_ADDEVT:		//	씨네마시 이벤트로 넘어오는 값들
			//--------------------------------------------------------
			{
				switch(GetData( pMsgList )->param[0])
				{
				case 0:
					//------------------------------------------------------------
					{
						//	//GetData( pMsgList )->param[1] = 0;
						//	//GetData( pMsgList )->param[2] = 1;

						pCinema->Pause_Cinematics();
						pFieldUi->SetTalkCinema(GetData( pMsgList )->param[1],
							GetData( pMsgList )->param[2]);//대사창 띄우기
						//pCinema->Load_Cinematics(PACK_DATA_CINEMA, DATA_CINEMA_SCENE1, 1, pField);
						break;
					}
				}
				break;
			}
		case EVT_GAME_KEYEVENT:	//	0번은 쓰면 안된다.	//각종 퀘스트 관련 호출자들의 완료여부 체크를 담당
			//--------------------------------------------------------
			{
				//false 값이 넘어온다면 NPC와의 대화일 경우에 정상적인 대화를 호출한다
				if(CheckEvent(GetData( pMsgList )->param[0]) == false)
				{//마을대화
					pFieldUi->SetTalkNPC(pField->m_nVillageUniqueIdx, GetData( pMsgList )->param[0]);
				}
				break;
			}
		case EVT_TALK_STARTEVT:		//대사창 띄우기
			//--------------------------------------------------------
			{
				pFieldUi->SetTalkCinema(GetData( pMsgList )->param[0],
					GetData( pMsgList )->param[1]);
				break;
			}
		case EVT_TALK_ENDEVT:
			//--------------------------------------------------------
			{
				switch(GetData( pMsgList )->param[0])
				{
				case (-1):
					//------------------------------------------------------------
					{
						//	남은 씨네마가 있으면 진행해준다.
						if(true == pCinema->s_cinematicsPause) 
						{
							pCinema->Resume_Cinematics();
						}
						else
						{
							UpdatePARSER_EVENT();

							//	이벤트를 체크한다.
							//PARSER_EVENT();
						}

						break;
					}				
				}
				break;
			}
		case EVT_PROLOGUE:
			//--------------------------------------------------------
			{
				LoadPrologue(DATA_TALK_OPENING, GetData( pMsgList )->param[0]);
				break;
			}
		case EVT_WATCH:
			//--------------------------------------------------------
			{
				SetTimerEvent(GetData( pMsgList )->param[0]);
				break;
			}
		case EVT_ADD_MONSTER:
			//--------------------------------------------------------
			{
				AddRegenMonSetting(GetData( pMsgList )->param[0]);
				break;
			}
		case EVT_SET_CAHRPOS:
			//--------------------------------------------------------
			{
				//AddRegenMonSetting(GetData( pMsgList )->param[0]);
				if(hero)
				{
					hero->InitCharPos(GetData( pMsgList )->param[0], GetData( pMsgList )->param[1], GetData( pMsgList )->param[2]);
					s_Camera.cX = 0;
					SetCameraMove();
				}

				UpdatePARSER_EVENT();
				/*
				//	이벤트를 체크한다.
				switch(PARSER_EVENT())
				{
				case EVT_NULL:
				case EVT_TALK_STARTEVT:
				case EVT_SET_CAHRPOS:
				{
				isHoldingPaint = false;
				isHoldingProcess = false;
				isHoldingKey = false;
				SetFadeIn(5);
				break;
				}
				case EVT_WATCH:
				{
				isHoldingPaint = false;
				isHoldingProcess = false;
				isHoldingKey = false;
				break;
				}
				default:
				{
				isHoldingPaint = true;
				isHoldingProcess = true;
				isHoldingKey = true;
				break;
				}

				}
				*/
				break;
			}
		}

		SAFE_DELETE( GetData(pMsgList) );
		pMsgList->Delete();
	}

	//	필드 이벤트들을 날려준다.
	if(bIsCrearMsg)
	{
		CLEAR_EVENT_MSG();
	}
}

//---------------------------이거이거이거이거이거이거이거이거----------------------------------
bool GFieldBattle::CheckEvent(int param)
//--------------------------------------------------------------------------
{

	//GET_MAINQUEST_IDX() - 메인퀘스트 넘버
	switch(param)
	{
	case NPC_CALL_kail://트레져 헌터 카일
		{
			if(MAIN_QUEST_findKail == GET_MAINQUEST_IDX()){//트레져 헌터 카일을 찾아라
				//pFieldUi->SetTalkCinema(0, 0);
				SET_EVENT(4);
				PARSER_EVENT();
				return true;
			}
			if(MAIN_QUEST_oldMedal == GET_MAINQUEST_IDX()){//스몰 타운 초원 에서 낡은 메달 10개를 구하라!
				//10개 이상이냐?
				if(pPopupUi->GET_Count(ITEM_CALL_medal) >= MAX_medal){
					pPopupUi->REMOVE_Item(ITEM_CALL_medal,ALL_DEL);	
					SET_EVENT(5);
					PARSER_EVENT();
					return true;
				}
			}
			if(MAIN_QUEST_callKail == GET_MAINQUEST_IDX()){//카일의 호출
				SET_EVENT(6);
				PARSER_EVENT();
				return true;
			}
			break;
		}
	case NPC_CALL_kaien://카이엔
		{
			if(MAIN_QUEST_goKaien == GET_MAINQUEST_IDX()){//카이엔 에게 소개장을 전달 하라
				SET_EVENT(7);
				PARSER_EVENT();
				return true;
			}
			if(MAIN_QUEST_returnKaien == GET_MAINQUEST_IDX()){//카이엔 에게 돌아가라
				SET_EVENT(12);
				PARSER_EVENT();
				return true;
			}
			break;
		}
	case NPC_CALL_citizen1://시민1
		{
			if(MAIN_QUEST_goCitizen1 == GET_MAINQUEST_IDX()){//엘시안 문명의 정보를 수집하라 시민 1
				SET_EVENT(8);
				PARSER_EVENT();
				return true;
			}
			break;
		}
	case NPC_CALL_citizen2://시민2
		{
			//int aaa = GET_MAINQUEST_IDX();
			if(MAIN_QUEST_goCitizen2 == GET_MAINQUEST_IDX()){//엘시안 문명의 정보를 수집하라 시민 2
				SET_EVENT(9);
				PARSER_EVENT();
				return true;
			}
			break;
		}
	case NPC_CALL_citizenK://시민k
		{
			if(MAIN_QUEST_goCitizenK == GET_MAINQUEST_IDX()){//엘시안 문명의 정보를 수집하라 시민 K
				SET_EVENT(10);
				PARSER_EVENT();
				return true;
			}

			if(MAIN_QUEST_findCrystal == GET_MAINQUEST_IDX()){//자수정을 구해와서 K씨와 대화하라 

				//10개 이상이냐?
				if(pPopupUi->GET_Count(ITEM_CALL_crystal) >= MAX_crystal){
					pPopupUi->REMOVE_Item(ITEM_CALL_crystal,ALL_DEL);	
					SET_EVENT(11);
					PARSER_EVENT();
					return true;
				}

			}
			break;
		}
	case NPC_CALL_scholar://고고학자
		{
			if(MAIN_QUEST_goScholar == GET_MAINQUEST_IDX()){//고고학자를 찾아라
				SET_EVENT(13);
				PARSER_EVENT();
				return true;
			}
			break;
		} 
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case FIELD_CALL_caveOpen://동굴 입구
		{
			if(MAIN_QUEST_findOpen == GET_MAINQUEST_IDX()){//동굴 입구를 찾아라!!
				SET_EVENT(2);
				PARSER_EVENT();
				return true;
			}
			break;
		}
	case FIELD_CALL_smallTown://스몰 타운
		{
			if(MAIN_QUEST_goSmallTown == GET_MAINQUEST_IDX()){//일지를 돌려주자, 서브 - 미들 타운으로 가라
				SET_EVENT(14);
				PARSER_EVENT();
				return true;
			}
			break;
		}
	case FIELD_CALL_forest1://초원 첫번째 비오는 필드
		{
			if(FIELD_CALL_forest1 == GET_MAINQUEST_IDX()){//일지를 돌려주자, 서브 - 시즈를 찾아라
				SET_EVENT(15);
				PARSER_EVENT();
				return true;
			}
			break;
		}
	case FIELD_CALL_forest2://초원 두번째 비오는 필드
		{
			if(FIELD_CALL_forest2 == GET_MAINQUEST_IDX()){//일지를 돌려주자, 서브 - 바위를 부셔라
				SET_EVENT(16);
				PARSER_EVENT();
				return true;
			}
			break;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case ITEM_CALL_tntBoom://폭약
		{
			if(MAIN_QUEST_openDoor == GET_MAINQUEST_IDX()){//동굴 입구를 열어라!
				if(pPopupUi->GET_Count(ITEM_CALL_tntBoom) >= MAX_tntBoom){
					pPopupUi->REMOVE_Item(ITEM_CALL_tntBoom,ALL_DEL);
					SET_EVENT(3);
					PARSER_EVENT();
					return true;
				}
			}
			break;
		}
	case ITEM_CALL_diary://일지
		{
			if(MAIN_QUEST_findDiary == GET_MAINQUEST_IDX()){//잃어버린 일지를 찾아라
				if(pPopupUi->GET_Count(ITEM_CALL_diary) >= MAX_diary){
					SET_EVENT(14);
					PARSER_EVENT();
					return true;
				}
			}
			break;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	case TIMER_CALL_end://타이머
		{
			//switch(GET_MAINQUEST_IDX()){//동굴 입구를 열어라!
			//	case kfhasldfkj:
			//			if(바위 다 파괴){
			//			}else{
			//				SET_EVENT(3);
			//				PARSER_EVENT();
			//				pPopupUi->REMOVE_Item(ITEM_CALL_tntBoom,1);
			//			}
			int a = 0;
			break;

		}


	}
	return false;
}
//--------------------------------------------------------------------------
void GFieldBattle::SetFadeOut(int time)
//--------------------------------------------------------------------------
{
	m_nIsDarkScreen = true;
	if(0 == time)	{time = 5;}
	m_nFadeOutTime = time;
}


//--------------------------------------------------------------------------
void GFieldBattle::SetFadeIn(int time)
//--------------------------------------------------------------------------
{
	m_nIsDarkScreen = false;
	if(0 == time)	{time = 5;}
	m_nFadeInTime = time;
}

//--------------------------------------------------------------------------
void GFieldBattle::LoadPrologue(int idx, int textIdx)
//--------------------------------------------------------------------------
{

	m_nPrologueTimer = 0;
	m_bIsPlayPrologue = true;
	pFieldUi->SetTalkCinema(idx-DATA_TALK_SCENE_00, textIdx);
}


//--------------------------------------------------------------------------
bool GFieldBattle::UpdatePrologue()
//--------------------------------------------------------------------------
{
	if(false == m_bIsPlayPrologue)	{return false;}
	//	if(false == pFieldUi->s_TalkBox.View)	{return false;}

	m_nPrologueTimer++;

	if( 40 < m_nPrologueTimer)
	{
		m_nPrologueTimer = 0;							//	다음 신으로 업데이트 해준다.
		pFieldUi->KeyTalkBox(MH_KEY_5, false);			//	키 이벤트

		if(false == pFieldUi->s_TalkBox.View)
		{
			m_bIsPlayPrologue = false;
		}
	}

	return true;
}

//--------------------------------------------------------------------------
bool GFieldBattle::PaintPrologue()
//--------------------------------------------------------------------------
{
	if(false == m_bIsPlayPrologue)	{return false;}
	//	if(false == pFieldUi->s_TalkBox.View)	{return false;}

	//	페이드 아웃

	if(30 < m_nPrologueTimer)
	{
		SUTIL_SetColor(0x000000);
		SUTIL_FillRect(0,0,240,320);
		return true;
	}

	if(20 < m_nPrologueTimer)
	{
		Paint_ChangeScrollEvent(1);
		return true;
	}

	SUTIL_SetColor(0x000000);
	SUTIL_FillRect(0,0,240,320);

	pFieldUi->PaintTalkBox();

	//	페이드 인
	if(5 > m_nPrologueTimer)
	{
		for(int loop = 0; loop < 5-m_nPrologueTimer; loop++)
		{
			Paint_ChangeScrollEvent(1);
		}
	}

	return true;
}


//--------------------------------------------------------------------------
void GFieldBattle::ProcessTimerEvent()
//--------------------------------------------------------------------------
{
	if(0 < m_nSaveEvtTimer)
	{
		m_nSaveEvtTimer--;

		if(0 == m_nSaveEvtTimer)
		{
			ADD_EVENT_MSG(EVT_GAME_KEYEVENT, 0, TIMER_CALL_end);
			//	이벤트를 보낸다.
		}
	}
}


//--------------------------------------------------------------------------
void GFieldBattle::SetTimerEvent(int nTime)
//--------------------------------------------------------------------------
{
	m_nSaveEvtTimer = nTime;
}


//--------------------------------------------------------------------------
void GFieldBattle::UpdatePARSER_EVENT()
//--------------------------------------------------------------------------
{
	//	로딩이 끝났을 경우
	RELEASE_LOADING();

	switch(PARSER_EVENT())
	{
	case EVT_NULL:
	case EVT_TALK_STARTEVT:
		{
			if(m_nIsDarkScreen)
			{
				SetFadeIn(5);
			}
			isHoldingProcess = false;
			isHoldingPaint = false;
			isHoldingKey = false;
			break;
		}

		//EVT_CHANGE_STAGE
		//EVT_WATCH
		//EVT_SET_CAHRPOS
		//		case EVT_START_CINEMA:
		//
		//		{
		//		}
	}
}

/*
//--------------------------------------------------------------------------
void GFieldBattle::ProcessWorldMap()
//--------------------------------------------------------------------------
{
//	pCinema->Update_Cinematics();
}


//--------------------------------------------------------------------------
void GFieldBattle::PaintWorldMap()
//--------------------------------------------------------------------------
{

SUTIL_SetColor(0x000000);
SUTIL_FillRect(0,0,240,320);

//	pCinema->Paint_Cinematics();

switch(m_nEventIndex)
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
void GFieldBattle::KeyEvtWorldMap(int m_keyCode, int m_keyRepeat)
//--------------------------------------------------------------------------
{
switch(m_nEventIndex)
{
case 1000:
//------------------------------------------------------------
{
switch(m_keyCode)
{
case MH_KEY_1:	{AppMain::ResvNextState(MS_GAME_FIELD, 801, 0);	break;}
case MH_KEY_2:	{AppMain::ResvNextState(MS_GAME_FIELD, 101, 1950);		break;}
case MH_KEY_3:	{AppMain::ResvNextState(MS_GAME_FIELD, 200, 0);	break;}
case MH_KEY_4:	{AppMain::ResvNextState(MS_GAME_FIELD, 104, 0);		break;}
}

break;
}
case 1001:
//------------------------------------------------------------
{
switch(m_keyCode)
{
case MH_KEY_1:	{AppMain::ResvNextState(MS_GAME_FIELD, 803, 0);		break;}
case MH_KEY_2:	{AppMain::ResvNextState(MS_GAME_FIELD, 105, 450);		break;}
case MH_KEY_3:	{AppMain::ResvNextState(MS_GAME_FIELD, 113, 50);		break;}
case MH_KEY_4:	{AppMain::ResvNextState(MS_GAME_FIELD, 205, 50);		break;}
case MH_KEY_5:	{AppMain::ResvNextState(MS_GAME_FIELD, 104, 1450);		break;}
}

break;
}

}
}
*/

