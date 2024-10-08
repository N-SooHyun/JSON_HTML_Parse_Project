#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<math.h>

int main() {
	json_parse_main();
	//json_parse_file();
	
	
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