#pragma once

// WM_COPYDATA 전송 시 데이터 타입 식별자
// 송수신 양쪽(SendDlg / ReceiverDlg)에서 동일한 값을 사용해야 합니다.


// 여기에 기록되는 코드값이 아마 Charka v5 프로토콜 코드 값들이 될것 같습니다. 



static const DWORD COPYDATA_PERSON_PROTO = 9001;
static const DWORD COPYDATA_FILTER_DATA = 9002;
