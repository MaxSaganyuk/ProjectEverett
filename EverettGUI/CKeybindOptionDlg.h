#pragma once
#include "afxdialogex.h"

#include "EverettEngine.h"

#include <memory>

#include "DLLLoaderCommon.h"

// CKeybindOptionDlg dialog

class CKeybindOptionDlg : public DLLLoaderCommon
{
	DECLARE_DYNAMIC(CKeybindOptionDlg)

public:
	CKeybindOptionDlg(
		EverettEngine& engine, 
		std::vector<std::pair<std::string, std::string>>& selectedScriptDllInfo, 
		CWnd* pParent = nullptr
	);
	virtual ~CKeybindOptionDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG5 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   

	DECLARE_MESSAGE_MAP()
private:
	BOOL OnInitDialog() override;

	void SetEditToKeyName();
	void ResetPollForKeyPressThread();

	LRESULT OnBringEverettGuiBack(WPARAM wParam, LPARAM lParam);

	EverettEngine& engineRef;
	CEdit keyNameEdit;
	CButton keybindInterButton;
	std::unique_ptr<std::thread> pollForKeyPressThread;

	std::string keyName;

	afx_msg void OnKeybindInterClick();

	constexpr static int BringEverettGuiBack = WM_USER + 4;
};
