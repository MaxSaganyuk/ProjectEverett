// CPlaceObjectDialog.cpp : implementation file
//

#include "pch.h"
#include "EverettGUI.h"
#include "afxdialogex.h"
#include "CPlaceObjectDialog.h"


// CPlaceObjectDialog dialog

IMPLEMENT_DYNAMIC(CPlaceObjectDialog, CDialogEx)

CPlaceObjectDialog::CPlaceObjectDialog(
	const std::string& objectTypeName, 
	const std::string& sourceObjectTypeName,
	const std::vector<std::string>& objectNameList, 
	CWnd* pParent
)
	: 
	CDialogEx(IDD_DIALOG2, pParent), 
	objectTypeName(objectTypeName), 
	sourceObjectTypeName(sourceObjectTypeName), 
	objectNameList(objectNameList)
{
}

CPlaceObjectDialog::~CPlaceObjectDialog()
{
}

BOOL CPlaceObjectDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(CA2T(("Place " + objectTypeName).c_str()));
	choiceLabel.SetWindowTextW(CA2T(("Choose " + sourceObjectTypeName + ':').c_str()));
	nameLabel.SetWindowTextW(CA2T((objectTypeName + " name:").c_str()));

	for (auto& object : objectNameList)
	{
		objectChoice.AddString(CA2T(object.c_str()));
	}

	return true;
}

void CPlaceObjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, objectChoice);
	DDX_Control(pDX, IDC_EDIT1, nameEdit);
	DDX_Control(pDX, IDC_CHOICE_LABEL, choiceLabel);
	DDX_Control(pDX, IDC_NAME_LABEL, nameLabel);
}


BEGIN_MESSAGE_MAP(CPlaceObjectDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPlaceObjectDialog::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CPlaceObjectDialog::OnModelChoiceChange)
END_MESSAGE_MAP()


// CPlaceObjectDialog message handlers


void CPlaceObjectDialog::OnBnClickedOk()
{
	CString newNameStr;
	nameEdit.GetWindowTextW(newNameStr);

	newName = CT2A(newNameStr);

	CDialogEx::OnOK();
}

size_t CPlaceObjectDialog::GetChosenIndex()
{
	return chosenIndex;
}

std::string CPlaceObjectDialog::GetChosenObject()
{
	return chosenObject;
}

std::string CPlaceObjectDialog::GetNewObjectName()
{
	return newName;
}

void CPlaceObjectDialog::OnModelChoiceChange()
{
	chosenIndex = objectChoice.GetCurSel();

	CString modelStr;
	objectChoice.GetLBText(chosenIndex, modelStr);

	nameEdit.SetWindowTextW(modelStr);

	chosenObject = CT2A(modelStr);
}
