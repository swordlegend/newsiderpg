#include "Character.h"
#include "ASpriteInstance.h"
#include "Position3D.h"
#include "Physics.h"
//#include "UI_GameWG.h"

//int Character::x;
//int Character::y;
//int Character::posX;
//int Character::posY;

#include "Macro.h"


	HeroAbility Character::s_Ability;
	HeroStatus Character::s_Status;
	Skill Character::s_Skill;
	Skill_Set Character::s_Skill_Set;


	ItemBag Character::s_ItemEquip[10];
	ItemAbility Character::s_ItemAbil[10];
	ItemBag Character::s_ItemSlot[4];
	ItemBag Character::s_ItemBag[4][32];
	ItemBag Character::s_ItemPick;
	MustAction Character::s_MustAction;
	class ASprite*	Character::_spr_Hero;



	Character::Character(bool Woman, int Look, void* s__ASpriteSet)
	{


		s_Status.SEX = SAVELOAD_sex;

		s_ASpriteSet = (ASpriteSet*)s__ASpriteSet;//Void 포인터를 스프라이트 포인트struct 포인터로 치환

		_ins_Debuff = GL_NEW ASpriteInstance(s_ASpriteSet->pDebuffAs, 100, 240, NULL);//디버프 인스턴스


		_spr_Hero = SUTIL_LoadSprite(PACK_SPRITE, Check_sex(SPRITE_MAN_BODY,SPRITE_WOMAN_BODY));
		
		_ins_Hero = GL_NEW ASpriteInstance(_spr_Hero, 100, 250, NULL);
		_spr_Hero->SetBlendFrame(Check_sex(FRAME_MAN_BODY_BLEND,FRAME_WOMAN_BODY_BLEND));
		InitCostume();




		m_Hero_Physics = GL_NEW Physics(HERO_WRIGHT);






		{
			for (int xx = 0;xx<5;xx++)
				s_Skill.Equip_A[xx] = -1;
			for (int xx = 0;xx<9;xx++)
				s_Skill.Equip_P[xx] = -1;


			s_Skill.Equip_A[0] = -1;
			s_Skill.Equip_A[1] = -1;
			s_Skill.Equip_A[2] = -1;
			s_Skill.Equip_A[3] = -1;
			s_Skill.Equip_A[4] = -1;


			//for(int xx = 0;xx<21;xx++){
			//	s_Skill.Level_A[xx]=1;	//액티브 스킬레벨
			//}
			s_Skill.Level_A[4]=1;
			s_Skill.Level_A[7]=1;
			s_Skill.Level_A[14]=1;

			for(int xx = 0;xx<28-1;xx++){
				s_Skill.Level_P[xx]=1;	//패시브 스킬레벨
			}
		}

		{//initial - 케릭터 수치관련 초기화

			s_Ability.JAB_KNIFE	=false;	//검사 - 직업 선택 가능하다면 true
			s_Ability.JAB_GUN	=false;	//총사 - 직업 선택 가능하다면 true
			s_Ability.JAB_MAGIC	=false;	//술사 - 직업 선택 가능하다면 true

			s_Status.LEVEL = 1;
			switch(SAVELOAD_MainStyle){
				case 0:
					s_Skill.Equip_A[0] = 4;
					s_Ability.JAB_KNIFE	=true;	//검사 - 직업 선택 가능하다면 true
					s_Ability.STR = 7 + (s_Status.LEVEL-1)*2;
					s_Ability.DEX = 0 + (s_Status.LEVEL-1)*2;	//민첩-건너 공격력(100%) , 회피,크리
					s_Ability.CON = 10 + (s_Status.LEVEL-1)*2;	//체력-생명력, 방어력(30%),생명력 회복률
					s_Ability.INT = 3 + (s_Status.LEVEL-1)*2;	//지능-마법사 공격력(건너의 130%) , 마나
					s_Ability.FAM = 0 + (s_Status.LEVEL-1)*2;	//명성-별도 스탯 , 퀘스트 습득 및 마을 사람과의 대화 필요
					break;
				case 1:
					s_Skill.Equip_A[0] = 7;
					s_Ability.JAB_GUN	=true;	//총사 - 직업 선택 가능하다면 true
					s_Ability.STR = 3 + (s_Status.LEVEL-1)*2;
					s_Ability.DEX = 7 + (s_Status.LEVEL-1)*2;	//민첩-건너 공격력(100%) , 회피,크리
					s_Ability.CON = 5 + (s_Status.LEVEL-1)*2;	//체력-생명력, 방어력(30%),생명력 회복률
					s_Ability.INT = 5 + (s_Status.LEVEL-1)*2;	//지능-마법사 공격력(건너의 130%) , 마나
					s_Ability.FAM = 0 + (s_Status.LEVEL-1)*2;	//명성-별도 스탯 , 퀘스트 습득 및 마을 사람과의 대화 필요
					break;
				case 2:
					s_Skill.Equip_A[0] = 14;
					s_Ability.JAB_MAGIC	=true;	//술사 - 직업 선택 가능하다면 true
					s_Ability.STR = 0 + (s_Status.LEVEL-1)*2;
					s_Ability.DEX = 5 + (s_Status.LEVEL-1)*2;	//민첩-건너 공격력(100%) , 회피,크리
					s_Ability.CON = 3 + (s_Status.LEVEL-1)*2;	//체력-생명력, 방어력(30%),생명력 회복률
					s_Ability.INT = 10 + (s_Status.LEVEL-1)*2;	//지능-마법사 공격력(건너의 130%) , 마나
					s_Ability.FAM = 0 + (s_Status.LEVEL-1)*2;	//명성-별도 스탯 , 퀘스트 습득 및 마을 사람과의 대화 필요
					break;
			}
			
			s_Status.ELEMENTAL = _b_JabNum = (s_Ability.JAB_KNIFE?0:(s_Ability.JAB_GUN?1:2));
			s_Status.EXP_MAX = 0;

			Init_Skill();// 각 위치에 스킬 스프라이트를 할당한다.    ☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
			Set_state_calculate();

			s_Status.LIFE = s_Status.LIFE_MAX;
			s_Status.MANA = s_Status.MANA_MAX;
			s_Status.EXP = 0;

			s_Skill_Set.Cool_TimeMax[5] = 20;
			s_Status.Qslot[0]=-1;//퀵슬롯 비우기
			s_Status.Qslot[1]=-1;//퀵슬롯 비우기
		}
		_m_test=1;
		s_MustAction.must_Action = -1;
		_m_actNum = HERO_STOP;//SangHo - 주인공 케릭터의 액션 상태값
		_b_LookRight = Look;

		s_KeyQueue.delTick=5;
		s_Damage.Type = DAMAGE_NOT;


	}
	Character::~Character()
	{
		SUTIL_FreeSpriteInstance(_ins_Debuff);
		SUTIL_FreeSpriteInstance(_ins_Hero);
		SUTIL_FreeSprite(_spr_Hero);


		for (int xx = 0; xx<5; xx++)
		{
			SUTIL_FreeSpriteInstance(_ins_Skill[xx][0]);
			SUTIL_FreeSpriteInstance(_ins_Skill[xx][1]);
			SUTIL_FreeSprite(_spr_Skill[xx]);
		}

		SAFE_DEL(m_Hero_Physics);

	}
	void Character::KeyEvent(int m_keyCode, int m_keyRepeat)
	{

		if(m_keyCode == MH_KEY_4)m_keyCode=MH_KEY_LEFT;		//값대치
		if(m_keyCode == MH_KEY_6)m_keyCode=MH_KEY_RIGHT;	//값대치
		if(m_keyCode == MH_KEY_2)m_keyCode=MH_KEY_UP;		//값대치
		if(m_keyCode == MH_KEY_8)m_keyCode=MH_KEY_DOWN;		//값대치
		if(m_keyCode == MH_KEY_5)m_keyCode=MH_KEY_SELECT;	//값대치


		if(TOWN){//마을 안이면 공격키를 모두 캔슬한다
			switch(m_keyCode){
				case MH_KEY_SELECT:
				case MH_KEY_1:
				case MH_KEY_3:
				case MH_KEY_7:
				case MH_KEY_9:
				case MH_KEY_0:
					m_keyCode=0;
					break;
			}
		}
		

		if(_b_Key_Nullity){return;}//키입력을 무효화 한다. //키제어 구조체를 만들것
		

		if(Check_command(m_keyCode, m_keyRepeat)){	//m_keyCode 가 커맨드 대상키이다-> 큐에 추가 대었다면

			int temp_Num = Event_command();

			if(temp_Num){						//커맨드 판정이 났다면
				_move_Order = temp_Num;							//커맨드 부여
			}else{

				if(_b_Key_Protect){//키 보호상태라면

					_move_Order = Event_skill(m_keyCode,m_keyRepeat,_move_Order);

				}else//키 보호상태가 아니라면 
				_move_Order = Event_move(m_keyCode,m_keyRepeat);		//초기행동 부여
			}			

		}else{										//커맨드키가 추가되지 않았다면
			if(_b_Key_Protect){//키 보호상태라면

				_move_Order = Event_skill(m_keyCode,m_keyRepeat,_move_Order);

			}else//키 보호상태가 아니라면 
			_move_Order = Event_move(m_keyCode,m_keyRepeat);	//초기행동 부여
		}
	}
	void Character::Process()
	{
		NUMBER++;//증가자

		

		if(_move_Order == HERO_STOP)Recovery();//매 프레임마다 자동적으로 회복

		_move_Order = SetDamageDecide();//SangHo - 물리값에 의한 이벤트 처리 



		SetActionDecide(_move_Order);//SangHo - 가상의 이벤트 처리
	}
	void Character::Paint()//SangHo - flags값을 넣지않을경우 0으로 간주한다
	{


		
		int tmpXZ[2];//리턴받을 배열
		_ins_Hero->Get_AFrameXZ(&tmpXZ[0]);
		int tmpx = tmpXZ[0];
		char tmpz = tmpXZ[1];
		m_HEADTOP = tmpz-HERO_HEIGHT;//주인공 머리꼭대기 좌표
		
		//	paint shadow
		SUTIL_Paint_Frame(s_ASpriteSet->pShadowAs ,FRAME_SHADOW_SHADOW_2 , tmpx + _ins_Hero->CameraX , _ins_Hero->m_posY,0);

		//	paint 발사형 스킬 이펙트
		Paint_Knife();

		//	paint DOWN skill
		if(s_Skill_Set.DOWN_SkillEffect)
		{
			//_ins_Skill[s_Skill_Set.Num][1]->m_posX = _ins_Hero->m_posX;//스킬좌표에 케릭터 좌표를 반영한다
			//_ins_Skill[s_Skill_Set.Num][1]->m_posY = _ins_Hero->m_posY;//스킬좌표에 케릭터 좌표를 반영한다
			SUTIL_SetDirAsprite(_ins_Skill[s_Skill_Set.Num][1], _b_LookRight);
			SUTIL_PaintAsprite(_ins_Skill[s_Skill_Set.Num][1],S_INCLUDE_SORT);
		}

		//	paint character
		SUTIL_PaintAsprite(_ins_Hero,S_INCLUDE_SORT);

		//	paint Debuff
		Paint_Debuff(tmpx , tmpz);

		//	paint OVER skill
		if(s_Skill_Set.OVER_SkillEffect)
		{
			//_ins_Skill[s_Skill_Set.Num][0]->m_posX = _ins_Hero->m_posX;//스킬좌표에 케릭터 좌표를 반영한다
			//_ins_Skill[s_Skill_Set.Num][0]->m_posY = _ins_Hero->m_posY;//스킬좌표에 케릭터 좌표를 반영한다
			SUTIL_SetDirAsprite(_ins_Skill[s_Skill_Set.Num][0], _b_LookRight);
			SUTIL_PaintAsprite(_ins_Skill[s_Skill_Set.Num][0],S_INCLUDE_SORT);
		}
		


		Paint_Bollet();//발사형 스킬 이펙트

		Paint_LevelUp();//레벨업 이펙트

		Check_Sound();
		

		
		//if(_b_Effect).
		//{
		//	SUTIL_SetDirAsprite(_ins_Skill[s_Skill_Set.Num], _b_LookRight);
		//	SUTIL_PaintAsprite(_ins_Skill[s_Skill_Set.Num],S_INCLUDE_SORT);
		//}
	}
///////////////////////////////////////////////////////////////////
///////////////private/////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
	/******************************************  KeyEvent  ***************************************/
	int Character::Event_command(){

		if(s_Skill_Set.act)return false;//★스킬중에는 커맨드를 입력은 받지만 동작은 하지않도록 한다.

		for(int xx = 0, length = sizeof(s_C_Skill)/(sizeof(short)*cSkill_length) ; xx < length ; xx++){
			if(s_KeyQueue.count >= s_C_Skill[xx].count)
			{
				bool Temp_command = true;
				for(int yy = 0 ; yy<s_C_Skill[xx].count ; yy++){
					if(s_KeyQueue.full[ (s_KeyQueue.count - s_C_Skill[xx].count) + yy] != s_C_Skill[xx].c_ins_Skill[yy] ){
						Temp_command = false;
						break;
					}
				}

				if(Temp_command){

					switch(s_C_Skill[xx].name){
						case HERO_LINE_MOVE_UP	:if(!Get_Skill(SKILL_P_P_lineMove)){return _move_Order;}break;//패시브
						case HERO_LINE_MOVE_DOWN:if(!Get_Skill(SKILL_P_P_lineMove)){return _move_Order;}break;//패시브
						case HERO_CHARGE		:
							if(!Get_Skill(Check_jab(SKILL_P_P_guard,SKILL_P_P_chargeShot,SKILL_P_P_manaCharge))){return false;}//패시브
							if(_b_JabNum==JAB_KNIGHT && (s_Status.MANA < PER(s_Status.MANA_MAX,30))){return false;}//가드 - 마나가 부족하면 발동하지않는다

							break;
					}

					if(HERO_LINE_MOVE_UP == _move_Order)return _move_Order;//행동 변화없음 - 라인 이동중에는 행동보장
					if(HERO_LINE_MOVE_DOWN == _move_Order)return _move_Order;//행동 변화없음 - 라인 이동중에는 행동보장
					if(HERO_DASH_RIGHT == s_C_Skill[xx].name){_m_dashRepeat=0; Set_left_right(SDIR_RIGHT);}
					if(HERO_DASH_LEFT == s_C_Skill[xx].name){_m_dashRepeat=0; Set_left_right(SDIR_LEFT);}
					if(HERO_CHARGE == s_C_Skill[xx].name && HERO_STOP != _move_Order){
						return false;
					}//커맨드를 스탠딩자세까지로 미룬다

					//_b_ActionEnd = false;
					_b_Key_Protect=true;
					return s_C_Skill[xx].name;
				}
			}
		}

		return 0;
	}
	int Character::Event_move(int m_keyCode,int m_keyRepeat){
		//s_Throw.ThrowPossible = false;//초기화들 관련해서는 나중에 초기화 관련 메소드 제작할것
		_m_Hero_AttackNum=0;
		switch(m_keyCode){
			case 0:
				return HERO_STOP;
			case MH_KEY_LEFT:
				return HERO_WALK_LEFT;

			case MH_KEY_RIGHT:
				return HERO_WALK_RIGHT;

			case MH_KEY_UP:
				return HERO_WALK_UP;

			case MH_KEY_DOWN:
				return HERO_WALK_DOWN;

			case MH_KEY_SELECT:
				if(m_keyRepeat)return 0;//키릴리즈에 반응하지않는다.

				if(Get_Skill(SKILL_P_P_catch)){
					s_Throw.ThrowPossible = true;//잡기
					s_Throw.ThrowNum = 0;//잡기
				}
				_b_Key_Protect=true;
				return HERO_ATTACK1; //HERO_THROW;

			case MH_KEY_1:
				if(m_keyRepeat || Check_skill_impossible(0))return 0;//스킬을 쓸 수 없는 상황이다
				
				s_Skill_Set.Input_Key=MH_KEY_1;
				return Check_weapon( s_Skill_Set.Skill_ID[0]/7 , HERO_SKILL_0);

			case MH_KEY_3:
				if(m_keyRepeat || Check_skill_impossible(1))return 0;//스킬을 쓸 수 없는 상황이다

				s_Skill_Set.Input_Key=MH_KEY_3;
				return Check_weapon( s_Skill_Set.Skill_ID[1]/7 , HERO_SKILL_1);

			case MH_KEY_7:
				if(m_keyRepeat || Check_skill_impossible(2))return 0;//스킬을 쓸 수 없는 상황이다

				s_Skill_Set.Input_Key=MH_KEY_7;
				return Check_weapon( s_Skill_Set.Skill_ID[2]/7 , HERO_SKILL_2);

			case MH_KEY_9:
				if(m_keyRepeat || Check_skill_impossible(3))return 0;//스킬을 쓸 수 없는 상황이다

				s_Skill_Set.Input_Key=MH_KEY_9;
				return Check_weapon( s_Skill_Set.Skill_ID[3]/7 , HERO_SKILL_3);

			case MH_KEY_0:
				if(m_keyRepeat || Check_skill_impossible(4))return 0;//스킬을 쓸 수 없는 상황이다

				s_Skill_Set.Input_Key=MH_KEY_0;
				return Check_weapon( s_Skill_Set.Skill_ID[4]/7 , HERO_SKILL_4);


			case MH_KEY_ASTERISK:
				if(m_keyRepeat)return 0;//키릴리즈에 반응하지않는다.

				//_b_ActionEnd = false;
				_b_Key_Protect=true;
				return HERO_SWITCH;

			case MH_KEY_POUND:
				if(m_keyRepeat || Check_skill_impossible(5))return 0;//스킬을 쓸 수 없는 상황이다


				PopupUi::USE_item(&s_ItemBag[s_Status.Qslot[0]][s_Status.Qslot[1]]);
				return 0;
			case MH_KEY_SEND:
				for(int xx = 0;SUBQUEST_MAX>xx;xx++)SUBQUEST_IDX(xx)=QUEST_FIN;			
				break;
			case MH_KEY_SOFT2:
				s_Ability.FAM++;
				SUTIL_LoadAspritePack(PACK_SPRITE_COSTUME);//팩열기
/////////////////////////////////////
 				ChangeCostume(PAL_HEAD,RND(0,4),RND(0,4));
 				ChangeCostume(PAL_BODY,RND(0,4),RND(0,4));
 				ChangeCostume(PAL_ARM,RND(0,4),RND(0,4));
 				ChangeCostume(PAL_LEG,RND(0,4),RND(0,4));
 ////////////////////////////////////
 				ChangeCostume(PAL_KNIFE1,RND(0,9),RND(0,1));
 				ChangeCostume(PAL_KNIFE2,RND(0,9),RND(0,1));
 				ChangeCostume(PAL_GUN,RND(0,9),RND(0,1));
 				ChangeCostume(PAL_ORB,RND(0,9),RND(0,1));
///////////////////////////////////
				SUTIL_ReleaseAspritePack();//팩닫기
				return 0;


		}

		return 0;

	}
	int Character::Event_skill(int m_keyCode,int m_keyRepeat,int m_actNum){

		switch(m_actNum){
				case HERO_STOP:
					break;

				case HERO_WALK_LEFT:
					break;

				case HERO_WALK_RIGHT:
					break;

				case HERO_WALK_UP:
					break;

				case HERO_WALK_DOWN:
					break;

				case HERO_ATTACK1:
					return Set_skill_chain(0, m_keyCode, MH_KEY_SELECT, HERO_ATTACK1, HERO_ATTACK2, HERO_ATTACK1END);
					break;

				case HERO_ATTACK2:
					return Set_skill_chain(1, m_keyCode, MH_KEY_SELECT, HERO_ATTACK2, HERO_ATTACK3, HERO_ATTACK2END);
					break;

				case HERO_ATTACK3:
					if(_b_JabNum==0)
						return Set_skill_chain(2, m_keyCode, MH_KEY_SELECT, HERO_ATTACK3, HERO_ATTACK4, HERO_ATTACK3END);
					else
						return Set_skill_chain(2, m_keyCode, MH_KEY_SELECT, HERO_ATTACK3, HERO_STOP, HERO_STOP);//마법과 총은 3연타로끝남
					break;

				case HERO_ATTACK4:
					return Set_skill_chain(3, m_keyCode, MH_KEY_SELECT, HERO_ATTACK4, HERO_STOP, HERO_STOP);
					break;

				case HERO_ATTACK1END:
					if(_b_ActionEnd){
						return HERO_STOP;
					}
					return HERO_ATTACK1END;
					break;

				case HERO_ATTACK2END:
					if(_b_ActionEnd){
						return HERO_STOP;
					}
					return HERO_ATTACK2END;
					break;

				case HERO_ATTACK3END:
					if(_b_ActionEnd){
						return HERO_STOP;
					}
					return HERO_ATTACK3END;
					break;

				case HERO_SKILL_0:
				case HERO_SKILL_1:
				case HERO_SKILL_2:
				case HERO_SKILL_3:
				case HERO_SKILL_4:
					switch( s_Skill_Set.Skill_ID[s_Skill_Set.Num] ){//예외적인 스킬들
						case 0://어검술
							if(_b_ActionEnd){
								_b_Knife_SetNum = s_Skill_Set.Num; //이스킬을 사용중에 다른스킬을 쓰면 Num값이 바뀌므로 저장필요
								s_Knife_Eff[3].act = true;
								return 0;
							}
							return m_actNum;
							break;
						case 1://보드타기
							if(_b_ActionEnd){
								s_Throw.ThrowPossible = true;//잡기
								s_Throw.ThrowNum = 2;//잡기
								return HERO_SKILL_E1;
							}
							return m_actNum;
							break;

						case 6://칼 공중 잡기스킬
							return Set_skill_chain(0, m_keyCode, s_Skill_Set.Input_Key, m_actNum, HERO_SKILL_C2, HERO_SKILL_E1);
							break;

						case 13://이퀄리브리엄
							return Set_skill_chain(0, m_keyCode, s_Skill_Set.Input_Key, m_actNum, HERO_SKILL_C2, HERO_SKILL_E1);
							break;

						
						case 14:
							if(_ins_Hero->m_nCrtFrame == 1)s_Bullet_Eff[0].act = true;//발사
							if(_ins_Hero->m_nCrtFrame == 4)s_Bullet_Eff[1].act = true;//발사
							if(_b_ActionEnd){
								return 0;
							}
							break;

						case 15:
							if(_b_ActionEnd){
								s_Bullet_Eff[3].act = true;//발사
								return 0;

							}
							break;

						case 20:
							if(_ins_Hero->m_nCrtFrame == 10)s_Bullet_Eff[2].act = true;//발사
							if(_b_ActionEnd){
								return 0;
							}
							break;
							

						default:
							if(_b_ActionEnd){
								return 0;
							}
							return m_actNum;
							break;
					}
					break;
				case HERO_SKILL_C2:
					if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 1){
						if(_b_ActionEnd){return 0;}
					}else{
						return  Set_skill_chain(1, m_keyCode, s_Skill_Set.Input_Key, m_actNum, HERO_SKILL_C3, HERO_SKILL_E2);
					}
					break;
				case HERO_SKILL_C3:
					if(_b_ActionEnd){
						if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 6){
							s_Throw.ThrowPossible = true;//잡기
							s_Throw.ThrowNum = 1;//잡기
						}
						return HERO_SKILL_E3;
					}
					break;
				case HERO_SKILL_C4:
					if(_b_ActionEnd){return 0;}
					break;


				case HERO_SKILL_E1:
					if(_b_ActionEnd){return 0;}
					break;
				case HERO_SKILL_E2:
					if(_b_ActionEnd){return 0;}
					break;
				case HERO_SKILL_E3:
					if(_b_ActionEnd){return 0;}
					break;


				case HERO_SWITCH:
					if(_b_ActionEnd){//스위칭 액션이 끝났을때 정자세로 돌아갈것인가? 아니면 이어지는 스킬이 연계 될것인가?

						if(s_WeaponSwitch.act){
							s_WeaponSwitch.act = false;
							return s_WeaponSwitch.next_Event;
						}else{
							return HERO_STOP;
						}

					}
					return HERO_SWITCH;
					break;

				case HERO_LINE_MOVE_UP:
				case HERO_LINE_MOVE_DOWN:
					if(_b_ActionEnd){
						_b_Key_Protect=false;
						return 0;
					}
					break;

				case HERO_DASH_RIGHT:
				case HERO_DASH_LEFT:
					if(m_keyCode == MH_KEY_SELECT){
						if(Get_Skill(SKILL_P_P_dashAtt)){
							return HERO_DASH_ATT;
						}
					}
					if(m_keyCode ==  MH_KEY_1 || m_keyCode ==  MH_KEY_3 || m_keyCode ==  MH_KEY_7 || m_keyCode ==  MH_KEY_9 || m_keyCode ==  MH_KEY_0){
						return Event_move(m_keyCode,m_keyRepeat);//대쉬중 스킬사용
					}

					if(_m_dashRepeat<4){
						_m_dashRepeat++;
						return m_actNum;
					}else{
						return HERO_DASH_STOP;
					}

					
					break;

				case HERO_DASH_STOP:
				case HERO_DASH_ATT:
					if(_b_ActionEnd){
						return HERO_STOP;
					}
					break;

				case HERO_CHARGE:
					if(m_keyCode == MH_KEY_SELECT){
						switch(_b_JabNum){
							case JAB_KNIGHT:
								_ins_Hero->m_bLoop = true;
								break;
							case JAB_GUNNER:
								if(_b_ActionEnd){
									_ins_Hero->m_bLoop = true;
									_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_G_CHARGE_01);
								}
								break;
							case JAB_MAGE:
								_ins_Hero->m_bLoop = true;

								s_Status.MANA += s_Status.MANA_MAX*(NUMBER+1)/50 - s_Status.MANA_MAX*(NUMBER)/50 ;//마나 채우기
								if(s_Status.MANA > s_Status.MANA_MAX)
									s_Status.MANA = s_Status.MANA_MAX;
								

								break;
						}
					}else{//놓으면
						if(_b_JabNum == JAB_GUNNER){
							if(_ins_Hero->m_nCrtModule==ANIM_WOMAN_BODY_A_G_CHARGE_01)
								return HERO_CHARGE_SHOOT;
						}
						s_KeyQueue.count=0;//인덱스 초기화//축기는 해제시 큐풀을 초기화해준다
						return HERO_STOP;
					}
					
					break;
				case HERO_CHARGE_SHOOT:
					if(_b_ActionEnd){
						s_KeyQueue.count=0;//인덱스 초기화//축기는 해제시 큐풀을 초기화해준다
						return HERO_STOP;
					}
					break;

				case DAMAGE_FLY:
					break;

				case DAMAGE_GROUND:
					break;
					
				case HERO_DOWNED:
					break;

				case HERO_AWAKE:
					break;

				case HERO_THROW:
					if(_b_ActionEnd){
						return HERO_STOP;
					}
					break;

				case HERO_JumpUp:
				case HERO_JumpDown:
					if(_b_ActionEnd){
						_ins_Hero->b_MoveLock = false;
						s_MustAction.must_Action = -1;
						_b_Key_Protect=false;
						return HERO_STOP;
					}
					break;

		}

		return m_actNum;	
		
	}
	/******************************************  Process  ***************************************/	
	void Character::Recovery(){//매 프레임마다 자동적으로 취해지는 연산
		
		if(s_Status.LIFE<=0)//주인공 사망시 작동하지않음
			return;

		int FNFH = FULL_NEED_FRAME_HP * 100 /(100+Get_Skill(SKILL_P_O_recoveryUp));//패시브
		int FNFM = FULL_NEED_FRAME_MP * 100 /(100+Get_Skill(SKILL_P_O_recoveryUp));//패시브

		if(s_Status.LIFE < s_Status.LIFE_MAX){
			s_Status.LIFE += s_Status.LIFE_MAX*(NUMBER+1)/FNFH - s_Status.LIFE_MAX*(NUMBER)/FNFH ;//피 채우기
			if(s_Status.LIFE > s_Status.LIFE_MAX)s_Status.LIFE = s_Status.LIFE_MAX;
		}

		
		if(s_Status.MANA < s_Status.MANA_MAX){
			s_Status.MANA += s_Status.MANA_MAX*(NUMBER+1)/FNFM - s_Status.MANA_MAX*(NUMBER)/FNFM ;//마나 채우기
			if(s_Status.MANA > s_Status.MANA_MAX)s_Status.MANA = s_Status.MANA_MAX;
		}
	}
	int Character::SetDamageDecide(){

		
		if(s_Throw.act){//던지기 무적
			switch(s_Throw.ThrowNum){
				case 0://일반던지기
					s_Throw.act = false;
					_b_Hero_Protect = true;
					_b_Key_Protect = true;
					return HERO_THROW;
				case 1://공중잡기
					s_Throw.act = false;
					return HERO_SKILL_C4;
				case 2://보드타기
					s_Throw.act = false;
					return HERO_SKILL_C2;
			}
		}
		

		if(_b_Hero_Protect){// 충격 리액션 없음
			s_Damage.Type = DAMAGE_NOT;
			_b_Key_Nullity = false;
			return _move_Order;
		}


		if(s_Damage.Type == DAMAGE_FLY){//공중으로 날으는 데미지
			Position3D Move = m_Hero_Physics->process();

			_ins_Hero->m_posX += Move.x;
			_ins_Hero->m_posY += Move.y;
			_ins_Hero->m_posZ += Move.z;

			if(_ins_Hero->m_posZ >= 0){//땅에 도달
				_ins_Hero->m_posZ = 0;

				m_Hero_Physics->setRebound();
				s_Damage.Bound++;
				s_Damage.Bound_Num=0;


				if(s_Damage.Bound > 4 && !m_Hero_Physics->SaveAccel.x && !m_Hero_Physics->SaveAccel.z){//바운드 끝
					s_Damage.Type = DAMAGE_NOT;

					_b_Must_Decide = true;
					return HERO_DOWNED;
				}
				
				
			}
			Set_fly_motion(s_Damage,Move);//현재 인덱스와 물리량을 기준으로 취해야할 행동을 지정해준다

			s_Damage.Bound_Num++;//바운드 인덱스 증가

			return DAMAGE_FLY;

		}else if(s_Damage.Type == DAMAGE_GROUND){//땅에서 밀려나는 데미지(넘어지지 않음)
			Position3D Move = m_Hero_Physics->process();

			if(s_Damage.Direction == D_RIGHT){//물리량이 오른쪽으로 적용되고 있을때
				if(Move.x > 0){
					_ins_Hero->m_posX += Move.x;
				}else{
					if(s_Damage.Bound){
						s_Damage.Type = DAMAGE_NOT;
						_b_Must_Decide = false;
						return HERO_STOP;
					}
					s_Damage.Bound++;//움직임이 멈추면 바운드 증가
				}
			}else{//물리량이 왼쪽으로 적용되고 있을때
				if(Move.x < 0){
					_ins_Hero->m_posX += Move.x;
				}else{
					if(s_Damage.Bound){
						s_Damage.Type = DAMAGE_NOT;
						_b_Must_Decide = false;
						return HERO_STOP;
					}
					s_Damage.Bound++;//움직임이 멈추면 바운드 증가
				}
			}


			Set_fly_motion(s_Damage,Move);//현재 인덱스와 물리량을 기준으로 취해야할 행동을 지정해준다

			s_Damage.Bound_Num++;//바운드 인덱스 증가

			return DAMAGE_GROUND;
		}

		return _move_Order;
	}
	void Character::SetActionDecide(int m_actNum)
	{//_ins_Hero Animation 값 변경 및 기타 수치변경 - 움직임 제어 
		//_b_ActionEnd = false;

		//if(_b_ActionEnd){_move_Order = HERO_STOP;_b_Must_Decide=false;}

		if(s_MustAction.must_Action>=0){//강제적으로 취해야 할 행동이 있다면
			_ins_Hero->m_posX = s_MustAction.must_X;
			_ins_Hero->m_posY = s_MustAction.must_Y;
			_move_Order = m_actNum = s_MustAction.must_Action;
			Set_left_right(s_MustAction.must_Look);
			_b_Key_Protect = true;
			
			//s_MustAction.must_Action = -1;
		}


		if(m_actNum == HERO_STOP){
			_b_Key_Nullity = false;
			_b_Key_Protect=false;
			_b_Hero_Protect = false;
			s_Skill_Set.act = false;
			
		}

		
		
		if(_m_actNum != m_actNum || _b_Must_Decide){//만약 다르다면 해당 행동으로 교체
			//_ins_Hero->UpdateTempXZ(APPLY_X|APPLY_Z);//SangHo - 이전 Ani 의 최종 X좌표를 원점에 반영, 높이(APPLY_Z) 존재시 높이수치반영
			_ins_Hero->m_bLoop = true;

			s_Skill_Set.OVER_SkillEffect = false;
			s_Skill_Set.DOWN_SkillEffect = false;
			switch(m_actNum){
				case HERO_STOP:
					_ins_Hero->m_posZ = 0;
					if(TOWN){//마을 임
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_TOWN_STAND);
					}else{
						_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_STAND,ANIM_WOMAN_BODY_A_G_STAND,ANIM_WOMAN_BODY_A_O_STAND));
					}
					
					break;

				case HERO_WALK_LEFT:
					Set_left_right(SDIR_LEFT);
					if(TOWN){//마을 임
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_TOWN_WALK);
					}else{
						_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_WALK,ANIM_WOMAN_BODY_A_G_WALK,ANIM_WOMAN_BODY_A_O_WALK));
					}
					break;

				case HERO_WALK_RIGHT:
					Set_left_right(SDIR_RIGHT);
					if(TOWN){//마을 임
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_TOWN_WALK);
					}else{
						_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_WALK,ANIM_WOMAN_BODY_A_G_WALK,ANIM_WOMAN_BODY_A_O_WALK));
					}
					break;

				case HERO_WALK_UP:
				case HERO_WALK_DOWN:
					if(TOWN){//마을 임
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_TOWN_WALK_VERTICAL);
					}else{
						_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_WALK_VERTICAL,ANIM_WOMAN_BODY_A_G_WALK_VERTICAL,ANIM_WOMAN_BODY_A_O_WALK_VERTICAL));
					}
					break;

				case HERO_ATTACK1:
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_ATT_1,ANIM_WOMAN_BODY_A_G_ATT1,ANIM_WOMAN_BODY_A_O_ATT1));
					break;
				case HERO_ATTACK2:
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_ATT_2,ANIM_WOMAN_BODY_A_G_ATT2,ANIM_WOMAN_BODY_A_O_ATT2));
					break;
				case HERO_ATTACK3:
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_ATT_3,ANIM_WOMAN_BODY_A_G_ATT3,ANIM_WOMAN_BODY_A_O_ATT3));
					break;
				case HERO_ATTACK4:
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_ATT_4,0,0));
					break;

				case HERO_ATTACK1END:
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_ATT_1AFTER,ANIM_WOMAN_BODY_A_G_ATT1AFTER,ANIM_WOMAN_BODY_A_O_ATT1AFTER));
					break;
				case HERO_ATTACK2END:
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_ATT_2AFTER,ANIM_WOMAN_BODY_A_G_ATT2AFTER,ANIM_WOMAN_BODY_A_O_ATT2AFTER));
					break;
				case HERO_ATTACK3END:
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_ATT_3AFTER,0,0));
					break;



				case HERO_SKILL_0:
				case HERO_SKILL_1:
				case HERO_SKILL_2:
				case HERO_SKILL_3:
				case HERO_SKILL_4:

					s_Skill_Set.act = true;
					_ins_Hero->m_bLoop = false;

					_ins_Skill[s_Skill_Set.Num][1]->m_bLoop = false;
					_ins_Skill[s_Skill_Set.Num][0]->m_bLoop = false;
					_ins_Hero->SetAnim(s__ins_Skill[ s_Skill_Set.Skill_ID[s_Skill_Set.Num] ][ 3 ]);
					
					s_Skill_Set.OVER_SkillEffect = false;
					s_Skill_Set.DOWN_SkillEffect = false;
					if(s__ins_Skill[ s_Skill_Set.Skill_ID[s_Skill_Set.Num] ][ 4 ]>=0){
						s_Skill_Set.OVER_SkillEffect = true;
						_ins_Skill[s_Skill_Set.Num][0]->SetAnim(s__ins_Skill[ s_Skill_Set.Skill_ID[s_Skill_Set.Num] ][ 4 ]);
						_ins_Skill[s_Skill_Set.Num][0]->m_posZ = 0;
					}
					if(s__ins_Skill[ s_Skill_Set.Skill_ID[s_Skill_Set.Num] ][ 5 ]>=0){
						s_Skill_Set.DOWN_SkillEffect = true;
						_ins_Skill[s_Skill_Set.Num][1]->SetAnim(s__ins_Skill[ s_Skill_Set.Skill_ID[s_Skill_Set.Num] ][ 5 ]);
						_ins_Skill[s_Skill_Set.Num][1]->m_posZ = 0;
					}
					_ins_Skill[s_Skill_Set.Num][1]->m_posX = _ins_Hero->m_posX;//스킬좌표에 케릭터 좌표를 반영한다
					_ins_Skill[s_Skill_Set.Num][1]->m_posY = _ins_Hero->m_posY;//스킬좌표에 케릭터 좌표를 반영한다
					_ins_Skill[s_Skill_Set.Num][0]->m_posX = _ins_Hero->m_posX;//스킬좌표에 케릭터 좌표를 반영한다
					_ins_Skill[s_Skill_Set.Num][0]->m_posY = _ins_Hero->m_posY;//스킬좌표에 케릭터 좌표를 반영한다
					

					
					break;
				/*case HERO_SKILL_1:
					s_Skill_Set.Num = 1;
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(s__ins_Skill[ s_Skill_Set.Skill_ID[1] ][ 3 ]);
					_ins_Skill[1][1]->m_bLoop = false;
					_ins_Skill[1][1]->SetAnim(0);
					s_Skill_Set.OVER_SkillEffect = true;
					break;
				case HERO_SKILL_2:
					s_Skill_Set.Num = 2;
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(s__ins_Skill[ s_Skill_Set.Skill_ID[2] ][ 3 ]);
					_ins_Skill[2][1]->m_bLoop = false;
					_ins_Skill[2][1]->SetAnim(0);
					s_Skill_Set.OVER_SkillEffect = true;
					break;
				case HERO_SKILL_3:
					s_Skill_Set.Num = 3;
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(s__ins_Skill[ s_Skill_Set.Skill_ID[3] ][ 3 ]);
					_ins_Skill[3][1]->m_bLoop = false;
					_ins_Skill[3][1]->SetAnim(ANIM_WOMAN_S_G7_A_G_SKILL7_1_E);
					s_Skill_Set.OVER_SkillEffect = true;
					break;
				case HERO_SKILL_4:
					s_Skill_Set.Num = 4;
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(s__ins_Skill[ s_Skill_Set.Skill_ID[4] ][ 3 ]);
					_ins_Skill[4][1]->m_bLoop = false;
					_ins_Skill[4][1]->SetAnim(0);
					s_Skill_Set.OVER_SkillEffect = true;
					break;*/



				case HERO_SKILL_E1: //연타스킬의 예외처리
					//익셉션 스위치 코드, 삽입
					_ins_Hero->m_bLoop = false;
					if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 1 ){//보드 타는 스킬
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_S_SKILL2_1AFTER);
					}else if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 6 ){//칼 공중 잡기스킬
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_S_SKILL7_1AFTER);
					}else if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 13){//이퀄리브리엄
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_G_SKILL7_1AFTER);
					} 
					break;
				case HERO_SKILL_C2: //연타스킬의 예외처리
					_ins_Hero->m_bLoop = false;
					if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 1 ){//보드 타는 스킬
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_S_SKILL2_2);
						s_Skill_Set.OVER_SkillEffect = true;//스킬이펙트를 출력한다.
						_ins_Skill[s_Skill_Set.Num][0]->m_bLoop = false;
						_ins_Skill[s_Skill_Set.Num][0]->SetAnim(ANIM_WOMAN_S_S2_A_S_SKILL2_2_E);
					}else if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 6 ){//칼 공중 잡기스킬
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_S_SKILL7_2);
						s_Skill_Set.OVER_SkillEffect = true;//스킬이펙트를 출력한다.
						_ins_Skill[s_Skill_Set.Num][0]->m_bLoop = false;
						_ins_Skill[s_Skill_Set.Num][0]->SetAnim(ANIM_WOMAN_S_S7_A_S_SKILL7_2_E);
					}else if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 13){//이퀄리브리엄
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_G_SKILL7_2);
						s_Skill_Set.OVER_SkillEffect = true;//스킬이펙트를 출력한다.
						_ins_Skill[s_Skill_Set.Num][0]->m_bLoop = false;
						_ins_Skill[s_Skill_Set.Num][0]->SetAnim(ANIM_WOMAN_S_G7_A_G_SKILL7_2_E);
					} 
					_ins_Skill[s_Skill_Set.Num][1]->m_posX = _ins_Hero->m_posX;//스킬좌표에 케릭터 좌표를 반영한다
					_ins_Skill[s_Skill_Set.Num][1]->m_posY = _ins_Hero->m_posY;//스킬좌표에 케릭터 좌표를 반영한다
					_ins_Skill[s_Skill_Set.Num][0]->m_posX = _ins_Hero->m_posX;//스킬좌표에 케릭터 좌표를 반영한다
					_ins_Skill[s_Skill_Set.Num][0]->m_posY = _ins_Hero->m_posY;//스킬좌표에 케릭터 좌표를 반영한다
					break;
				case HERO_SKILL_E2: //연타스킬의 예외처리
					_ins_Hero->m_bLoop = false;
					if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 6 ){//칼 공중 잡기스킬
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_S_SKILL7_2AFTER);
					}else if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 13){//이퀄리브리엄
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_G_SKILL7_2AFTER);
					} 
					break;
				case HERO_SKILL_C3: //연타스킬의 예외처리
					_ins_Hero->m_bLoop = false;
					if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 6 ){//칼 공중 잡기스킬
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_S_SKILL7_3);
						s_Skill_Set.OVER_SkillEffect = true;//스킬이펙트를 출력한다.
						_ins_Skill[s_Skill_Set.Num][0]->m_bLoop = false;
						_ins_Skill[s_Skill_Set.Num][0]->SetAnim(ANIM_WOMAN_S_S7_A_S_SKILL7_3_E);
					}else if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 13){//이퀄리브리엄
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_G_SKILL7_3);
						s_Skill_Set.OVER_SkillEffect = true;//스킬이펙트를 출력한다.
						_ins_Skill[s_Skill_Set.Num][0]->m_bLoop = false;
						_ins_Skill[s_Skill_Set.Num][0]->SetAnim(ANIM_WOMAN_S_G7_A_G_SKILL7_3_E);
						s_Skill_Set.DOWN_SkillEffect = true;//스킬이펙트를 출력한다.
						_ins_Skill[s_Skill_Set.Num][1]->m_bLoop = false;
						_ins_Skill[s_Skill_Set.Num][1]->SetAnim(ANIM_WOMAN_S_G7_A_G_SKILL7_3_EBACK);
					}
					_ins_Skill[s_Skill_Set.Num][1]->m_posX = _ins_Hero->m_posX;//스킬좌표에 케릭터 좌표를 반영한다
					_ins_Skill[s_Skill_Set.Num][1]->m_posY = _ins_Hero->m_posY;//스킬좌표에 케릭터 좌표를 반영한다
					_ins_Skill[s_Skill_Set.Num][0]->m_posX = _ins_Hero->m_posX;//스킬좌표에 케릭터 좌표를 반영한다
					_ins_Skill[s_Skill_Set.Num][0]->m_posY = _ins_Hero->m_posY;//스킬좌표에 케릭터 좌표를 반영한다
					break;
				case HERO_SKILL_E3: //연타스킬의 예외처리
					_ins_Hero->m_bLoop = false;
					if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 6 ){//칼 공중 잡기스킬
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_S_SKILL7_3AFTER);
					} 
					break;
				case HERO_SKILL_C4:
					_ins_Hero->m_bLoop = false;
					if(s_Skill_Set.Skill_ID[s_Skill_Set.Num] == 6 ){//칼 공중 잡기스킬
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_S_SKILL7_4);
						s_Skill_Set.OVER_SkillEffect = true;//스킬이펙트를 출력한다.
						_ins_Skill[s_Skill_Set.Num][0]->m_bLoop = false;
						_ins_Skill[s_Skill_Set.Num][0]->SetAnim(ANIM_WOMAN_S_S7_A_S_SKILL7_4_E);
					}
					_ins_Skill[s_Skill_Set.Num][1]->m_posX = _ins_Hero->m_posX;//스킬좌표에 케릭터 좌표를 반영한다
					_ins_Skill[s_Skill_Set.Num][1]->m_posY = _ins_Hero->m_posY;//스킬좌표에 케릭터 좌표를 반영한다
					_ins_Skill[s_Skill_Set.Num][0]->m_posX = _ins_Hero->m_posX;//스킬좌표에 케릭터 좌표를 반영한다
					_ins_Skill[s_Skill_Set.Num][0]->m_posY = _ins_Hero->m_posY;//스킬좌표에 케릭터 좌표를 반영한다
					break;
				case HERO_SWITCH:
					_ins_Hero->m_bLoop = false;

					if(s_Ability.JAB_KNIFE+s_Ability.JAB_GUN+s_Ability.JAB_MAGIC>1){
						int _b_JabNext;

						if(s_WeaponSwitch.act){
							_b_JabNext = s_WeaponSwitch.nextWeapon;
						}else{
							switch(_b_JabNum){
								case 0://검
									if(s_Ability.JAB_GUN){
										_b_JabNext = 1;
									}else{
										_b_JabNext = 2;
									}
									break;
								case 1://총
									if(s_Ability.JAB_MAGIC){
										_b_JabNext = 2;
									}else{
										_b_JabNext = 0;
									}
									break;
								case 2://마법
									if(s_Ability.JAB_KNIFE){
										_b_JabNext = 0;
									}else{
										_b_JabNext = 1;
									}
									break;
							}
						}

						switch ((_b_JabNum*10) + _b_JabNext){
							case 01://검>총
								_ins_Hero->SetAnim(s_WeaponSwitch.act ? ANIM_WOMAN_BODY_A_S_SWAPSKILL : ANIM_WOMAN_BODY_A_S_SWAP_TO_G);
								break;
							case 02://검>마법
								_ins_Hero->SetAnim(s_WeaponSwitch.act ? ANIM_WOMAN_BODY_A_S_SWAPSKILL : ANIM_WOMAN_BODY_A_S_SWAP_TO_O);
								break;
							case 10://총>검
								_ins_Hero->SetAnim(s_WeaponSwitch.act ? ANIM_WOMAN_BODY_A_G_SWAPSKILL : ANIM_WOMAN_BODY_A_G_SWAP_TO_S);
								break;
							case 12://총>마법
								_ins_Hero->SetAnim(s_WeaponSwitch.act ? ANIM_WOMAN_BODY_A_G_SWAPSKILL : ANIM_WOMAN_BODY_A_G_SWAP_TO_O);
								break;
							case 20://마법>검
								_ins_Hero->SetAnim(s_WeaponSwitch.act ? ANIM_WOMAN_BODY_A_O_SWAPSKILL : ANIM_WOMAN_BODY_A_O_SWAP_TO_S);
								break;
							case 21://마법>총
								_ins_Hero->SetAnim(s_WeaponSwitch.act ? ANIM_WOMAN_BODY_A_O_SWAPSKILL : ANIM_WOMAN_BODY_A_O_SWAP_TO_G);
								break;
						}
						
						_b_JabNum = _b_JabNext;
						s_Status.ELEMENTAL = _b_JabNum;//속성부여
					}
					break;

				case HERO_LINE_MOVE_UP:
				case HERO_LINE_MOVE_DOWN:
					_ins_Hero->m_posZ = 0;//애니메이션 중에 회전 등으로 인해 Z값이 0이 아닐 수도 있다
					_ins_Hero->m_bLoop = false;
					if(TOWN){//마을 임
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_TOWN_LINE_MOVE);
					}else{
						_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_LINE_MOVE,ANIM_WOMAN_BODY_A_G_LINE_MOVE,ANIM_WOMAN_BODY_A_O_LINE_MOVE));
					}
					break;

				case HERO_DASH_RIGHT:
				case HERO_DASH_LEFT:
					_ins_Hero->m_posZ = 0;//애니메이션 중에 회전 등으로 인해 Z값이 0이 아닐 수도 있다
					_ins_Hero->m_bLoop = true;
					if(TOWN){//마을 임
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_TOWN_DASH);
					}else{
						_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_DASH,ANIM_WOMAN_BODY_A_G_DASH,ANIM_WOMAN_BODY_A_O_DASH));
					}
					break;

				case HERO_DASH_STOP:
					_ins_Hero->m_bLoop = false;
					if(TOWN){//마을 임
						_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_TOWN_DASH_STOP);
					}else{
						_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_DASHSTOP,ANIM_WOMAN_BODY_A_G_DASHSTOP,ANIM_WOMAN_BODY_A_O_DASHSTOP));
					}
					break;

				case HERO_DASH_ATT:
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_DASHATT,ANIM_WOMAN_BODY_A_G_DASHATT,ANIM_WOMAN_BODY_A_O_DASHATT));
					break;

				case HERO_CHARGE:
					if(_b_JabNum==JAB_KNIGHT) s_Status.MANA -= PER(s_Status.MANA_MAX,30);//가드 - 마나소비

					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_CHARGE,ANIM_WOMAN_BODY_A_G_CHARGE,ANIM_WOMAN_BODY_A_O_CHARGE));
					break;

				case HERO_CHARGE_SHOOT:
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_G_CHARGE_02);
					break;

				case DAMAGE_FLY:
					break;
				case DAMAGE_GROUND:
					break;

	
				case HERO_DOWNED:
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_6,ANIM_WOMAN_BODY_A_G_FLYHI_6,ANIM_WOMAN_BODY_A_O_FLYHI_6));
					_move_Order = HERO_DOWNED_2;
					break;

				case HERO_DOWNED_2:
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_DOWNED,ANIM_WOMAN_BODY_A_G_DOWNED,ANIM_WOMAN_BODY_A_O_DOWNED));
					s_Damage.Down_Time--;
					if(s_Damage.Down_Time<=0)
						_move_Order = HERO_AWAKE;
					break;

				case HERO_AWAKE:
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_AWAKE,ANIM_WOMAN_BODY_A_G_AWAKE,ANIM_WOMAN_BODY_A_O_AWAKE));
					_ins_Hero->m_bLoop = false;
					_b_Must_Decide = false;
					_move_Order = HERO_STOP;
					break;

				case HERO_THROW:
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_THROW,ANIM_WOMAN_BODY_A_G_THROW,ANIM_WOMAN_BODY_A_O_THROW));
					_ins_Hero->m_bLoop = false;
					break;
				case HERO_JumpUp:
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_CLIMB);
					_ins_Hero->b_MoveLock = true;
					break;
				case HERO_JumpDown:
					_ins_Hero->m_bLoop = false;
					_ins_Hero->SetAnim(ANIM_WOMAN_BODY_A_FALL);
					_ins_Hero->b_MoveLock = true;
					break;


			}
			_m_actNum = m_actNum;

		}




		_b_ActionEnd = !_ins_Hero->UpdateSpriteAnim();//케릭터 에니메이션 업데이트 실행

		if(s_Skill_Set.OVER_SkillEffect) _ins_Skill[s_Skill_Set.Num][0]->UpdateSpriteAnim();//스킬 에니메이션 업데이트 실행
		if(s_Skill_Set.DOWN_SkillEffect) _ins_Skill[s_Skill_Set.Num][1]->UpdateSpriteAnim();//스킬 에니메이션 업데이트 실행



		switch(m_actNum){//축계 이동 Y 라인 움직임 예외처리
			case HERO_WALK_UP:
				if(_ins_Hero->m_posY>TEMP_UP_LIMIT)
					_ins_Hero->m_posY-=5;//75%
					//_ins_Hero->m_posX-=5;//75% //For namyoung
				break;
			case HERO_WALK_DOWN:
				if(_ins_Hero->m_posY<TEMP_DOWN_LIMIT)
					_ins_Hero->m_posY+=5;//75%
				break;
			case HERO_LINE_MOVE_UP:
				if(_ins_Hero->m_posY>TEMP_UP_LIMIT && _ins_Hero->m_nCrtFrame<4)
					_ins_Hero->m_posY-=10;//75%
				break;
			case HERO_LINE_MOVE_DOWN:
				if(_ins_Hero->m_posY<TEMP_DOWN_LIMIT && _ins_Hero->m_nCrtFrame<4)
					_ins_Hero->m_posY+=10;//75%
				break;

		}





//SangHo - 주인공의 이벤트 영역
		switch(m_actNum){
			case HERO_WALK_UP://상
			case HERO_LINE_MOVE_UP://상
				s_Event_Rect.Direction = 1;
				break;
			case HERO_WALK_DOWN://하
			case HERO_LINE_MOVE_DOWN://하
				s_Event_Rect.Direction = 2;
				break;
			case HERO_WALK_LEFT://좌
			case HERO_DASH_LEFT://좌
				s_Event_Rect.Direction = 3;
				break;
			case HERO_WALK_RIGHT://우
			case HERO_DASH_RIGHT://우
				s_Event_Rect.Direction = 4;
				break;
		}

		int tempX = (_ins_Hero->m_posX);
		int tempY = (_ins_Hero->m_posY);
		switch(s_Event_Rect.Direction){
			case 1://상
				s_Event_Rect.W = 20;
				s_Event_Rect.H = 21;
				s_Event_Rect.X1 = tempX - s_Event_Rect.W/2;
				s_Event_Rect.Y1 = tempY - s_Event_Rect.H;
				break;
			case 2://하
				s_Event_Rect.W = 20;
				s_Event_Rect.H = 21;
				s_Event_Rect.X1 = tempX - s_Event_Rect.W/2;
				s_Event_Rect.Y1 = tempY;
				break;
			case 3://좌
				s_Event_Rect.W = 30;
				s_Event_Rect.H = 20;
				s_Event_Rect.X1 = tempX - s_Event_Rect.W;
				s_Event_Rect.Y1 = tempY - s_Event_Rect.H/2;
				break;
			case 4://우
				s_Event_Rect.W = 30;
				s_Event_Rect.H = 20;
				s_Event_Rect.X1 = tempX;
				s_Event_Rect.Y1 = tempY - s_Event_Rect.H/2;
				break;
		}

		int a = 0;
		s_Event_Rect.X2 = s_Event_Rect.X1 + s_Event_Rect.W;
		s_Event_Rect.Y2 = s_Event_Rect.Y1 + s_Event_Rect.H;

		




	}


	/******************************************  Paint  ***************************************/
	void Character::Paint_Debuff(int drawX,int drawZ)
	{// 디버프 이펙트를 그릴때

		if(!s_Debuff.All)//디버프가 하나도 걸려있지않다면 
			return;


		_ins_Debuff->m_posX = drawX;
		_ins_Debuff->m_posY = _ins_Hero->m_posY;
		//char m_tempZ = drawZ;//_ins_Hero->m_sprite->_aframes[((_ins_Hero->m_sprite->_anims_af_start[_ins_Hero->m_nCrtModule] + _ins_Hero->m_nCrtFrame) * 5)+3] & 0xFF;
		_ins_Debuff->m_posZ = drawZ;
		//_ins_Debuff->m_posZ = m_posZ - HERO_HEIGHT;

		_ins_Debuff->CameraX = _ins_Hero->CameraX;
		_ins_Debuff->CameraY = _ins_Hero->CameraY;




		for(int xx =0;xx<3;xx++){//디버프의 해제와 그래픽적인 구현
			if(xx < 2 && s_Debuff.stack[xx][0] == 0){
				s_Debuff.stack[xx][0] = s_Debuff.stack[xx+1][0];
				s_Debuff.stack[xx][1] = s_Debuff.stack[xx+1][1];
				s_Debuff.stack[xx+1][0] = 0;
				s_Debuff.stack[xx+1][1] = 0;
			}

			
			if(s_Debuff.stack[xx][0]){
				switch(s_Debuff.stack[xx][0]){
					case DEBUF_WEAK://약화
						if(xx == 0 ){// 0번째(최신) 디버프이며
							if(s_Debuff.stack[xx][1] == 0 || _ins_Debuff->m_nCrtModule != ANIM_WEFFECT_A_DEBUFF_EMPTY){// 디버프 타이머가 지금 시작 되었다면
								_ins_Debuff->SetAnim(ANIM_WEFFECT_A_DEBUFF_EMPTY);
								_ins_Debuff->m_bLoop = true;
							}
							int pxx = 16;//31 이 넘으면 안됨
							s_Debuff.color = (s_Debuff.color+1)%pxx;
							int pyy = (s_Debuff.color <= (pxx/2) ? (s_Debuff.color<<12)+(s_Debuff.color<<7) : ((pxx-s_Debuff.color)<<12)+((pxx-s_Debuff.color)<<7)); 
							_ins_Hero->m_sprite->SetBlendCustom(true,false,7,pyy);
						}
						if(s_Debuff.stack[xx][1] > 100){//디버프가 종료 되었다면
							s_Debuff.All -= s_Debuff.stack[xx][0];//디버프 해제
							s_Debuff.stack[xx][0] = 0;
							if(xx == 0 )_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}

						break;
					case DEBUF_ICE://아이스
						if(xx == 0 ){// 0번째(최신) 디버프이며
							if(s_Debuff.stack[xx][1] == 0 || _ins_Debuff->m_nCrtModule != ANIM_WEFFECT_A_DEBUFF_COLD){// 디버프 타이머가 지금 시작 되었다면
								_ins_Debuff->SetAnim(ANIM_WEFFECT_A_DEBUFF_COLD);
								_ins_Debuff->m_bLoop = true;
							}
							int pxx = 31;//31 이 넘으면 안됨
							s_Debuff.color = (s_Debuff.color+2)%pxx;
							int pyy = (s_Debuff.color <= (pxx/2) ? (s_Debuff.color<<1) : ((pxx-s_Debuff.color)<<1)); 
							//int pyy = (s_Debuff.color <= (pxx/2) ? (s_Debuff.color<<11)+(s_Debuff.color<<6)+(s_Debuff.color<<1) : ((pxx-s_Debuff.color)<<11)+((pxx-s_Debuff.color)<<6)+((pxx-s_Debuff.color)<<1)); 
							_ins_Hero->m_sprite->SetBlendCustom(true,false,7,pyy);
						}
						if(s_Debuff.stack[xx][1] > 30){//디버프가 종료 되었다면
							s_Debuff.All -= s_Debuff.stack[xx][0];//디버프 해제
							s_Debuff.stack[xx][0] = 0;
							if(xx == 0 )_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}
						break;
					case DEBUF_FREEZE://동결
						if(xx == 0 ){// 0번째(최신) 디버프이며
							if(s_Debuff.stack[xx][1] == 0 || _ins_Debuff->m_nCrtModule != ANIM_WEFFECT_A_DEBUFF_FREEZ_1 || _ins_Debuff->m_nCrtModule != ANIM_WEFFECT_A_DEBUFF_FREEZ_3){// 디버프 타이머가 지금 시작 되었다면
								_ins_Debuff->SetAnim(ANIM_WEFFECT_A_DEBUFF_FREEZ_1);
								_ins_Debuff->m_bLoop = false;
							}else if(s_Debuff.stack[xx][1] == 50-2){// 디버프 타이머가 지금 시작 되었다면
								_ins_Debuff->SetAnim(ANIM_WEFFECT_A_DEBUFF_FREEZ_3);
								_ins_Debuff->m_bLoop = false;
							}
							_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}
						if(s_Debuff.stack[xx][1] > 50){//디버프가 종료 되었다면
							s_Debuff.All -= s_Debuff.stack[xx][0];//디버프 해제
							s_Debuff.stack[xx][0] = 0;
							if(xx == 0 )_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}
						break;
					case DEBUF_STUN://스턴
						if(xx == 0 ){// 0번째(최신) 디버프이며
							if(s_Debuff.stack[xx][1] == 0 || _ins_Debuff->m_nCrtModule != ANIM_WEFFECT_A_DEBUFF_STUN){// 디버프 타이머가 지금 시작 되었다면
								_ins_Debuff->SetAnim(ANIM_WEFFECT_A_DEBUFF_STUN);
								_ins_Debuff->m_bLoop = true;
							}
							_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
							_ins_Debuff->m_posZ -= HERO_HEIGHT;//머리에서 그려준다
						}
						if(s_Debuff.stack[xx][1] > 20){//디버프가 종료 되었다면
							s_Debuff.All -= s_Debuff.stack[xx][0];//디버프 해제
							s_Debuff.stack[xx][0] = 0;
							if(xx == 0 )_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}
						break;
					case DEBUF_STONE://석화
						if(xx == 0 ){// 0번째(최신) 디버프이며
							if(s_Debuff.stack[xx][1] == 0 || _ins_Debuff->m_nCrtModule != ANIM_WEFFECT_A_DEBUFF_EMPTY){// 디버프 타이머가 지금 시작 되었다면
								_ins_Debuff->SetAnim(ANIM_WEFFECT_A_DEBUFF_EMPTY);
								_ins_Debuff->m_bLoop = true;
							}
							_ins_Hero->m_sprite->SetBlendCustom(true,false,8,0);
						}
						if(s_Debuff.stack[xx][1] > 50){//디버프가 종료 되었다면
							s_Debuff.All -= s_Debuff.stack[xx][0];//디버프 해제
							s_Debuff.stack[xx][0] = 0;
							if(xx == 0 )_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}
						break;
					case DEBUF_POISON://독
						if(xx == 0 ){// 0번째(최신) 디버프이며
							if(s_Debuff.stack[xx][1] == 0 || _ins_Debuff->m_nCrtModule != ANIM_WEFFECT_A_DEBUFF_VENOM){// 디버프 타이머가 지금 시작 되었다면
								_ins_Debuff->SetAnim(ANIM_WEFFECT_A_DEBUFF_VENOM);
								_ins_Debuff->m_bLoop = true;
							}
							int pxx = 16;//31 이 넘으면 안됨
							s_Debuff.color = (s_Debuff.color+1)%pxx;
							int pyy = (s_Debuff.color <= (pxx/2) ? (s_Debuff.color<<7) : ((pxx-s_Debuff.color)<<7)); 
							_ins_Hero->m_sprite->SetBlendCustom(true,false,7,pyy);
							_ins_Debuff->m_posZ -= HERO_HEIGHT;//머리에서 그려준다

						}
						if(s_Debuff.stack[xx][1] > 50){//디버프가 종료 되었다면
							s_Debuff.All -= s_Debuff.stack[xx][0];//디버프 해제
							s_Debuff.stack[xx][0] = 0;
							if(xx == 0 )_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}
						break;
					case DEBUF_CONFUSE://혼란
						if(xx == 0 ){// 0번째(최신) 디버프이며
							if(s_Debuff.stack[xx][1] == 0 || _ins_Debuff->m_nCrtModule != ANIM_WEFFECT_A_DEBUFF_CHAOS){// 디버프 타이머가 지금 시작 되었다면
								_ins_Debuff->SetAnim(ANIM_WEFFECT_A_DEBUFF_CHAOS);
								_ins_Debuff->m_bLoop = true;
							}
							int pxx = 16;//31 이 넘으면 안됨
							s_Debuff.color = (s_Debuff.color+1)%pxx;
							int pyy = (s_Debuff.color <= (pxx/2) ? (s_Debuff.color<<12)+(s_Debuff.color<<1) : ((pxx-s_Debuff.color)<<12)+((pxx-s_Debuff.color)<<1));  
							_ins_Hero->m_sprite->SetBlendCustom(true,false,7,pyy);
							_ins_Debuff->m_posZ -= HERO_HEIGHT;//머리에서 그려준다
						}
						if(s_Debuff.stack[xx][1] > 100){//디버프가 종료 되었다면
							s_Debuff.All -= s_Debuff.stack[xx][0];//디버프 해제
							s_Debuff.stack[xx][0] = 0;
							if(xx == 0 )_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}
						break;
					case DEBUF_VAMPIRE://흡혈 A
						if(xx == 0 ){// 0번째(최신) 디버프이며
							if(s_Debuff.stack[xx][1] == 0 || _ins_Debuff->m_nCrtModule != ANIM_WEFFECT_A_DEBUFF_EMPTY){// 디버프 타이머가 지금 시작 되었다면
								_ins_Debuff->SetAnim(ANIM_WEFFECT_A_DEBUFF_EMPTY);
								_ins_Debuff->m_bLoop = true;
							}
							_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}
						if(s_Debuff.stack[xx][1] > 50){//디버프가 종료 되었다면
							s_Debuff.All -= s_Debuff.stack[xx][0];//디버프 해제
							s_Debuff.stack[xx][0] = 0;
							if(xx == 0 )_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}
						break;
					case DEBUF_DE_VAMPIRE://흡혈 B
						if(xx == 0 ){// 0번째(최신) 디버프이며
							if(s_Debuff.stack[xx][1] == 0 || _ins_Debuff->m_nCrtModule != ANIM_WEFFECT_A_DEBUFF_EMPTY){// 디버프 타이머가 지금 시작 되었다면
								_ins_Debuff->SetAnim(ANIM_WEFFECT_A_DEBUFF_EMPTY);
								_ins_Debuff->m_bLoop = true;
							}
							int pxx = 28;//31 이 넘으면 안됨
							s_Debuff.color = (s_Debuff.color+2)%pxx;
							int pyy = (s_Debuff.color <= (pxx/2) ? (s_Debuff.color<<12) : ((pxx-s_Debuff.color)<<12)); 
							_ins_Hero->m_sprite->SetBlendCustom(true,false,7,pyy);
						}
						if(s_Debuff.stack[xx][1] > 50){//디버프가 종료 되었다면
							s_Debuff.All -= s_Debuff.stack[xx][0];//디버프 해제
							s_Debuff.stack[xx][0] = 0;
							if(xx == 0 )_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}
						break;
					case DEBUF_MANABURN://마나연소
						if(xx == 0 ){// 0번째(최신) 디버프이며
							if(s_Debuff.stack[xx][1] == 0 || _ins_Debuff->m_nCrtModule != ANIM_WEFFECT_A_DEBUFF_MANABURN){// 디버프 타이머가 지금 시작 되었다면
								_ins_Debuff->SetAnim(ANIM_WEFFECT_A_DEBUFF_MANABURN);
								_ins_Debuff->m_bLoop = true;
							}
							_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}
						if(s_Debuff.stack[xx][1] > 50){//디버프가 종료 되었다면
							s_Debuff.All -= s_Debuff.stack[xx][0];//디버프 해제
							s_Debuff.stack[xx][0] = 0;
							if(xx == 0 )_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}
						break;
					case DEBUF_SILENCE://침묵
						if(xx == 0 ){// 0번째(최신) 디버프이며
							if(s_Debuff.stack[xx][1] == 0 || _ins_Debuff->m_nCrtModule != ANIM_WEFFECT_A_DEBUFF_SILENCE){// 디버프 타이머가 지금 시작 되었다면
								_ins_Debuff->SetAnim(ANIM_WEFFECT_A_DEBUFF_SILENCE);
								_ins_Debuff->m_bLoop = true;
							}
							_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
							_ins_Debuff->m_posZ -= HERO_HEIGHT;//머리에서 그려준다
						}
						if(s_Debuff.stack[xx][1] > 50){//디버프가 종료 되었다면
							s_Debuff.All -= s_Debuff.stack[xx][0];//디버프 해제
							s_Debuff.stack[xx][0] = 0;
							if(xx == 0 )_ins_Hero->m_sprite->SetBlendCustom(false,false,0,0);
						}
						break;
				}
				s_Debuff.stack[xx][1]++;
			}
			
		}

		_ins_Debuff->UpdateSpriteAnim();
		SUTIL_PaintAsprite(_ins_Debuff,S_INCLUDE_SORT);

	}
	void Character::Paint_LevelUp()
	{// 레벨업 이펙트를 그릴때
		if(s_LV_Eff.act){

			if(s_LV_Eff.LVup_ActionEnd){
				s_LV_Eff.act = false;
				s_LV_Eff.LVupEff_Num = 0;
				s_LV_Eff.LVup_ActionEnd = false;
				SUTIL_FreeSpriteInstance(s_LV_Eff.LVup_Eff_Ins);
			}else{
				if(s_LV_Eff.LVupEff_Num==0){
					s_LV_Eff.LVup_Eff_Ins = GL_NEW ASpriteInstance(_spr_Hero, 100, 200, NULL);// 0번째 배열, 실사용시는 define 필요

					s_LV_Eff.LVup_Eff_Ins->m_bLoop = false;
					s_LV_Eff.LVup_Eff_Ins->SetAniMoveLock(true);
					s_LV_Eff.LVup_Eff_Ins->SetAnim(ANIM_WOMAN_BODY_A_LEVELUP);
				}

				s_LV_Eff.LVup_Eff_Ins->m_posX = _ins_Hero->m_posX;
				s_LV_Eff.LVup_Eff_Ins->m_posY = _ins_Hero->m_posY;
				char m_posZ = _ins_Hero->m_sprite->_aframes[((_ins_Hero->m_sprite->_anims_af_start[_ins_Hero->m_nCrtModule] + _ins_Hero->m_nCrtFrame) * 5)+3] & 0xFF;
				s_LV_Eff.LVup_Eff_Ins->m_posZ = m_posZ;

				s_LV_Eff.LVup_Eff_Ins->CameraX = _ins_Hero->CameraX;
				s_LV_Eff.LVup_Eff_Ins->CameraY = _ins_Hero->CameraY;
				SUTIL_PaintAsprite(s_LV_Eff.LVup_Eff_Ins,S_INCLUDE_SORT);
				s_LV_Eff.LVup_ActionEnd = !s_LV_Eff.LVup_Eff_Ins->UpdateSpriteAnim();//Effect 에니메이션 업데이트 실행
				s_LV_Eff.LVupEff_Num++;
			}


			if(s_LV_Eff.LVup_Eff_Ins != NULL){

			}
		}
	}

	void Character::Paint_Knife()
	{// 호밍 나이프 버프 상태일때

		if(s_Knife_Eff[3].act){//버프지속

			if(s_Knife_Eff[3].LVup_ActionEnd){//버프 종료
				s_Knife_Eff[3].act = false;
				s_Knife_Eff[3].LVupEff_Num = 0;
				s_Knife_Eff[3].LVup_ActionEnd = false;
				///*SUTIL_FreeSpriteInstance*/SAFE_DELETE(s_Knife_Eff[3].LVup_Eff_Ins);
				SAFE_DEL(s_Knife_Eff[3].LVup_Eff_Ins);
				SAFE_DEL(s_Knife_Eff[4].LVup_Eff_Ins);
				for(int i =0;i<3;i++){
					s_Knife_Eff[i].act = false;
					SAFE_DEL(s_Knife_Eff[i].LVup_Eff_Ins);
				}

			}else{
				
					
				if(s_Knife_Eff[3].LVupEff_Num==0){//초기할당
					s_Knife_Eff[0].LVup_Eff_Ins = GL_NEW ASpriteInstance(_spr_Skill[_b_Knife_SetNum], 100, 200, NULL);// 칼1
					s_Knife_Eff[1].LVup_Eff_Ins = GL_NEW ASpriteInstance(_spr_Skill[_b_Knife_SetNum], 100, 200, NULL);// 칼2
					s_Knife_Eff[2].LVup_Eff_Ins = GL_NEW ASpriteInstance(_spr_Skill[_b_Knife_SetNum], 100, 200, NULL);// 칼3


					s_Knife_Eff[3].LVup_Eff_Ins = GL_NEW ASpriteInstance(_spr_Skill[_b_Knife_SetNum], 100, 200, NULL);// 0번째 배열, 실사용시는 define 필요
					s_Knife_Eff[3].LVup_Eff_Ins->m_bLoop = true;
					s_Knife_Eff[3].LVup_Eff_Ins->SetAnim(ANIM_WOMAN_S_S1_A_S_SKILL1_BUFF_E_BACK);
					s_Knife_Eff[3].LVup_Eff_Ins->m_posX = _ins_Hero->m_posX ;
					s_Knife_Eff[3].LVup_Eff_Ins->m_posY = _ins_Hero->m_posY;
					s_Knife_Eff[3].LVup_Eff_Ins->m_flags = _ins_Hero->m_flags;

					s_Knife_Eff[4].LVup_Eff_Ins = GL_NEW ASpriteInstance(_spr_Skill[_b_Knife_SetNum], 100, 200, NULL);// 0번째 배열, 실사용시는 define 필요
				}else if(s_Knife_Eff[3].LVup_Eff_Ins->m_nCrtModule == ANIM_WOMAN_S_S1_A_S_SKILL1_ATT_E_BACK){
					if(!s_Knife_Eff[0].act &&!s_Knife_Eff[1].act &&!s_Knife_Eff[2].act && s_Knife_Eff[3].LVup_Eff_Ins->m_nCrtFrame>6){ //발사되고난뒤
						//int aa =0;
						
						if(s_Knife_Eff[3].LVupEff_Num >= 200)
						{//버프종료
							s_Knife_Eff[3].LVup_ActionEnd = true;
						}else{
							s_Knife_Eff[3].LVup_Eff_Ins->SetAnim(ANIM_WOMAN_S_S1_A_S_SKILL1_REFILL_E_BACK);
							s_Knife_Eff[3].LVup_Eff_Ins->m_bLoop = false;
						}

						



						
					}
				}else if(s_Knife_Eff[3].LVup_Eff_Ins->m_nCrtModule == ANIM_WOMAN_S_S1_A_S_SKILL1_REFILL_E_BACK){
					if(_b_Knife_ActionEnd){
						s_Knife_Eff[3].LVup_Eff_Ins->m_bLoop = true;
						s_Knife_Eff[3].LVup_Eff_Ins->SetAnim(ANIM_WOMAN_S_S1_A_S_SKILL1_BUFF_E_BACK);
						s_Knife_Eff[3].LVup_Eff_Ins->m_posX = _ins_Hero->m_posX ;
						s_Knife_Eff[3].LVup_Eff_Ins->m_posY = _ins_Hero->m_posY;
						s_Knife_Eff[3].LVup_Eff_Ins->m_flags = _ins_Hero->m_flags;

					}
				}


				s_Knife_Eff[3].LVup_Eff_Ins->m_posX = _ins_Hero->m_posX ;
				s_Knife_Eff[3].LVup_Eff_Ins->m_posY = _ins_Hero->m_posY;
				if(s_Knife_Eff[3].LVup_Eff_Ins->m_nCrtModule !=ANIM_WOMAN_S_S1_A_S_SKILL1_ATT_E_BACK)
					s_Knife_Eff[3].LVup_Eff_Ins->m_flags = _ins_Hero->m_flags;
				s_Knife_Eff[3].LVup_Eff_Ins->CameraX = _ins_Hero->CameraX;
				s_Knife_Eff[3].LVup_Eff_Ins->CameraY = _ins_Hero->CameraY;
				_b_Knife_ActionEnd = !s_Knife_Eff[3].LVup_Eff_Ins->UpdateSpriteAnim();//Effect 에니메이션 업데이트 실행


				

				SUTIL_PaintAsprite(s_Knife_Eff[3].LVup_Eff_Ins,S_INCLUDE_SORT);

				s_Knife_Eff[4].LVup_Eff_Ins->m_posX = _ins_Hero->m_posX ;
				s_Knife_Eff[4].LVup_Eff_Ins->m_posY = _ins_Hero->m_posY+1;
				s_Knife_Eff[4].LVup_Eff_Ins->m_flags = s_Knife_Eff[3].LVup_Eff_Ins->m_flags;
				s_Knife_Eff[4].LVup_Eff_Ins->CameraX = _ins_Hero->CameraX;
				s_Knife_Eff[4].LVup_Eff_Ins->CameraY = _ins_Hero->CameraY;
				s_Knife_Eff[4].LVup_Eff_Ins->UpdateSpriteAnim();//Effect 에니메이션 업데이트 실행

				if(s_Knife_Eff[3].LVup_Eff_Ins->m_nCrtModule == ANIM_WOMAN_S_S1_A_S_SKILL1_ATT_E_BACK)
					SUTIL_PaintAsprite(s_Knife_Eff[4].LVup_Eff_Ins,S_INCLUDE_SORT);

				
				



				s_Knife_Eff[3].LVupEff_Num++;
				
			


				for(int i =0;i<3;i++){
					if(s_Knife_Eff[i].act){
						if(s_Knife_Eff[i].LVup_ActionEnd){
							s_Knife_Eff[i].act = false;
							s_Knife_Eff[i].LVupEff_Num = 0;
							s_Knife_Eff[i].LVup_ActionEnd = false;
							///*SUTIL_FreeSpriteInstance*/SAFE_DELETE(s_Knife_Eff[i].LVup_Eff_Ins);
							//SAFE_DEL(s_Knife_Eff[i].LVup_Eff_Ins);
						}else{
							if(s_Knife_Eff[i].LVupEff_Num==0){//초기할당

								
								s_Knife_Eff[i].LVup_Eff_Ins->m_bLoop = true;

								s_Knife_Eff[i].LVup_Eff_Ins->SetAnim(ANIM_WOMAN_S_S1_DAGGER);

								s_Knife_Eff[i].LVup_Eff_Ins->m_posX = _ins_Hero->m_posX ;
								s_Knife_Eff[i].LVup_Eff_Ins->m_posY = _ins_Hero->m_posY;
								//s_Knife_Eff[i].LVup_Eff_Ins->m_flags = _ins_Hero->m_flags;
							}else{
								s_Knife_Eff[i].LVup_Eff_Ins->CameraX = _ins_Hero->CameraX;
								s_Knife_Eff[i].LVup_Eff_Ins->CameraY = _ins_Hero->CameraY;
								SUTIL_PaintAsprite(s_Knife_Eff[i].LVup_Eff_Ins,S_INCLUDE_SORT);
								//s_Knife_Eff[i].LVup_ActionEnd = !s_Knife_Eff[i].LVup_Eff_Ins->UpdateSpriteAnim();//Effect 에니메이션 업데이트 실행
								s_Knife_Eff[i].LVup_Eff_Ins->UpdateSpriteAnim();//Effect 에니메이션 업데이트 실행
								
							}
							s_Knife_Eff[i].LVupEff_Num++;
							if(s_Knife_Eff[i].LVupEff_Num >= 30)s_Knife_Eff[i].LVup_ActionEnd = true;


							
						}


						if(s_Knife_Eff[i].LVup_Eff_Ins != NULL){

						}
					}
				}

			}




		}
	}


	void Character::Paint_Bollet()
	{// 발사형 이펙트를 그릴때 총 3가지의 경우가 있다
		for(int i =0;i<4;i++){
			if(s_Bullet_Eff[i].act){
				if(s_Bullet_Eff[i].LVup_ActionEnd){
					s_Bullet_Eff[i].act = false;
					s_Bullet_Eff[i].LVupEff_Num = 0;
					s_Bullet_Eff[i].LVup_ActionEnd = false;
					///*SUTIL_FreeSpriteInstance*/SAFE_DELETE(s_Bullet_Eff[i].LVup_Eff_Ins);
					SAFE_DEL(s_Bullet_Eff[i].LVup_Eff_Ins);
				}else{
					if(s_Bullet_Eff[i].LVupEff_Num==0){//초기할당
						
						s_Bullet_Eff[i].LVup_Eff_Ins = GL_NEW ASpriteInstance(_spr_Skill[s_Skill_Set.Num], 100, 200, NULL);// 0번째 배열, 실사용시는 define 필요

						s_Bullet_Eff[i].LVup_Eff_Ins->m_bLoop = true;

						if(i==0)s_Bullet_Eff[i].LVup_Eff_Ins->SetAnim(ANIM_WOMAN_S_O1_A_O_SKILL1_BULLET_1);
						if(i==1)s_Bullet_Eff[i].LVup_Eff_Ins->SetAnim(ANIM_WOMAN_S_O1_A_O_SKILL1_BULLET_2);
						if(i==2)s_Bullet_Eff[i].LVup_Eff_Ins->SetAnim(ANIM_WOMAN_S_O7_A_O_SKILL7_BULLET);
						if(i==3)s_Bullet_Eff[i].LVup_Eff_Ins->SetAnim(ANIM_WOMAN_S_O2_A_O_SKILL2_FTOTEM_E);

						if(i==3){ 
							s_Bullet_Eff[i].LVup_Eff_Ins->m_posX = _ins_Hero->m_posX+(_ins_Hero->m_flags? -38:+38);
							s_Bullet_Eff[i].LVup_Eff_Ins->m_posY = _ins_Hero->m_posY-1;
							s_Bullet_Eff[i].LVup_Eff_Ins->m_flags = _ins_Hero->m_flags;
						}else{
							s_Bullet_Eff[i].LVup_Eff_Ins->m_posX = _ins_Hero->m_posX ;
							s_Bullet_Eff[i].LVup_Eff_Ins->m_posY = _ins_Hero->m_posY;
							s_Bullet_Eff[i].LVup_Eff_Ins->m_flags = _ins_Hero->m_flags;
						}
						
						
					}

					
					s_Bullet_Eff[i].LVup_Eff_Ins->CameraX = _ins_Hero->CameraX;
					s_Bullet_Eff[i].LVup_Eff_Ins->CameraY = _ins_Hero->CameraY;
					SUTIL_PaintAsprite(s_Bullet_Eff[i].LVup_Eff_Ins,S_INCLUDE_SORT);
					//s_Bullet_Eff[i].LVup_ActionEnd = !s_Bullet_Eff[i].LVup_Eff_Ins->UpdateSpriteAnim();//Effect 에니메이션 업데이트 실행
					s_Bullet_Eff[i].LVup_Eff_Ins->UpdateSpriteAnim();//Effect 에니메이션 업데이트 실행
					s_Bullet_Eff[i].LVupEff_Num++;

					if(i==3){
						if(s_Bullet_Eff[i].LVupEff_Num >= 50)s_Bullet_Eff[i].LVup_ActionEnd = true;
					}else{
						if(s_Bullet_Eff[i].LVupEff_Num >= 10)s_Bullet_Eff[i].LVup_ActionEnd = true;
					}

					
				}


				if(s_Bullet_Eff[i].LVup_Eff_Ins != NULL){

				}
			}
		}
	}


	/********************************************  ETC  *****************************************/
	bool Character::Check_command(int m_keyCode, int m_keyRepeat){//연속된 커맨드값을 큐에 저장, 또는 초기화 하는 용도로쓰임


		if(s_KeyQueue.tick < s_KeyQueue.delTick)s_KeyQueue.tick++;


		if(m_keyRepeat){// m_keyRepeat 상태일때
			if(m_keyCode!=MH_KEY_SELECT){//MH_KEY_SELECT 가 아닌 다른키는
				return false;//커맨드 큐의 내용에 변함이 없다
			}
		}else {// m_keyRepeat 상태가 아닐때
			if(m_keyCode==MH_KEY_SELECT && s_KeyQueue.full[s_KeyQueue.count]==MH_KEY_SELECT){//MH_KEY_SELECT 키일때 중복 기록하지 않는다
				return false;//커맨드 큐의 내용에 변함이 없다
			}
		}


		if(m_keyCode==MH_KEY_SELECT || m_keyCode==MH_KEY_UP || m_keyCode==MH_KEY_DOWN || m_keyCode==MH_KEY_LEFT || m_keyCode==MH_KEY_RIGHT)//커맨드 입력 대상 키가 아니면 리턴
		{

			


			if(s_KeyQueue.tick >= s_KeyQueue.delTick)
			{//허용된 틱 연결타임을 초과하였으므로 초기화 
				s_KeyQueue.count=0;//인덱스 초기화
			}

			s_KeyQueue.tick=0;//연결타임 초기화


			short arrayLength = sizeof(s_KeyQueue.full)/sizeof(int); // 배열의 길이
			


			if(s_KeyQueue.count < arrayLength){//배열의 크기보다 현재 인덱스가 작거나 같으면

				s_KeyQueue.full[s_KeyQueue.count]=m_keyCode;

			}else{//배열의 크기보다 현재 인덱스가 크면

				s_KeyQueue.count = arrayLength - 1;//배열의 끝으로 인덱스 수정

				for(int xx = 0 ; xx< (arrayLength-1) ; xx++){//배열 쉬프트
					s_KeyQueue.full[xx]=s_KeyQueue.full[xx+1];
				}
				s_KeyQueue.full[s_KeyQueue.count]=m_keyCode;//배열의 끝에 현재값 할당
			}

			s_KeyQueue.count++;

			return true;//커맨드 큐의 내용이 수정대었다
		}

		return false;//커맨드 큐의 내용에 변함이 없다
	}
	bool Character::Check_skill_impossible(int m_skillnum){//스킬 사용 가능여부를 판단

		
		if(s_Damage.Type == DAMAGE_FLY)
			return true;//플라잉 데미지시 에는 기술 사용을 막는다
	
		if(m_skillnum == 5){//물약
			if(s_Skill_Set.Cool_TimeNow[m_skillnum]){
				return true;//쿨타임이 남아있거나 마나가 부족하면 스킬은 나가지 않는다
			}else{
				s_Skill_Set.Cool_TimeNow[m_skillnum] = s_Skill_Set.Cool_TimeMax[m_skillnum];
			}
		}else{
			if(s_Skill_Set.Cool_TimeNow[m_skillnum]  ||  s_Status.MANA < s_Skill_Set.Need_Mana[m_skillnum] || (s_Skill.Equip_A[m_skillnum] == -1)){
				return true;//쿨타임이 남아있거나 마나가 부족하거나 슬롯이 비었으면 스킬은 나가지 않는다
			}else{
				s_Skill_Set.Cool_TimeNow[m_skillnum] = s_Skill_Set.Cool_TimeMax[m_skillnum];
				s_Status.MANA -= s_Skill_Set.Need_Mana[m_skillnum];
			}
		}
		

		
		_b_Hero_Protect=true;
		_b_Key_Protect=true;
		s_Skill_Set.Num = m_skillnum;
		return false;
	}
	int Character::Check_jab(int m_Jab_Knight,int  m_Jab_Gunner,int  m_Jab_Magi)
	{//SangHo - 직업별 행동을 자연분화시킨다
		switch(_b_JabNum){
			case JAB_KNIGHT:
				return m_Jab_Knight;
			case JAB_GUNNER:
				return m_Jab_Gunner;
			case JAB_MAGE:
				return m_Jab_Magi;

			default:
				return m_Jab_Knight;
		}

	}
	int Character::Check_sex(int  m_Man,int m_Woman)
	{//SangHo - 직업별 행동을 자연분화시킨다
		switch(s_Status.SEX){
			case SEX_MAN:
				return m_Man;
			case SEX_WOMAN:
				return m_Woman;
			default:
				return m_Woman;
		}

	}
	int Character::Check_weapon(int m_Must_Weapon , int m_if_true_set_action )
	{//SangHo - 해당 직업이 아니면 해당 직업으로 바꾸는 액션을 취한다, 또한 액션이 끝나면 이어질 스킬을 지정한다
		if(_b_JabNum == m_Must_Weapon){
			return m_if_true_set_action;
		}else{
			s_WeaponSwitch.act = true;
			s_WeaponSwitch.nextWeapon = m_Must_Weapon;
			//s_WeaponSwitch.Key_Protection =_m_Key_Protection;
			s_WeaponSwitch.next_Event =  m_if_true_set_action;
			//_m_Key_Protection = MH_KEY_ASTERISK;
			return HERO_SWITCH;
		}
	}
	int Character::Set_skill_chain(int chain_Num, int m_keyCode, int chain_Key, int skill_Now, int skill_Next, int skill_Stop)
	{//체인 액션에 따른 각종 이벤트 분기처리
		if(_b_ActionEnd){
			if(_m_Hero_AttackNum >= (chain_Num*2+2)){

				return skill_Next;
			}else{

				return skill_Stop;
			}
		}else{
			if(_m_Hero_AttackNum==(chain_Num*2+0) && m_keyCode==0)//SangHo - 키 release
				_m_Hero_AttackNum=(chain_Num*2+1);
			if(_m_Hero_AttackNum==(chain_Num*2+1) && m_keyCode==chain_Key)//SangHo - 키 Press
				_m_Hero_AttackNum=(chain_Num*2+2);
			if(_m_Hero_AttackNum==(chain_Num*2+2) && m_keyCode==0)//SangHo - 키 release Next 키 값 사전처리
				_m_Hero_AttackNum=(chain_Num*2+3);

			return skill_Now;
		}
	}
	void Character::Set_left_right(int Look)
	{
		_b_LookRight=Look;
		SUTIL_SetDirAsprite(_ins_Hero, _b_LookRight);
		//		_ins_Hero->m_flags = _b_LookRight=Look;
	}
	void Character::Set_fly_motion(Damage& p_Damage,Position3D& p_Position3D){
      
		switch(p_Damage.Type){//데미지 타입
			case DAMAGE_FLY:
				if(p_Damage.Bound_Num!=0){						//체공 프레임	
					if(LOW_BOUND < m_Hero_Physics->SaveAccel.z){	//낮은 바운딩
						_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_5,ANIM_WOMAN_BODY_A_G_FLYHI_5,ANIM_WOMAN_BODY_A_O_FLYHI_5));
					}else{											//높은 바운딩

						if(-HIGH_PITCH >= p_Position3D.z){										//상승중	
							_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_2,ANIM_WOMAN_BODY_A_G_FLYHI_2,ANIM_WOMAN_BODY_A_O_FLYHI_2));
						}else if(HIGH_PITCH > p_Position3D.z && -HIGH_PITCH < p_Position3D.z){	//체공중
							_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_3,ANIM_WOMAN_BODY_A_G_FLYHI_3,ANIM_WOMAN_BODY_A_O_FLYHI_3));
						}else{																	//낙하중								
							_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_4,ANIM_WOMAN_BODY_A_G_FLYHI_4,ANIM_WOMAN_BODY_A_O_FLYHI_4));
						}

					}
				}else{							//"첫번째 프레임" - 맞거나 바운딩되는 직후의
					if(p_Damage.Bound==0){			//맞은 직후 모션인가?
						_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_1,ANIM_WOMAN_BODY_A_G_FLYHI_1,ANIM_WOMAN_BODY_A_O_FLYHI_1));
					}else{							//바운딩 직후 모션인가?
						//if(LAST_BOUND < m_Hero_Physics->SaveAccel.z){	//Z 충격량이 LAST_BOUND 보다 작아서 더이상 튀지않을때
						//	_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_6,ANIM_WOMAN_BODY_A_G_FLYHI_6,ANIM_WOMAN_BODY_A_O_FLYHI_6));
						//}else{											//Z 충격량이 LAST_BOUND 보다 커서 튀어오를때
							_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_3,ANIM_WOMAN_BODY_A_G_FLYHI_3,ANIM_WOMAN_BODY_A_O_FLYHI_3));
						//}
					}
				}
				break;

			case DAMAGE_GROUND:
				if(p_Damage.Bound_Num!=0){//맞아서 밀리고 있는 상황
					if(s_Damage.Bound){
						_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_DAMAGE_3,ANIM_WOMAN_BODY_A_G_DAMAGE_3,ANIM_WOMAN_BODY_A_O_DAMAGE_3));
					}else{
						_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_DAMAGE_2,ANIM_WOMAN_BODY_A_G_DAMAGE_2,ANIM_WOMAN_BODY_A_O_DAMAGE_2));
					}
				}else{//"첫번째 프레임" - 맞은 직후
					_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_DAMAGE_1,ANIM_WOMAN_BODY_A_G_DAMAGE_1,ANIM_WOMAN_BODY_A_O_DAMAGE_1));
				}
				//if(p_Damage.Bound_Num!=0){						//체공 프레임	
				//	if(LOW_BOUND < m_Hero_Physics->SaveAccel.z){	//낮은 바운딩
				//		_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_5,ANIM_WOMAN_BODY_A_G_FLYHI_5,ANIM_WOMAN_BODY_A_O_FLYHI_5));
				//	}else{											//높은 바운딩

				//		if(-HIGH_PITCH >= p_Position3D.z){										//상승중	
				//			_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_2,ANIM_WOMAN_BODY_A_G_FLYHI_2,ANIM_WOMAN_BODY_A_O_FLYHI_2));
				//		}else if(HIGH_PITCH > p_Position3D.z && -HIGH_PITCH < p_Position3D.z){	//체공중
				//			_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_3,ANIM_WOMAN_BODY_A_G_FLYHI_3,ANIM_WOMAN_BODY_A_O_FLYHI_3));
				//		}else{																	//낙하중								
				//			_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_4,ANIM_WOMAN_BODY_A_G_FLYHI_4,ANIM_WOMAN_BODY_A_O_FLYHI_4));
				//		}

				//	}
				//}else{							//"첫번째 프레임" - 맞거나 바운딩되는 직후의
				//	if(p_Damage.Bound==0){			//맞은 직후 모션인가?
				//		_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_1,ANIM_WOMAN_BODY_A_G_FLYHI_1,ANIM_WOMAN_BODY_A_O_FLYHI_1));
				//	}else{							//바운딩 직후 모션인가?
				//		if(LAST_BOUND < m_Hero_Physics->SaveAccel.z){	//Z 충격량이 LAST_BOUND 보다 작아서 더이상 튀지않을때
				//			_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_6,ANIM_WOMAN_BODY_A_G_FLYHI_6,ANIM_WOMAN_BODY_A_O_FLYHI_6));
				//		}else{											//Z 충격량이 LAST_BOUND 보다 커서 튀어오를때
				//			_ins_Hero->SetAnim(Check_jab(ANIM_WOMAN_BODY_A_S_FLYHI_3,ANIM_WOMAN_BODY_A_G_FLYHI_3,ANIM_WOMAN_BODY_A_O_FLYHI_3));
				//		}
				//	}
				//}
				break;

		}

	}


	void Character::Set_state_calculate(){
		//m_Hero_Physics = GL_NEW Physics(HERO_WRIGHT);
		//s_Ability._STR = 3;
		//s_Ability._DEX = 7;	//민첩-건너 공격력(100%) , 회피,크리
		//s_Ability._CON = 5;	//체력-생명력, 방어력(30%),생명력 회복률
		//s_Ability._INT = 5;	//지능-마법사 공격력(건너의 130%) , 마나
		//s_Ability._FAM = 0;	//명성-별도 스탯 , 퀘스트 습득 및 마을 사람과의 대화 필요

		int add_DEF = 0;

		int add_CRI = 0;
		int add_AGI = 0;
		int add_HP = 0;
		int add_MP = 0;

		int add_STR = 0;
		int add_DEX = 0; 
		int add_CON = 0; 
		int add_INT = 0; 

		for (int slot = 0 ; slot<10 ; slot++){
			add_DEF +=s_ItemAbil[slot].DEF;

			add_CRI +=s_ItemAbil[slot].CRI;
			add_AGI +=s_ItemAbil[slot].AGI;
			add_HP  +=s_ItemAbil[slot].HP ;
			add_MP  +=s_ItemAbil[slot].MP ;

			add_STR +=s_ItemAbil[slot].STR;
			add_DEX +=s_ItemAbil[slot].DEX;
			add_CON +=s_ItemAbil[slot].CON;
			add_INT +=s_ItemAbil[slot].INT;
		}



		s_Status.LIFE_MAX		= (s_Ability.CON+add_CON) * 15 + Get_Skill(SKILL_P_S_lifeUp);//패시브//생명 0 이 되면 죽는다.
		s_Status.LIFE_MAX=PER(s_Status.LIFE_MAX,(100+add_HP));
		if(s_Status.LIFE>s_Status.LIFE_MAX)s_Status.LIFE = s_Status.LIFE_MAX;

		s_Status.MANA_MAX		= (s_Ability.INT+add_INT) * 5;//마나 0 이 되면 스킬을 못쓴다.
		s_Status.MANA_MAX=PER(s_Status.MANA_MAX,(100+add_MP + Get_Skill(SKILL_P_G_manaUp)));//패시브
		if(s_Status.MANA>s_Status.MANA_MAX)s_Status.MANA = s_Status.MANA_MAX;
		
		s_Status.DAMAGE;		//공식에 의해 상대에게 직접적인 타격을 주는 수치
		s_Status.DEFENSE = (s_Ability.CON+add_CON)/3+(s_Ability.STR+add_STR) + add_DEF;		//방어구에 의해 방어율 계산에 적용 되는 수치(주인공만 있음/이것을 디스 플레이함)
		s_Status.CRITICAL = (s_Ability.DEX+add_DEX)/6 + add_CRI;		//크리티컬 데미지 들어갈 확률
		s_Status.AVOID = (s_Ability.DEX+add_DEX)/8 + add_AGI;			//적의 공격을 피할 확률
		s_Status.EXP_MAX = 18*((s_Status.LEVEL+1)*(s_Status.LEVEL-1)+10);

		s_Status.ATTACK_MAX[0]=s_Status.ATTACK_MIN[0]=(s_Ability.STR*2 + s_Ability.DEX);		//칼 공격력 (힘*2+민첩)
		s_Status.ATTACK_MAX[1]=s_Status.ATTACK_MIN[1]=(s_Ability.DEX*3 + s_Ability.STR);		//총 공격력(민첩*3+힘)
		s_Status.ATTACK_MAX[2]=s_Status.ATTACK_MIN[2]=(s_Ability.INT*2);		//마법 공격력(지능*2)

		s_Status.ATTACK_MAX[0] += s_ItemAbil[2].ATT_MAX + ((s_ItemAbil[5].ATT_MAX+s_ItemAbil[6].ATT_MAX) / 2);
		s_Status.ATTACK_MIN[0] += s_ItemAbil[2].ATT_MIN + ((s_ItemAbil[5].ATT_MIN+s_ItemAbil[6].ATT_MIN) / 2);
		s_Status.ATTACK_MAX[1] += s_ItemAbil[2].ATT_MAX + (s_ItemAbil[8].ATT_MAX / 2);
		s_Status.ATTACK_MIN[1] += s_ItemAbil[2].ATT_MIN + (s_ItemAbil[8].ATT_MIN / 2);
		s_Status.ATTACK_MAX[2] += s_ItemAbil[2].ATT_MAX + (s_ItemAbil[9].ATT_MAX / 2);
		s_Status.ATTACK_MIN[2] += s_ItemAbil[2].ATT_MIN + (s_ItemAbil[9].ATT_MIN / 2);


		

		s_Status.DEFENSE_PER = s_Status.DEFENSE/30;	//방어력/30 = N 방어력으로 자동 계산 되는 데미지 상쇄 % (유저에겐 보여주지 않음)
		s_Status.LIFE_RECOVERY;	//체력이 회복 되는 수치
		s_Status.MANA_RECOVERY;	//마나가 회복 되는 수치
		s_Status.DAMAGE;			//공식에 의해 상대에게 직접적인 타격을 주는 수치

		//100 프레임 이상의 쿨타임을 가지는 스킬들은 쿨타임을 일정량 줄인다
		if(Get_Skill(SKILL_P_O_coolTimeDown)){//패시브
			for(int xx = 0;xx<5;xx++){
				if(s_Skill.Equip_A[xx]>=0){
					int need_frm = a_Skill_Table[s_Skill.Equip_A[xx]*10 + s_Skill_Set.Skill_LEVEL[xx]][2];//스킬 쿨타임
					if(need_frm >= 100){
						s_Skill_Set.Cool_TimeMax[xx] = need_frm - Get_Skill(SKILL_P_O_coolTimeDown);//패시브
					}else{
						s_Skill_Set.Cool_TimeMax[xx] = need_frm;
					}
					if(s_Skill_Set.Cool_TimeNow[xx]>s_Skill_Set.Cool_TimeMax[xx]){
						s_Skill_Set.Cool_TimeNow[xx]=s_Skill_Set.Cool_TimeMax[xx];
					}
				}
			}
		}

	}


	void Character::Init_Skill(){//스킬들의 초기화를 처리

		for (int xx = 0; xx<5; xx++)
		{		//각자의 값에 변화가 있을경우 각 값들을 Free 시켜주는 코드 
			SUTIL_FreeSpriteInstance(_ins_Skill[xx][0]);
			SUTIL_FreeSpriteInstance(_ins_Skill[xx][1]);
			SUTIL_FreeSprite(_spr_Skill[xx]);
		}


		SUTIL_LoadAspritePack(PACK_SPRITE);//팩열기

		for(int xx = 0;xx<5;xx++){
			if(s_Skill.Equip_A[xx]>=0){
				_spr_Skill[xx] = SUTIL_LoadAspriteFromPack(PACK_SPRITE, s__ins_Skill[s_Skill.Equip_A[xx]][1]);
				_ins_Skill[xx][0] = GL_NEW ASpriteInstance(_spr_Skill[xx], 0, 0, NULL);//백
				_ins_Skill[xx][1] = GL_NEW ASpriteInstance(_spr_Skill[xx], 0, 0, NULL);//프런트
				_ins_Skill[xx][0]->m_sprite->SetBlendFrame(s__ins_Skill[s_Skill.Equip_A[xx]][2]);//블랜딩


				s_Skill_Set.Skill_LEVEL[xx] = s_Skill.Level_A[s_Skill.Equip_A[xx]];//스킬 레벨
				
				s_Skill_Set.Skill_ID[xx] = s__ins_Skill[s_Skill.Equip_A[xx]][0];//스킬 고유ID

				//100 프레임 이상의 쿨타임을 가지는 스킬들은 쿨타임을 일정량 줄인다

				

				s_Skill_Set.Cool_TimeMax[xx] = a_Skill_Table[s_Skill.Equip_A[xx]*10 + s_Skill_Set.Skill_LEVEL[xx]][2];//스킬 쿨타임
				if(s_Skill_Set.Cool_TimeMax[xx] >= 100){
					s_Skill_Set.Cool_TimeMax[xx] = s_Skill_Set.Cool_TimeMax[xx] - Get_Skill(SKILL_P_O_coolTimeDown);//패시브
				}

				s_Skill_Set.Need_Mana[xx] = a_Skill_Table[s_Skill.Equip_A[xx]*10 + s_Skill_Set.Skill_LEVEL[xx]][4];//필요마나
			}else{//빈소캣
				s_Skill_Set.Skill_ID[xx] = -1;
			}
		}

		SUTIL_ReleaseAspritePack();//팩닫기



	}



///////////////////////////////////////////////////////////////////
///////////////public//////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
	void Character::ReceiveAttack(Position3D& _Power){
		if(_b_Hero_Protect){
			s_Damage.Type = DAMAGE_NOT;
			_b_Key_Nullity = false;

			if(s_Status.LIFE>0){//주인공 사망시 스킬을 취소시켜버린다
				return;
			}
			
		}

		if(s_Status.LIFE<=0){//주인공 사망 초기화
			_move_Order = HERO_STOP;
			_b_Hero_Protect = false;
			_Power.z = -50;
		}


		Position3D Force; // 중력 또는 마찰계수 생성을 위한 임시 변수
		m_Hero_Physics->initForce();
		

		s_Damage.Down_Time=10;

		if(_b_JabNum == JAB_KNIGHT && _move_Order == HERO_CHARGE){
			if(_Power.x>0){
				_ins_Hero->m_posX+=2;//Set_left_right(SDIR_LEFT);
			}else if(_Power.x<0){
				_ins_Hero->m_posX-=2;//Set_left_right(SDIR_RIGHT);
			}
			return;

		}else if(_Power.z < 0 || (_ins_Hero->m_posZ) < 0){//Z축 파워가 있다면 또는 주인공이 공중에 있다면

			Force.Init(0,0,4);					//중력량
			m_Hero_Physics->setGrForce(Force);	//A - 중력 반영

			_b_Key_Nullity=true;
			s_Damage.Type = DAMAGE_FLY;
			s_Damage.Bound = 0;//바운딩 횟수 초기화
			s_Damage.Bound_Num = 0;//바운딩 인덱스 초기화
		}else{//Z축 파워가 없다면 뒤로 밀려나는 데미지
			s_Damage.Sum += ( _Power.x > 0 ? _Power.x : -_Power.x );//물리량 누적

			if( _Power.x > 0 ){	//가해지는 힘이 오른쪽이다
				//Set_left_right(SDIR_LEFT);
				s_Damage.Direction = D_RIGHT;
				Force.Init(-2,0,0);					//-마찰계수 생성
			}else{				//가해지는 힘이 왼쪽이다
				//Set_left_right(SDIR_RIGHT);
				s_Damage.Direction = D_LEFT;
				Force.Init(+2,0,0);					//+마찰계수 생성
			}

			m_Hero_Physics->setGrForce(Force);	//마찰계수 반영
			
			
			if(true || s_Damage.Sum < 50){
				s_Damage.Type = DAMAGE_GROUND;//넘어지진 않았다
			}else{
				_b_Key_Nullity=true;
				s_Damage.Type = DAMAGE_DOWN;//넘어진다
				s_Damage.Sum=0;
			}

			s_Damage.Bound = 0;//바운딩 횟수 초기화
			s_Damage.Bound_Num = 0;//바운딩 인덱스 초기화

		}

		m_Hero_Physics->setForce(_Power);	//B - 넘어온 파워를 반영
		
		
		
	}

	void Character::RCV_Damage(int monDamage){
		
		s_Status.LIFE -= monDamage;

		if(s_Status.LIFE < 0) s_Status.LIFE = 0;
	}
	int Character::SND_Damage(int monLevel, int monElemental , int monDefense , int D_index){

		int ATT = RND(s_Status.ATTACK_MIN[_b_JabNum],s_Status.ATTACK_MAX[_b_JabNum]);
		int percent = 100;
		int cri_Num = s_Status.CRITICAL;

		Critical = false;
		s_Status.DEBUFF = 0;
		

		int skill_Num = (D_index-50)/2;
		int table_Index = (skill_Num * 10) + s_Skill.Level_A[skill_Num];
		switch(D_index){//공식 인덱스 번호
		//평타
			case 0: percent = 100;
				if(RND(1,100) <= Get_Skill(SKILL_P_S_poisonAttack))s_Status.DEBUFF = s_Status.DEBUFF|DEBUF_POISON;//패시브
				break;
		//건 차지샷
			case 10:percent = 0;break;
		//건 차지샷
			case 40:percent = 200;break;
		//잡기
			case 41:percent = 400;break;
			case 42:percent = 200;break;

		/*어기어검*/
			case 50:percent = a_Skill_Table[table_Index][0]+Get_Skill(SKILL_P_S_wingSword);break;//패시브
			case 51:percent = a_Skill_Table[table_Index][1]+Get_Skill(SKILL_P_S_wingSword);break;//패시브
		/*다운스케이팅*/					
			case 52:percent = a_Skill_Table[table_Index][0];break;	
			case 53:percent = a_Skill_Table[table_Index][1];break;
		/*하이크래시*/						
			case 54:percent = a_Skill_Table[table_Index][0];break;	
			case 55:percent = a_Skill_Table[table_Index][1];break;
		/*공간참격*/						
			case 56:percent = a_Skill_Table[table_Index][0];cri_Num+=Get_Skill(SKILL_P_S_spaceCut);break;//패시브	
			case 57:percent = a_Skill_Table[table_Index][1];cri_Num+=Get_Skill(SKILL_P_S_spaceCut);break;//패시브
		/*돌격*/							
			case 58:percent = a_Skill_Table[table_Index][0];break;	
			case 59:percent = a_Skill_Table[table_Index][1];break;
		/*드래곤헌팅*/						
			case 60:percent = a_Skill_Table[table_Index][0];break;	
			case 61:percent = a_Skill_Table[table_Index][1];break;
		/*3연난무*/							
			case 62:percent = a_Skill_Table[table_Index][0];break;	
			case 63:percent = a_Skill_Table[table_Index][1];break;


		/*백다운피스톨*/
			case 64:percent = a_Skill_Table[table_Index][0];break;	
			case 65:percent = a_Skill_Table[table_Index][1];break;
		/*스턴*/							
			case 66:percent = a_Skill_Table[table_Index][0];s_Status.DEBUFF = s_Status.DEBUFF|DEBUF_STUN;break;	
			case 67:percent = a_Skill_Table[table_Index][1];break;
		/*스핀샷*/							
			case 68:percent = a_Skill_Table[table_Index][0];break;	
			case 69:percent = a_Skill_Table[table_Index][1];break;
		/*힐링팩*/							
			case 70:percent = a_Skill_Table[table_Index][0];break;	
			case 71:percent = a_Skill_Table[table_Index][1];break;
		/*안드로메다*/						
			case 72:percent = a_Skill_Table[table_Index][0];break;	
			case 73:percent = a_Skill_Table[table_Index][1];break;
		/*발칸샷*/							
			case 74:percent = a_Skill_Table[table_Index][0];break;	
			case 75:percent = a_Skill_Table[table_Index][1];break;
		/*3콤건컷터*/						
			case 76:percent = a_Skill_Table[table_Index][0];break;	
			case 77:percent = a_Skill_Table[table_Index][1];break;


		/*쌍염탄*/
			case 78:percent = a_Skill_Table[table_Index][0];break;
			case 79:percent = a_Skill_Table[table_Index][1];break;
		/*토템*/							
			case 80:percent = a_Skill_Table[table_Index][0];break;	
			case 81:percent = a_Skill_Table[table_Index][1];break;
		/*스카이락*/						
			case 82:percent = a_Skill_Table[table_Index][0];break;	
			case 83:percent = a_Skill_Table[table_Index][1];break;
		/*아이스드래곤*/					
			case 84:percent = a_Skill_Table[table_Index][0];break;	
			case 85:percent = a_Skill_Table[table_Index][1];s_Status.DEBUFF = s_Status.DEBUFF|DEBUF_FREEZE;break;
		/*블랙홀*/							
			case 86:percent = a_Skill_Table[table_Index][0];s_Status.DEBUFF = s_Status.DEBUFF|DEBUF_CONFUSE;break;	
			case 87:percent = a_Skill_Table[table_Index][1];break;
		/*뇌룡승천*/						
			case 88:percent = a_Skill_Table[table_Index][0];break;	
			case 89:percent = a_Skill_Table[table_Index][1];break;
		/*아이스에로우*/					
			case 90:percent = a_Skill_Table[table_Index][0];s_Status.DEBUFF = s_Status.DEBUFF|DEBUF_ICE;break;	
			case 91:percent = a_Skill_Table[table_Index][1];break;

		}

		ATT = PER(ATT,percent);//데미지 퍼센트 적용

		if(s_Status.LEVEL > monLevel + 4)ATT=PER(ATT,140);//본스터보다 5랩 높으면
		if(s_Status.LEVEL < monLevel - 4)ATT=PER(ATT, 70);//본스터보다 5랩 낮으면


		switch(_b_JabNum){//상성
			case JAB_KNIGHT:
				switch(monElemental){
					case ELEMENTAL_SNIPS:ATT=PER(ATT,130);	break;
					case ELEMENTAL_PAPER:ATT=PER(ATT, 40);	break;
				}
				ATT = PER(ATT,100+Get_Skill(SKILL_P_S_swordMaster)+Get_Skill(SKILL_P_O_immuneAttUp));//패시브
				break;
			case JAB_GUNNER:
				switch(monElemental){
					case ELEMENTAL_STONE:ATT=PER(ATT, 40);	break;
					case ELEMENTAL_PAPER:ATT=PER(ATT,130);	break;
				}
				ATT = PER(ATT,100+Get_Skill(SKILL_P_G_gunMaster)+Get_Skill(SKILL_P_O_immuneAttUp));//패시브
				break;
			case JAB_MAGE:
				switch(monElemental){
					case ELEMENTAL_STONE:ATT=PER(ATT,130);	break;
					case ELEMENTAL_SNIPS:ATT=PER(ATT, 40);	break;
				}
				ATT = PER(ATT,100+Get_Skill(SKILL_P_O_orbMaster)+Get_Skill(SKILL_P_O_immuneAttUp));//패시브
				break;
		}

		if(s_Status.LIFE<PER(s_Status.LIFE_MAX,20)) 
			ATT = PER(ATT,100+Get_Skill(SKILL_P_S_deadlyAttack));//패시브

		//방어율
		//ATT=MAX(ATT/8+1, PER(ATT,100-monDefense))+10000;
		ATT=MAX(ATT/8, PER(ATT,100-monDefense));
		
		
		
		if(RND(1,100)<=cri_Num) {
			ATT*=2;
			ATT = PER(ATT,100+Get_Skill(SKILL_P_G_criIncrease));//패시브
			Critical=true;
		}



//내가 뭘로때렸나...
		//	XYWH(AS_Inst, Frame_Index)	
		//AS_Inst->m_sprite->_frames_rects[Frame_Index]


		//int att_Frame_Index = p_Attacker->GetAnimFrame();// Attacker 애니메이션의 프레임에 고유 Index 값을 얻어온다


		//monDamage
		return ATT;
	}
	void Character::RCV_Debuff(int Debuff_Type){
		if(Debuff_Type == 0)//디버프가 없으면 리턴
			return;

		s_Debuff.All |=  Debuff_Type;//디버프를 계속 추가해 준다
		s_Debuff.color = 0;//디버프 컬러 초기화
		bool novice = true;
		for(int xx = 0;xx<3;xx++){//기존에 디버프중에 중복이 되는지 검사한다.
			if(s_Debuff.stack[xx][0]==Debuff_Type){//이미 있다면 그것을 0번과 위치를 바꿔준다(신규로 취급)
				novice = false;

				short yy = s_Debuff.stack[0][0];
				s_Debuff.stack[0][0] = s_Debuff.stack[xx][0];
				s_Debuff.stack[xx][0] = yy;

				s_Debuff.stack[xx][1] = s_Debuff.stack[0][1];
				s_Debuff.stack[0][1] = 0;//디버프 타이머 초기화
				break;
			}
		}
		if(novice){//중복이 아니라면 0번째에 기록해준다

			if(s_Debuff.stack[2][0])s_Debuff.All -= s_Debuff.stack[2][0];//마지막 버프에 값이 있으면 해제시킨다

			s_Debuff.stack[2][0] = s_Debuff.stack[1][0];
			s_Debuff.stack[2][1] = s_Debuff.stack[1][1];
			s_Debuff.stack[1][0] = s_Debuff.stack[0][0];
			s_Debuff.stack[1][1] = s_Debuff.stack[0][1];
			// 하나씩 뒤로 밀어서 확보된 0번에 값 기록
			s_Debuff.stack[0][0] = Debuff_Type;
			s_Debuff.stack[0][1] = 0;
		}
		switch(Debuff_Type){// 예외적인 처리를 하는부분 
			case DEBUF_WEAK:
				break;
		}
	}
	int Character::SND_Debuff(int Attack_Type){
		return s_Status.DEBUFF;
			//DEBUF_WEAK		(1 << 1)			//약화
			//DEBUF_ICE			(1 << 2)			//아이스
			//DEBUF_FREEZE		(1 << 3)			//동결
			//DEBUF_STUN		(1 << 4)			//스턴
			//DEBUF_STONE		(1 << 5)			//석화
			//DEBUF_POISON		(1 << 6)			//독
			//DEBUF_CONFUSE		(1 << 7)			//혼란
			//DEBUF_VAMPIRE		(1 << 8)			//흡혈A
			//DEBUF_DE_VAMPIRE	(1 << 9)			//흡혈B
			//DEBUF_MANABURN	(1 << 10)			//마나연소
			//DEBUF_SILENCE		(1 << 11)			//침묵
	}
	int Character::Set_Exp(int _Exp){
		s_Status.EXP += _Exp;
		if(s_Status.EXP_MAX < s_Status.EXP){
			s_Status.EXP -= s_Status.EXP_MAX;
			s_Status.LEVEL++;
			s_Ability.POINT += 3;
			if(RND(1,100) <= Get_Skill(SKILL_P_O_addAbility))
			{s_Ability.POINT += 2;}//패시브

			s_Ability.STR+=2;
			s_Ability.DEX+=2;
			s_Ability.CON+=2;
			s_Ability.INT+=2;
			s_Ability.FAM+=2;


			Set_state_calculate();
			if(!s_LV_Eff.act){
				s_LV_Eff.act = true;
				s_LV_Eff.LVupEff_Num = 0;
				s_LV_Eff.LVup_ActionEnd = false;
			}
			s_Status.LIFE = s_Status.LIFE_MAX;
			s_Status.MANA = s_Status.MANA_MAX;
			
		}
		return 0;
	}

	void Character::InitCharPos(int x, int y, int Look)
	{
		_ins_Hero->m_posX = _ins_Hero->m_lastX = x;
		_ins_Hero->m_posY = _ins_Hero->m_lastY = y;
		if(Look){
			_b_LookRight = Look;
			SUTIL_SetDirAsprite(_ins_Hero, _b_LookRight);
		}
		
	}

	void Character::InitCostume(){
		for(int xx = 0;xx<_spr_Hero->_nModules;xx++)
			_spr_Hero->_modules_data_pal[xx] = 0;

		for(int xx = 0;xx<LENGTH_HEAD;xx++)
			_spr_Hero->_modules_data_pal[START_HEAD+xx] = PAL_HEAD;

		for(int xx = 0;xx<LENGTH_BODY;xx++)
			_spr_Hero->_modules_data_pal[START_BODY+xx] = PAL_BODY;

		for(int xx = 0;xx<LENGTH_ARM;xx++)
			_spr_Hero->_modules_data_pal[START_ARM+xx] = PAL_ARM;

		for(int xx = 0;xx<LENGTH_LEG;xx++)
			_spr_Hero->_modules_data_pal[START_LEG+xx] = PAL_LEG;


		for(int xx = 0;xx<LENGTH_KNIFE1;xx++)
			_spr_Hero->_modules_data_pal[START_KNIFE1+xx] = PAL_KNIFE1;

		for(int xx = 0;xx<LENGTH_KNIFE2;xx++)
			_spr_Hero->_modules_data_pal[START_KNIFE2+xx] = PAL_KNIFE2;

		for(int xx = 0;xx<LENGTH_GUN;xx++)
			_spr_Hero->_modules_data_pal[START_GUN+xx] = PAL_GUN;

		for(int xx = 0;xx<LENGTH_ORB;xx++)
			_spr_Hero->_modules_data_pal[START_ORB+xx] = PAL_ORB;

		SUTIL_LoadAspritePack(PACK_SPRITE_COSTUME);//팩열기
		ChangeCostume(PAL_HEAD,0,0);
		ChangeCostume(PAL_BODY,0,0);
		ChangeCostume(PAL_ARM,0,0);
		ChangeCostume(PAL_LEG,0,0);
		////////////////////////////////////
		ChangeCostume(PAL_KNIFE1,0,0);
		ChangeCostume(PAL_KNIFE2,5,0);
		ChangeCostume(PAL_GUN,0,0);
		ChangeCostume(PAL_ORB,0,0);
		SUTIL_ReleaseAspritePack();//팩닫기
	}
	void Character::ChangeCostume(int part,int index,int pal)
	{
		int Start=0;
		int Length=0;
		switch(part){
			case PAL_HEAD	:index=Check_sex(SPRITE_COSTUME_MAN_0_HEAD,SPRITE_COSTUME_WOMAN_0_HEAD)+index		;Start=START_HEAD	;Length=LENGTH_HEAD	;break;
			case PAL_BODY	:index=Check_sex(SPRITE_COSTUME_MAN_0_BODY,SPRITE_COSTUME_WOMAN_0_BODY)+index		;Start=START_BODY	;Length=LENGTH_BODY	;break;
			case PAL_ARM	:index=Check_sex(SPRITE_COSTUME_MAN_0_ARM,SPRITE_COSTUME_WOMAN_0_ARM)+index		;Start=START_ARM	;Length=LENGTH_ARM	;break;
			case PAL_LEG	:index=Check_sex(SPRITE_COSTUME_MAN_0_LEG,SPRITE_COSTUME_WOMAN_0_LEG)+index		;Start=START_LEG	;Length=LENGTH_LEG	;break;
			case PAL_KNIFE1	:index=SPRITE_COSTUME_HUMAN_0_SWORD+index	;Start=START_KNIFE1 ;Length=LENGTH_KNIFE1;break;
			case PAL_KNIFE2	:index=SPRITE_COSTUME_HUMAN_0_SWORD+index	;Start=START_KNIFE2	;Length=LENGTH_KNIFE2;break;
			case PAL_GUN	:index=SPRITE_COSTUME_HUMAN_0_GUN+index		;Start=START_GUN	;Length=LENGTH_GUN	;break;
			case PAL_ORB	:index=SPRITE_COSTUME_HUMAN_0_ORB+index		;Start=START_ORB	;Length=LENGTH_ORB	;break;
		}

		//교체할 코스튬을 불러온다//SUTIL_LoadAspriteFromPack은 SUTIL_LoadSprite와는 달리 팩의 열고 닫는 사이에가 아니면 실행되지않는다
		class ASprite*	Part_Sprite = SUTIL_LoadAspriteFromPack(PACK_SPRITE_COSTUME, index);



		int gap = - (_spr_Hero->_modules_data_off[Start+Length] - _spr_Hero->_modules_data_off[Start])//삭제할 파츠
			+ (Part_Sprite->_modules_data_off[Part_Sprite->_nModules]);//대체할 파츠
		int len = _spr_Hero->_modules_data_off[_spr_Hero->_nModules] + gap;

		byte* temp_data = GL_NEW byte[len];//새로운 데이터풀을 만든다

		arraycopy(_spr_Hero->_modules_data, 0, 
			temp_data, 0, 
			_spr_Hero->_modules_data_off[Start]);//교체 part 이전 부분을 복제한다

		arraycopy(Part_Sprite->_modules_data, 0, 
			temp_data, _spr_Hero->_modules_data_off[Start], 
			Part_Sprite->_modules_data_off[Part_Sprite->_nModules]);//교체 part 대상을 복제한다

		arraycopy(_spr_Hero->_modules_data, _spr_Hero->_modules_data_off[Start+Length], 
			temp_data, _spr_Hero->_modules_data_off[Start] + Part_Sprite->_modules_data_off[Part_Sprite->_nModules], 
			_spr_Hero->_modules_data_off[_spr_Hero->_nModules] - _spr_Hero->_modules_data_off[Start+Length]);//교체 part 이후 부분을 복제한다


		SAFE_DEL_ARRAY(_spr_Hero->_modules_data);
		_spr_Hero->_modules_data = temp_data;
		_spr_Hero->_modules_image->m_modulesPixel = (unsigned char*)temp_data;
		int * aa = _spr_Hero->_modules_image->m_modulesPixelOffset;

		//교체 part off 변경
		for(int xx = 0;xx<Length;xx++){
			_spr_Hero->_modules_data_off[Start+xx]=_spr_Hero->_modules_data_off[Start]+Part_Sprite->_modules_data_off[xx];
		}
		//교체 part 이후의 off값에 Gap 반영
		for(int xx = Start+Length;xx<=_spr_Hero->_nModules;xx++){
			_spr_Hero->_modules_data_off[xx] += gap;
		}

		////팔렛트 값 바꿔치기
		for(int xx = 0;xx<Part_Sprite->_colors;xx++){
			*(_spr_Hero->_pal[part]+xx) = *(Part_Sprite->_pal[pal]+xx);
		}

		SUTIL_FreeSprite(Part_Sprite);//스프라이트 지우기

	}


	void Character::Check_Sound(){
		int soundID = -1;
		switch (_move_Order){
			case HERO_ATTACK1:
				switch(_b_JabNum){
					case JAB_KNIGHT:	if(_ins_Hero->m_nCrtFrame==0)soundID = EFFECT_SWORD_A;	break;
					case JAB_GUNNER:	if(_ins_Hero->m_nCrtFrame==2)soundID = EFFECT_GUN_A;	break;
					case JAB_MAGE:		if(_ins_Hero->m_nCrtFrame==4)soundID = EFFECT_ORB_A;	break;
				}
				
				break;
			case HERO_ATTACK2:
				switch(_b_JabNum){
					case JAB_KNIGHT:	if(_ins_Hero->m_nCrtFrame==2)soundID = EFFECT_SWORD_B;	break;
					case JAB_GUNNER:	if(_ins_Hero->m_nCrtFrame==6)soundID = EFFECT_GUN_A;	break;
					case JAB_MAGE:		if(_ins_Hero->m_nCrtFrame==2)soundID = EFFECT_ORB_B;	break;
				}
				break;
			case HERO_ATTACK3:
				switch(_b_JabNum){
					case JAB_KNIGHT:	if(_ins_Hero->m_nCrtFrame==4)soundID = EFFECT_SWORD_C;	break;
					case JAB_GUNNER:	if(_ins_Hero->m_nCrtFrame==4)soundID = EFFECT_GUN_A;
										if(_ins_Hero->m_nCrtFrame==11)soundID = EFFECT_GUN_B;
										break;
					case JAB_MAGE:		if(_ins_Hero->m_nCrtFrame==1)soundID = EFFECT_ORB_C;	break;
				}
				break;
			case HERO_ATTACK4:
				if(_ins_Hero->m_nCrtFrame==2)soundID = EFFECT_SWORD_D;
				break;

			case HERO_ATTACK1END:
				if(_b_JabNum == JAB_GUNNER && _ins_Hero->m_nCrtFrame==3)soundID = EFFECT_GUN_B;
				break;
			case HERO_ATTACK2END:
				if(_b_JabNum == JAB_GUNNER && _ins_Hero->m_nCrtFrame==4)soundID = EFFECT_GUN_B;
				break;
		}

		if(soundID >=0){ 
			SUTIL_Sound_Play(soundID,false);
			//SUTIL_SOUND_Update();
		}

	}
	bool Character::Set_MustAction(int x, int y,int Action,int Look){
		s_MustAction.must_X = x;
		s_MustAction.must_Y = y;
		s_MustAction.must_Action = Action;
		s_MustAction.must_Look = Look;

		return true;
	}



	int Character::Get_Skill(int passive_Num){//패시브
		passive_Num-=SKILL_P_S_swordMaster;
		if(passive_Num<0)return 0;

		for(int xx = 0;xx<9;xx++){
			if(Character::s_Skill.Equip_P[xx] == (passive_Num)){
				return a_Passive_Table[ passive_Num * 5 + (s_Skill.Level_P[passive_Num]-1) ][0];//-1 의 이유는 배열은 0부터시작하니까
			}
		}
		return 0;
	}
