
#include "pch.h"
#include "framework.h"
#include "TestMFC.h"
#include "TestMFCDlg.h"
#include "afxdialogex.h"
#include "person.pb.h"
#include "filter_data.pb.h"
#include "ReceiverDlg.h"
#include "CopyDataDefs.h"
#include <fstream>






#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTestMFCDlg::CTestMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTMFC_DIALOG, pParent)
	, m_pReceiverDlg(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_COPYDATA()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CTestMFCDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()

BOOL CTestMFCDlg::OnInitDialog()
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

void CTestMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
	
}

void CTestMFCDlg::OnPaint()
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

HCURSOR CTestMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// -------------------------------------------------------
// WM_COPYDATA 수신 처리
// -------------------------------------------------------
BOOL CTestMFCDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
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
// WM_DESTROY — ReceiverDlg 수명 관리
// -------------------------------------------------------
void CTestMFCDlg::OnDestroy()
{
	if (m_pReceiverDlg != nullptr)
	{
		m_pReceiverDlg->DestroyWindow();
		delete m_pReceiverDlg;
		m_pReceiverDlg = nullptr;
	}
	CDialogEx::OnDestroy();
}


void CTestMFCDlg::OnBnClickedButtonSend()
{
    // -------------------------------------------------------
    // 1단계: Person 메시지 객체 생성 및 필드 설정
    // -------------------------------------------------------
    tutorial::Person oPerson;
    oPerson.set_name("TestUser");
    oPerson.set_id(1001);
    oPerson.set_email("test@example.com");

    // -------------------------------------------------------
    // 2단계: 바이너리 직렬화 (Serialize)
    // -------------------------------------------------------
    std::string strSerializedData;
    if (!oPerson.SerializeToString(&strSerializedData))
    {
        AfxMessageBox(_T("직렬화(SerializeToString) 실패"));
        return;
    }

    // -------------------------------------------------------
    // 3단계: 수신 윈도우 검색
    // FindWindow(클래스명, 윈도우타이틀) — 수신 프로그램에 맞게 변경하세요.
    // -------------------------------------------------------
    HWND hTargetWnd = ::FindWindow(NULL, _T("ReceiverApp"));
    if (hTargetWnd == NULL)
    {
        AfxMessageBox(_T("수신 프로그램 윈도우를 찾을 수 없습니다.\n윈도우 타이틀을 확인하세요."));
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
    // wParam : 송신 윈도우 핸들, lParam : COPYDATASTRUCT 포인터
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


// 1. 데이터 파일로 안전하게 저장하기 (Serialize)
//void CTestMFCDlg::SaveProtobufData()
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
//void CTestMFCDlg::LoadProtobufData()
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
