#include "pch.h"
#include "framework.h"
#include "TestMFC.h"
#include "ReceiverDlg.h"
#include "afxdialogex.h"
#include "person.pb.h"
#include "CopyDataDefs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CReceiverDlg, CDialogEx)
    ON_WM_COPYDATA()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_SEND, &CReceiverDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()


CReceiverDlg::CReceiverDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_TESTMFC_DIALOG, pParent)
{
}

void CReceiverDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BOOL CReceiverDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 윈도우 타이틀 설정 — SendApp 에서 FindWindow(NULL, "ReceiverApp") 으로 찾습니다.
    SetWindowText(_T("ReceiverApp"));

    // ES_MULTILINE 은 창 생성 시에만 적용 가능 → 기존 컨트롤 파괴 후 재생성
    CEdit* pOldEdit = static_cast<CEdit*>(GetDlgItem(IDC_EDIT_RECV));
    if (pOldEdit != nullptr)
    {
        CRect rcEdit;
        pOldEdit->GetWindowRect(&rcEdit);
        ScreenToClient(&rcEdit);
        pOldEdit->DestroyWindow();

        m_editRecv.CreateEx(
            WS_EX_CLIENTEDGE, _T("EDIT"), nullptr,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL
            | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | ES_LEFT,
            rcEdit,
            this,
            IDC_EDIT_RECV
        );
    }

    return TRUE;
}

// 모달리스 대화상자이므로 OK/Cancel 로 닫히지 않고 숨깁니다.
void CReceiverDlg::OnOK()
{
    ShowWindow(SW_HIDE);
}

void CReceiverDlg::OnCancel()
{
    ShowWindow(SW_HIDE);
}

void CReceiverDlg::OnDestroy()
{
    CDialogEx::OnDestroy();
}


// -------------------------------------------------------
// WM_COPYDATA 수신 처리
// -------------------------------------------------------
BOOL CReceiverDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
    // 데이터 타입 식별자 검사
    if (pCopyDataStruct->dwData != COPYDATA_PERSON_PROTO)
    {
        return FALSE;
    }

    // protobuf 역직렬화
    tutorial::Person oPerson;
    if (!oPerson.ParseFromArray(pCopyDataStruct->lpData, pCopyDataStruct->cbData))
    {
        SetDlgItemText(IDC_EDIT_RECV, _T("[오류] protobuf 역직렬화 실패"));
        return FALSE;
    }

    // IDC_EDIT_RECV 에 수신 내용 표시
    CString strDisplay;
    strDisplay.Format(
        _T("== 수신 완료 ==\r\n")
        _T("이름  : %S\r\n")
        _T("ID    : %d\r\n")
        _T("이메일: %S\r\n")
        _T("크기  : %d bytes"),
        oPerson.name().c_str(),
        oPerson.id(),
        oPerson.email().c_str(),
        pCopyDataStruct->cbData
    );
    SetDlgItemText(IDC_EDIT_RECV, strDisplay);

    return TRUE;  // 처리 완료 → 송신 측 SendMessage 반환값 = 1
}


// -------------------------------------------------------
// IDC_BUTTON_SEND : Person 데이터를 SendApp 으로 전송
// -------------------------------------------------------
void CReceiverDlg::OnBnClickedButtonSend()
{
    // -------------------------------------------------------
    // 1단계: Person 메시지 객체 생성 및 필드 설정
    // -------------------------------------------------------
    tutorial::Person oPerson;
    oPerson.set_name("ReceiverUser");
    oPerson.set_id(2002);
    oPerson.set_email("receiver@example.com");

    // -------------------------------------------------------
    // 2단계: 바이너리 직렬화
    // -------------------------------------------------------
    std::string strSerializedData;
    if (!oPerson.SerializeToString(&strSerializedData))
    {
        AfxMessageBox(_T("직렬화(SerializeToString) 실패"));
        return;
    }

    // -------------------------------------------------------
    // 3단계: 수신 윈도우 검색 (SendApp)
    // TestMFCDlg::OnInitDialog 에서 SetWindowText("SendApp") 으로 설정합니다.
    // -------------------------------------------------------
    HWND hTargetWnd = ::FindWindow(NULL, _T("SendApp"));
    if (hTargetWnd == NULL)
    {
        AfxMessageBox(_T("수신 프로그램 윈도우(SendApp)를 찾을 수 없습니다."));
        return;
    }

    // -------------------------------------------------------
    // 4단계: COPYDATASTRUCT 구성
    // -------------------------------------------------------
    COPYDATASTRUCT oCds;
    oCds.dwData = COPYDATA_PERSON_PROTO;                              // 데이터 타입 식별자
    oCds.cbData = static_cast<DWORD>(strSerializedData.size());      // 데이터 크기 (바이트)
    oCds.lpData = const_cast<char*>(strSerializedData.data());       // 직렬화된 바이너리 포인터

    // -------------------------------------------------------
    // 5단계: WM_COPYDATA 전송
    // -------------------------------------------------------
    LRESULT lResult = ::SendMessage(
        hTargetWnd,
        WM_COPYDATA,
        reinterpret_cast<WPARAM>(GetSafeHwnd()),
        reinterpret_cast<LPARAM>(&oCds)
    );

    // -------------------------------------------------------
    // 6단계: 결과 출력
    // -------------------------------------------------------
    CString strMsg;
    if (lResult)
    {
        strMsg.Format(
            _T("WM_COPYDATA 전송 성공\n")
            _T("이름  : %S\n")
            _T("ID    : %d\n")
            _T("이메일: %S\n")
            _T("크기  : %d bytes"),
            oPerson.name().c_str(),
            oPerson.id(),
            oPerson.email().c_str(),
            oCds.cbData
        );
    }
    else
    {
        strMsg = _T("WM_COPYDATA 전송 실패\n수신 프로그램이 메시지를 처리하지 않았습니다.");
    }
    AfxMessageBox(strMsg);
}
