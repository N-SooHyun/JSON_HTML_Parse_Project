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
void json_parse_file() {
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

typedef enum { STRING, OBJECT, LIST } ValueType;

typedef struct Value {	//메인
	ValueType type;	//0:STR, 1:OBJ, 2:LIST
	union {	//한 개만 사용되기에 union으로 메모리 낭비X
		char* stringValue;			//type:0 단순 문자열 value
		struct KeyValue* object;	//객체(리스트일경우만)
		struct ListValue* list;		//리스트
	};
}Value;

typedef struct KeyValue {		
	char* key;					//키(문자열)
	struct Value* value;		//값(str, obj, list) 3개의 경우의 수 존재
	struct KeyValue* next;		//현재 객체의 다음 키
}KeyValue;

typedef struct ListValue {
	struct Value* value;	//
	struct ListValue* next;
}ListValue;

void json_parse_add_key() {
	//객체에 새로운 키-쌍 생성
}

typedef struct Stack_Frame {
	int capacity;
	char* Data;
	int top;
	struct Stack_Frame* next_Stack_Frame;	//스택에 스택
}Stack_Frame;

void json_parse_init_stack(Stack_Frame* stack_frame, int capacity) {
	stack_frame->capacity = capacity;
	stack_frame->top = -1;
	stack_frame->Data = (char*)malloc(sizeof(char) * capacity);
	stack_frame->next_Stack_Frame = NULL;
}

void json_parse_main() {
	char test_arr_json[1024] = { "{ \"name\":\"홍길동\", \"age\":23}" };
	//최대 만들 수 있는 스택프레임 100개, 스택 내 들어가는 문자개수 100개
	const int stack_frame_count = 100;
	const int stack_data_count = 100;
	struct Stack_Frame* stack_frame = (Stack_Frame*)malloc(sizeof(Stack_Frame)*100);
	memset(stack_frame, 0, sizeof(Stack_Frame) * 100);

	
		
	// 1. "{" json객체생성 -> 스택프레임생성 -> 해쉬테이블 생성
	// 1-2 "}" json객체종료 -> 스택프레임제거
	// 2. ":" 키값과 밸류값을 구분짓는 구분점 -> 해쉬테이블의 필드와 데이터를 구분
	// 2-2 " 문자열임을 문자열의 시작을 의미
	// 2-3 !" 문자로 받으나 "가 없었다면 숫자임을 암시한 후 데이터 처리
	// 3. "[" 밸류값이 여러개가 존재함을 암시 -> {}, "~","~", []와 같이 객체가 들어올수도 있음
	char c;
	int in_string_ck = 0; //문자열의 시작과 끝 체크
	int key_ck = 1, value_ck = 0;  //문자열 키,밸류의 시작과 끝 체크
	for (int i = 0; test_arr_json[i] != '\0'; i++) {
		c = test_arr_json[i];

		switch (c) {
			case '\\':
				break;
			case '{':	//객체시작->스택프레임생성->해쉬테이블 생성
				for (int stack_ck = 0; stack_ck < stack_frame_count; stack_ck++) {	//스택프레임새로생성
					if (&stack_frame[stack_ck] == NULL) {
						json_parse_init_stack(&stack_frame[stack_ck], stack_data_count);
						stack_frame->top++;
						stack_frame->Data[stack_frame->top] = c;
						break;
					}
				}
				break;
			case '"':	//문자열의 시작과 끝을 의미
				in_string_ck = !in_string_ck;
				if (in_string_ck) {	//1 문자열 시작
					if (key_ck) { //키 시작
						
					}
					else {	//밸류 시작
						
					}
				}
				else {
					if (key_ck) {	//키 끝
						key_ck = 0;
						value_ck = 1;
					}
					else {//밸류 끝
						value_ck = 0;
						key_ck = 1;
					}
				}
				break;

					
		}

	}


	
	




	


	
}

void json_parse_table() {
	char test_arr_json[1024] = { "{ \"name\":\"홍길동\", \"age\":23}" };



}