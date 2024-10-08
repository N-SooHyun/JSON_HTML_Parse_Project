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
	union {	//�� ���� ���Ǳ⿡ union���� �޸� ����X
		char stringValue[1024];			//type:0 �ܼ� ���ڿ� value
		struct KeyValue* object;	//��ü(����Ʈ�ϰ�츸)
		struct ListValue* list;		//����Ʈ
		int intValue;			//������
		double doubleValue;		//�Ǽ���
	};
}Value;

typedef struct KeyValue {		//����
	char keyValue[1024];					//Ű(���ڿ�)
	struct Value* value;		//Ű�� �� ���� ��(str, obj, list) 3���� ����� �� ����
	struct KeyValue* next;		//���� ��ü�� ���� Ű
}KeyValue;

typedef struct ListValue {
	struct Value* value;	//
	struct ListValue* next;
}ListValue;