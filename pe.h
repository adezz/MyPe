DWORD CopyFileBufferToImageBuffer(PVOID pFileBuffer,PVOID pImageBuffer);		
DWORD CopyImageBufferToNewBuffer(PVOID pImageBuffer,PVOID* pNewBuffer);
void FileBufferToAddShellcode(PVOID pFileBuffer);
void AddNewSection(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer);
								
DWORD FOA_TO_RVA(PVOID FileAddress, DWORD FOA,PDWORD pRVA);
DWORD RVA_TO_FOA(PVOID FileAddress, DWORD RVA, PDWORD pFOA);
void MyReadFile(PVOID* pFileBuffer,PDWORD BufferLenth);
void MyWriteFile(PVOID pMemBuffer,size_t size);
int GetBufferLength(PVOID Buffer);

#define FILENAME "C:\\Documents and Settings\\Administrator\\����\\ipmsg.exe"
#define NEWFILENAME "C:\\Documents and Settings\\Administrator\\����\\NEW_ipmsg.exe"


int GetBufferLength(PVOID Buffer){
	int BufferLength;
	BufferLength = ftell(Buffer);
	return BufferLength;
}

//ReadPEFile:���ļ���ȡ��������				
//����˵����				
//lpszFile �ļ�·��				
//pFileBuffer ������ָ��				
//����ֵ˵����				
//��ȡʧ�ܷ���0  ���򷵻�ʵ�ʶ�ȡ�Ĵ�С				
void MyReadFile(PVOID* pFileBuffer,PDWORD BufferLenth){
	FILE* File;
	File = fopen(FILENAME,"rb");

	if(File == NULL){
		printf("�ļ������ʧ��");
		return;
	}

	//��ȡ�ļ�
	fseek(File,0,SEEK_END);
	*BufferLenth = ftell(File);
	fseek(File,0,SEEK_SET);

	//�����¿ռ�
	*pFileBuffer = (PVOID)malloc(*BufferLenth);

	//�ڴ�����
	memset(*pFileBuffer,0,*BufferLenth);

	//��ȡ���ڴ滺����
	fread(*pFileBuffer,*BufferLenth,1,File);

	//�ر��ļ����
	fclose(File);
}

//**************************************************************************								
//CopyFileBufferToImageBuffer:���ļ���FileBuffer���Ƶ�ImageBuffer								
//����˵����								
//pFileBuffer  FileBufferָ��								
//pImageBuffer ImageBufferָ��								
//����ֵ˵����								
//��ȡʧ�ܷ���0  ���򷵻ظ��ƵĴ�С								
						
DWORD CopyFileBufferToImageBuffer(PVOID pFileBuffer,PVOID* pImageBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_NT_HEADERS pImageNtHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	DWORD ImageBufferSize = 0;
	int i=0;
	

	// DOSͷ
	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;

	// ��׼PE
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);

	// ��ѡPE
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + IMAGE_SIZEOF_FILE_HEADER);

	//�ڱ���
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	//��ȡImageBufffer���ڴ��С
	ImageBufferSize = pImageOptionalHeader->SizeOfImage;
	
	//ΪpImageBuffer�����ڴ�ռ�
	*pImageBuffer = (PVOID)malloc(ImageBufferSize);

	if (*pImageBuffer == NULL)
	{
		printf("malloc failed");
		return -1;
	}

	//����
	memset(*pImageBuffer, 0, ImageBufferSize);
	
	// ����ͷ+�ڱ�
	memcpy(*pImageBuffer, pFileBuffer, pImageOptionalHeader->SizeOfHeaders);


	//ѭ�������ڱ�

	for(i=0;i<pImageFileHeader->NumberOfSections;i++){
		memcpy(
			(PVOID)((DWORD)*pImageBuffer + pImageSectionHeaderGroup[i].VirtualAddress), // Ҫ������λ�� ImageBuffer�е�ÿ�������ݵ�ƫ��λ��
			(PVOID)((DWORD)pFileBuffer + pImageSectionHeaderGroup[i].PointerToRawData), // ��������λ���� Filebuffer�е�ÿ�������ݵ�ƫ��λ��
			pImageSectionHeaderGroup[i].SizeOfRawData // �������Ĵ�СΪ ÿ�������ݵ��ļ������С
		);
	}

	return 0;
}						


//**************************************************************************								
//CopyImageBufferToNewBuffer:��ImageBuffer�е����ݸ��Ƶ��µĻ�����								
//����˵����								
//pImageBuffer ImageBufferָ��								
//pNewBuffer NewBufferָ��								
//����ֵ˵����								
//��ȡʧ�ܷ���0  ���򷵻ظ��ƵĴ�С															
DWORD CopyImageBufferToNewBuffer(PVOID pImageBuffer,PVOID* pNewBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_NT_HEADERS pImageNtHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	DWORD NewBufferSize = 0;
	int i;
	int j;
	
	// DOSͷ
	pImageDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	
	//pImageNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew);
	
	// ��׼PE
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	
	// ��ѡPE
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + IMAGE_SIZEOF_FILE_HEADER);
	
	//�ڱ���
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);
	
	//��ȡNewBufferSize���ڴ��С
	NewBufferSize = pImageOptionalHeader->SizeOfHeaders;


	//��ѭ�����Ͻ����ݵĴ�С
	for(j=0;j<pImageFileHeader->NumberOfSections;j++){
		NewBufferSize += pImageSectionHeaderGroup[j].SizeOfRawData;
	}


	//ΪNewBufferSize�����ڴ�ռ�
	*pNewBuffer = (PVOID)malloc(NewBufferSize);
		
	if (*pNewBuffer == NULL)
	{
		printf("malloc failed");
		return -1;
	}

	//����
	memset(*pNewBuffer, 0, NewBufferSize);
	
	// ����ͷ+�ڱ�
	memcpy(*pNewBuffer, pImageBuffer, pImageOptionalHeader->SizeOfHeaders);
	
	
	//ѭ�������ڱ�
	for(i=0;i<pImageFileHeader->NumberOfSections;i++){
		memcpy(
			(PVOID)((DWORD)*pNewBuffer + pImageSectionHeaderGroup[j].PointerToRawData),
			(PVOID)((DWORD)pImageBuffer + pImageSectionHeaderGroup[j].VirtualAddress),
			pImageSectionHeaderGroup[j].SizeOfRawData
		);
	}

	return NewBufferSize;
}	


//���ܣ�FOA ת�� RVA

DWORD FOA_TO_RVA(PVOID FileAddress, DWORD FOA,PDWORD pRVA)
{
	int ret = 0;
	int i;
	
	PIMAGE_DOS_HEADER pDosHeader				= (PIMAGE_DOS_HEADER)(FileAddress);
	PIMAGE_FILE_HEADER pFileHeader				= (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader	= (PIMAGE_OPTIONAL_HEADER32)((DWORD)pFileHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSectionGroup			= (PIMAGE_SECTION_HEADER)((DWORD)pOptionalHeader + pFileHeader->SizeOfOptionalHeader);
	
	//RVA���ļ�ͷ�� �� SectionAlignment ���� FileAlignment ʱRVA����FOA
	if (FOA < pOptionalHeader->SizeOfHeaders || pOptionalHeader->SectionAlignment == pOptionalHeader->FileAlignment)
	{
		*pRVA = FOA;
		return ret;
	}
	
	//ѭ���ж�FOA�ڽ�����
	for (i=0;i < pFileHeader->NumberOfSections; i++)
	{
		if (FOA >= pSectionGroup[i].PointerToRawData && FOA < pSectionGroup[i].PointerToRawData + pSectionGroup[i].SizeOfRawData)
		{
			*pRVA = FOA - pSectionGroup[i].PointerToRawData + pSectionGroup[i].VirtualAddress;
			
			return *pRVA;
		}
	}
	
	//û���ҵ���ַ
	ret = -4;
	printf("func FOA_TO_RVA() Error: %d ��ַת��ʧ�ܣ�\n", ret);
	return ret;
}


//���ܣ�RVA ת�� FOA
DWORD RVA_TO_FOA(PVOID FileAddress, DWORD RVA, PDWORD pFOA)
{
	int ret = 0;
	int i=0;
	PIMAGE_DOS_HEADER pDosHeader				= (PIMAGE_DOS_HEADER)(FileAddress);
	PIMAGE_FILE_HEADER pFileHeader				= (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader	= (PIMAGE_OPTIONAL_HEADER32)((DWORD)pFileHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSectionGroup			= (PIMAGE_SECTION_HEADER)((DWORD)pOptionalHeader + pFileHeader->SizeOfOptionalHeader);
	
	//RVA���ļ�ͷ�� �� SectionAlignment ���� FileAlignment ʱRVA����FOA
	if (RVA < pOptionalHeader->SizeOfHeaders || pOptionalHeader->SectionAlignment == pOptionalHeader->FileAlignment)
	{
		*pFOA = RVA;
		return ret;
	}
	
	//ѭ���ж�RVA�ڽ�����
	for (i = 0; i < pFileHeader->NumberOfSections; i++)
	{
		if (RVA >= pSectionGroup[i].VirtualAddress && RVA < pSectionGroup[i].VirtualAddress + pSectionGroup[i].Misc.VirtualSize)
		{
			*pFOA = pSectionGroup[i].PointerToRawData + RVA - pSectionGroup[i].VirtualAddress;
			return ret;
		}
	}
	
	//û���ҵ���ַ
	ret = -4;
	printf("func RAV_TO_FOA() Error: %d ��ַת��ʧ�ܣ�\n", ret);
	return ret;
}
								

//���ܣ������ļ� 
void MyWriteFile(PVOID pMemBuffer,size_t size){
	
	FILE* File;
	File = fopen(NEWFILENAME,"wb");
	if(File == NULL){
		printf("�ļ������ʧ��");
		return;
	}
	fwrite(pMemBuffer,size,1,File);
	printf("�ļ�����ɹ�!");
	fclose(File);

}


//���ܣ����shellcode
void FileBufferToAddShellcode(PVOID pFileBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;

	DWORD CodeAddress = 0; //Ҫ���shellcode�ĵ�ַ
	DWORD FuncAddress; //MESSAGEBOX��ַ
	HMODULE hModule; //����User32

	DWORD FOA = 0;
	DWORD RVA = 0;

	BYTE SHELLCODE[] = {
		0X6A,0X00,0X6A,0X00,0X6A,0X00,0X6A,0X00,
		0XE8,0X00,0X00,0X00,0X00,
		0XE9,0X00,0X00,0X00,0X00
	};

	DWORD E8_Next_Address;
	DWORD E9_Next_Address;
	DWORD EntryOfAddress;



	// DOSͷ
	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	
	// ��׼PE
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	
	// ��ѡPE
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + IMAGE_SIZEOF_FILE_HEADER);
	
	//�ڱ���
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	//��ȡMessagebox�ĵ�ַ
	hModule = LoadLibrary("User32.dll");
	FuncAddress = (DWORD)GetProcAddress(hModule, "MessageBoxA");


	// CodeAddressΪSHELLCODE���ļ��е���ʼ��ַ
	CodeAddress = (DWORD)pImageSectionHeaderGroup[0].PointerToRawData + (DWORD)pImageSectionHeaderGroup[0].Misc.VirtualSize;


	// ����E8����ָ�����һ�е�ַ��RVA
	E8_Next_Address = CodeAddress + 13;
	
	FOA_TO_RVA(pFileBuffer,E8_Next_Address,&RVA);


	// X = ����Ҫ��ת�ĵ�ַ - E8����ָ�����һ�е�ַ
	E8_Next_Address = FuncAddress - (RVA + pImageOptionalHeader->ImageBase);

	//���E8�հ׵Ŀհײ���
	memcpy(&SHELLCODE[9], &E8_Next_Address, 4);


	// ����E9����ָ�����һ�е�ַ��RVA
	E9_Next_Address = CodeAddress + 18;

	FOA_TO_RVA(pFileBuffer,E9_Next_Address,&RVA);

	//�ٻ�ȡԭ����ڵ�ַ��VA
	EntryOfAddress = pImageOptionalHeader->AddressOfEntryPoint;


	// X = ����Ҫ��ת�ĵ�ַ - E9����ָ�����һ�е�ַ
	E9_Next_Address = EntryOfAddress - RVA;

	memcpy(&SHELLCODE[14],&E9_Next_Address,4);

	//�������shellcode������ٰ�shellcode�Ž�ȥ
	memcpy((PVOID)((DWORD)pFileBuffer+CodeAddress),SHELLCODE,0x20);
	
	//����滻OEP��λ�ã��滻Ϊshellcode�ĵ�ַ

	FOA_TO_RVA(pFileBuffer,CodeAddress,&RVA);

	pImageOptionalHeader->AddressOfEntryPoint = RVA;

}

void AddNewSection(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	PIMAGE_SECTION_HEADER NewSec = NULL;

	DWORD isOk;
	DWORD NewLength=0;
	PVOID LastSection = NULL;
	PVOID CodeSection = NULL;

	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	//�ж��Ƿ����������Ӧ�Ľڱ�
	isOk = (DWORD)pImageOptionalHeader->SizeOfHeaders - ((DWORD)pImageDosHeader->e_lfanew + IMAGE_SIZEOF_FILE_HEADER + pImageFileHeader->SizeOfOptionalHeader + 40*pImageFileHeader->NumberOfSections);
	if(isOk < 80){
		printf("�ռ�̫С �޷��������!");
		return;
	}

	//���ɶ�Ӧ���ڴ��С�Ŀռ�
	NewLength += *OldBufferSize + 0x1000;
	*pNewBuffer = (PVOID)malloc(NewLength);
	ZeroMemory(*pNewBuffer,NewLength);

	//����֮ǰ�ڴ�ռ� �� ��ǰ�����ɵ��ڴ�ռ�
	memcpy(*pNewBuffer,pFileBuffer,*OldBufferSize);

	//��ȡ�µĽṹ��
	pImageDosHeader = (PIMAGE_DOS_HEADER)(*pNewBuffer);
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	// pImageFileHeader->NumberOfSections�޸�
	pImageFileHeader->NumberOfSections = pImageFileHeader->NumberOfSections + 1;
	
	// pImageOptionalHeader->SizeOfImage�޸�
	pImageOptionalHeader->SizeOfImage = (DWORD)pImageOptionalHeader->SizeOfImage + 0x1000;

	// ���ƴ���εĽ����ݵ� ��ǰ���һ�������ݺ���
	CodeSection = (PVOID)(&pImageSectionHeaderGroup[0]);
	LastSection = (PVOID)(DWORD)(&pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1]);
	memcpy(LastSection,CodeSection,40);
	
	//�����������
 	NewSec = (PIMAGE_SECTION_HEADER)LastSection;
	strcpy(NewSec,".NewSec");
	NewSec->Misc.VirtualSize = 0x1000;
	NewSec->SizeOfRawData = 0x1000;
	NewSec->VirtualAddress = pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].VirtualAddress + pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].SizeOfRawData;
	NewSec->PointerToRawData = pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].PointerToRawData + pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-2].SizeOfRawData;
	*OldBufferSize = NewLength;
}