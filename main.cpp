#include <bits/stdc++.h>
#include <stdint.h>
#include "mem.h"
using namespace std;

int main()
{
	if(!Mem_Init(4096))
	{
		cout<<"init failed";
		return 1;
	}
	// void *ptr=Mem_Alloc(2);
	void *ptr1=Mem_Alloc(4);
	// void *ptr3=Mem_Alloc(2);
	//
	int* t1 = (int *) Mem_Alloc(4);
	*t1 = 12;
	printf("%d\n", *t1);
	char* t2 = (char *) Mem_Alloc(1);
	*t2 = 'a';
	printf("%c\n", *t2);

	int* arr1 = (int *) Mem_Alloc(8);
	arr1[0] = 1;
	arr1[1] = 2;
	printf("%d\n", arr1[0]);
	printf("%d\n", arr1[1]);


	
	Mem_Free(ptr1+2);
	Mem_Free(ptr1+10);
	

	// cout<<"Is Valid of Ptr1 "<<Mem_IsValid(ptr1)<<endl;
	// cout<<"Is Valid of Ptr1+1 "<<Mem_IsValid(ptr1+1)<<endl;
	// cout<<"Is Valid of Ptr1+2 "<<Mem_IsValid(ptr1+2)<<endl;
	// cout<<"Is Valid of Ptr1+3 "<<Mem_IsValid(ptr1+3)<<endl;
	int test=4;
	while(test--){
		cout<<"Is Valid of Ptr1 + "<<test<<" "<<Mem_IsValid(ptr1+test)<<endl;
	}
	garbage_collector();
	return 0;
}
