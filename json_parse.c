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

typedef enum { STRING, OBJECT, LIST, INTEGER, DOUB } ValueType;

typedef struct Value {
	ValueType type;	//0:STR, 1:OBJ, 2:LIST, 3:INT
	union {	//한 개만 사용되기에 union으로 메모리 낭비X
		char stringValue[1024];			//type:0 단순 문자열 value
		struct KeyValue* object;	//객체(리스트일경우만)
		struct ListValue* list;		//리스트
		int intValue;			//정수형
		double doubleValue;		//실수형
	};
}Value;

typedef struct KeyValue {		//메인
	char keyValue[1024];					//키(문자열)
	struct Value* value;		//키의 한 쌍의 값(str, obj, list) 3개의 경우의 수 존재
	struct KeyValue* next;		//현재 객체의 다음 키
}KeyValue;

typedef struct ListValue {
	struct Value* value;	//
	struct ListValue* next;
}ListValue;

typedef struct Stack_Frame {
	int capacity;
	char* Data;
	int top;
	struct Stack_Frame* next_Stack_Frame;	//스택에 스택
}Stack_Frame;

Stack_Frame* json_parse_init_stack_frame(int capacity) {
	Stack_Frame* stack = (Stack_Frame*)malloc(sizeof(Stack_Frame) * capacity);
	memset(stack, 0, sizeof(Stack_Frame)*capacity);
	return stack;
}

Stack_Frame* json_parse_init_stack_create() {
	int capacity = 100;
	Stack_Frame* stack = (Stack_Frame*)malloc(sizeof(Stack_Frame));
	stack->capacity = capacity; 
	stack->Data = (char*)malloc(sizeof(char) * capacity);
	stack->top = -1;
	stack->next_Stack_Frame = NULL;
	return stack;
}

void json_parse_num_fun(KeyValue* key, char* json, int* idx) {
	char c = json[(*idx)];	//정수라 그냥 바로 넣어주기
	char data[100];
	int i = 0;
	double number;
	do {
		data[i++] = c;
		c = json[++(*idx)];
	} while (c != "," && (c != '}' && c!=' '));
	--(*idx);
	data[i] = '\0';
	number = strtod(data, NULL);
	if (number == (int)number) {//정수형
		key->value->type = 3;
		key->value->intValue = (int)number;
	}
	else {//실수형
		key->value->type = 4;
		key->value->doubleValue = number;
	}
}

void json_parse_str_fun(KeyValue* key, char* json, int* idx, int ck_key_value) {
	char c = json[(*idx)];	//c == "
	char data[100];
	int i = 0;
	do {
		c = json[++(*idx)];
		data[i++] = c;
	} while (c != '"');
	data[--i] = '\0';	//끝임을 알림

	if (ck_key_value == 1) {//키값 문자열 넣기
		strcpy_s(key->keyValue, sizeof(char) * 100, data);
	}
	else if(ck_key_value==0){//밸류값 문자열 넣기
		
		strcpy_s(key->value->stringValue, sizeof(char) * 100, data);
	}
}

void json_parse_obj_make_fun(Stack_Frame* stack, Value* table, KeyValue* key, char* json, int* idx) {	//{객체생성 스택
	//객체 생성
	stack->next_Stack_Frame = json_parse_init_stack_create();	//객체 스택 생성
	table->type = 1;	//obj타입
	table->object = key;

	char c = json[++(*idx)];
	while (c != '}') {
		while (c == ' ') c = json[++(*idx)];
		//키값 삽입
		json_parse_str_fun(key, json, idx, 1);	//키값 문자열 넣기 함수
		c = json[++(*idx)];
		while (c == ' ') c = json[++(*idx)];
		//밸류값 삽입
		if (c == ':') {
			c = json[++(*idx)];
			while (c == ' ') c = json[++(*idx)];
			key->value = (Value*)malloc(sizeof(Value));
			//3가지 경우의수 str,int,list
			if (c == '"') {	//문자 밸류
				key->value->type = 0;	//0:str
				json_parse_str_fun(key, json, idx, 0);	//밸류값 문자열 넣기 함수
			}
			else if (c == '[') {//리스트 값 의미

			}
			else {	//int or double 의미
				json_parse_num_fun(key, json, idx);
			}
		}
		c = json[++(*idx)];
		while (c == ' ') c = json[++(*idx)];
		if (c == ',') {//객체에 키:값이 여러개가 되는거임
			++(*idx);
			key->next = (KeyValue*)malloc(sizeof(KeyValue));
			key = key->next;
			c = json[++(*idx)];
		} 
	}
}

void json_parse_main() {	//메인 스택
	char test_arr_json[1024] = { "{ \"name\":\"홍길동\", \"age\":23}" };

	//메인 스택프레임 생성
	const int stack_max_count = 100;
	Stack_Frame* main_stack = json_parse_init_stack_create();

	//전역느낌의 해쉬테이블 초기화
	Value main_table;
	KeyValue main_key;

	char c;
	int idx;
	for (idx = 0; ; idx++) {
		c = test_arr_json[idx];
		switch (c) {
			case '{':	//main()->obj()생성
				//테이블에 객체 생성 후 키값 and 밸류값 넣기
				json_parse_obj_make_fun(main_stack, &main_table, &main_key, test_arr_json, &idx);
				break;
		}
		break;
	}
	
	printf("%s\n", main_key.keyValue);
	printf("%s\n", main_key.value->stringValue);
	printf("%s\n", main_key.next->keyValue);
	printf("%d\n", main_key.next->value->intValue);
}