DWORD CopyFileBufferToImageBuffer(PVOID pFileBuffer,PVOID pImageBuffer);		
DWORD CopyImageBufferToNewBuffer(PVOID pImageBuffer,PVOID* pNewBuffer);
void FileBufferToAddShellcode(PVOID pFileBuffer);
void AddNewSection(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer);
void ExpandSection(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer);
void printfPE(PVOID pFileBuffer);
void printfRELOCATION(PVOID pFileBuffer);
DWORD FOA_TO_RVA(PVOID FileAddress, DWORD FOA,PDWORD pRVA);
DWORD RVA_TO_FOA(PVOID FileAddress, DWORD RVA, PDWORD pFOA);
void MyReadFile(PVOID *pFileBuffer,PDWORD BufferLenth);
void MyWriteFile(PVOID pMemBuffer,DWORD BufferLenth);
int GetBufferLength(PVOID Buffer);

#define FILENAME "C:\\Documents and Settings\\Administrator\\����\\mydll.dll"
#define NEWFILENAME "C:\\Documents and Settings\\Administrator\\����\\NEW_ipmsg.exe"

int GetBufferLength(PVOID Buffer){
	int BufferLength;
	BufferLength = ftell(Buffer);
	return BufferLength;
}

//**************************************************************************
//ReadPEFile:���ļ���ȡ��������
//����˵��:			
//lpszFile �ļ�·��
//pFileBuffer ������ָ��
//����ֵ˵��:	
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

	//���°�Fileָ��ָ���ļ��Ŀ�ͷ
	fseek(File,0,SEEK_SET);

	//�����¿ռ�
	*pFileBuffer = (PVOID)malloc(*BufferLenth);

	//�ڴ�����
	memset(*pFileBuffer,0,*BufferLenth);

	//��ȡ���ڴ滺����
	fread(*pFileBuffer,*BufferLenth,1,File);// һ�ζ���*bufferlenth���ֽڣ��ظ�1��

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


//**************************************************************************								
//FOA_TO_RVA:FOA ת�� RVA							
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
		if (RVA >= pSectionGroup[i].VirtualAddress && RVA < pSectionGroup[i].VirtualAddress + pSectionGroup[i].PointerToRawData)
		{
			*pFOA =  RVA - pSectionGroup[i].VirtualAddress + pSectionGroup[i].PointerToRawData;
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
	free(pMemBuffer);


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


void ExpandSection(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer){

	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	PIMAGE_SECTION_HEADER NewSec = NULL;

	DWORD TheBiggerOfSizeOfRawDataOrVirtualSize = 0;
	DWORD NewLength=0;
	
	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);

	
	//���ɶ�Ӧ���ڴ��С�Ŀռ�
	NewLength += *OldBufferSize + 0x1000;
	*pNewBuffer = (PVOID)malloc(NewLength);
	ZeroMemory(*pNewBuffer,NewLength);
	
	//����֮ǰ�ڴ�ռ� �� ��ǰ�����ɵ��ڴ�ռ�
	memcpy(*pNewBuffer,pFileBuffer,*OldBufferSize);

	//�޸Ľ����ݵ�ƫ��
	if(pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1].Misc.VirtualSize > pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1].SizeOfRawData){
		TheBiggerOfSizeOfRawDataOrVirtualSize = pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1].Misc.VirtualSize;
	}else{
		TheBiggerOfSizeOfRawDataOrVirtualSize = pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1].SizeOfRawData;
	}

	pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1].Misc.VirtualSize = TheBiggerOfSizeOfRawDataOrVirtualSize;
	pImageSectionHeaderGroup[pImageFileHeader->NumberOfSections-1].SizeOfRawData = TheBiggerOfSizeOfRawDataOrVirtualSize;

	// pImageOptionalHeader->SizeOfImage�޸�
	pImageOptionalHeader->SizeOfImage = (DWORD)pImageOptionalHeader->SizeOfImage + 0x1000;

	*OldBufferSize = NewLength;
}


void printfPE(PVOID pFileBuffer){
    PIMAGE_DOS_HEADER pDosHeader = NULL;    
    PIMAGE_NT_HEADERS pNTHeader = NULL; 
    PIMAGE_FILE_HEADER pPEHeader = NULL;    
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;  
    PIMAGE_SECTION_HEADER pSectionHeader = NULL;

	PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;
	PVOID AddressOfNamesTable = NULL;
	DWORD AddressOfNameOrdinalsNumber = NULL;
	PVOID FunctionOfAddress = NULL;
	char FunName[10] = {0};
	int i,j;

	DWORD FOA;
	char SectionName[9] = {0};

    pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeader->e_lfanew);
    pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  
    pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER); 
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + IMAGE_SIZEOF_NT_OPTIONAL_HEADER);


    //�ж��Ƿ�����Ч��MZ��־��Ҳ����0x5A4D��ȡǰ�ĸ��ֽ�
    if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)    
    {   
        printf("������Ч��MZ��־\n");
        free(pFileBuffer);
        return ; 
    }   
	

    
    //��ӡDOSͷ    
    printf("********************DOSͷ********************\n\n"); 
    printf("_IMAGE_DOS_HEADERMZ->e_magic MZ��־��0x%x\n",pDosHeader->e_magic);
    printf("_IMAGE_DOS_HEADERMZ->e_lfanewָ��PE��־��0x%x\n",pDosHeader->e_lfanew);
    printf("\n");
	
    //�ж��Ƿ�����Ч��PE��־  
    if(*((PDWORD)((DWORD)pFileBuffer+pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)  
    {   
        printf("������Ч��PE��־\n");
        free(pFileBuffer);
        return ;
    }   
	
    
    //��ӡNTͷ 
    printf("********************NTͷ********************\n\n");  
    printf("_IMAGE_NT_HEADERS->Signature�ļ�PE��ʶ��0x%x\n",pNTHeader->Signature);
    printf("\n");
	

    printf("********************PEͷ********************\n\n");  
    printf("_IMAGE_FILE_HEADER->Machine֧�ֵ�CPU��0x%x\n",pPEHeader->Machine);
    printf("_IMAGE_FILE_HEADER->NumberOfSections�ڵ�������0x%x\n",pPEHeader->NumberOfSections);
    printf("_IMAGE_FILE_HEADER->SizeOfOptionalHeader��ѡPEͷ�Ĵ�С��0x%x\n",pPEHeader->SizeOfOptionalHeader);
    printf("\n");

	
    printf("********************OPTIOIN_PEͷ********************\n\n");  
    printf("_IMAGE_OPTIONAL_HEADER->Magic�ֱ�ϵͳλ��:0x%x\n",pOptionHeader->Magic);
    printf("_IMAGE_OPTIONAL_HEADER->AddressOfEntryPoint�������:0x%x\n",pOptionHeader->AddressOfEntryPoint);
    printf("_IMAGE_OPTIONAL_HEADER->ImageBase�ڴ澵���ַ:0x%x\n",pOptionHeader->ImageBase);
    printf("_IMAGE_OPTIONAL_HEADER->SectionAlignment�ڴ�����С:0x%x\n",pOptionHeader->SectionAlignment);
    printf("_IMAGE_OPTIONAL_HEADER->FileAlignment�ļ������С:0x%x\n",pOptionHeader->FileAlignment);
    printf("_IMAGE_OPTIONAL_HEADER->SizeOfImage�ڴ���PE�Ĵ�С(SectionAlignment������):0x%x\n",pOptionHeader->SizeOfImage);
    printf("_IMAGE_OPTIONAL_HEADER->SizeOfHeadersͷ+�ڱ����ļ�����Ĵ�С:0x%x\n",pOptionHeader->SizeOfImage);
    printf("_IMAGE_OPTIONAL_HEADER->NumberOfRvaAndSizesĿ¼����Ŀ:0x%x\n",pOptionHeader->NumberOfRvaAndSizes);
	
    printf("\n");
	
    //�ڱ�
    printf("********************�ڱ�********************\n\n");
    
    for(i=1;i<=pPEHeader->NumberOfSections;i++){
        char SectionName[9] ={0};
        strcpy(SectionName,(char *)pSectionHeader->Name);
        printf("_IMAGE_SECTION_HEADER->Name:%s\n",SectionName);
        printf("_IMAGE_SECTION_HEADER->VirtualSize:0x%x\n",pSectionHeader->Misc);
        printf("_IMAGE_SECTION_HEADER->VirtualAddress:0x%x\n",pSectionHeader->VirtualAddress);
        printf("_IMAGE_SECTION_HEADER->SizeOfRawData:0x%x\n",pSectionHeader->SizeOfRawData);
        printf("_IMAGE_SECTION_HEADER->PointerToRawData:0x%x\n",pSectionHeader->PointerToRawData);
        printf("_IMAGE_SECTION_HEADER->Characteristics:0x%x\n",pSectionHeader->Characteristics);
        pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pSectionHeader + IMAGE_SIZEOF_SECTION_HEADER);
        printf("\n");
    }


	RVA_TO_FOA(pFileBuffer,pOptionHeader->DataDirectory[0].VirtualAddress,&FOA);
	
	//������ĵ�ַ
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer + FOA);
	
	//Ŀ¼���е�16�ű�Ľ���
	
	//�Ƚ�����һ�ű�������
	printf("********************������********************\n\n");
	printf("������������ַ:%x\n",pOptionHeader->DataDirectory[0].VirtualAddress);
	printf("������Ĵ�С:%x\n",pOptionHeader->DataDirectory[0].Size);
	printf("_IMAGE_EXPORT_DIRECTORY->Characteristics: 0x%x\n",pExportDirectory->Characteristics);
	printf("_IMAGE_EXPORT_DIRECTORY->TimeDateStampʱ���: 0x%x\n",pExportDirectory->TimeDateStamp);
	printf("_IMAGE_EXPORT_DIRECTORY->MajorVersion: 0x%x\n",pExportDirectory->MajorVersion);
	printf("_IMAGE_EXPORT_DIRECTORY->MinorVersion: 0x%x\n",pExportDirectory->MinorVersion);
	printf("_IMAGE_EXPORT_DIRECTORY->Nameָ��õ������ļ����ַ���: 0x%x\n",pExportDirectory->Name);
	printf("_IMAGE_EXPORT_DIRECTORY->Base����������ʼ���: 0x%x\n",pExportDirectory->Base);
	printf("_IMAGE_EXPORT_DIRECTORY->NumberOfFunctions���е��������ĸ���: 0x%x\n",pExportDirectory->NumberOfFunctions);
	printf("_IMAGE_EXPORT_DIRECTORY->NumberOfNames�Ժ������ֵ����ĺ�������: 0x%x\n",pExportDirectory->NumberOfNames);
	printf("_IMAGE_EXPORT_DIRECTORY->RVA_AddressOfFunctions����������ַ��: 0x%x\n",pExportDirectory->AddressOfFunctions);
	printf("_IMAGE_EXPORT_DIRECTORY->RAV_AddressOfNames�����������Ʊ�: 0x%x\n",pExportDirectory->AddressOfNames);
	printf("_IMAGE_EXPORT_DIRECTORY->RVA_AddressOfNameOrdinals����������ű�: 0x%x\n",pExportDirectory->AddressOfNameOrdinals);	

	printf("\n");




	//1�������������Ʊ���Ѱ�ҵ���������ַ��AddressOfNames��һ��ָ�������Ƶ�RVA��ַ����Ҫ��ת��Ϊ �ļ�ƫ�Ƶ�ַ
	RVA_TO_FOA(pFileBuffer,pExportDirectory->AddressOfNames,&FOA);

	//printf("pExportDirectory->AddressOfNames�����������Ʊ�: 0x%x\n",FOA);

	//2���ټ���pFileBuffer��ת��Ϊ�ļ���ַ���õ��������ƴ洢�ĵط����׵�ַ����ǰ���׵�ַ��RVA��Ҳ��Ҫ����RVA -> FOAת��
	AddressOfNamesTable = (PVOID)(*(PDWORD)((DWORD)pFileBuffer+(DWORD)FOA)); 
	RVA_TO_FOA(pFileBuffer,(DWORD)AddressOfNamesTable,&FOA); // // �����������Ʊ��к������Ƶ�FOA

	//AddressOfNamesTable = (PVOID)FOA;
	AddressOfNamesTable = (PVOID)((DWORD)pFileBuffer + (DWORD)FOA); // ����pFileBufferλ�þ͵��������ĺ������Ʊ�ĵ�ַ
	printf("\n");
	
	//3���õ��������Ʊ���ļ���ַ��ÿ������������ ռ�ĸ��ֽڣ�Ȼ����б����ж�	
	for(j=0;j<pExportDirectory->NumberOfNames;j++){
		//(PDWORD)((DWORD)AddressOfNamesTable + 4*j);
		//��ȡ��ǰ�������Ʊ��еĺ������ƣ�Ȼ��ѭ���ж�
		strcpy(FunName,(PVOID)((DWORD)AddressOfNamesTable + (strlen(AddressOfNamesTable)+1)*j)); //����+1 �����һ���ֽ�Ϊ���ֽ� ��ô��Ϊ������
		if(0 == memcmp((PDWORD)((DWORD)AddressOfNamesTable + (DWORD)(strlen(AddressOfNamesTable)+1)*j),(PDWORD)FunName,strlen(FunName))){

			//AddressOfNamesTable = (DWORD)AddressOfNamesTable + (DWORD)(strlen(AddressOfNamesTable)+1)

		

			//4���ҵ���ű�AddressOfNameOrdinals�±�����Ӧ�ĵ�ֵ����ű���ÿ����Առ2�ֽ� word����
			RVA_TO_FOA(pFileBuffer,pExportDirectory->AddressOfNameOrdinals,&FOA);
			AddressOfNameOrdinalsNumber = *(PWORD)((DWORD)FOA + (DWORD)pFileBuffer + (DWORD)j*2);
			//5��ͨ����ű����±���õ�ֵȥ����������ַ��AddressOfFunctions��Ѱ�� ��ֵ�±��Ӧ��ֵ
			RVA_TO_FOA(pFileBuffer,pExportDirectory->AddressOfFunctions,&FOA);
			printf("�������: %d\t",AddressOfNameOrdinalsNumber);
			printf("��������Ϊ: %s\t",FunName);
			printf("����������ַ��ĵ�ַΪ��0x%.8x\n",*(PDWORD)(PVOID)((DWORD)FOA + (DWORD)pFileBuffer + AddressOfNameOrdinalsNumber*4));
		}
	}
	
	printf("\n");

	printf("********************�����********************\n\n");
	printf("�����������ַ:%x\n",pOptionHeader->DataDirectory[1].VirtualAddress);
	printf("�����Ĵ�С:%x\n",pOptionHeader->DataDirectory[1].Size);

	
	printf("\n");

	printf("********************��Դ��********************\n\n");
	printf("��Դ��������ַ:%x\n",pOptionHeader->DataDirectory[2].VirtualAddress);
	printf("��Դ��Ĵ�С:%x\n",pOptionHeader->DataDirectory[2].Size);
	printf("\n");

    //�ͷ��ڴ�  
    free(pFileBuffer);  
}


void printfRELOCATION(PVOID pFileBuffer){

	PIMAGE_DOS_HEADER pDosHeader = NULL;    
    PIMAGE_NT_HEADERS pNTHeader = NULL; 
    PIMAGE_FILE_HEADER pPEHeader = NULL;    
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;  
    PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_BASE_RELOCATION pRelocationDirectory = NULL;
	DWORD FOA;
	DWORD RealData;
	int NumberOfRelocation = 0;
	PVOID Location = NULL;
	int i;

    pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeader->e_lfanew);
    pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  
    pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER); 
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + IMAGE_SIZEOF_NT_OPTIONAL_HEADER);

	// _IMAGE_DATA_DIRECTORY�е�ָ���ض�λ��������ַת��ΪFOA��ַ
	printf("%x\n",pOptionHeader->DataDirectory[5].VirtualAddress);

	RVA_TO_FOA(pFileBuffer,pOptionHeader->DataDirectory[5].VirtualAddress,&FOA);

	pRelocationDirectory = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer+FOA);
	printf("%x",pRelocationDirectory->SizeOfBlock);
	while(pRelocationDirectory->SizeOfBlock && pRelocationDirectory->VirtualAddress){
		NumberOfRelocation = (pRelocationDirectory->SizeOfBlock - 8)/2;
		Location = (DWORD)pRelocationDirectory + 8;
		for(i=0;i<NumberOfRelocation;i++){
			if((DWORD)Location >> 12 == 0){
				continue;
			}
			Location = (PVOID)((DWORD)Location << 4);
			RealData = (DWORD)pRelocationDirectory->VirtualAddress + (DWORD)Location; 
			RVA_TO_FOA(pFileBuffer,RealData,&FOA);
			printf("��%d�� ��Ҫ�޸��ĵ�ַ��RVA:0x%x  �ض�λ��ĵ�ַ:0x%x",NumberOfRelocation,(DWORD)pFileBuffer + (DWORD)Location,RealData);
			Location = (DWORD)Location + 4;
		}
		pRelocationDirectory = (PIMAGE_BASE_RELOCATION)((DWORD)pRelocationDirectory + (DWORD)pRelocationDirectory->SizeOfBlock);
	}
}