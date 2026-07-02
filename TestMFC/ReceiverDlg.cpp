#include "pch.h"
#include "framework.h"
#include "TestMFC.h"
#include "ReceiverDlg.h"
#include "afxdialogex.h"
#include "CopyDataDefs.h"

#include "protobuf/filter_data.pb.h"
#include "protobuf/person.pb.h"

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
    if (pCopyDataStruct->dwData == COPYDATA_PERSON_PROTO)
    {
        tutorial::Person oPerson;
        if (!oPerson.ParseFromArray(pCopyDataStruct->lpData, pCopyDataStruct->cbData))
        {
            SetDlgItemText(IDC_EDIT_RECV, _T("[오류] Person 역직렬화 실패"));
            return FALSE;
        }

        CString strDisplay;
        strDisplay.Format(
            _T("== Person 수신 완료 ==\r\n")
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
        return TRUE;
    }
    else if (pCopyDataStruct->dwData == COPYDATA_FILTER_DATA)
    {
        my_msg::FilterConfig oFilterConfig;
        if (!oFilterConfig.ParseFromArray(pCopyDataStruct->lpData, pCopyDataStruct->cbData))
        {
            SetDlgItemText(IDC_EDIT_RECV, _T("[오류] FilterConfig 역직렬화 실패"));
            return FALSE;
        }

        CString strDisplay;
        strDisplay.Format(_T("== FilterConfig 수신 완료 ==\r\n크기 : %d bytes\r\n"), pCopyDataStruct->cbData);

        for (const auto& oPair : oFilterConfig.filter_map())
        {
            CString strKey(oPair.first.c_str());
            strDisplay.AppendFormat(_T("[%s]\r\n"), (LPCTSTR)strKey);

            const my_msg::FilterItem& oItem = oPair.second;

            strDisplay += _T("  Include: ");
            for (int i = 0; i < oItem.include_filters_size(); ++i)
            {
                if (i > 0)
                {
                    strDisplay += _T(", ");
                }
                strDisplay += CString(oItem.include_filters(i).c_str());
            }
            strDisplay += _T("\r\n");

            strDisplay += _T("  Exclude: ");
            for (int i = 0; i < oItem.exclude_filters_size(); ++i)
            {
                if (i > 0)
                {
                    strDisplay += _T(", ");
                }
                strDisplay += CString(oItem.exclude_filters(i).c_str());
            }
            strDisplay += _T("\r\n");
        }

        SetDlgItemText(IDC_EDIT_RECV, strDisplay);
        return TRUE;
    }

    return FALSE;
}


// -------------------------------------------------------
// IDC_BUTTON_SEND : Person + FilterConfig 데이터를 SendApp 으로 전송
// -------------------------------------------------------
void CReceiverDlg::OnBnClickedButtonSend()
{
    // -------------------------------------------------------
    // 1단계: 수신 윈도우 검색 (SendApp)
    // SendDlg::OnInitDialog 에서 SetWindowText("SendApp") 으로 설정합니다.
    // -------------------------------------------------------
    HWND hTargetWnd = ::FindWindow(NULL, _T("SendApp"));
    if (hTargetWnd == NULL)
    {
        AfxMessageBox(_T("수신 프로그램 윈도우(SendApp)를 찾을 수 없습니다."));
        return;
    }

    // -------------------------------------------------------
    // 2단계: Person 메시지 생성 → 직렬화 → 전송
    // -------------------------------------------------------
    tutorial::Person oPerson;
    oPerson.set_name("ReceiverUser");
    oPerson.set_id(2002);
    oPerson.set_email("receiver@example.com");

    std::string strPersonData;
    if (!oPerson.SerializeToString(&strPersonData))
    {
        AfxMessageBox(_T("Person 직렬화(SerializeToString) 실패"));
        return;
    }

    COPYDATASTRUCT oCdsPerson;
    oCdsPerson.dwData = COPYDATA_PERSON_PROTO;
    oCdsPerson.cbData = static_cast<DWORD>(strPersonData.size());
    oCdsPerson.lpData = const_cast<char*>(strPersonData.data());

    LRESULT lResultPerson = ::SendMessage(
        hTargetWnd,
        WM_COPYDATA,
        reinterpret_cast<WPARAM>(GetSafeHwnd()),
        reinterpret_cast<LPARAM>(&oCdsPerson)
    );

    // -------------------------------------------------------
    // 3단계: FilterConfig 메시지 생성 → 직렬화 → 전송
    // -------------------------------------------------------
    my_msg::FilterConfig oFilterConfig;
    auto& oFilterItem = (*oFilterConfig.mutable_filter_map())["FilterList2"];
    oFilterItem.add_include_filters("SERVER");
    oFilterItem.add_include_filters("SERVER200");
    oFilterItem.add_exclude_filters("LINUX");

    std::string strFilterData;
    if (!oFilterConfig.SerializeToString(&strFilterData))
    {
        AfxMessageBox(_T("FilterConfig 직렬화(SerializeToString) 실패"));
        return;
    }

    COPYDATASTRUCT oCdsFilter;
    oCdsFilter.dwData = COPYDATA_FILTER_DATA;
    oCdsFilter.cbData = static_cast<DWORD>(strFilterData.size());
    oCdsFilter.lpData = const_cast<char*>(strFilterData.data());

    LRESULT lResultFilter = ::SendMessage(
        hTargetWnd,
        WM_COPYDATA,
        reinterpret_cast<WPARAM>(GetSafeHwnd()),
        reinterpret_cast<LPARAM>(&oCdsFilter)
    );

    // -------------------------------------------------------
    // 4단계: 결과 출력
    // -------------------------------------------------------
    CString strMsg;
    strMsg.Format(
        _T("[ Person 전송 : %s ]\r\n")
        _T("이름  : %S\r\n")
        _T("ID    : %d\r\n")
        _T("이메일: %S\r\n")
        _T("크기  : %d bytes\r\n\r\n")
        _T("[ FilterConfig 전송 : %s ]\r\n")
        _T("키 수 : %d\r\n")
        _T("크기  : %d bytes"),
        lResultPerson ? _T("성공") : _T("실패"),
        oPerson.name().c_str(),
        oPerson.id(),
        oPerson.email().c_str(),
        static_cast<DWORD>(strPersonData.size()),
        lResultFilter ? _T("성공") : _T("실패"),
        oFilterConfig.filter_map_size(),
        static_cast<DWORD>(strFilterData.size())
    );
    AfxMessageBox(strMsg);
}
