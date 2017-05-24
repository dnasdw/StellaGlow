#include <sdw.h>

int UMain(int argc, UChar* argv[])
{
	if (argc != 3)
	{
		return 1;
	}
	FILE* fp = UFopen(argv[1], USTR("rb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uStrSize = ftell(fp);
	if (uStrSize % 2 != 0)
	{
		fclose(fp);
		return 1;
	}
	uStrSize /= 2;
	fseek(fp, 0, SEEK_SET);
	Char16_t* pStr = new Char16_t[uStrSize];
	fread(pStr, 2, uStrSize, fp);
	fclose(fp);
	if (pStr[uStrSize - 1] != 0)
	{
		delete[] pStr;
		return 1;
	}
	u32 uCount = 0;
	for (u32 i = 0; i < uStrSize; uCount++, i++)
	{
		Char16_t* pTemp = pStr + i;
		for (n32 j = 0; pTemp[j] != 0; j++)
		{
			i++;
		}
	}
	delete[] pStr;
	fp = UFopen(argv[2], USTR("rb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uTxtSize = ftell(fp);
	if (uTxtSize % 2 != 0)
	{
		fclose(fp);
		return 1;
	}
	uTxtSize /= 2;
	fseek(fp, 0, SEEK_SET);
	Char16_t* pTemp = new Char16_t[uTxtSize + 1];
	fread(pTemp, 2, uTxtSize, fp);
	fclose(fp);
	if (pTemp[0] != 0xFEFF)
	{
		delete[] pTemp;
		return 1;
	}
	pTemp[uTxtSize] = 0;
	wstring sTxt = U16ToW(pTemp + 1);
	delete[] pTemp;
	fp = UFopen(argv[1], USTR("wb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	wstring::size_type uPos0 = 0;
	for (u32 i = 0; i < uCount; i++)
	{
		wstring sNum = Format(L"No.%d\r\n", i);
		uPos0 = sTxt.find(sNum, uPos0);
		if (uPos0 == wstring::npos)
		{
			fclose(fp);
			return 1;
		}
		uPos0 += sNum.size();
		uPos0 = sTxt.find(L"======================================\r\n", uPos0);
		if (uPos0 == wstring::npos)
		{
			fclose(fp);
			return 1;
		}
		uPos0 += wcslen(L"======================================\r\n");
		wstring::size_type uPos1 = sTxt.find(L"\r\n--------------------------------------", uPos0);
		if (uPos1 == wstring::npos)
		{
			fclose(fp);
			return 1;
		}
		wstring sStmt = sTxt.substr(uPos0, uPos1 - uPos0);
		uPos0 = uPos1 + wcslen(L"\r\n--------------------------------------");
		sStmt = Replace(sStmt, L"\r\n", L"\n");
		sStmt = Replace(sStmt, L"<r>", L"\r");
		sStmt = Replace(sStmt, L"[======================================]", L"======================================");
		sStmt = Replace(sStmt, L"[--------------------------------------]", L"--------------------------------------");
		sStmt = Replace(sStmt, L"[No].", L"No.");
		U16String sStmt16 = WToU16(sStmt);
		fwrite(sStmt16.c_str(), 2, sStmt16.size() + 1, fp);
	}
	fclose(fp);
	return 0;
}
