#include<windows.h>
#include<stdio.h>
#include "pe.h"

int main(){
	PVOID pFileBuffer = NULL;
	PVOID pImageBuffer = NULL;
	PVOID pNewBuffer = NULL;
	DWORD BufferLength = 0;

	// ��ȡ�ļ����ڴ��� ��ʱ�ڴ��еĴ�С���ļ������С
	MyReadFile(&pFileBuffer,&BufferLength);

	// ��ӡPE�ṹ������������
	//printfPE(pFileBuffer);

	// ��ӡ�ض�λ�� ����ӡ��ʱ��ǵð�RVA_TO_FOAǰ����ж�ע�͵���ԭ���ǿ����ļ�������ڴ����һ�� ���ǿ���ȫ�ֱ�����ԭ���²�һ��
	//PrintRelocation(pFileBuffer);

	// ��ӡ�����
	//PrintfImportTable(pFileBuffer);

	// ��ӡ�󶨵����
	//PrintBindImportTable(pFileBuffer);

	// �ƶ���������ҽ���ע�����
	MoveAndInjectImportTable(pFileBuffer,&BufferLength,&pNewBuffer);

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

	//�����ļ�
	//MyWriteFile(pNewBuffer,BufferLength);

	//�ƶ������� ����������� �Լ��ƶ������ݶ��ŵ��ڱ������ˣ� ��ʵ�ŵ�λ���Ƕ�Ӧ�ڱ�����Ľ����ݵ�λ�á�������
	//MoveExportTable(pFileBuffer,&BufferLength,&pNewBuffer);

	//�ƶ��ض�λ��
	//MoveRelocationTable(pFileBuffer,&BufferLength, &pNewBuffer);

	return 0;
}