
# 목표


아래 코드를 참조해서 filter_data.proto 부분도 Send / Recv 로직을 구현해줄래?

OnCopyData 부분을 참조하면 좋을꺼야

static const DWORD COPYDATA_FILTER_DATA = 9002;




# 수정 대상

# 요구사항


1. 코드 스타일: 
   - Visual Studio 2019 환경 , C++ 11 표준 사용중
   - Allman 스타일(중괄호 다음 줄), 헝가리언 표기법 준수.
   - 한 줄 제어문도 줄바꿈/중괄호 필수.
   - 인코딩: UTF-8 with BOM.   
2. 기존 로직 보존: 기존 프로젝트의 다른 코드는 건드리지 마세요(재탐색 금지).
3. 응답은 항상 한글로 해주세요.
4. commit 은 하지 않고 commit 로그를 제공해주세요. 사용자가 테스트 후 정상 동작이 확인되면
   commit 해달라고 요청할겁니다.
5. 아래 포맷의 형식은 분석 제외하세요 
 ( .obj , .tlog ,  .idb , .pdb , .exe.recipe ,  )

# 출력 형식

1. 수정된 코드 블록