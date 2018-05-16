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
		if (pFlw + uFlwSize - pUnknown2 != 16 + *reinterpret_cast<u32*>(pUnknown2 + 8) * 4)
		{
			delete[] pFlw;
			return 1;
		}
	}
	SData0* pData0 = reinterpret_cast<SData0*>(pFlw + (reinterpret_cast<u8*>(&pFlwHeader->Data0Offset) - reinterpret_cast<u8*>(pFlwHeader)) + pFlwHeader->Data0Offset);
	fp = UFopen(argv[2], USTR("rb"), false);
	if (fp == nullptr)
	{
		delete[] pFlw;
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uTxtSize = ftell(fp);
	if (uTxtSize % 2 != 0)
	{
		fclose(fp);
		delete[] pFlw;
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
		delete[] pFlw;
		return 1;
	}
	pTemp[uTxtSize] = 0;
	wstring sTxt = U16ToW(pTemp + 1);
	delete[] pTemp;
	fp = UFopen(argv[3], USTR("rb"), false);
	if (fp == nullptr)
	{
		delete[] pFlw;
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	uTxtSize = ftell(fp);
	if (uTxtSize % 2 != 0)
	{
		fclose(fp);
		delete[] pFlw;
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
		delete[] pFlw;
		return 1;
	}
	pTemp[uTxtSize] = 0;
	wstring sOuter = U16ToW(pTemp + 1);
	delete[] pTemp;
	fp = UFopen(argv[4], USTR("rb"), false);
	if (fp == nullptr)
	{
		delete[] pFlw;
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	uTxtSize = ftell(fp);
	if (uTxtSize % 2 != 0)
	{
		fclose(fp);
		delete[] pFlw;
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
		delete[] pFlw;
		return 1;
	}
	pTemp[uTxtSize] = 0;
	wstring sInner = U16ToW(pTemp + 1);
	delete[] pTemp;
	fp = UFopen(argv[1], USTR("wb"), false);
	if (fp == nullptr)
	{
		delete[] pFlw;
		return 1;
	}
	fwrite(pFlw, 1, uFlwSize, fp);
	u32 uFlwSizeCurrent = static_cast<u32>(Align(uFlwSize, 4));
	fseek(fp, uFlwSizeCurrent, SEEK_SET);
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
			if (uOffset1 - uOffset0 > 12 && pData1->Data[1] % 2 == 0 && pData1->Data[2] == 4 && uOffset1 == static_cast<u32>(Align(uOffset0 + 8 + pData1->Data[2] + pData1->Data[1], 4)))
			{
				Char16_t* pTxt16 = reinterpret_cast<Char16_t*>(reinterpret_cast<u8*>(pData1) + 8 + pData1->Data[2]);
				u32 uSize = pData1->Data[1] / 2;
				if (uSize == 0)
				{
					break;
				}
				if (pTxt16[uSize - 1] != 0)
				{
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
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				if (importStr(i, sTxt, sOuter, sInner, sTxt16) != 0)
				{
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				uSize = (uSize + 1) / 2 * 2;
				pData1->Data[1] = static_cast<u32>(sTxt16.size()) * 2;
				if (sTxt16.size() <= uSize)
				{
					memcpy(pTxt16, sTxt16.c_str(), sTxt16.size() * 2);
					memset(pTxt16 + sTxt16.size(), 0, (uSize - sTxt16.size()) * 2);
				}
				else
				{
					pData1->Data[2] = uFlwSizeCurrent - (uOffset0 + 8);
					fwrite(sTxt16.c_str(), 2, (sTxt16.size() + 1) / 2 * 2, fp);
					uFlwSizeCurrent = ftell(fp);
				}
			}
			break;
		case 0x2:
			if (uOffset1 - uOffset0 > 12 && pData1->Data[1] % 2 == 0 && pData1->Data[2] == 4 && uOffset1 == static_cast<u32>(Align(uOffset0 + 8 + pData1->Data[2] + pData1->Data[1], 4)))
			{
				Char16_t* pTxt16 = reinterpret_cast<Char16_t*>(reinterpret_cast<u8*>(pData1) + 8 + pData1->Data[2]);
				u32 uSize = pData1->Data[1] / 2;
				if (uSize == 0)
				{
					break;
				}
				if (pTxt16[uSize - 1] != 0)
				{
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
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				if (importStr(i, sTxt, sOuter, sInner, sTxt16) != 0)
				{
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				uSize = (uSize + 1) / 2 * 2;
				pData1->Data[1] = static_cast<u32>(sTxt16.size()) * 2;
				if (sTxt16.size() <= uSize)
				{
					memcpy(pTxt16, sTxt16.c_str(), sTxt16.size() * 2);
					memset(pTxt16 + sTxt16.size(), 0, (uSize - sTxt16.size()) * 2);
				}
				else
				{
					pData1->Data[2] = uFlwSizeCurrent - (uOffset0 + 8);
					fwrite(sTxt16.c_str(), 2, (sTxt16.size() + 1) / 2 * 2, fp);
					uFlwSizeCurrent = ftell(fp);
				}
			}
			break;
		case 0x3:
			if (uOffset1 - uOffset0 > 12 && pData1->Data[1] % 2 == 0 && pData1->Data[2] == 4 && uOffset1 == static_cast<u32>(Align(uOffset0 + 8 + pData1->Data[2] + pData1->Data[1], 4)))
			{
				Char16_t* pTxt16 = reinterpret_cast<Char16_t*>(reinterpret_cast<u8*>(pData1) + 8 + pData1->Data[2]);
				u32 uSize = pData1->Data[1] / 2;
				if (uSize == 0)
				{
					break;
				}
				if (pTxt16[uSize - 1] != 0)
				{
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
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				if (importStr(i, sTxt, sOuter, sInner, sTxt16) != 0)
				{
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				uSize = (uSize + 1) / 2 * 2;
				pData1->Data[1] = static_cast<u32>(sTxt16.size()) * 2;
				if (sTxt16.size() <= uSize)
				{
					memcpy(pTxt16, sTxt16.c_str(), sTxt16.size() * 2);
					memset(pTxt16 + sTxt16.size(), 0, (uSize - sTxt16.size()) * 2);
				}
				else
				{
					pData1->Data[2] = uFlwSizeCurrent - (uOffset0 + 8);
					fwrite(sTxt16.c_str(), 2, (sTxt16.size() + 1) / 2 * 2, fp);
					uFlwSizeCurrent = ftell(fp);
				}
			}
			break;
		case 0x7D0:
			{
				if (pData1->Data[1] != 4)
				{
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				if (pData1->Data[3] % 2 != 0)
				{
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				if (pData1->Data[4] != 8)
				{
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				if (pData1->Data[5] != pData1->Data[2] + 4)
				{
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				Char16_t* pTxt16 = reinterpret_cast<Char16_t*>(reinterpret_cast<u8*>(pData1) + 0x14 + pData1->Data[5]);
				u32 uSize = pData1->Data[3] / 2;
				if (uSize == 0)
				{
					break;
				}
				if (pTxt16[uSize - 1] != 0)
				{
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				U16String sTxt16;
				for (u32 j = 0; j < uSize; j++)
				{
					if (pTxt16[j] == '<' && j + 2 < uSize && pTxt16[j + 1] == 'p' && pTxt16[j + 2] == '>')
					{
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
				if (importStr(i, sTxt, sOuter, sInner, sTxt16) != 0)
				{
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				uSize = (uSize + 1) / 2 * 2;
				pData1->Data[3] = static_cast<u32>(sTxt16.size()) * 2;
				if (sTxt16.size() <= uSize)
				{
					memcpy(pTxt16, sTxt16.c_str(), sTxt16.size() * 2);
					memset(pTxt16 + sTxt16.size(), 0, (uSize - sTxt16.size()) * 2);
				}
				else
				{
					pData1->Data[5] = uFlwSizeCurrent - (uOffset0 + 0x14);
					fwrite(sTxt16.c_str(), 2, (sTxt16.size() + 1) / 2 * 2, fp);
					uFlwSizeCurrent = ftell(fp);
				}
			}
			break;
		case 0xBBD:
			{
				if (pData1->Data[1] != 4)
				{
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				if (pData1->Data[3] != 4)
				{
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				Char16_t* pTxt16 = reinterpret_cast<Char16_t*>(reinterpret_cast<u8*>(pData1) + 0xC + pData1->Data[3]);
				u32 uSize = (uOffset1 - uOffset0 - 0xC - pData1->Data[3]) / 2;
				if (uSize == 0)
				{
					break;
				}
				if (pTxt16[uSize - 1] != 0)
				{
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				U16String sTxt16;
				for (u32 j = 0; ; j++)
				{
					if (pTxt16[j] == '<' && j + 2 < uSize && pTxt16[j + 1] == 'p' && pTxt16[j + 2] == '>')
					{
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
				if (importStr(i, sTxt, sOuter, sInner, sTxt16) != 0)
				{
					fclose(fp);
					delete[] pFlw;
					return 1;
				}
				uSize = (uSize + 1) / 2 * 2;
				if (sTxt16.size() <= uSize)
				{
					memcpy(pTxt16, sTxt16.c_str(), sTxt16.size() * 2);
					memset(pTxt16 + sTxt16.size(), 0, (uSize - sTxt16.size()) * 2);
				}
				else
				{
					pData1->Data[3] = uFlwSizeCurrent - (uOffset0 + 0xC);
					fwrite(sTxt16.c_str(), 2, (sTxt16.size() + 1) / 2 * 2, fp);
					uFlwSizeCurrent = ftell(fp);
				}
			}
			break;
		default:
			break;
		}
	}
	fseek(fp, 0, SEEK_SET);
	fwrite(pFlw, 1, uFlwSize, fp);
	fclose(fp);
	delete[] pFlw;
	return 0;
}
