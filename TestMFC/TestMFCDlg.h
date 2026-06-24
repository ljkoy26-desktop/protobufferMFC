#pragma once

class CReceiverDlg;  // 전방 선언

class CTestMFCDlg : public CDialogEx
{
public:
	CTestMFCDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTMFC_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON			m_hIcon;
	CReceiverDlg*	m_pReceiverDlg;		// 모달리스 수신 대화상자 포인터
	CEdit			m_editRecv;			// IDC_EDIT_RECV 멀티라인 재생성 소유

	virtual BOOL    OnInitDialog();
	afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void    OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL    OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void    OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSend();

/*	void SaveProtobufData();
	void LoadProtobufData();*/	
};
