#include <sdw.h>

int importStr(u32 a_uIndex, wstring& a_sTxt, wstring& a_sOuter, wstring& a_sInner, U16String& a_sStmt16)
{
	static wstring::size_type uOuterPos0 = 0;
	static wstring::size_type uPos0 = 0;
	static wstring::size_type uInnerPos0 = 0;
	wstring sOuterNum = Format(L"No.%d,", a_uIndex);
	uOuterPos0 = a_sOuter.find(sOuterNum, uOuterPos0);
	if (uOuterPos0 == wstring::npos)
	{
		return 1;
	}
	uOuterPos0 += sOuterNum.size();
	n32 nCount = SToN32(a_sOuter.c_str() + uOuterPos0);
	uOuterPos0 = a_sOuter.find(L"======================================\r\n", uOuterPos0);
	if (uOuterPos0 == wstring::npos)
	{
		return 1;
	}
	uOuterPos0 += wcslen(L"======================================\r\n");
	wstring::size_type uOuterPos1 = a_sOuter.find(L"\r\n--------------------------------------", uOuterPos0);
	if (uOuterPos1 == wstring::npos)
	{
		return 1;
	}
	wstring sOuterStmt = a_sOuter.substr(uOuterPos0, uOuterPos1 - uOuterPos0);
	uOuterPos0 = uOuterPos1 + wcslen(L"\r\n--------------------------------------");
	wstring::size_type uOuterPlaceHolderPos = 0;
	for (n32 i = 0; i < nCount; i++)
	{
		wstring sOuterPlaceHolder = Format(L"{%d}", i);
		uOuterPlaceHolderPos = sOuterStmt.find(sOuterPlaceHolder, uOuterPlaceHolderPos);
		if (uOuterPlaceHolderPos == wstring::npos)
		{
			return 1;
		}
		wstring sNum = Format(L"No.%d,%d,", a_uIndex, i);
		uPos0 = a_sTxt.find(sNum, uPos0);
		if (uPos0 == wstring::npos)
		{
			return 1;
		}
		uPos0 += sNum.size();
		n32 nInnerCount = SToN32(a_sTxt.c_str() + uPos0);
		uPos0 = a_sTxt.find(L"======================================\r\n", uPos0);
		if (uPos0 == wstring::npos)
		{
			return 1;
		}
		uPos0 += wcslen(L"======================================\r\n");
		wstring::size_type uPos1 = a_sTxt.find(L"\r\n--------------------------------------", uPos0);
		if (uPos1 == wstring::npos)
		{
			return 1;
		}
		wstring sStmt = a_sTxt.substr(uPos0, uPos1 - uPos0);
		uPos0 = uPos1 + wcslen(L"\r\n--------------------------------------");
		for (n32 j = 0; j < nInnerCount; j++)
		{
			wstring sPlaceHolder = Format(L"{%d}", j);
			wstring::size_type uPlaceHolderPos = sStmt.find(sPlaceHolder);
			if (uPlaceHolderPos == wstring::npos)
			{
				return 1;
			}
			wstring sInnerNum = Format(L"No.%d,%d,%d\r\n", a_uIndex, i, j);
			uInnerPos0 = a_sInner.find(sInnerNum, uInnerPos0);
			if (uInnerPos0 == wstring::npos)
			{
				return 1;
			}
			uInnerPos0 += sInnerNum.size();
			uInnerPos0 = a_sInner.find(L"======================================\r\n", uInnerPos0);
			if (uInnerPos0 == wstring::npos)
			{
				return 1;
			}
			uInnerPos0 += wcslen(L"======================================\r\n");
			wstring::size_type uInnerPos1 = a_sInner.find(L"\r\n--------------------------------------", uInnerPos0);
			if (uInnerPos1 == wstring::npos)
			{
				return 1;
			}
			wstring sInnerStmt = a_sInner.substr(uInnerPos0, uInnerPos1 - uInnerPos0);
			uInnerPos0 = uInnerPos1 + wcslen(L"\r\n--------------------------------------");
			sStmt.replace(uPlaceHolderPos, sPlaceHolder.size(), sInnerStmt);
		}
		sOuterStmt.replace(uOuterPlaceHolderPos, sOuterPlaceHolder.size(), sStmt);
		uOuterPlaceHolderPos += sStmt.size();
	}
	static wregex rABIL(L"<ABIL(\\d{4})>", regex_constants::ECMAScript);
	if (regex_search(sOuterStmt, rABIL))
	{
		sOuterStmt = regex_replace(sOuterStmt, rABIL, wstring(L"@ABIL$1"));
	}
	if (sOuterStmt.find(L"<ABIL") != wstring::npos)
	{
		return 1;
	}
	static wregex rCOL(L"<COL(\\d)>", regex_constants::ECMAScript);
	if (regex_search(sOuterStmt, rCOL))
	{
		sOuterStmt = regex_replace(sOuterStmt, rCOL, wstring(L"@COL$1"));
	}
	if (sOuterStmt.find(L"<COL") != wstring::npos)
	{
		return 1;
	}
	sOuterStmt = Replace(sOuterStmt, L"</COL>", L"@COLD");
	sOuterStmt = Replace(sOuterStmt, L"<CECN>", L"@CECN");
	static wregex rCHAR(L"<CHAR(\\[[0-9A-Z]{5}\\])>", regex_constants::ECMAScript);
	if (regex_search(sOuterStmt, rCHAR))
	{
		sOuterStmt = regex_replace(sOuterStmt, rCHAR, wstring(L"@CHAR$1"));
	}
	if (sOuterStmt.find(L"<CHAR") != wstring::npos)
	{
		return 1;
	}
	sOuterStmt = Replace(sOuterStmt, L"<ITEM%05d>", L"@ITEM%05d");
	sOuterStmt = Replace(sOuterStmt, L"<ITEM2424>", L"@ITEM2424");
	if (sOuterStmt.find(L"<ITEM") != wstring::npos)
	{
		return 1;
	}
	sOuterStmt = Replace(sOuterStmt, L"<MBIG>", L"@MBIG");
	static wregex rSKIL(L"<SKIL(\\d{4})>", regex_constants::ECMAScript);
	if (regex_search(sOuterStmt, rSKIL))
	{
		sOuterStmt = regex_replace(sOuterStmt, rSKIL, wstring(L"@SKIL$1"));
	}
	if (sOuterStmt.find(L"<SKIL") != wstring::npos)
	{
		return 1;
	}
	sOuterStmt = Replace(sOuterStmt, L"<WSHK>", L"@WSHK");
	sOuterStmt = Replace(sOuterStmt, L"\r\n", L"\n");
	sOuterStmt = Replace(sOuterStmt, L"<r>", L"\r");
	sOuterStmt = Replace(sOuterStmt, L"[======================================]", L"======================================");
	sOuterStmt = Replace(sOuterStmt, L"[--------------------------------------]", L"--------------------------------------");
	sOuterStmt = Replace(sOuterStmt, L"[No].", L"No.");
	a_sStmt16 = WToU16(sOuterStmt);
	for (u32 i = 0; i < static_cast<u32>(a_sStmt16.size()); i++)
	{
		if (a_sStmt16[i] == '<' && i + 2 < static_cast<u32>(a_sStmt16.size()) && a_sStmt16[i + 1] == 'p' && a_sStmt16[i + 2] == '>')
		{
			a_sStmt16[i] = 0;
			a_sStmt16.erase(i + 1, 2);
		}
	}
	return 0;
}

int UMain(int argc, UChar* argv[])
{
	if (argc != 5)
	{
		return 1;
	}
	FILE* fp = UFopen(argv[2], USTR("rb"), false);
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
	fp = UFopen(argv[3], USTR("rb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	uTxtSize = ftell(fp);
	if (uTxtSize % 2 != 0)
	{
		fclose(fp);
		return 1;
	}
	uTxtSize /= 2;
	fseek(fp, 0, SEEK_SET);
	pTemp = new Char16_t[uTxtSize + 1];
	fread(pTemp, 2, uTxtSize, fp);
	fclose(fp);
	if (pTemp[0] != 0xFEFF)
	{
		delete[] pTemp;
		return 1;
	}
	pTemp[uTxtSize] = 0;
	wstring sOuter = U16ToW(pTemp + 1);
	delete[] pTemp;
	fp = UFopen(argv[4], USTR("rb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	uTxtSize = ftell(fp);
	if (uTxtSize % 2 != 0)
	{
		fclose(fp);
		return 1;
	}
	uTxtSize /= 2;
	fseek(fp, 0, SEEK_SET);
	pTemp = new Char16_t[uTxtSize + 1];
	fread(pTemp, 2, uTxtSize, fp);
	fclose(fp);
	if (pTemp[0] != 0xFEFF)
	{
		delete[] pTemp;
		return 1;
	}
	pTemp[uTxtSize] = 0;
	wstring sInner = U16ToW(pTemp + 1);
	delete[] pTemp;
	U16String sTxt16;
	if (importStr(0, sTxt, sOuter, sInner, sTxt16) != 0)
	{
		return 1;
	}
	fp = UFopen(argv[1], USTR("wb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fwrite(sTxt16.c_str(), 2, sTxt16.size(), fp);
	fclose(fp);
	return 0;
}
