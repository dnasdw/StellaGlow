#ifndef LZSS_H_
#define LZSS_H_

#include <sdw.h>

class CLzss
{
public:
	static bool GetUncompressedSize(const u8* a_pCompressed, u32 a_uCompressedSize, u32& a_uUncompressedSize);
	static u32 GetCompressBoundSize(u32 a_uUncompressedSize, n32 a_nCompressAlign);
	static bool Uncompress(const u8* a_pCompressed, u32 a_uCompressedSize, u8* a_pUncompressed, u32& a_uUncompressedSize);
	static bool Compress(const u8* a_pUncompressed, u32 a_uUncompressedSize, u8* a_pCompressed, u32& a_uCompressedSize, n32 a_nCompressAlign);
private:
	struct SCompressInfo
	{
		n16* Left;
		n16* Parent;
		n16* Right;
	};
	CLzss();
	static void initTree(SCompressInfo* a_pInfo, void* a_pWork);
	static int insertNode(SCompressInfo* a_pInfo, int a_nBufferPos, int& a_nPos);
	static void eraseNode(SCompressInfo* a_pInfo, int a_nPos);
	static const int s_nBufferSize;
	static const int s_nMaxSize;
	static const int s_nThreshold;
	static const int s_nCompressWorkSize;
	static u8 s_uBuffer[];
};

#endif	// LZSS_H_
