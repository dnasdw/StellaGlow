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
	map<u32, wstring> mTxt;
	for (u32 uIndex = 0, i = 0; i < uStrSize; uIndex++, i++)
	{
		Char16_t* pTemp = pStr + i;
		mTxt.insert(make_pair(uIndex, U16ToW(pTemp)));
		for (n32 j = 0; pTemp[j] != 0; j++)
		{
			i++;
		}
	}
	delete[] pStr;
	fp = UFopen(argv[2], USTR("wb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fwrite("\xFF\xFE", 2, 1, fp);
	for (u32 i = 0; i < static_cast<u32>(mTxt.size()); i++)
	{
		wstring& sTxt = mTxt[i];
		wstring::size_type uPos0 = 0;
		uPos0 = sTxt.find(L"[No].");
		if (uPos0 != wstring::npos)
		{
			fclose(fp);
			return 1;
		}
		uPos0 = sTxt.find(L"[--------------------------------------]");
		if (uPos0 != wstring::npos)
		{
			fclose(fp);
			return 1;
		}
		uPos0 = sTxt.find(L"[======================================]");
		if (uPos0 != wstring::npos)
		{
			fclose(fp);
			return 1;
		}
		uPos0 = sTxt.find(L"<r>");
		if (uPos0 != wstring::npos)
		{
			fclose(fp);
			return 1;
		}
		sTxt = Replace(sTxt, L"No.", L"[No].");
		sTxt = Replace(sTxt, L"--------------------------------------", L"[--------------------------------------]");
		sTxt = Replace(sTxt, L"======================================", L"[======================================]");
		sTxt = Replace(sTxt, L'\r', L"<r>");
		sTxt = Replace(sTxt, L'\n', L"\r\n");
		if (ftell(fp) != 2)
		{
			fu16printf(fp, L"\r\n\r\n");
		}
		fu16printf(fp, L"No.%d\r\n", i);
		fu16printf(fp, L"--------------------------------------\r\n");
		fu16printf(fp, L"%ls\r\n", sTxt.c_str());
		fu16printf(fp, L"======================================\r\n");
		fu16printf(fp, L"%ls\r\n", sTxt.c_str());
		fu16printf(fp, L"--------------------------------------\r\n");
	}
	fclose(fp);
	return 0;
}
