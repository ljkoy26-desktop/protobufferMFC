
#include "pch.h"
#include "framework.h"
#include "TestMFC.h"
#include "TestMFCDlg.h"
#include "afxdialogex.h"
#include "person.pb.h"
#include "filter_data.pb.h"
#include <fstream>






#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTestMFCDlg::CTestMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTMFC_DIALOG, pParent)
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
	ON_BN_CLICKED(IDC_BUTTON1, &CTestMFCDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

BOOL CTestMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

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

void CTestMFCDlg::OnBnClickedButton1()
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
    // 3단계: 바이너리 역직렬화 (Parse)
    // -------------------------------------------------------
    tutorial::Person oParsedPerson;
    if (!oParsedPerson.ParseFromString(strSerializedData))
    {
        AfxMessageBox(_T("역직렬화(ParseFromString) 실패"));
        return;
    }

    // -------------------------------------------------------
    // 4단계: 결과 출력
    // (%S : Unicode CString::Format 에서 char* 를 wide 로 변환)
    // -------------------------------------------------------
    CString strMsg;
    strMsg.Format(
        _T("== Proto Buffer 직렬화/역직렬화 성공 ==\n")
        _T("이름  : %S\n")
        _T("ID    : %d\n")
        _T("이메일: %S"),
        oParsedPerson.name().c_str(),
        oParsedPerson.id(),
        oParsedPerson.email().c_str()
    );
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
