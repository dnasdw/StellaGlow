#include <sdw.h>

#include SDW_MSC_PUSH_PACKED
struct SFlwHeader
{
	u32 Signature;
	u32 Unknown0x4;
	u32 Count;
	u32 Unknown0xC;
	u32 Unknown0x10;
	u32 Unknown0x14;
	u32 Data0Offset;
	u32 Unknown0Offset;
	u32 Unknown1Offset;
	u32 Unknown2Offset;
} SDW_GNUC_PACKED;

struct SData0
{
	u32 Unknown0x0;
	u32 Unknown0x4;
	u32 Data1Offset;
} SDW_GNUC_PACKED;

struct SData1
{
	u32 Data[1];
} SDW_GNUC_PACKED;

struct SUnknown0
{
	u32 Count;
	u32 Offset;
} SDW_GNUC_PACKED;
#include SDW_MSC_POP_PACKED

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
	a_sTxt = Replace(a_sTxt, L"\r\n", L"\n");
	a_sTxt = Replace(a_sTxt, L'\r', L"<r>");
	a_sTxt = Replace(a_sTxt, L'\n', L"\r\n");
	static wregex rABIL(L"@ABIL(\\d{4})", regex_constants::ECMAScript);
	if (regex_search(a_sTxt, rABIL))
	{
		a_sTxt = regex_replace(a_sTxt, rABIL, wstring(L"<ABIL$1>"));
	}
	if (a_sTxt.find(L"@ABIL") != wstring::npos)
	{
		return 1;
	}
	static wregex rCOL(L"@COL(\\d)", regex_constants::ECMAScript);
	if (regex_search(a_sTxt, rCOL))
	{
		a_sTxt = regex_replace(a_sTxt, rCOL, wstring(L"<COL$1>"));
	}
	a_sTxt = Replace(a_sTxt, L"@COLD", L"</COL>");
	if (a_sTxt.find(L"@COL") != wstring::npos)
	{
		return 1;
	}
	a_sTxt = Replace(a_sTxt, L"@CECN", L"<CECN>");
	static wregex rCHAR(L"@CHAR(\\[[0-9A-Z]{5}\\])", regex_constants::ECMAScript);
	if (regex_search(a_sTxt, rCHAR))
	{
		a_sTxt = regex_replace(a_sTxt, rCHAR, wstring(L"<CHAR$1>"));
	}
	if (a_sTxt.find(L"@CHAR") != wstring::npos)
	{
		return 1;
	}
	a_sTxt = Replace(a_sTxt, L"@ITEM%05d", L"<ITEM%05d>");
	a_sTxt = Replace(a_sTxt, L"@ITEM2424", L"<ITEM2424>");
	if (a_sTxt.find(L"@ITEM") != wstring::npos)
	{
		return 1;
	}
	a_sTxt = Replace(a_sTxt, L"@MBIG", L"<MBIG>");
	static wregex rSKIL(L"@SKIL(\\d{4})", regex_constants::ECMAScript);
	if (regex_search(a_sTxt, rSKIL))
	{
		a_sTxt = regex_replace(a_sTxt, rSKIL, wstring(L"<SKIL$1>"));
	}
	if (a_sTxt.find(L"@SKIL") != wstring::npos)
	{
		return 1;
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
	u32 uFlwSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	u8* pFlw = new u8[uFlwSize];
	fread(pFlw, 1, uFlwSize, fp);
	fclose(fp);
	SFlwHeader* pFlwHeader = reinterpret_cast<SFlwHeader*>(pFlw);
	if (pFlwHeader->Signature != SDW_CONVERT_ENDIAN32('FLW\0'))
	{
		delete[] pFlw;
		return 1;
	}
	if (pFlwHeader->Unknown0x4 != 0)
	{
		delete[] pFlw;
		return 1;
	}
	if (pFlwHeader->Unknown0xC != 0)
	{
		delete[] pFlw;
		return 1;
	}
	if (pFlwHeader->Unknown0x10 != 0 && pFlwHeader->Unknown0x10 != 1)
	{
		delete[] pFlw;
		return 1;
	}
	if (pFlwHeader->Unknown0x10 == 0)
	{
		if (pFlwHeader->Unknown2Offset != 0)
		{
			delete[] pFlw;
			return 1;
		}
	}
	else
	{
		if (pFlwHeader->Unknown2Offset == 0)
		{
			delete[] pFlw;
			return 1;
		}
	}
	if (pFlwHeader->Unknown0x14 != 0)
	{
		delete[] pFlw;
		return 1;
	}
	u32 uOffset = sizeof(SFlwHeader);
	if (pFlwHeader->Data0Offset != uOffset - (reinterpret_cast<u8*>(&pFlwHeader->Data0Offset) - reinterpret_cast<u8*>(pFlwHeader)))
	{
		delete[] pFlw;
		return 1;
	}
	uOffset += pFlwHeader->Count * sizeof(SData0);
	if (pFlwHeader->Unknown0Offset != uOffset - (reinterpret_cast<u8*>(&pFlwHeader->Unknown0Offset) - reinterpret_cast<u8*>(pFlwHeader)))
	{
		delete[] pFlw;
		return 1;
	}
	SUnknown0* pUnknown0 = reinterpret_cast<SUnknown0*>(pFlw + uOffset);
	uOffset = uFlwSize;
	for (u32 i = 0; i < pFlwHeader->Count; i++)
	{
		if (pUnknown0[i].Offset != 0)
		{
			uOffset = sizeof(SFlwHeader) + pFlwHeader->Count * sizeof(SData0) + i * sizeof(SUnknown0) + 4 + pUnknown0[i].Offset;
			break;
		}
	}
	if (pFlwHeader->Unknown1Offset != 0)
	{
		delete[] pFlw;
		return 1;
	}
	if (pFlwHeader->Unknown2Offset != 0)
	{
		if (static_cast<u32>(reinterpret_cast<u8*>(&pFlwHeader->Unknown2Offset) - reinterpret_cast<u8*>(pFlwHeader)) + pFlwHeader->Unknown2Offset < uOffset)
		{
			uOffset = static_cast<u32>(reinterpret_cast<u8*>(&pFlwHeader->Unknown2Offset) - reinterpret_cast<u8*>(pFlwHeader)) + pFlwHeader->Unknown2Offset;
		}
		u8* pUnknown2 = pFlw + (reinterpret_cast<u8*>(&pFlwHeader->Unknown2Offset) - reinterpret_cast<u8*>(pFlwHeader)) + pFlwHeader->Unknown2Offset;
		if (*reinterpret_cast<u32*>(pUnknown2) != 0)
		{
			delete[] pFlw;
			return 1;
		}
		if (*reinterpret_cast<u32*>(pUnknown2 + 4) != 4)
		{
			delete[] pFlw;
			return 1;
		}
		if (*reinterpret_cast<u32*>(pUnknown2 + 12) != 4)
		{
			delete[] pFlw;
			return 1;
		}
		if (static_cast<u32>(pFlw + uFlwSize - pUnknown2) < 16 + *reinterpret_cast<u32*>(pUnknown2 + 8) * 4)
		{
			delete[] pFlw;
			return 1;
		}
	}
	SData0* pData0 = reinterpret_cast<SData0*>(pFlw + (reinterpret_cast<u8*>(&pFlwHeader->Data0Offset) - reinterpret_cast<u8*>(pFlwHeader)) + pFlwHeader->Data0Offset);
	fp = UFopen(argv[2], USTR("wb"), false);
	if (fp == nullptr)
	{
		delete[] pFlw;
		return 1;
	}
	fwrite("\xFF\xFE", 2, 1, fp);
	FILE* fpOuter = UFopen(argv[3], USTR("wb"), false);
	if (fpOuter == nullptr)
	{
		fclose(fp);
		delete[] pFlw;
		return 1;
	}
	fwrite("\xFF\xFE", 2, 1, fpOuter);
	FILE* fpInner = UFopen(argv[4], USTR("wb"), false);
	if (fpInner == nullptr)
	{
		fclose(fpOuter);
		fclose(fp);
		delete[] pFlw;
		return 1;
	}
	fwrite("\xFF\xFE", 2, 1, fpInner);
	for (u32 i = 0; i < pFlwHeader->Count; i++)
	{
		if (pData0[i].Data1Offset == 0)
		{
			continue;
		}
		u32 uOffset0 = sizeof(SFlwHeader) + i * sizeof(SData0) + 8 + pData0[i].Data1Offset;
		u32 uOffset1 = uOffset;
		for (u32 j = i + 1; j < pFlwHeader->Count; j++)
		{
			if (pData0[j].Data1Offset != 0)
			{
				uOffset1 = sizeof(SFlwHeader) + j * sizeof(SData0) + 8 + pData0[j].Data1Offset;
				break;
			}
		}
		SData1* pData1 = reinterpret_cast<SData1*>(reinterpret_cast<u8*>(&pData0[i].Data1Offset) + pData0[i].Data1Offset);
		switch (pData1->Data[0])
		{
		case 0x1:
			if (uOffset1 - uOffset0 > 12 && pData1->Data[1] % 2 == 0 && pData1->Data[2] % 4 == 0 && ((pData1->Data[2] == 4/* && uOffset1 == static_cast<u32>(Align(uOffset0 + 8 + pData1->Data[2] + pData1->Data[1], 4))*/) || uOffset0 + 8 + pData1->Data[2] > uOffset1) && uOffset0 + 8 + pData1->Data[2] + pData1->Data[1] <= uFlwSize)
			{
				Char16_t* pTxt16 = reinterpret_cast<Char16_t*>(reinterpret_cast<u8*>(pData1) + 8 + pData1->Data[2]);
				u32 uSize = pData1->Data[1] / 2;
				if (uSize == 0)
				{
					break;
				}
				if (pTxt16[uSize - 1] != 0)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				U16String sTxt16;
				u32 uCount = 0;
				for (u32 j = 0; j < uSize; j++)
				{
					if (pTxt16[j] == '<' && j + 2 < uSize && pTxt16[j + 1] == 'p' && pTxt16[j + 2] == '>')
					{
						fclose(fpInner);
						fclose(fpOuter);
						fclose(fp);
						delete[] pFlw;
						return 1;
					}
					if (pTxt16[j] == 0)
					{
						sTxt16.append(WToU16(L"<p>"));
						uCount++;
					}
					else
					{
						sTxt16.append(1, pTxt16[j]);
					}
				}
				if (uCount != pData1->Data[0])
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				wstring sTxt = U16ToW(sTxt16);
				if (exportStr(i, sTxt, fp, fpOuter, fpInner) != 0)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
			}
			break;
		case 0x2:
			if (uOffset1 - uOffset0 > 12 && pData1->Data[1] % 2 == 0 && pData1->Data[2] % 4 == 0 && ((pData1->Data[2] == 4/* && uOffset1 == static_cast<u32>(Align(uOffset0 + 8 + pData1->Data[2] + pData1->Data[1], 4))*/) || uOffset0 + 8 + pData1->Data[2] > uOffset1) && uOffset0 + 8 + pData1->Data[2] + pData1->Data[1] <= uFlwSize)
			{
				Char16_t* pTxt16 = reinterpret_cast<Char16_t*>(reinterpret_cast<u8*>(pData1) + 8 + pData1->Data[2]);
				u32 uSize = pData1->Data[1] / 2;
				if (uSize == 0)
				{
					break;
				}
				if (pTxt16[uSize - 1] != 0)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				U16String sTxt16;
				u32 uCount = 0;
				for (u32 j = 0; j < uSize; j++)
				{
					if (pTxt16[j] == '<' && j + 2 < uSize && pTxt16[j + 1] == 'p' && pTxt16[j + 2] == '>')
					{
						fclose(fpInner);
						fclose(fpOuter);
						fclose(fp);
						delete[] pFlw;
						return 1;
					}
					if (pTxt16[j] == 0)
					{
						sTxt16.append(WToU16(L"<p>"));
						uCount++;
					}
					else
					{
						sTxt16.append(1, pTxt16[j]);
					}
				}
				if (uCount != pData1->Data[0])
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				wstring sTxt = U16ToW(sTxt16);
				if (exportStr(i, sTxt, fp, fpOuter, fpInner) != 0)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
			}
			break;
		case 0x3:
			if (uOffset1 - uOffset0 > 12 && pData1->Data[1] % 2 == 0 && pData1->Data[2] % 4 == 0 && ((pData1->Data[2] == 4/* && uOffset1 == static_cast<u32>(Align(uOffset0 + 8 + pData1->Data[2] + pData1->Data[1], 4))*/) || uOffset0 + 8 + pData1->Data[2] > uOffset1) && uOffset0 + 8 + pData1->Data[2] + pData1->Data[1] <= uFlwSize)
			{
				Char16_t* pTxt16 = reinterpret_cast<Char16_t*>(reinterpret_cast<u8*>(pData1) + 8 + pData1->Data[2]);
				u32 uSize = pData1->Data[1] / 2;
				if (uSize == 0)
				{
					break;
				}
				if (pTxt16[uSize - 1] != 0)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				U16String sTxt16;
				u32 uCount = 0;
				for (u32 j = 0; j < uSize; j++)
				{
					if (pTxt16[j] == '<' && j + 2 < uSize && pTxt16[j + 1] == 'p' && pTxt16[j + 2] == '>')
					{
						fclose(fpInner);
						fclose(fpOuter);
						fclose(fp);
						delete[] pFlw;
						return 1;
					}
					if (pTxt16[j] == 0)
					{
						sTxt16.append(WToU16(L"<p>"));
						uCount++;
					}
					else
					{
						sTxt16.append(1, pTxt16[j]);
					}
				}
				if (uCount != pData1->Data[0])
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				wstring sTxt = U16ToW(sTxt16);
				if (exportStr(i, sTxt, fp, fpOuter, fpInner) != 0)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
			}
			break;
		case 0x7D0:
			{
				if (pData1->Data[1] != 4)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				if (pData1->Data[3] % 2 != 0)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				if (pData1->Data[4] != 8)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				//if (pData1->Data[5] != pData1->Data[2] + 4)
				//{
				//	fclose(fpInner);
				//	fclose(fpOuter);
				//	fclose(fp);
				//	delete[] pFlw;
				//	return 1;
				//}
				Char16_t* pTxt16 = reinterpret_cast<Char16_t*>(reinterpret_cast<u8*>(pData1) + 0x14 + pData1->Data[5]);
				u32 uSize = pData1->Data[3] / 2;
				if (uSize == 0)
				{
					break;
				}
				if (pTxt16[uSize - 1] != 0)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				U16String sTxt16;
				for (u32 j = 0; j < uSize; j++)
				{
					if (pTxt16[j] == '<' && j + 2 < uSize && pTxt16[j + 1] == 'p' && pTxt16[j + 2] == '>')
					{
						fclose(fpInner);
						fclose(fpOuter);
						fclose(fp);
						delete[] pFlw;
						return 1;
					}
					if (pTxt16[j] == 0)
					{
						sTxt16.append(WToU16(L"<p>"));
					}
					else
					{
						sTxt16.append(1, pTxt16[j]);
					}
				}
				wstring sTxt = U16ToW(sTxt16);
				if (exportStr(i, sTxt, fp, fpOuter, fpInner) != 0)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
			}
			break;
		case 0xBBD:
			{
				if (pData1->Data[1] != 4)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				//if (pData1->Data[3] != 4)
				//{
				//	fclose(fpInner);
				//	fclose(fpOuter);
				//	fclose(fp);
				//	delete[] pFlw;
				//	return 1;
				//}
				Char16_t* pTxt16 = reinterpret_cast<Char16_t*>(reinterpret_cast<u8*>(pData1) + 0xC + pData1->Data[3]);
				u32 uSize = (uOffset1 - uOffset0 - 0xC - pData1->Data[3]) / 2;
				if (uSize == 0)
				{
					break;
				}
				if (pTxt16[uSize - 1] != 0)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				uSize = (uFlwSize - uOffset0 - 0xC - pData1->Data[3]) / 2;
				U16String sTxt16;
				for (u32 j = 0; ; j++)
				{
					if (pTxt16[j] == '<' && j + 2 < uSize && pTxt16[j + 1] == 'p' && pTxt16[j + 2] == '>')
					{
						fclose(fpInner);
						fclose(fpOuter);
						fclose(fp);
						delete[] pFlw;
						return 1;
					}
					if (pTxt16[j] == 0)
					{
						sTxt16.append(WToU16(L"<p>"));
						break;
					}
					else
					{
						sTxt16.append(1, pTxt16[j]);
					}
				}
				wstring sTxt = U16ToW(sTxt16);
				if (exportStr(i, sTxt, fp, fpOuter, fpInner) != 0)
				{
					fclose(fpInner);
					fclose(fpOuter);
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
			}
			break;
		default:
			break;
		}
	}
	fclose(fpInner);
	fclose(fpOuter);
	fclose(fp);
	delete[] pFlw;
	return 0;
}
