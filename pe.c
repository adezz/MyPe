#include<windows.h>
#include<stdio.h>
#include "pe.h"

int main(){

	PVOID pFileBuffer; // ��ָ�� pFileBuffer = 0x00000000 &pFileBuffer = 0012f7c
	PVOID pImageBuffer = NULL;
	PVOID pNewBuffer = NULL;
	DWORD BufferLength = 0;
	char MyFunctionName[] = "mul";

	// ��ȡ�ļ����ڴ��� ��ʱ�ڴ��еĴ�С���ļ������С
	MyReadFile(&pFileBuffer,&BufferLength);

	// ��ӡPE�ṹ������������ �����
	printfPE(pFileBuffer);

	// ��ӡ�ض�λ��
	//printfRELOCATION(pFileBuffer);

	// �ļ���С���ڴ��С
	//CopyFileBufferToImageBuffer(pFileBuffer,&pImageBuffer);

	// ���shellcode
	//FileBufferToAddShellcode(pFileBuffer);
	
	// ����imagebase 
	//NewBufferSize = CopyImageBufferToNewBuffer(pImageBuffer,&pNewBuffer);

	// ������
	//AddNewSection(pFileBuffer,&BufferLength,&pNewBuffer);

	// �����
	//ExpandSection(pFileBuffer,&BufferLength,&pNewBuffer);


	//MyWriteFile(pNewBuffer,BufferLength);



	return 0;
}