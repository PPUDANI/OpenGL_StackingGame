
// PPU_Stacking_GameView.cpp: CPPUStackingGameView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "PPU_Stacking_Game.h"
#endif

#include "PPU_Stacking_GameDoc.h"
#include "PPU_Stacking_GameView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPPUStackingGameView

IMPLEMENT_DYNCREATE(CPPUStackingGameView, CView)

BEGIN_MESSAGE_MAP(CPPUStackingGameView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CPPUStackingGameView 생성/소멸

CPPUStackingGameView::CPPUStackingGameView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

	//보드 변수 초기화
	Board_PosX = 160.0f;
	Board_PosY = 0.0f;
	Board_SizeX = 350.0f;
	Board_SizeY = 40.0f;

	//스페이스바 초기화
	SpaceCount = 0;

	//블록 속도 초기화
	BlockSpeed = 4.0f;

	//블록 높이 초기화
	addHeight = 18.0f;

	//블록 변수 이동의 시작, 끝 좌표
	BlockStartPos = 30.0f;
	BlockEndPos = 650.0f;

	// 시점 변환 변수 초기화
	CameraUptotal = 0.0f;
	CameraUpStd = 15;
	CameraSpeed = 3;
	CameraUp = FALSE;

	//이동 블록 배열초기화
	MoveBlock[SpaceCount][0] = BlockStartPos;
	MoveBlock[SpaceCount][1] = Board_SizeY;
	MoveBlock[SpaceCount][2] = Board_SizeX;
	MoveBlock[SpaceCount][3] = addHeight;

	//점수판 자릿수 변수 초기화
	SC_1seat = 0;
	SC_10seat = 0;
}

CPPUStackingGameView::~CPPUStackingGameView()
{
}

BOOL CPPUStackingGameView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

// CPPUStackingGameView 그리기

void CPPUStackingGameView::OnDraw(CDC* pDC)
{
	CPPUStackingGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	// 먼저 버퍼를 초기화 한다. 
	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderScene(); // 이 함수는 실제로 그리는 코드를 수행한다.
	::glFinish(); // 렌더링 파이프라인을 열어 실제로 화면상에 그린다.

	// 만약 더블 버퍼링이 사용된다면 뒤 버퍼의 내용을 앞 버퍼와 바뀐다. 
	::SwapBuffers(m_pDC->GetSafeHdc());
}


// CPPUStackingGameView 진단

#ifdef _DEBUG
void CPPUStackingGameView::AssertValid() const
{
	CView::AssertValid();
}

void CPPUStackingGameView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPPUStackingGameDoc* CPPUStackingGameView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPPUStackingGameDoc)));
	return (CPPUStackingGameDoc*)m_pDocument;
}
#endif //_DEBUG


// CPPUStackingGameView 메시지 처리기


int CPPUStackingGameView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	InitializeOpenGL();
	SetTimer(0, 16.66f, NULL);
	return 0;
}


bool CPPUStackingGameView::InitializeOpenGL()
{
	// TODO: 여기에 구현 코드 추가.
	m_pDC = new CClientDC(this); // Get a DC for the Client Area

	// 함수는 첫 번째로 클라이언트 영역에 대한 DC를 얻고 NULL이 아닌지 확인한다. 
	if (m_pDC == NULL)
	{
		MessageBox(_T("Error Obtaining DC")); return FALSE;
	}

	// 픽셀 포맷을 설정하기 위해 SetupPixelFormat() 함수를 호출하고 성공했는지를 검사한다. 
	if (!SetupPixelFormat())
	{
		return FALSE;
	}

	// DC로부터 렌더링 컨텍스트를 생성하고 성공했는지 검사한다. 
	m_hRC = ::wglCreateContext(m_pDC->GetSafeHdc());

	if (m_hRC == 0)
	{
		MessageBox(_T("Error Creating RC")); return FALSE;
	}

	// 생성된 RC를 현재 사용한다고 지정한다. 
	if (::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC) == FALSE)
	{
		MessageBox(_T("Error making RC Current"));
		return FALSE;
	}

	// Specify Black as the clear color
	::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Specify the back of the buffer as clear depth
	::glClearDepth(1.0f); // Enable Depth Testing
	::glEnable(GL_DEPTH_TEST); return TRUE;
}


bool CPPUStackingGameView::SetupPixelFormat()
{
	// TODO: 여기에 구현 코드 추가.
	// OpenGL 윈도우의 기능들은 OpenGL 렌더링 윈도우를 위해 선택된 픽셀 포맷에 의존한다. 
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), // 구조체의 크기
		1, // 버전 넘버
		PFD_DRAW_TO_WINDOW | // 윈도우에 그린다.
		PFD_SUPPORT_OPENGL | // OpenGL을 사용한다.
		PFD_DOUBLEBUFFER, // 더블버퍼링 사용한다.
		PFD_TYPE_RGBA, // RGBA 타입의 색을 사용한다.
		24, // 24비트 컬러 사용
		0, 0, 0, 0, 0, 0, // 컬러비트 무시
		0, // 알파버퍼 사용 안 함
		0, // 시프트 비트 무시
		0, // 누적버퍼 사용 안 함
		0, 0, 0, 0, // 누적비트 사용 안 함
		16, // 16비트 깊이 버퍼
		0, // 스텐실 버퍼 사용 안 함
		0, // 보조버퍼 사용 안 함
		PFD_MAIN_PLANE, // 메인 레이어
		0, // reserved
		0, 0, 0 // 레이어 마스크 사용 안 함
	};

	// 픽셀 포맷을 설정하고 ChoosePixelFormat 함수를 호출해 이용 가능한 
	// 가장 근접한 픽셀 포맷을 구한다.
	int m_nPixelFormat = ::ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);
	if (m_nPixelFormat == 0)
	{
		return FALSE;
	}

	// SetPixelFormat 함수를 호출하여 함수는 인덱스에 의해 지정된 포맷에 지정된 
	// 디바이스 컨텍스트의 픽셀 포맷을 설정한다.
	if (::SetPixelFormat(m_pDC->GetSafeHdc(), m_nPixelFormat, &pfd) == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}


void CPPUStackingGameView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	GLdouble aspect_ratio;

	// width/height ratio 변수 선언
	// 윈도우의 높이나 폭이 0인 경우 Return 한다.
	if (0 >= cx || 0 >= cy)
	{
		return;
	}
	::glViewport(0, 0, cx, cy);// 전체 클라이언트 영역을 Viewport로 설정

	// Aspect ratio을 계산
	// 이것이 모든 dimension 비율을 똑같이 유지한다.
	aspect_ratio = (GLdouble)cx / (GLdouble)cy;

	// Projection matrix를 설정하고 초기화
	::glMatrixMode(GL_PROJECTION); ::glLoadIdentity();

	if (cx <= cy)
	{
		m_fWinHeight = 150.0f / aspect_ratio;
		m_fWinWidth = 150.0f;
	}
	else
	{
		m_fWinHeight = 460.0f;
		m_fWinWidth = 460.0f * aspect_ratio;
	}

	gluOrtho2D(0.0f, m_fWinWidth, 0.0f, m_fWinHeight);
	// Modelview Matrix를 선정하고 초기화
	// 이것은 나중에 모든 명령이 Modelview matrix에 영향을 받는다는 것을 의미한다. 
	::glMatrixMode(GL_MODELVIEW); ::glLoadIdentity();
}


void CPPUStackingGameView::RenderScene()
{
	// TODO: 여기에 구현 코드 추가.
	// TODO: 여기에 구현 코드 추가.
	SC_1seat = SpaceCount % 10;
	SC_10seat = SpaceCount / 10;

	//점수판 점수 출력
	// 점수 컬러


	glColor3f(1.0f, 1.0f, 0.0f);

	// 10의 자리 점수 출력
	switch (SC_10seat) {
	case 0:
		glRectf(SB_Num[0][4], SB_Num[0][1], SB_Num[0][4] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[2][4], SB_Num[2][1], SB_Num[2][4] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[3][4], SB_Num[3][1], SB_Num[3][4] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[4][4], SB_Num[4][1], SB_Num[4][4] + SB_Num[4][2], SB_Num[4][1] + SB_Num[4][3]);
		glRectf(SB_Num[5][4], SB_Num[5][1], SB_Num[5][4] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][4], SB_Num[6][1], SB_Num[6][4] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 1:
		glRectf(SB_Num[5][4], SB_Num[5][1], SB_Num[5][4] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][4], SB_Num[6][1], SB_Num[6][4] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 2:
		glRectf(SB_Num[0][4], SB_Num[0][1], SB_Num[0][4] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[1][4], SB_Num[1][1], SB_Num[1][4] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[2][4], SB_Num[2][1], SB_Num[2][4] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[4][4], SB_Num[4][1], SB_Num[4][4] + SB_Num[4][2], SB_Num[4][1] + SB_Num[4][3]);
		glRectf(SB_Num[5][4], SB_Num[5][1], SB_Num[5][4] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		break;
	case 3:
		glRectf(SB_Num[0][4], SB_Num[0][1], SB_Num[0][4] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[1][4], SB_Num[1][1], SB_Num[1][4] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[2][4], SB_Num[2][1], SB_Num[2][4] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[5][4], SB_Num[5][1], SB_Num[5][4] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][4], SB_Num[6][1], SB_Num[6][4] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 4:
		glRectf(SB_Num[1][4], SB_Num[1][1], SB_Num[1][4] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[3][4], SB_Num[3][1], SB_Num[3][4] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[5][4], SB_Num[5][1], SB_Num[5][4] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][4], SB_Num[6][1], SB_Num[6][4] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 5:
		glRectf(SB_Num[0][4], SB_Num[0][1], SB_Num[0][4] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[1][4], SB_Num[1][1], SB_Num[1][4] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[2][4], SB_Num[2][1], SB_Num[2][4] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[3][4], SB_Num[3][1], SB_Num[3][4] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[6][4], SB_Num[6][1], SB_Num[6][4] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 6:
		glRectf(SB_Num[0][4], SB_Num[0][1], SB_Num[0][4] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[1][4], SB_Num[1][1], SB_Num[1][4] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[2][4], SB_Num[2][1], SB_Num[2][4] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[3][4], SB_Num[3][1], SB_Num[3][4] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[4][4], SB_Num[4][1], SB_Num[4][4] + SB_Num[4][2], SB_Num[4][1] + SB_Num[4][3]);
		glRectf(SB_Num[6][4], SB_Num[6][1], SB_Num[6][4] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 7:
		glRectf(SB_Num[0][4], SB_Num[0][1], SB_Num[0][4] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[3][4], SB_Num[3][1], SB_Num[3][4] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[5][4], SB_Num[5][1], SB_Num[5][4] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][4], SB_Num[6][1], SB_Num[6][4] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 8:
		glRectf(SB_Num[0][4], SB_Num[0][1], SB_Num[0][4] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[1][4], SB_Num[1][1], SB_Num[1][4] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[2][4], SB_Num[2][1], SB_Num[2][4] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[3][4], SB_Num[3][1], SB_Num[3][4] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[4][4], SB_Num[4][1], SB_Num[4][4] + SB_Num[4][2], SB_Num[4][1] + SB_Num[4][3]);
		glRectf(SB_Num[5][4], SB_Num[5][1], SB_Num[5][4] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][4], SB_Num[6][1], SB_Num[6][4] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 9:
		glRectf(SB_Num[0][4], SB_Num[0][1], SB_Num[0][4] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[1][4], SB_Num[1][1], SB_Num[1][4] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[2][4], SB_Num[2][1], SB_Num[2][4] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[3][4], SB_Num[3][1], SB_Num[3][4] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[5][4], SB_Num[5][1], SB_Num[5][4] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][4], SB_Num[6][1], SB_Num[6][4] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	}	// 점수 출력 	// 점수 출력 
	 // 1의 자리 점수 출력
	switch (SC_1seat) {
	case 0:
		glRectf(SB_Num[0][0], SB_Num[0][1], SB_Num[0][0] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[2][0], SB_Num[2][1], SB_Num[2][0] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[3][0], SB_Num[3][1], SB_Num[3][0] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[4][0], SB_Num[4][1], SB_Num[4][0] + SB_Num[4][2], SB_Num[4][1] + SB_Num[4][3]);
		glRectf(SB_Num[5][0], SB_Num[5][1], SB_Num[5][0] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][0], SB_Num[6][1], SB_Num[6][0] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 1:
		glRectf(SB_Num[5][0], SB_Num[5][1], SB_Num[5][0] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][0], SB_Num[6][1], SB_Num[6][0] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 2:
		glRectf(SB_Num[0][0], SB_Num[0][1], SB_Num[0][0] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[1][0], SB_Num[1][1], SB_Num[1][0] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[2][0], SB_Num[2][1], SB_Num[2][0] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[4][0], SB_Num[4][1], SB_Num[4][0] + SB_Num[4][2], SB_Num[4][1] + SB_Num[4][3]);
		glRectf(SB_Num[5][0], SB_Num[5][1], SB_Num[5][0] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		break;
	case 3:
		glRectf(SB_Num[0][0], SB_Num[0][1], SB_Num[0][0] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[1][0], SB_Num[1][1], SB_Num[1][0] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[2][0], SB_Num[2][1], SB_Num[2][0] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[5][0], SB_Num[5][1], SB_Num[5][0] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][0], SB_Num[6][1], SB_Num[6][0] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 4:
		glRectf(SB_Num[1][0], SB_Num[1][1], SB_Num[1][0] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[3][0], SB_Num[3][1], SB_Num[3][0] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[5][0], SB_Num[5][1], SB_Num[5][0] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][0], SB_Num[6][1], SB_Num[6][0] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 5:
		glRectf(SB_Num[0][0], SB_Num[0][1], SB_Num[0][0] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[1][0], SB_Num[1][1], SB_Num[1][0] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[2][0], SB_Num[2][1], SB_Num[2][0] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[3][0], SB_Num[3][1], SB_Num[3][0] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[6][0], SB_Num[6][1], SB_Num[6][0] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 6:
		glRectf(SB_Num[0][0], SB_Num[0][1], SB_Num[0][0] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[1][0], SB_Num[1][1], SB_Num[1][0] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[2][0], SB_Num[2][1], SB_Num[2][0] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[3][0], SB_Num[3][1], SB_Num[3][0] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[4][0], SB_Num[4][1], SB_Num[4][0] + SB_Num[4][2], SB_Num[4][1] + SB_Num[4][3]);
		glRectf(SB_Num[6][0], SB_Num[6][1], SB_Num[6][0] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 7:
		glRectf(SB_Num[0][0], SB_Num[0][1], SB_Num[0][0] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[3][0], SB_Num[3][1], SB_Num[3][0] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[5][0], SB_Num[5][1], SB_Num[5][0] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][0], SB_Num[6][1], SB_Num[6][0] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 8:
		glRectf(SB_Num[0][0], SB_Num[0][1], SB_Num[0][0] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[1][0], SB_Num[1][1], SB_Num[1][0] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[2][0], SB_Num[2][1], SB_Num[2][0] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[3][0], SB_Num[3][1], SB_Num[3][0] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[4][0], SB_Num[4][1], SB_Num[4][0] + SB_Num[4][2], SB_Num[4][1] + SB_Num[4][3]);
		glRectf(SB_Num[5][0], SB_Num[5][1], SB_Num[5][0] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][0], SB_Num[6][1], SB_Num[6][0] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	case 9:
		glRectf(SB_Num[0][0], SB_Num[0][1], SB_Num[0][0] + SB_Num[0][2], SB_Num[0][1] + SB_Num[0][3]);
		glRectf(SB_Num[1][0], SB_Num[1][1], SB_Num[1][0] + SB_Num[1][2], SB_Num[1][1] + SB_Num[1][3]);
		glRectf(SB_Num[2][0], SB_Num[2][1], SB_Num[2][0] + SB_Num[2][2], SB_Num[2][1] + SB_Num[2][3]);
		glRectf(SB_Num[3][0], SB_Num[3][1], SB_Num[3][0] + SB_Num[3][2], SB_Num[3][1] + SB_Num[3][3]);
		glRectf(SB_Num[5][0], SB_Num[5][1], SB_Num[5][0] + SB_Num[5][2], SB_Num[5][1] + SB_Num[5][3]);
		glRectf(SB_Num[6][0], SB_Num[6][1], SB_Num[6][0] + SB_Num[6][2], SB_Num[6][1] + SB_Num[6][3]);
		break;
	}


	// 보드 출력 
	glColor3f(0.0f, 0.0f, 1.0f);
	glRectf(Board_PosX, Board_PosY - CameraUptotal, Board_PosX + Board_SizeX, Board_PosY + Board_SizeY - CameraUptotal);


	/*
	//ScoreBoard 1seat 출력
	glColor3f(1.0f, 0.0f, 0.0f);
	glRectf(SB1_PosX, SB_PosY, SB1_PosX + SB_SizeX, SB_PosY + SB_SizeY);

	//ScoreBoard 10seat 출력
	glColor3f(1.0f, 0.0f, 0.0f);
	glRectf(SB2_PosX, SB_PosY, SB2_PosX + SB_SizeX, SB_PosY + SB_SizeY);
	*/


	for (int i = 0; i < 100; i++)
	{
		//층 구별을 위해 여러가지 색으로 블록을 표현함.
		if (i % 3 == 0)
			glColor3f(1.0f, 1.0f, 1.0f);
		else if (i % 2 == 0)
			glColor3f(0.0f, 1.0f, 1.0f);
		else
			glColor3f(1.0f, 0.0f, 1.0f);

		// 블록 출력 부분.
		glRectf(MoveBlock[i][0], MoveBlock[i][1] - CameraUptotal, MoveBlock[i][0] + MoveBlock[i][2], MoveBlock[i][1] + MoveBlock[i][3] - CameraUptotal);
	}
}

void CPPUStackingGameView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// 주기마다 BlockSpeed 만큼 블럭이 이동함
	MoveBlock[SpaceCount][0] += BlockSpeed;

	// 블럭 좌우 이동 코드
	if (MoveBlock[SpaceCount][0] > BlockEndPos - MoveBlock[SpaceCount][2] || MoveBlock[SpaceCount][0] < 30)
		BlockSpeed = -BlockSpeed;

	//CameraUp이 true면 이동, false면 멈춤
	if (CameraUp)
	{
		CameraUptotal += CameraSpeed;
	}

	// 카메라가 일정 높이에 다다르면 카메라 이동 멈츰
	if (CameraUptotal == (CameraUpStd * addHeight) * (SpaceCount / CameraUpStd))
	{
		CameraUp = FALSE;
	}

	InvalidateRect(NULL, FALSE);
	CView::OnTimer(nIDEvent);
}


void CPPUStackingGameView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnKeyDown(nChar, nRepCnt, nFlags);

	switch (nChar) {

	case VK_SPACE:
		SpaceCount++;

		if (SpaceCount % CameraUpStd == 0)
		{
			CameraUp = TRUE;
		}

		// 처음 블록은 Board 를 기준으로 블록이 잘려야 하기 때문에 1인 경우와 아닌 경우로 나뉨.
		if (SpaceCount == 1)
		{
			if (MoveBlock[SpaceCount - 1][0] < Board_PosX)
			{
				// 왼쪽의 어긋난 블록을 자르는 코드로 이는 길이와 좌표 모두 바꾸어주어야 한다.
				MoveBlock[SpaceCount - 1][2] -= Board_PosX - MoveBlock[SpaceCount - 1][0];
				MoveBlock[SpaceCount - 1][0] = Board_PosX;
			}
			else if (MoveBlock[SpaceCount - 1][0] + MoveBlock[SpaceCount - 1][2] > Board_PosX + Board_SizeX)
			{
				// BlockPos + BlockSize = BoardPos + BoartSize
				// 위 식은 코드로 사용할 수 없으므로 왼쪽 피연산자 하나를 오른쪽으로 이동시킨다.
				// 이 경우엔 길이만 바꾸면 된다.
				MoveBlock[SpaceCount - 1][2] = Board_PosX + Board_SizeX - MoveBlock[SpaceCount - 1][0];
			}
		}
		else
		{
			// 위에 올리지 못했을 시 게임 오버 처리인 규칙인데 정확히 같을 시 끝나지 않는 오류가 있어서
			// '<' 부등호를 정확히 좌표가 같아도 게임오버 처리하게 '<='로 바꿈
			if (MoveBlock[SpaceCount - 1][0] + MoveBlock[SpaceCount - 1][2] <= MoveBlock[SpaceCount - 2][0] ||
				MoveBlock[SpaceCount - 2][0] + MoveBlock[SpaceCount - 2][2] <= MoveBlock[SpaceCount - 1][0])
			{
				CString str;
				str.Format(L"     Game over! \n  최고 층 수 : %d층", SpaceCount);
				AfxMessageBox(str);

				// 게임 데이터 초기화
				GameReset();
				break;
			}
			else if (MoveBlock[SpaceCount - 1][0] < MoveBlock[SpaceCount - 2][0])
			{
				// 왼쪽의 어긋난 블록을 자르는 코드.
				MoveBlock[SpaceCount - 1][2] -= MoveBlock[SpaceCount - 2][0] - MoveBlock[SpaceCount - 1][0];
				MoveBlock[SpaceCount - 1][0] = MoveBlock[SpaceCount - 2][0];
			}
			else if (MoveBlock[SpaceCount - 1][0] + MoveBlock[SpaceCount - 1][2] > MoveBlock[SpaceCount - 2][0] + MoveBlock[SpaceCount - 2][2])
			{
				// 오른쪽의 어긋난 블록을 자르는 코드.
				MoveBlock[SpaceCount - 1][2] = MoveBlock[SpaceCount - 2][0] + MoveBlock[SpaceCount - 2][2] - MoveBlock[SpaceCount - 1][0];
			}

			if (SpaceCount == 100)
			{
				// 게임 데이터 초기화

				CString str;
				str.Format(L"     Congratulation! \n   You built 100 floors! \n         You Win!");
				AfxMessageBox(str);

				GameReset();
				break;
			}
		}

		//스페이스가 눌릴 때 마다 다음 블록값을 우선 기본적인 값으로 초기화 한다.
		if (SpaceCount % 2 == 1)
		{
			MoveBlock[SpaceCount][0] = BlockEndPos - MoveBlock[SpaceCount - 1][2];
			MoveBlock[SpaceCount][1] = MoveBlock[0][1] + SpaceCount * addHeight;
			MoveBlock[SpaceCount][2] = MoveBlock[SpaceCount - 1][2];
			MoveBlock[SpaceCount][3] = MoveBlock[0][3];
		}
		else
		{
			MoveBlock[SpaceCount][0] = BlockStartPos;
			MoveBlock[SpaceCount][1] = MoveBlock[0][1] + SpaceCount * addHeight;
			MoveBlock[SpaceCount][2] = MoveBlock[SpaceCount - 1][2];
			MoveBlock[SpaceCount][3] = MoveBlock[0][3];
		}

		break;
	}
}

BOOL CPPUStackingGameView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//백그라운드에서 배경을 지우는 것을 방지하기 위해 return부분을 다음과 같이 수정한다.
	return TRUE;
}

void CPPUStackingGameView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	// RC를 Non-Current화 하는데, wglMakeCurrent에 0, 0 인자를 전달하여 호출한다. 
	if (::wglMakeCurrent(0, 0) == FALSE)
	{
		MessageBox(_T("Could not make RC non-current"));
	}

	// wglDeleteContext() 함수를 호출하여 RC를 지우는데, 
	// 이 함수는 인자로써 지울 RC의 핸들을 넘긴다.
	if (::wglDeleteContext(m_hRC) == FALSE)
	{
		MessageBox(_T("Could not delete RC"));
	}

	// DC를 지우고 NULL값을 할당한다. 
	if (m_pDC)
	{
		delete m_pDC;
	}

	m_pDC = NULL;

	KillTimer(0);
}


void CPPUStackingGameView::GameReset()
{
	// TODO: 여기에 구현 코드 추가.
	CameraUptotal = 0;
	SpaceCount = 0;
	CameraUp = FALSE;
	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 4; j++) {
			MoveBlock[i][j] = 0;
		}
	}

	MoveBlock[SpaceCount][0] = BlockStartPos;
	MoveBlock[SpaceCount][1] = Board_SizeY;
	MoveBlock[SpaceCount][2] = Board_SizeX;
	MoveBlock[SpaceCount][3] = addHeight;
}
