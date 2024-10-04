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

typedef enum { STRING, OBJECT, LIST } ValueType;

typedef struct Value {	//����
	ValueType type;	//0:STR, 1:OBJ, 2:LIST
	union {	//�� ���� ���Ǳ⿡ union���� �޸� ����X
		char* stringValue;			//type:0 �ܼ� ���ڿ� value
		struct KeyValue* object;	//��ü(����Ʈ�ϰ�츸)
		struct ListValue* list;		//����Ʈ
	};
}Value;

typedef struct KeyValue {		
	char* key;					//Ű(���ڿ�)
	struct Value* value;		//��(str, obj, list) 3���� ����� �� ����
	struct KeyValue* next;		//���� ��ü�� ���� Ű
}KeyValue;

typedef struct ListValue {
	struct Value* value;	//
	struct ListValue* next;
}ListValue;

void json_parse_add_key() {
	//��ü�� ���ο� Ű-�� ����
}

typedef struct Stack_Frame {
	int capacity;
	char* Data;
	int top;
	struct Stack_Frame* next_Stack_Frame;	//���ÿ� ����
}Stack_Frame;

void json_parse_init_stack(Stack_Frame* stack_frame, int capacity) {
	stack_frame->capacity = capacity;
	stack_frame->top = -1;
	stack_frame->Data = (char*)malloc(sizeof(char) * capacity);
	stack_frame->next_Stack_Frame = NULL;
}

void json_parse_main() {
	char test_arr_json[1024] = { "{ \"name\":\"ȫ�浿\", \"age\":23}" };
	//�ִ� ���� �� �ִ� ���������� 100��, ���� �� ���� ���ڰ��� 100��
	const int stack_frame_count = 100;
	const int stack_data_count = 100;
	struct Stack_Frame* stack_frame = (Stack_Frame*)malloc(sizeof(Stack_Frame)*100);
	memset(stack_frame, 0, sizeof(Stack_Frame) * 100);

	
		
	// 1. "{" json��ü���� -> ���������ӻ��� -> �ؽ����̺� ����
	// 1-2 "}" json��ü���� -> ��������������
	// 2. ":" Ű���� ������� �������� ������ -> �ؽ����̺��� �ʵ�� �����͸� ����
	// 2-2 " ���ڿ����� ���ڿ��� ������ �ǹ�
	// 2-3 !" ���ڷ� ������ "�� �����ٸ� �������� �Ͻ��� �� ������ ó��
	// 3. "[" ������� �������� �������� �Ͻ� -> {}, "~","~", []�� ���� ��ü�� ���ü��� ����
	char c;
	int in_string_ck = 0; //���ڿ��� ���۰� �� üũ
	int key_ck = 1, value_ck = 0;  //���ڿ� Ű,����� ���۰� �� üũ
	for (int i = 0; test_arr_json[i] != '\0'; i++) {
		c = test_arr_json[i];

		switch (c) {
			case '\\':
				break;
			case '{':	//��ü����->���������ӻ���->�ؽ����̺� ����
				for (int stack_ck = 0; stack_ck < stack_frame_count; stack_ck++) {	//���������ӻ��λ���
					if (&stack_frame[stack_ck] == NULL) {
						json_parse_init_stack(&stack_frame[stack_ck], stack_data_count);
						stack_frame->top++;
						stack_frame->Data[stack_frame->top] = c;
						break;
					}
				}
				break;
			case '"':	//���ڿ��� ���۰� ���� �ǹ�
				in_string_ck = !in_string_ck;
				if (in_string_ck) {	//1 ���ڿ� ����
					if (key_ck) { //Ű ����
						
					}
					else {	//��� ����
						
					}
				}
				else {
					if (key_ck) {	//Ű ��
						key_ck = 0;
						value_ck = 1;
					}
					else {//��� ��
						value_ck = 0;
						key_ck = 1;
					}
				}
				break;

					
		}

	}


	
	




	


	
}

void json_parse_table() {
	char test_arr_json[1024] = { "{ \"name\":\"ȫ�浿\", \"age\":23}" };



}