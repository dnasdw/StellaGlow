#include <sdw.h>

n32 getTextIndex(const wstring& a_sText)
{
	n32 nIndex = 0;
	n32 nTextSize = static_cast<n32>(a_sText.size());
	for (; nIndex < nTextSize; nIndex++)
	{
		if (a_sText[nIndex] != L'<')
		{
			break;
		}
		else
		{
			if (nIndex + 2 >= nTextSize)
			{
				UPrintf(USTR("%") PRIUS USTR("\n"), WToU(a_sText.c_str() + nIndex).c_str());
				break;
			}
			bool bVisible = false;
			if (a_sText[nIndex + 1] == L'/')
			{
				if (nIndex + 3 >= nTextSize)
				{
					UPrintf(USTR("%") PRIUS USTR("\n"), WToU(a_sText.c_str() + nIndex).c_str());
					break;
				}
				wstring::size_type uPos = a_sText.find(L'>', nIndex);
				if (uPos == wstring::npos)
				{
					UPrintf(USTR("%") PRIUS USTR("\n"), WToU(a_sText.c_str() + nIndex).c_str());
					break;
				}
				bVisible = true;
			}
			else
			{
				wstring::size_type uPos = a_sText.find(L'>', nIndex);
				if (uPos == wstring::npos)
				{
					UPrintf(USTR("%") PRIUS USTR("\n"), WToU(a_sText.c_str() + nIndex).c_str());
					break;
				}
				if (true
					&& !StartWith(a_sText, L"<p>", nIndex)
					&& !StartWith(a_sText, L"<MBIG>", nIndex)
					&& !StartWith(a_sText, L"<WSHK>", nIndex)
					)
				{
					bVisible = true;
				}
				else
				{
					nIndex = static_cast<n32>(uPos);
				}
			}
			if (bVisible)
			{
				break;
			}
		}
	}
	return nIndex;
}

n32 getInnerTextIndex(const wstring& a_sText)
{
	n32 nIndex = 0;
	n32 nTextSize = static_cast<n32>(a_sText.size());
	for (; nIndex < nTextSize; nIndex++)
	{
		if (a_sText[nIndex] != L'<')
		{
			break;
		}
		else
		{
			if (nIndex + 2 >= nTextSize)
			{
				UPrintf(USTR("%") PRIUS USTR("\n"), WToU(a_sText.c_str() + nIndex).c_str());
				break;
			}
			bool bVisible = false;
			if (a_sText[nIndex + 1] == L'/')
			{
				if (nIndex + 3 >= nTextSize)
				{
					UPrintf(USTR("%") PRIUS USTR("\n"), WToU(a_sText.c_str() + nIndex).c_str());
					break;
				}
				wstring::size_type uPos = a_sText.find(L'>', nIndex);
				if (uPos == wstring::npos)
				{
					UPrintf(USTR("%") PRIUS USTR("\n"), WToU(a_sText.c_str() + nIndex).c_str());
					break;
				}
				bVisible = true;
			}
			else
			{
				wstring::size_type uPos = a_sText.find(L'>', nIndex);
				if (uPos == wstring::npos)
				{
					UPrintf(USTR("%") PRIUS USTR("\n"), WToU(a_sText.c_str() + nIndex).c_str());
					break;
				}
				bVisible = true;
			}
			if (bVisible)
			{
				break;
			}
		}
	}
	return nIndex;
}

wstring getMainText(const wstring& a_sText, wstring& a_sPrefix, wstring& a_sSuffix)
{
	wstring sText;
	a_sPrefix.clear();
	a_sSuffix.clear();
	if (a_sText.empty())
	{
		return sText;
	}
	vector<wstring> vStmt = RegexSplitWith(a_sText, L"<.*?>");
	n32 nPrefixIndex = 0;
	for (; nPrefixIndex < static_cast<n32>(vStmt.size()); nPrefixIndex++)
	{
		wstring& sStmt = vStmt[nPrefixIndex];
		n32 nTextIndex = getTextIndex(sStmt);
		if (nTextIndex != sStmt.size())
		{
			for (n32 i = 0; i < nPrefixIndex; i++)
			{
				a_sPrefix += vStmt[i];
			}
			a_sPrefix += sStmt.substr(0, nTextIndex);
			sText += sStmt.substr(nTextIndex);
			nPrefixIndex++;
			break;
		}
		else if (nPrefixIndex == static_cast<n32>(vStmt.size()) - 1)
		{
			a_sPrefix = a_sText;
		}
	}
	n32 nSuffixIndex = static_cast<n32>(vStmt.size()) - 1;
	for (; nSuffixIndex >= nPrefixIndex; nSuffixIndex--)
	{
		wstring& sStmt = vStmt[nSuffixIndex];
		n32 nTextIndex = getTextIndex(sStmt);
		if (nTextIndex != sStmt.size())
		{
			for (n32 i = nSuffixIndex + 1; i < static_cast<n32>(vStmt.size()); i++)
			{
				a_sSuffix += vStmt[i];
			}
			break;
		}
		else if (nSuffixIndex == nPrefixIndex)
		{
			for (n32 i = nSuffixIndex; i < static_cast<n32>(vStmt.size()); i++)
			{
				a_sSuffix += vStmt[i];
			}
		}
	}
	for (n32 i = nPrefixIndex; i <= nSuffixIndex; i++)
	{
		sText += vStmt[i];
	}
	return sText;
}

int exportStr(u32 a_uIndex, wstring& a_sTxt, FILE* a_fp, FILE* a_fpOuter, FILE* a_fpInner)
{
	wstring::size_type uPos0 = a_sTxt.find(L"[No].");
	if (uPos0 != wstring::npos)
	{
		return 1;
	}
	uPos0 = a_sTxt.find(L"[--------------------------------------]");
	if (uPos0 != wstring::npos)
	{
		return 1;
	}
	uPos0 = a_sTxt.find(L"[======================================]");
	if (uPos0 != wstring::npos)
	{
		return 1;
	}
	uPos0 = a_sTxt.find(L"<r>");
	if (uPos0 != wstring::npos)
	{
		return 1;
	}
	uPos0 = a_sTxt.find(L"<ABIL");
	if (uPos0 != wstring::npos)
	{
		return 1;
	}
	uPos0 = a_sTxt.find(L"<COL");
	if (uPos0 != wstring::npos)
	{
		return 1;
	}
	uPos0 = a_sTxt.find(L"</COL>");
	if (uPos0 != wstring::npos)
	{
		return 1;
	}
	uPos0 = a_sTxt.find(L"<CECN>");
	if (uPos0 != wstring::npos)
	{
		return 1;
	}
	uPos0 = a_sTxt.find(L"<CHAR");
	if (uPos0 != wstring::npos)
	{
		return 1;
	}
	uPos0 = a_sTxt.find(L"<ITEM");
	if (uPos0 != wstring::npos)
	{
		return 1;
	}
	uPos0 = a_sTxt.find(L"<MBIG>");
	if (uPos0 != wstring::npos)
	{
		return 1;
	}
	uPos0 = a_sTxt.find(L"<SKIL");
	if (uPos0 != wstring::npos)
	{
		return 1;
	}
	uPos0 = a_sTxt.find(L"<WSHK>");
	if (uPos0 != wstring::npos)
	{
		return 1;
	}
	a_sTxt = Replace(a_sTxt, L"No.", L"[No].");
	a_sTxt = Replace(a_sTxt, L"--------------------------------------", L"[--------------------------------------]");
	a_sTxt = Replace(a_sTxt, L"======================================", L"[======================================]");
	a_sTxt = Replace(a_sTxt, L'\r', L"<r>");
	a_sTxt = Replace(a_sTxt, L'\n', L"\r\n");
	static wregex rABIL(L"@ABIL(\\d{4})", regex_constants::ECMAScript);
	if (regex_search(a_sTxt, rABIL))
	{
		a_sTxt = regex_replace(a_sTxt, rABIL, L"<ABIL$1>");
	}
	static wregex rCOL(L"@COL(\\d)", regex_constants::ECMAScript);
	if (regex_search(a_sTxt, rCOL))
	{
		a_sTxt = regex_replace(a_sTxt, rCOL, L"<COL$1>");
	}
	a_sTxt = Replace(a_sTxt, L"@COLD", L"</COL>");
	a_sTxt = Replace(a_sTxt, L"@CECN", L"<CECN>");
	static wregex rCHAR(L"@CHAR(\\[[0-9A-Z]{5}\\])", regex_constants::ECMAScript);
	if (regex_search(a_sTxt, rCHAR))
	{
		a_sTxt = regex_replace(a_sTxt, rCHAR, L"<CHAR$1>");
	}
	a_sTxt = Replace(a_sTxt, L"@ITEM%05d", L"<ITEM%05d>");
	a_sTxt = Replace(a_sTxt, L"@ITEM2424", L"<ITEM2424>");
	a_sTxt = Replace(a_sTxt, L"@MBIG", L"<MBIG>");
	static wregex rSKIL(L"@SKIL(\\d{4})", regex_constants::ECMAScript);
	if (regex_search(a_sTxt, rSKIL))
	{
		a_sTxt = regex_replace(a_sTxt, rSKIL, L"<SKIL$1>");
	}
	a_sTxt = Replace(a_sTxt, L"@WSHK", L"<WSHK>");
	vector<wstring> vStmt = RegexSplitWith(a_sTxt, L"<p>");
	wstring sOuter;
	n32 nIndex = 0;
	for (n32 i = 0; i < static_cast<n32>(vStmt.size()); i++)
	{
		wstring& sStmt = vStmt[i];
		wstring sPrefix;
		wstring sSuffix;
		wstring sMainText = getMainText(sStmt, sPrefix, sSuffix);
		if (sMainText.empty())
		{
			sOuter += sPrefix;
			continue;
		}
		sOuter += Format(L"%ls{%d}%ls", sPrefix.c_str(), nIndex, sSuffix.c_str());
		vector<wstring> vInnerStmt = RegexSplitWith(sMainText, L"<.*?>");
		sMainText.clear();
		n32 nInnerIndex = 0;
		for (n32 k = 0; k < static_cast<n32>(vInnerStmt.size()); k++)
		{
			wstring& sInnerStmt = vInnerStmt[k];
			n32 nInnerTextIndex = getInnerTextIndex(sInnerStmt);
			if (nInnerTextIndex == 0)
			{
				sMainText += sInnerStmt;
			}
			else if (nInnerTextIndex != sInnerStmt.size())
			{
				wstring sInner = sInnerStmt.substr(0, nInnerTextIndex);
				if (ftell(a_fpInner) != 2)
				{
					fu16printf(a_fpInner, L"\r\n\r\n");
				}
				fu16printf(a_fpInner, L"No.%d,%d,%d\r\n", a_uIndex, nIndex, nInnerIndex);
				fu16printf(a_fpInner, L"--------------------------------------\r\n");
				fu16printf(a_fpInner, L"%ls\r\n", sInner.c_str());
				fu16printf(a_fpInner, L"======================================\r\n");
				fu16printf(a_fpInner, L"%ls\r\n", sInner.c_str());
				fu16printf(a_fpInner, L"--------------------------------------\r\n");
				sMainText += Format(L"{%d}%ls", nInnerIndex, sInnerStmt.substr(nInnerTextIndex).c_str());
				nInnerIndex++;
			}
			else
			{
				if (k == static_cast<n32>(vInnerStmt.size()) - 1)
				{
					if (ftell(a_fpInner) != 2)
					{
						fu16printf(a_fpInner, L"\r\n\r\n");
					}
					fu16printf(a_fpInner, L"No.%d,%d,%d\r\n", a_uIndex, nIndex, nInnerIndex);
					fu16printf(a_fpInner, L"--------------------------------------\r\n");
					fu16printf(a_fpInner, L"%ls\r\n", sInnerStmt.c_str());
					fu16printf(a_fpInner, L"======================================\r\n");
					fu16printf(a_fpInner, L"%ls\r\n", sInnerStmt.c_str());
					fu16printf(a_fpInner, L"--------------------------------------\r\n");
					sMainText += Format(L"{%d}", nInnerIndex);
					nInnerIndex++;
				}
				else
				{
					vInnerStmt[k + 1] = sInnerStmt + vInnerStmt[k + 1];
					sInnerStmt.clear();
				}
			}
		}
		if (ftell(a_fp) != 2)
		{
			fu16printf(a_fp, L"\r\n\r\n");
		}
		fu16printf(a_fp, L"No.%d,%d,%d\r\n", a_uIndex, nIndex, nInnerIndex);
		fu16printf(a_fp, L"--------------------------------------\r\n");
		fu16printf(a_fp, L"%ls\r\n", sMainText.c_str());
		fu16printf(a_fp, L"======================================\r\n");
		fu16printf(a_fp, L"%ls\r\n", sMainText.c_str());
		fu16printf(a_fp, L"--------------------------------------\r\n");
		nIndex++;
	}
	if (ftell(a_fpOuter) != 2)
	{
		fu16printf(a_fpOuter, L"\r\n\r\n");
	}
	fu16printf(a_fpOuter, L"No.%d,%d\r\n", a_uIndex, nIndex);
	fu16printf(a_fpOuter, L"--------------------------------------\r\n");
	fu16printf(a_fpOuter, L"%ls\r\n", a_sTxt.c_str());
	fu16printf(a_fpOuter, L"======================================\r\n");
	fu16printf(a_fpOuter, L"%ls\r\n", sOuter.c_str());
	fu16printf(a_fpOuter, L"--------------------------------------\r\n");
	return 0;
}

int UMain(int argc, UChar* argv[])
{
	if (argc != 5)
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
	if (uStrSize == 0)
	{
		delete[] pStr;
		return 0;
	}
	if (pStr[uStrSize - 1] != 0)
	{
		delete[] pStr;
		return 1;
	}
	fp = UFopen(argv[2], USTR("wb"), false);
	if (fp == nullptr)
	{
		delete[] pStr;
		return 1;
	}
	fwrite("\xFF\xFE", 2, 1, fp);
	FILE* fpOuter = UFopen(argv[3], USTR("wb"), false);
	if (fpOuter == nullptr)
	{
		fclose(fp);
		delete[] pStr;
		return 1;
	}
	fwrite("\xFF\xFE", 2, 1, fpOuter);
	FILE* fpInner = UFopen(argv[4], USTR("wb"), false);
	if (fpInner == nullptr)
	{
		fclose(fpOuter);
		fclose(fp);
		delete[] pStr;
		return 1;
	}
	fwrite("\xFF\xFE", 2, 1, fpInner);
	U16String sTxt16;
	for (u32 j = 0; j < uStrSize; j++)
	{
		if (pStr[j] == '<' && j + 2 < uStrSize && pStr[j + 1] == 'p' && pStr[j + 2] == '>')
		{
			fclose(fpInner);
			fclose(fpOuter);
			fclose(fp);
			delete[] pStr;
			return 1;
		}
		if (pStr[j] == 0)
		{
			sTxt16.append(WToU16(L"<p>"));
		}
		else
		{
			sTxt16.append(1, pStr[j]);
		}
	}
	wstring sTxt = U16ToW(sTxt16);
	if (exportStr(0, sTxt, fp, fpOuter, fpInner) != 0)
	{
		fclose(fpInner);
		fclose(fpOuter);
		fclose(fp);
		delete[] pStr;
		return 1;
	}
	fclose(fpInner);
	fclose(fpOuter);
	fclose(fp);
	delete[] pStr;
	return 0;
}
