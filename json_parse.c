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
	ValueType type;	//0:STR, 1:OBJ, 2:LIST, 3:INT, 4:DOUB
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

void json_parse_num_fun(KeyValue* key, char* json, int* idx, int ck) {
	char c = json[(*idx)];	//정수라 그냥 바로 넣어주기
	char data[100];
	int i = 0;
	double number;
	do {
		data[i++] = c;
		c = json[++(*idx)];
	} while (c != ',' && c != '}' && c != ' ' && c != ']');
	--(*idx);
	data[i] = '\0';
	number = strtod(data, NULL);
	if (ck == 0) {	//객체일경우
		if (number == (int)number) {//정수형
			key->value->type = 3;
			key->value->intValue = (int)number;
		}
		else {//실수형
			key->value->type = 4;
			key->value->doubleValue = number;
		}
	}
	else {
		if (number == (int)number) {//정수형
			key->value->list->value->type = 3;
			key->value->list->value->intValue = (int)number;
		}
		else {//실수형
			key->value->list->value->type = 4;
			key->value->list->value->doubleValue = number;
		}
	}
}

void json_parse_str_fun(KeyValue* key, char* json, int* idx, int ck_key_value) {
	char c = json[(*idx)];	//c == "
	if (c != '"') {
		printf("json 구문에 문법 오류가 있습니다.");
		exit(1);
	}
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
	}else if(ck_key_value=2){//밸류값 리스트
		strcpy_s(key->value->list->value->stringValue, sizeof(char) * 100, data);
	}
}

void json_parse_obj_make_fun(Value* table, KeyValue* key, char* json, int* idx);

void json_parse_list_fun(KeyValue* key, char* json, int* idx) {
	char c = json[++(*idx)];	//리스트 시작 문자
	while (c == ' ' || c == '\n') c = json[++(*idx)];
	key->value->type = 2;		//2:list
	key->value->list = (ListValue*)malloc(sizeof(ListValue));
	key->value->list->value = (Value*)malloc(sizeof(Value));
	while (c != ']') {
		while (c == ' ' || c == '\n') c = json[++(*idx)];
		if (c == '"') {
			key->value->list->value->type = 0; //0:str
			json_parse_str_fun(key, json, idx, 2);
		}
		else if (c == '{') {
			key->value->list->value->object = (KeyValue*)malloc(sizeof(KeyValue));
			json_parse_obj_make_fun(key->value->list->value, key->value->list->value->object, json, idx);
		}
		else {	//정수 혹은 실수인경우
			json_parse_num_fun(key, json, idx,2);
		}
		c = json[++(*idx)];
		while (c == ' ' || c == '\n') c = json[++(*idx)];
		if (c == ',') {	//다음 리스트 생성
			key->value->list->next = (ListValue*)malloc(sizeof(ListValue));
			key->value->list = key->value->list->next;
			key->value->list->value = (Value*)malloc(sizeof(Value));
			c = json[++(*idx)];
		}
	}
}

void json_parse_obj_make_fun(Value* table, KeyValue* key, char* json, int* idx) {	//{객체생성 스택
	if (table != NULL) {
		table->type = 1;	//obj타입
		table->object = key;
	}

	char c = json[++(*idx)];
	while (c != '}') {
		while (c == ' ' || c == '\n') c = json[++(*idx)];
		//키값 삽입
		json_parse_str_fun(key, json, idx, 1);	//키값 문자열 넣기 함수
		c = json[++(*idx)];
		while (c == ' ' || c == '\n') c = json[++(*idx)];
		//밸류값 삽입
		if (c == ':') {
			c = json[++(*idx)];
			while (c == ' ' || c == '\n') c = json[++(*idx)];
			key->value = (Value*)malloc(sizeof(Value));
			//3가지 경우의수 str,int,list
			if (c == '"') {	//문자 밸류
				key->value->type = 0;	//0:str
				json_parse_str_fun(key, json, idx, 0);	//밸류값 문자열 넣기 함수
			}
			else if (c == '[') {//리스트 값 의미
				//리스트의 경우 obj, str, char, int, double 다 섞일 수 있음
				json_parse_list_fun(key, json, idx);
			}
			else {	//int or double 의미
				json_parse_num_fun(key, json, idx,0);
			}
		}
		c = json[++(*idx)];
		while (c == ' ' || c == '\n') c = json[++(*idx)];
		if (c == ',') { //객체에 키:값 한 쌍이 여러개가 되는거임
			++(*idx);
			key->next = (KeyValue*)malloc(sizeof(KeyValue));
			key = key->next;
			c = json[++(*idx)];
		} 
	}
}

void json_parse_main() {	//메인 스택
	FILE* pFile = NULL;
	char test_arr_json[1024];
	fopen_s(&pFile, "json\\test.json", "r");
	int i;
	for (i = 0; i < 1024; i++) {
		test_arr_json[i] = fgetc(pFile);
		if (test_arr_json[i] == EOF) break;
	}
	test_arr_json[i] = '\0';
	

	//메인 스택프레임 생성
	const int stack_max_count = 100;

	//전역느낌의 해쉬테이블 초기화
	Value main_table;
	KeyValue main_key;

	char c;
	int idx;
	for (idx = 0; ; idx++) {
		c = test_arr_json[idx];
		switch (c) {
			case ' ':
				break;
			case '{':	//main()->obj()생성
				//테이블에 객체 생성 후 키값 and 밸류값 넣기
				json_parse_obj_make_fun(&main_table, &main_key, test_arr_json, &idx);
				//종료시 test_arr_json[idx] = '}'
				break;
			case '"':	//문자열
				json_parse_str_fun(&main_key, test_arr_json, &idx, 1);
				//종료시 test_arr_json[idx] = '"'
				break;
			case '[':	//리스트
				json_parse_list_fun(&main_key, test_arr_json, &idx);
				//종료시 test_arr_json[idx] = ']'
				break;
			default:	//정수
				json_parse_num_fun(&main_key, test_arr_json, &idx, 0);
				//종료시 test_arr_json[idx] = '정수끝값 반환'
				break;
		}
		if ((c = test_arr_json[++idx]) == ',') ++idx;
	}

	while (1) {
		switch (main_table.type) {
			case 0:	//str
				break;
			case 1:	//obj
				break;
			case 2:	//list
				break;
			case 3:	//int
				break;
			case 4: //double
				break;
			default:
				break;
		}
	}
	
}