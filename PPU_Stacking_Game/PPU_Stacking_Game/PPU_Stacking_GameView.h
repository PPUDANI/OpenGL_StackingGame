
// PPU_Stacking_GameView.h: CPPUStackingGameView 클래스의 인터페이스
//

#pragma once


class CPPUStackingGameView : public CView
{
protected: // serialization에서만 만들어집니다.
	CPPUStackingGameView() noexcept;
	DECLARE_DYNCREATE(CPPUStackingGameView)

// 특성입니다.
public:
	CPPUStackingGameDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CPPUStackingGameView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	HGLRC m_hRC;
	CDC* m_pDC;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	bool InitializeOpenGL();
	bool SetupPixelFormat();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void RenderScene();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	GLfloat m_fWinHeight;
	GLfloat m_fWinWidth;


	//스페이스바 누른 횟수
	int SpaceCount;

	//보드 변수
	GLsizei Board_SizeX;
	GLsizei Board_SizeY;
	GLfloat Board_PosX;
	GLfloat Board_PosY;


	// 움직이는 블록 데이터 배열
	//  PosX    PosY    SizeX  SizeY      
	// [n][0]  [n][1]   [n][2]   [n][3]
	GLfloat MoveBlock[100][4] = { 0, };

	//블록 변수 최소, 최대 이동 좌표
	GLfloat BlockStartPos;
	GLfloat BlockEndPos;

	//블록 높이
	GLfloat addHeight;

	//블록 씬 마다 이동 거리
	GLfloat BlockSpeed;

	//점수판 규격 예시 변수 선언 및 초기화(숫자 표현 배열 때문에 여기서 함)

	GLfloat SB_UnitRatio = 13.0f; // 디지털 유닛 기본 비율
	GLfloat SB_SizeX = 4 * SB_UnitRatio; // 가로 비율
	GLfloat SB_SizeY = 5 * SB_UnitRatio; // 세로 비율

	GLfloat SB1_PosX = 330.0f; //1seat X좌표
	GLfloat SB2_PosX = SB1_PosX - SB_SizeX - 3.0f; //2seat X좌표
	GLfloat SB_PosY = 390.0f; //점수판 Y좌표


	//디지털 숫자 표현 데이터
	GLfloat SB_Num[7][5] =
	{
		// 수평 유닛
		{         SB1_PosX,           SB_PosY + 4 * SB_UnitRatio,   4 * SB_UnitRatio,    SB_UnitRatio,            SB2_PosX           },
		{         SB1_PosX,           SB_PosY + 2 * SB_UnitRatio,   4 * SB_UnitRatio,    SB_UnitRatio,            SB2_PosX           },
		{         SB1_PosX,                     SB_PosY,            4 * SB_UnitRatio,    SB_UnitRatio,            SB2_PosX           },

		// 수직 유닛
		{         SB1_PosX,             SB_PosY + SB_SizeY / 2,       SB_UnitRatio,      SB_SizeY / 2,            SB2_PosX           },
		{         SB1_PosX,                     SB_PosY,              SB_UnitRatio,      SB_SizeY / 2,            SB2_PosX           },
		{ SB1_PosX + 3 * SB_UnitRatio,    SB_PosY + SB_SizeY / 2,       SB_UnitRatio,      SB_SizeY / 2,   SB2_PosX + 3 * SB_UnitRatio },
		{ SB1_PosX + 3 * SB_UnitRatio,            SB_PosY,              SB_UnitRatio,      SB_SizeY / 2,   SB2_PosX + 3 * SB_UnitRatio }
	};

	bool CameraUp;
	GLfloat CameraUptotal;
	int CameraUpStd;

	int SC_1seat;
	int SC_10seat;


	int CameraSpeed;
	void GameReset();
};

#ifndef _DEBUG  // PPU_Stacking_GameView.cpp의 디버그 버전
inline CPPUStackingGameDoc* CPPUStackingGameView::GetDocument() const
   { return reinterpret_cast<CPPUStackingGameDoc*>(m_pDocument); }
#endif

