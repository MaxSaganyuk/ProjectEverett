
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "EverettGUI.h"

#include "TreeManager.h"

#include "CBrowseAndLoadDialog.h"
#include "CPlaceObjectDialog.h"
#include "CLoadingDialog.h"
#include "CObjectEditDialog.h"
#include "CKeybindOptionDlg.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_APP_ABOUT,   &CMainFrame::OnLoadModel)
	ON_COMMAND(ID_BUTTON32771, &CMainFrame::OnPlaceSolid)
	ON_COMMAND(ID_BUTTON32772, &CMainFrame::OnPlaceLight)
	ON_COMMAND(ID_BUTTON32773, &CMainFrame::OnPlaceSound)
	ON_COMMAND(ID_BUTTON32774, &CMainFrame::OnCameraOptions)
	ON_COMMAND(ID_BUTTON32775, &CMainFrame::OnKeybindOptions)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	engine.CreateAndSetupMainWindow(800, 600, "Everett");
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, nullptr))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	CCreateContext context;
	context.m_pNewViewClass = RUNTIME_CLASS(CMainWindow);
	context.m_pCurrentDoc = nullptr;

	mainWindow.reset(reinterpret_cast<CMainWindow*>(CreateView(&context)));

	mainWindow->SetEverettEngineRef(engine);

	mainWindow->ShowWindow(SW_SHOW);
	SetActiveView(mainWindow.get());
	BringWindowToTop();
	engine.AddCurrentWindowHandler("EverettGUI");
	RecalcLayout();

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnLoadModel()
{
	CBrowseAndLoadDialog loadModelDlg("Model", [this](const std::string& path) { return engine.GetModelInDirList(path); }, engine.GetCreatedModels());
	
	if (loadModelDlg.DoModal() == IDOK)
	{
		CLoadingDialog loadingDlg(
			{ 
				[this, &loadModelDlg]() 
				{ 
					return engine.CreateModel(loadModelDlg.GetChosenPathAndFilename(), loadModelDlg.GetChosenName()); 
				} 
			}
		);

		if (loadingDlg.DoModal() == IDOK && loadingDlg.IsSuccess())
		{
			mainWindow->GetObjectTree().AddModelToTree(loadModelDlg.GetChosenName());
		}
	
	}
}

void CMainFrame::OnPlaceSolid()
{
	CPlaceObjectDialog placeSolidDlg("Solid", engine.GetCreatedModels());

	if (placeSolidDlg.DoModal() == IDOK)
	{
		engine.CreateSolid(placeSolidDlg.GetChosenObject(), placeSolidDlg.GetNewObjectName());
		mainWindow->GetObjectTree().AddSolidToTree(placeSolidDlg.GetChosenObject(), placeSolidDlg.GetNewObjectName());
	}
}

void CMainFrame::OnPlaceLight()
{
	CPlaceObjectDialog placeLightDlg("Light", engine.GetLightTypeList());

	if (placeLightDlg.DoModal() == IDOK)
	{
		engine.CreateLight(
			placeLightDlg.GetNewObjectName(), 
			static_cast<EverettEngine::LightTypes>(placeLightDlg.GetChosenIndex())
		);
		mainWindow->GetObjectTree().AddLightToTree(placeLightDlg.GetChosenObject(), placeLightDlg.GetNewObjectName());
	}
}

void CMainFrame::OnPlaceSound()
{
	CBrowseAndLoadDialog placeSoundDlg(
		"Sound", 
		[this](const std::string& path) { return engine.GetSoundInDirList(path); }, 
		engine.GetNamesByObject(EverettEngine::ObjectTypes::Sound)
	);

	if (placeSoundDlg.DoModal() == IDOK)
	{
		engine.CreateSound(placeSoundDlg.GetChosenPathAndFilename(), placeSoundDlg.GetChosenName());
		mainWindow->GetObjectTree().AddSoundToTree(placeSoundDlg.GetChosenName());
	}
}

void CMainFrame::OnCameraOptions()
{
	CObjectEditDialog objectEditDlg(engine, EverettEngine::ObjectTypes::Camera, mainWindow->GetSelectedScriptDllInfo());

	objectEditDlg.DoModal();
}

void CMainFrame::OnKeybindOptions()
{
	CKeybindOptionDlg keybindDlg(engine, mainWindow->GetSelectedScriptDllInfo());

	keybindDlg.DoModal();
}