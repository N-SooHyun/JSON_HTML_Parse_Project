#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<math.h>

/*
	JSON�� �⺻���� ������ Ű:���� �� ������ �̷���� ���·� {"key" : "value"} �� ���¸� ��ü����Ѵ�.
	���� []�迭�� ���·ε� ���μ� �پ��� ��Ҹ� ���Խ�ų���� �ִ�.
	1.�⺻���� JSON�� ��ü { "name" : "John", "age" : 25, "isStudent" : true} 
	2. JSON �迭 { "fruits" : ["apple", "banana", "orange"]} 
	3. ��ø�� ��ü {"person":{"name":"Jane","address" : {"street" : "123 Main St", "city" : "New York", "zipcode" : "10001"} } }
	4. �迭�ȿ� ��ü {"student" [{"name" : "Alice", "age":22},{"name":"Bob","age":23}] }
	5. �迭 ���� ��ø�迭 {"numbers" : [[1,2,3],[4,5,6],[7,8,9]] }
	6. ��ü�� �迭�� ���� ���� {"class" : "Math", "students" : [{"name":"Charlie","scores":[90,85,88]},{"name":"Diana", "scores":[92,81,89]} ] }
	7. ���� NULL�� json {"name" : "Emily", "age" : null, "email", "emily@example.com"}
	8. �پ��� ������ Ÿ���� ���� JSON {"product":"Laptop","price":799.99,"available":true,"specs":{"cpu":"Intel i7","ram":"16GB","storage":"512GB SSD"},"colors":["black","silver","white"]}
	9. ���� JSON true, 123.45 , "Hello Json"
	10. �� ��ü, �迭 {} []

	�̷� JSON�� �Ľ��ϴ°� ������
	1. FILE�� ���� ���� JSON������ �޸𸮿� �ø���
	2. ������������ �����Ͽ� ��ü�� �迭�� ��Ƽ� ����
	2-1. "{", "[" ���������ӻ��� "}", "]" ���������� �ı�
	2-2. ���������� ���� �� Ű:���� ���¸� ������ ����
	2-3. �� ������ �ؽ����̺��� ���·� ����
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

typedef struct Stack_Frame {
	int capacity;
	char* Data;
	int top;
	struct Stack_Frame* next_Stack_Frame;	//���ÿ� ����
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
	char c = json[(*idx)];	//������ �׳� �ٷ� �־��ֱ�
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
	if (number == (int)number) {//������
		key->value->type = 3;
		key->value->intValue = (int)number;
	}
	else {//�Ǽ���
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
	data[--i] = '\0';	//������ �˸�

	if (ck_key_value == 1) {//Ű�� ���ڿ� �ֱ�
		strcpy_s(key->keyValue, sizeof(char) * 100, data);
	}
	else if(ck_key_value==0){//����� ���ڿ� �ֱ�
		
		strcpy_s(key->value->stringValue, sizeof(char) * 100, data);
	}
}

void json_parse_obj_make_fun(Stack_Frame* stack, Value* table, KeyValue* key, char* json, int* idx) {	//{��ü���� ����
	//��ü ����
	stack->next_Stack_Frame = json_parse_init_stack_create();	//��ü ���� ����
	table->type = 1;	//objŸ��
	table->object = key;

	char c = json[++(*idx)];
	while (c != '}') {
		while (c == ' ') c = json[++(*idx)];
		//Ű�� ����
		json_parse_str_fun(key, json, idx, 1);	//Ű�� ���ڿ� �ֱ� �Լ�
		c = json[++(*idx)];
		while (c == ' ') c = json[++(*idx)];
		//����� ����
		if (c == ':') {
			c = json[++(*idx)];
			while (c == ' ') c = json[++(*idx)];
			key->value = (Value*)malloc(sizeof(Value));
			//3���� ����Ǽ� str,int,list
			if (c == '"') {	//���� ���
				key->value->type = 0;	//0:str
				json_parse_str_fun(key, json, idx, 0);	//����� ���ڿ� �ֱ� �Լ�
			}
			else if (c == '[') {//����Ʈ �� �ǹ�

			}
			else {	//int or double �ǹ�
				json_parse_num_fun(key, json, idx);
			}
		}
		c = json[++(*idx)];
		while (c == ' ') c = json[++(*idx)];
		if (c == ',') {//��ü�� Ű:���� �������� �Ǵ°���
			++(*idx);
			key->next = (KeyValue*)malloc(sizeof(KeyValue));
			key = key->next;
			c = json[++(*idx)];
		} 
	}
}

void json_parse_main() {	//���� ����
	char test_arr_json[1024] = { "{ \"name\":\"ȫ�浿\", \"age\":23}" };

	//���� ���������� ����
	const int stack_max_count = 100;
	Stack_Frame* main_stack = json_parse_init_stack_create();

	//���������� �ؽ����̺� �ʱ�ȭ
	Value main_table;
	KeyValue main_key;

	char c;
	int idx;
	for (idx = 0; ; idx++) {
		c = test_arr_json[idx];
		switch (c) {
			case '{':	//main()->obj()����
				//���̺� ��ü ���� �� Ű�� and ����� �ֱ�
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