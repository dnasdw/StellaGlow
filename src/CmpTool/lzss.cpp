#include "lzss.h"

const int CLzss::s_nBufferSize = 4096;
const int CLzss::s_nMaxSize = 18;
const int CLzss::s_nThreshold = 3;
const int CLzss::s_nCompressWorkSize = (s_nBufferSize + s_nBufferSize + s_nBufferSize + 256) * sizeof(n16);
u8 CLzss::s_uBuffer[s_nBufferSize + s_nMaxSize - 1] = {};

bool CLzss::GetUncompressedSize(const u8* a_pCompressed, u32 a_uCompressedSize, u32& a_uUncompressedSize)
{
	bool bResult = true;
	if (a_uCompressedSize >= 4)
	{
		a_uUncompressedSize = *reinterpret_cast<const u32*>(a_pCompressed);
	}
	else
	{
		bResult = false;
	}
	return bResult;
}

u32 CLzss::GetCompressBoundSize(u32 a_uUncompressedSize, n32 a_nCompressAlign)
{
	return (4 + (a_uUncompressedSize + 7) / 8 * 9 + a_nCompressAlign - 1) / a_nCompressAlign * a_nCompressAlign;
}

bool CLzss::Uncompress(const u8* a_pCompressed, u32 a_uCompressedSize, u8* a_pUncompressed, u32& a_uUncompressedSize)
{
	bool bResult = true;
	if (a_uCompressedSize >= 4)
	{
		u32 uUncompressedSize = *reinterpret_cast<const u32*>(a_pCompressed);
		if (a_uUncompressedSize >= uUncompressedSize)
		{
			a_uUncompressedSize = uUncompressedSize;
			const u8* pSrc = a_pCompressed + 4;
			u8* pDest = a_pUncompressed;
			memset(s_uBuffer, 0, s_nBufferSize);
			int nBufferPos = s_nBufferSize - s_nMaxSize;
			while (a_pUncompressed + a_uUncompressedSize - pDest > 0)
			{
				if (a_pCompressed + a_uCompressedSize - pSrc < 1)
				{
					bResult = false;
					break;
				}
				u8 uFlag = *pSrc++;
				for (int i = 0; i < 8; i++)
				{
					if ((uFlag >> i & 0x01) != 0)
					{
						if (a_pCompressed + a_uCompressedSize - pSrc < 1)
						{
							bResult = false;
							break;
						}
						u8 uData = *pSrc++;
						*pDest++ = uData;
						s_uBuffer[nBufferPos++] = uData;
						nBufferPos %= s_nBufferSize;
					}
					else
					{
						if (a_pCompressed + a_uCompressedSize - pSrc < 2)
						{
							bResult = false;
							break;
						}
						int nOffset = *pSrc++;
						nOffset |= (*pSrc << 4 & 0xF00);
						int nSize = (*pSrc++ & 0x0F) + s_nThreshold;
						if (nSize > a_pUncompressed + a_uUncompressedSize - pDest)
						{
							bResult = false;
							break;
						}
						for (int j = 0; j < nSize; j++)
						{
							u8 uData = s_uBuffer[(nOffset + j) % s_nBufferSize];
							*pDest++ = uData;
							s_uBuffer[nBufferPos++] = uData;
							nBufferPos %= s_nBufferSize;
						}
					}
					if (a_pUncompressed + a_uUncompressedSize - pDest <= 0)
					{
						break;
					}
				}
				if (!bResult)
				{
					break;
				}
			}
		}
		else
		{
			bResult = false;
		}
	}
	else
	{
		bResult = false;
	}
	return bResult;
}

bool CLzss::Compress(const u8* a_pUncompressed, u32 a_uUncompressedSize, u8* a_pCompressed, u32& a_uCompressedSize, n32 a_nCompressAlign)
{
	bool bResult = true;
	u8* pWork = new u8[s_nCompressWorkSize];
	do
	{
		*reinterpret_cast<u32*>(a_pCompressed) = a_uUncompressedSize;
		SCompressInfo info;
		initTree(&info, pWork);
		const u8* pSrc = a_pUncompressed;
		u8* pDest = a_pCompressed + 4;
		memset(s_uBuffer, 0, s_nBufferSize + s_nMaxSize - 1);
		int nBufferPos = s_nBufferSize - s_nMaxSize;
		memcpy(s_uBuffer + nBufferPos, pSrc, static_cast<int>(min<n64>(s_nMaxSize, a_pUncompressed + a_uUncompressedSize - pSrc)));
		int nPos = 0;
		for (int i = 1; i <= s_nMaxSize; i++)
		{
			insertNode(&info, nBufferPos - i, nPos);
		}
		while (a_pUncompressed + a_uUncompressedSize - pSrc > 0)
		{
			if (a_pCompressed + a_uCompressedSize - pDest < 1)
			{
				bResult = false;
				break;
			}
			u8* pFlag = pDest++;
			*pFlag = 0;
			for (int i = 0; i < 8; i++)
			{
				int nSize = insertNode(&info, nBufferPos, nPos);
				if (nSize > a_pUncompressed + a_uUncompressedSize - pSrc)
				{
					nSize = static_cast<n32>(a_pUncompressed + a_uUncompressedSize - pSrc);
				}
				if (nSize < s_nThreshold)
				{
					nSize = 1;
					if (a_pCompressed + a_uCompressedSize - pDest < 1)
					{
						bResult = false;
						break;
					}
					*pFlag |= 1 << i;
					*pDest++ = s_uBuffer[nBufferPos];
				}
				else
				{
					if (a_pCompressed + a_uCompressedSize - pDest < 2)
					{
						bResult = false;
						break;
					}
					*pDest++ = nPos & 0xFF;
					*pDest++ = (nPos >> 4 & 0xF0) | (nSize - s_nThreshold);
				}
				for (int j = 0; j < nSize; j++)
				{
					nPos = (nBufferPos + s_nMaxSize) % s_nBufferSize;
					eraseNode(&info, nPos);
					if (a_pUncompressed + a_uUncompressedSize - pSrc > s_nMaxSize + j)
					{
						s_uBuffer[nPos] = pSrc[s_nMaxSize + j];
						if (nPos < s_nMaxSize - 1)
						{
							s_uBuffer[s_nBufferSize + nPos] = s_uBuffer[nPos];
						}
					}
					nBufferPos = (nBufferPos + 1) % s_nBufferSize;
					if (j != nSize - 1)
					{
						insertNode(&info, nBufferPos, nPos);
					}
				}
				pSrc += nSize;
				if (a_pUncompressed + a_uUncompressedSize - pSrc <= 0)
				{
					break;
				}
			}
			if (!bResult)
			{
				break;
			}
		}
		if (!bResult)
		{
			break;
		}
		while ((pDest - a_pCompressed) % a_nCompressAlign != 0)
		{
			if (a_pCompressed + a_uCompressedSize - pDest < 1)
			{
				bResult = false;
				break;
			}
			*pDest++ = 0;
		}
		if (!bResult)
		{
			break;
		}
		a_uCompressedSize = static_cast<u32>(pDest - a_pCompressed);
	} while (false);
	delete[] pWork;
	return bResult;
}

CLzss::CLzss()
{
}

void CLzss::initTree(SCompressInfo* a_pInfo, void* a_pWork)
{
	a_pInfo->Left = reinterpret_cast<n16*>(a_pWork);
	a_pInfo->Parent = reinterpret_cast<n16*>(a_pWork) + s_nBufferSize;
	a_pInfo->Right = reinterpret_cast<n16*>(a_pWork) + s_nBufferSize + s_nBufferSize;
	for (int i = 0; i < 256; i++)
	{
		a_pInfo->Right[s_nBufferSize + i] = -1;
	}
	for (int i = 0; i < s_nBufferSize; i++)
	{
		a_pInfo->Parent[i] = -1;
	}
}

int CLzss::insertNode(SCompressInfo* a_pInfo, int a_nBufferPos, int& a_nPos)
{
	n16* pLeft = a_pInfo->Left;
	n16* pParent = a_pInfo->Parent;
	n16* pRight = a_pInfo->Right;
	int nCmp = 1;
	u8* pSrc = s_uBuffer + a_nBufferPos;
	int nPos = s_nBufferSize + *pSrc;
	pLeft[a_nBufferPos] = -1;
	pRight[a_nBufferPos] = -1;
	int nSize = 0;
	do
	{
		if (nCmp >= 0)
		{
			if (pRight[nPos] != -1)
			{
				nPos = pRight[nPos];
			}
			else
			{
				pRight[nPos] = a_nBufferPos;
				pParent[a_nBufferPos] = nPos;
				return nSize;
			}
		}
		else
		{
			if (pLeft[nPos] != -1)
			{
				nPos = pLeft[nPos];
			}
			else
			{
				pLeft[nPos] = a_nBufferPos;
				pParent[a_nBufferPos] = nPos;
				return nSize;
			}
		}
		int nCurrentSize = 1;
		for (; nCurrentSize < s_nMaxSize; nCurrentSize++)
		{
			if ((nCmp = pSrc[nCurrentSize] - s_uBuffer[nPos + nCurrentSize]) != 0)
			{
				break;
			}
		}
		if (nCurrentSize > nSize)
		{
			a_nPos = nPos;
			if ((nSize = nCurrentSize) == s_nMaxSize)
			{
				break;
			}
		}
	} while (true);
	pParent[a_nBufferPos] = pParent[nPos];
	pLeft[a_nBufferPos] = pLeft[nPos];
	pRight[a_nBufferPos] = pRight[nPos];
	if (pLeft[nPos] != -1)
	{
		pParent[pLeft[nPos]] = a_nBufferPos;
	}
	if (pRight[nPos] != -1)
	{
		pParent[pRight[nPos]] = a_nBufferPos;
	}
	if (pRight[pParent[nPos]] == nPos)
	{
		pRight[pParent[nPos]] = a_nBufferPos;
	}
	else
	{
		pLeft[pParent[nPos]] = a_nBufferPos;
	}
	pParent[nPos] = -1;
	return nSize;
}

void CLzss::eraseNode(SCompressInfo* a_pInfo, int a_nPos)
{
	n16* pLeft = a_pInfo->Left;
	n16* pParent = a_pInfo->Parent;
	n16* pRight = a_pInfo->Right;
	if (pParent[a_nPos] == -1)
	{
		return;
	}
	int nPos = -1;
	if (pRight[a_nPos] == -1)
	{
		nPos = pLeft[a_nPos];
	}
	else if (pLeft[a_nPos] == -1)
	{
		nPos = pRight[a_nPos];
	}
	else
	{
		nPos = pLeft[a_nPos];
		if (pRight[nPos] != -1)
		{
			do
			{
				nPos = pRight[nPos];
			} while (pRight[nPos] != -1);
			pRight[pParent[nPos]] = pLeft[nPos];
			if (pLeft[nPos] != -1)
			{
				pParent[pLeft[nPos]] = pParent[nPos];
			}
			pLeft[nPos] = pLeft[a_nPos];
			pParent[pLeft[a_nPos]] = nPos;
		}
		pRight[nPos] = pRight[a_nPos];
		pParent[pRight[a_nPos]] = nPos;
	}
	if (nPos != -1)
	{
		pParent[nPos] = pParent[a_nPos];
	}
	if (pRight[pParent[a_nPos]] == a_nPos)
	{
		pRight[pParent[a_nPos]] = nPos;
	}
	else
	{
		pLeft[pParent[a_nPos]] = nPos;
	}
	pParent[a_nPos] = -1;
}
