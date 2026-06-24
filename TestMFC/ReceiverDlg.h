#pragma once


// OnBnClickedButtonSend 와 OnCopyData 를 갖는 모달리스 수신 대화상자.
// IDD_TESTMFC_DIALOG 리소스를 재사용하므로 별도 .rc 추가 불필요.
class CReceiverDlg : public CDialogEx
{
public:
    CReceiverDlg(CWnd* pParent = nullptr);

    enum { IDD = IDD_TESTMFC_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    // 모달리스 대화상자 — OK/Cancel 로 닫히지 않도록 숨김 처리
    virtual void OnOK();
    virtual void OnCancel();

    afx_msg BOOL    OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
    afx_msg void    OnBnClickedButtonSend();
    afx_msg void    OnDestroy();

    DECLARE_MESSAGE_MAP()

private:
    // IDC_EDIT_RECV 를 멀티라인으로 재생성 후 소유 (메모리 관리용)
    CEdit   m_editRecv;
};
