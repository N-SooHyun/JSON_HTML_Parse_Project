#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<math.h>

/*
	JSON의 기본적인 구조는 키:값이 한 쌍으로 이루어진 형태로 {"key" : "value"} 이 형태를 객체라고한다.
	또한 []배열의 형태로도 감싸서 다양한 요소를 포함시킬수도 있다.
	1.기본적인 JSON의 객체 { "name" : "John", "age" : 25, "isStudent" : true} 
	2. JSON 배열 { "fruits" : ["apple", "banana", "orange"]} 
	3. 중첩된 객체 {"person":{"name":"Jane","address" : {"street" : "123 Main St", "city" : "New York", "zipcode" : "10001"} } }
	4. 배열안에 객체 {"student" [{"name" : "Alice", "age":22},{"name":"Bob","age":23}] }
	5. 배열 안의 중첩배열 {"numbers" : [[1,2,3],[4,5,6],[7,8,9]] }
	6. 객체와 배열이 섞인 구조 {"class" : "Math", "students" : [{"name":"Charlie","scores":[90,85,88]},{"name":"Diana", "scores":[92,81,89]} ] }
	7. 값이 NULL인 json {"name" : "Emily", "age" : null, "email", "emily@example.com"}
	8. 다양한 데이터 타입을 섞은 JSON {"product":"Laptop","price":799.99,"available":true,"specs":{"cpu":"Intel i7","ram":"16GB","storage":"512GB SSD"},"colors":["black","silver","white"]}
	9. 단일 JSON true, 123.45 , "Hello Json"
	10. 빈 객체, 배열 {} []

	이런 JSON을 파싱하는걸 만들어보자
	1. FILE을 통한 실제 JSON파일을 메모리에 올리기
	2. 스택프레임을 구현하여 객체와 배열을 담아서 진행
	2-1. "{", "[" 스택프레임생성 "}", "]" 스택프레임 파기
	2-2. 스택프레임 생성 후 키:값의 형태를 변수에 삽입
	2-3. 이 변수를 해쉬테이블의 형태로 구현
*/

void json_parse_main() {
	FILE* pFile = NULL;
	char arr[1024];
	fopen_s(&pFile, "json\\test.json", "r");
	int i;
	char c;
	for (i = 0; i < 1024; i++) {
		arr[i] = fgetc(pFile);
		if (arr[i] == EOF) break;
	}
	arr[i] = '\0';
	printf("%s\n", arr);


	
}