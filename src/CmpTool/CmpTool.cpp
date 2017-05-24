#include <sdw.h>
#include "lzss.h"

int uncompressCmp(const UChar* a_pCmpFileName, const UChar* a_pBinFileName)
{
	FILE* fp = UFopen(a_pCmpFileName, USTR("rb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uCompressedSize = ftell(fp);
	if (uCompressedSize < 8)
	{
		fclose(fp);
		return 1;
	}
	fseek(fp, 0, SEEK_SET);
	u8* pCompressed = new u8[uCompressedSize];
	fread(pCompressed, 1, uCompressedSize, fp);
	fclose(fp);
	u32 uSignature = *reinterpret_cast<u32*>(pCompressed);
	if (uSignature != SDW_CONVERT_ENDIAN32('IECP'))
	{
		delete[] pCompressed;
		return 1;
	}
	u32 uUncompressedSize = 0;
	if (!CLzss::GetUncompressedSize(pCompressed + 4, uCompressedSize - 4, uUncompressedSize))
	{
		delete[] pCompressed;
		return 1;
	}
	u8* pUncompressed = new u8[uUncompressedSize];
	if (!CLzss::Uncompress(pCompressed + 4, uCompressedSize - 4, pUncompressed, uUncompressedSize))
	{
		delete[] pUncompressed;
		delete[] pCompressed;
		return 1;
	}
	delete[] pCompressed;
	fp = UFopen(a_pBinFileName, USTR("wb"), false);
	if (fp == nullptr)
	{
		delete[] pUncompressed;
		return 1;
	}
	fwrite(pUncompressed, 1, uUncompressedSize, fp);
	fclose(fp);
	delete[] pUncompressed;
	return 0;
}

int compressCmp(const UChar* a_pBinFileName, const UChar* a_pCmpFileName)
{
	FILE* fp = UFopen(a_pBinFileName, USTR("rb"));
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uUncompressedSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	u8* pUncompressed = new u8[uUncompressedSize];
	fread(pUncompressed, 1, uUncompressedSize, fp);
	fclose(fp);
	u32 uCompressedSize = CLzss::GetCompressBoundSize(uUncompressedSize, 1);
	u8* pCompressed = new u8[uCompressedSize];
	if (!CLzss::Compress(pUncompressed, uUncompressedSize, pCompressed, uCompressedSize, 1))
	{
		delete[] pCompressed;
		delete[] pUncompressed;
		return 1;
	}
	delete[] pUncompressed;
	fp = UFopen(a_pCmpFileName, USTR("wb"));
	if (fp == nullptr)
	{
		delete[] pCompressed;
		return 1;
	}
	u32 uSignature = SDW_CONVERT_ENDIAN32('IECP');
	fwrite(&uSignature, 4, 1, fp);
	fwrite(pCompressed, 1, uCompressedSize, fp);
	fclose(fp);
	delete[] pCompressed;
	return 0;
}

int UMain(int argc, UChar* argv[])
{
	if (argc != 4)
	{
		return 1;
	}
	if (UCslen(argv[1]) == 1)
	{
		switch (*argv[1])
		{
		case USTR('U'):
		case USTR('u'):
			return uncompressCmp(argv[2], argv[3]);
		case USTR('C'):
		case USTR('c'):
			return compressCmp(argv[2], argv[3]);
		default:
			break;
		}
	}
	return 1;
}
