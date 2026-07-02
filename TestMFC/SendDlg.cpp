
#include "pch.h"
#include "framework.h"
#include "TestMFC.h"
#include "SendDlg.h"
#include "afxdialogex.h"
#include "ReceiverDlg.h"
#include "CopyDataDefs.h"
#include <fstream>


#include "protobuf/person.pb.h"
#include "protobuf/filter_data.pb.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSendDlg::CSendDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTMFC_DIALOG, pParent)
	, m_pReceiverDlg(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSendDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_COPYDATA()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CSendDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()

BOOL CSendDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// 윈도우 타이틀 설정 — ReceiverDlg 에서 FindWindow(NULL, "SendApp") 으로 찾습니다.
	SetWindowText(_T("SendApp"));

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

	// ReceiverDlg 모달리스 대화상자 생성 (같은 리소스 IDD_TESTMFC_DIALOG 재사용)
	m_pReceiverDlg = new CReceiverDlg(nullptr);
	m_pReceiverDlg->Create(IDD_TESTMFC_DIALOG, nullptr);

	// 현재 대화상자 오른쪽에 나란히 배치
	CRect rcThis;
	GetWindowRect(&rcThis);
	m_pReceiverDlg->SetWindowPos(
		nullptr,
		rcThis.right + 10, rcThis.top,
		0, 0,
		SWP_NOSIZE | SWP_NOZORDER
	);
	m_pReceiverDlg->ShowWindow(SW_SHOW);

	return TRUE;
}

void CSendDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
	
}

void CSendDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CSendDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// -------------------------------------------------------
// WM_COPYDATA 수신 처리
// -------------------------------------------------------
BOOL CSendDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
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
// WM_DESTROY — ReceiverDlg 수명 관리
// -------------------------------------------------------
void CSendDlg::OnDestroy()
{
	if (m_pReceiverDlg != nullptr)
	{
		m_pReceiverDlg->DestroyWindow();
		delete m_pReceiverDlg;
		m_pReceiverDlg = nullptr;
	}
	CDialogEx::OnDestroy();
}


void CSendDlg::OnBnClickedButtonSend()
{
    // -------------------------------------------------------
    // 1단계: 수신 윈도우 검색 (ReceiverApp)
    // -------------------------------------------------------
    HWND hTargetWnd = ::FindWindow(NULL, _T("ReceiverApp"));
    if (hTargetWnd == NULL)
    {
        AfxMessageBox(_T("수신 프로그램 윈도우를 찾을 수 없습니다.\n윈도우 타이틀을 확인하세요."));
        return;
    }

    // -------------------------------------------------------
    // 2단계: Person 메시지 생성 → 직렬화 → 전송
    // -------------------------------------------------------
    tutorial::Person oPerson;
    oPerson.set_name("TestUser");
    oPerson.set_id(1001);
    oPerson.set_email("test@example.com");
    oPerson.set_email2("test@example.com");

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
    auto& oFilterItem = (*oFilterConfig.mutable_filter_map())["FilterList1"];
    oFilterItem.add_include_filters("EMP");
    oFilterItem.add_include_filters("EMP100");
    oFilterItem.add_exclude_filters("MS");

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


// 1. 데이터 파일로 안전하게 저장하기 (Serialize)
//void CSendDlg::SaveProtobufData()
//{
//    my_msg::FilterConfig oConfig;
//
//    // "FilterList1" 이라는 키에 데이터 세팅
//    auto& oItem = (*oConfig.mutable_filter_map())["FilterList1"];
//    oItem.add_include_filters("EMP");
//    oItem.add_include_filters("EMP100");
//    oItem.add_exclude_filters("MS");
//
//    // 바이너리 파일로 출력
//    std::ofstream oOutput("C:\\Data\\filters.bin", std::ios::out | std::ios::binary);
//    if (oConfig.SerializeToOstream(&oOutput))
//    {
//        TRACE(_T("Protobuf 저장 성공!\n"));
//    }
//}
//
//// 2. 파일에서 데이터 읽어와 복구하기 (Parse)
//void CSendDlg::LoadProtobufData()
//{
//    my_msg::FilterConfig oConfig;
//    std::ifstream oInput("C:\\Data\\filters.bin", std::ios::in | std::ios::binary);
//
//    if (oConfig.ParseFromIstream(&oInput))
//    {
//        auto& oFilterMap = oConfig.filter_map();
//
//        // 데이터가 있는지 확인하고 꺼내 쓰기
//        if (oFilterMap.contains("FilterList1"))
//        {
//            const auto& oItem = oFilterMap.at("FilterList1");
//
//            // 꺼내서 CString 변환 후 리스트박스 등에 추가
//            for (int i = 0; i < oItem.include_filters_size(); ++i)
//            {
//                CString strInc(oItem.include_filters(i).c_str());
//                // m_listBox.AddString(strInc);
//            }
//        }
//    }
//}
//
