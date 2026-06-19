# 프로토 버퍼 (Protocol Buffer) 준비 가이드

## 환경 정보

- Visual Studio 2019 (v142)
- C++ 11, x64 플랫폼
- vcpkg 패키지: `protobuf:x64-windows` (libprotoc 33.4)

---

## vcpkg 설치 경로 (참조용)

```
VCPKG_ROOT = C:\Users\USER\Desktop\Github_clone\vcpkg download\vcpkg
VCPKG_X64  = $(VCPKG_ROOT)\installed\x64-windows
```

---

## STEP 1 : .proto 파일 작성

`TestMFC\` 폴더에 `person.proto` 파일을 생성합니다.
*(프로젝트에 이미 작성된 파일이 있으면 생략)*

```proto
syntax = "proto3";

package tutorial;

message Person {
    string name  = 1;
    int32  id    = 2;
    string email = 3;
}
```

---

## STEP 2 : C++ 소스 파일 생성 (protoc.exe 실행)

명령 프롬프트를 열고 `TestMFC\` 폴더로 이동 후 아래 명령 실행:

```bat
cd "C:\Users\USER\Desktop\Github_clone\protobufferMFC\TestMFC"

"C:\Users\USER\Desktop\Github_clone\vcpkg download\vcpkg\installed\x64-windows\tools\protobuf\protoc.exe" --cpp_out=. person.proto
```

실행 결과로 두 파일이 생성됩니다:

| 파일 | 용도 |
|------|------|
| `person.pb.h` | C++ 헤더 (메시지 클래스 선언) |
| `person.pb.cc` | C++ 구현 소스 (프로젝트에 추가 필요) |

---

## STEP 3 : Visual Studio 프로젝트 설정

### 3-1. 추가 포함 디렉터리

**프로젝트 속성 → C/C++ → 일반 → 추가 포함 디렉터리**
*(Debug x64 / Release x64 양쪽 모두 적용)*

```
C:\Users\USER\Desktop\Github_clone\vcpkg download\vcpkg\installed\x64-windows\include
```

### 3-2. 추가 라이브러리 디렉터리

**프로젝트 속성 → 링커 → 일반 → 추가 라이브러리 디렉터리**

| 구성 | 경로 |
|------|------|
| Debug \| x64 | `C:\Users\USER\Desktop\Github_clone\vcpkg download\vcpkg\installed\x64-windows\debug\lib` |
| Release \| x64 | `C:\Users\USER\Desktop\Github_clone\vcpkg download\vcpkg\installed\x64-windows\lib` |

### 3-3. 추가 종속성 (.lib 링크)

**프로젝트 속성 → 링커 → 입력 → 추가 종속성**

| 구성 | 추가할 .lib |
|------|------------|
| Debug \| x64 | `libprotobufd.lib` |
| Release \| x64 | `libprotobuf.lib` |

---

## STEP 4 : person.pb.cc 프로젝트에 추가

1. 솔루션 탐색기에서 `TestMFC` 프로젝트 **우클릭 → 추가 → 기존 항목**
2. `TestMFC\person.pb.cc` 선택 후 추가

### ⚠️ 중요: 미리 컴파일된 헤더 설정 변경

`person.pb.cc` 는 `pch.h` 를 포함하지 않으므로 아래 설정이 필수입니다.

1. 솔루션 탐색기에서 `person.pb.cc` **우클릭 → 속성**
2. **C/C++ → 미리 컴파일된 헤더 → 미리 컴파일된 헤더**
3. 값을 `미리 컴파일된 헤더 사용 안 함` 으로 변경
4. 모든 구성(Debug/Release)에 동일하게 적용

---

## STEP 5 : 런타임 DLL 복사

실행 파일(`bin\Debug x64\` 또는 `bin\Release x64\`) 폴더에 아래 DLL을 복사합니다.

| 구성 | 복사할 DLL 원본 경로 |
|------|---------------------|
| Debug \| x64 | `...\x64-windows\debug\bin\abseil_dll.dll` |
| Debug \| x64 | `...\x64-windows\debug\bin\libprotobufd.dll` |
| Release \| x64 | `...\x64-windows\bin\abseil_dll.dll` |
| Release \| x64 | `...\x64-windows\bin\libprotobuf.dll` |

> `...` = `C:\Users\USER\Desktop\Github_clone\vcpkg download\vcpkg\installed`

---

## STEP 6 : 테스트 코드 확인

`TestMFCDlg.cpp` 에 아래 include 가 추가되어 있는지 확인:

```cpp
#include "person.pb.h"
```

버튼1 클릭 시 동작 순서:
1. `tutorial::Person` 객체 생성 및 필드 설정
2. `SerializeToString()` 으로 바이너리 직렬화
3. `ParseFromString()` 으로 역직렬화
4. 결과를 `AfxMessageBox` 로 표시

---

## 준비 완료 후 파일 구조

```
TestMFC\
├── person.proto        ← 직접 작성 (메시지 정의)
├── person.pb.h         ← protoc 자동 생성
├── person.pb.cc        ← protoc 자동 생성 (프로젝트 추가 + PCH 제외 필수)
├── TestMFCDlg.h        ← 변경 없음
└── TestMFCDlg.cpp      ← include 추가 + 테스트 코드 구현
```
