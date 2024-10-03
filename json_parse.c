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