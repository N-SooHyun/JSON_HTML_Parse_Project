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

void json_parse_num_fun(KeyValue* key, char* json, int* idx, int ck) {
	char c = json[(*idx)];	//������ �׳� �ٷ� �־��ֱ�
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
	if (ck == 0) {	//��ü�ϰ��
		if (number == (int)number) {//������
			key->value->type = 3;
			key->value->intValue = (int)number;
		}
		else {//�Ǽ���
			key->value->type = 4;
			key->value->doubleValue = number;
		}
	}
	else {
		if (number == (int)number) {//������
			key->value->list->value->type = 3;
			key->value->list->value->intValue = (int)number;
		}
		else {//�Ǽ���
			key->value->list->value->type = 4;
			key->value->list->value->doubleValue = number;
		}
	}
}

void json_parse_str_fun(KeyValue* key, char* json, int* idx, int ck_key_value) {
	char c = json[(*idx)];	//c == "
	if (c != '"') {
		printf("json ������ ���� ������ �ֽ��ϴ�.");
		exit(1);
	}
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
	}else if(ck_key_value=2){//����� ����Ʈ
		strcpy_s(key->value->list->value->stringValue, sizeof(char) * 100, data);
	}
}

void json_parse_obj_make_fun(Value* table, KeyValue* key, char* json, int* idx);

void json_parse_list_fun(KeyValue* key, char* json, int* idx) {
	char c = json[++(*idx)];	//����Ʈ ���� ����
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
		else {	//���� Ȥ�� �Ǽ��ΰ��
			json_parse_num_fun(key, json, idx,2);
		}
		c = json[++(*idx)];
		while (c == ' ' || c == '\n') c = json[++(*idx)];
		if (c == ',') {	//���� ����Ʈ ����
			key->value->list->next = (ListValue*)malloc(sizeof(ListValue));
			key->value->list = key->value->list->next;
			key->value->list->value = (Value*)malloc(sizeof(Value));
			c = json[++(*idx)];
		}
	}
}

void json_parse_obj_make_fun(Value* table, KeyValue* key, char* json, int* idx) {	//{��ü���� ����
	if (table != NULL) {
		table->type = 1;	//objŸ��
		table->object = key;
	}

	char c = json[++(*idx)];
	while (c != '}') {
		while (c == ' ' || c == '\n') c = json[++(*idx)];
		//Ű�� ����
		json_parse_str_fun(key, json, idx, 1);	//Ű�� ���ڿ� �ֱ� �Լ�
		c = json[++(*idx)];
		while (c == ' ' || c == '\n') c = json[++(*idx)];
		//����� ����
		if (c == ':') {
			c = json[++(*idx)];
			while (c == ' ' || c == '\n') c = json[++(*idx)];
			key->value = (Value*)malloc(sizeof(Value));
			//3���� ����Ǽ� str,int,list
			if (c == '"') {	//���� ���
				key->value->type = 0;	//0:str
				json_parse_str_fun(key, json, idx, 0);	//����� ���ڿ� �ֱ� �Լ�
			}
			else if (c == '[') {//����Ʈ �� �ǹ�
				//����Ʈ�� ��� obj, str, char, int, double �� ���� �� ����
				json_parse_list_fun(key, json, idx);
			}
			else {	//int or double �ǹ�
				json_parse_num_fun(key, json, idx,0);
			}
		}
		c = json[++(*idx)];
		while (c == ' ' || c == '\n') c = json[++(*idx)];
		if (c == ',') { //��ü�� Ű:�� �� ���� �������� �Ǵ°���
			++(*idx);
			key->next = (KeyValue*)malloc(sizeof(KeyValue));
			key = key->next;
			c = json[++(*idx)];
		} 
	}
}

void json_parse_main() {	//���� ����
	FILE* pFile = NULL;
	char test_arr_json[1024];
	fopen_s(&pFile, "json\\test.json", "r");
	int i;
	for (i = 0; i < 1024; i++) {
		test_arr_json[i] = fgetc(pFile);
		if (test_arr_json[i] == EOF) break;
	}
	test_arr_json[i] = '\0';
	

	//���� ���������� ����
	const int stack_max_count = 100;

	//���������� �ؽ����̺� �ʱ�ȭ
	Value main_table;
	KeyValue main_key;

	char c;
	int idx;
	for (idx = 0; ; idx++) {
		c = test_arr_json[idx];
		switch (c) {
			case ' ':
				break;
			case '{':	//main()->obj()����
				//���̺� ��ü ���� �� Ű�� and ����� �ֱ�
				json_parse_obj_make_fun(&main_table, &main_key, test_arr_json, &idx);
				//����� test_arr_json[idx] = '}'
				break;
			case '"':	//���ڿ�
				json_parse_str_fun(&main_key, test_arr_json, &idx, 1);
				//����� test_arr_json[idx] = '"'
				break;
			case '[':	//����Ʈ
				json_parse_list_fun(&main_key, test_arr_json, &idx);
				//����� test_arr_json[idx] = ']'
				break;
			default:	//����
				json_parse_num_fun(&main_key, test_arr_json, &idx, 0);
				//����� test_arr_json[idx] = '�������� ��ȯ'
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