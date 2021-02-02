#ifdef USE_WINIMERGELIB
#include <Windows.h>
#include "WinIMergeLib.h"
#else
#include "ImgDiffBuffer.hpp"
#endif
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[])
{
#ifndef USE_WINIMERGELIB
	CImgDiffBuffer buffer;
#endif

	if (argc < 3)
	{
		std::cerr << "usage: cmdidiff image_file1 image_file2" << std::endl;
		exit(1);
	}

	std::locale::global(std::locale(""));
	std::array<std::filesystem::path, 2> filenames{argv[1], argv[2]};

#ifdef USE_WINIMERGELIB
	IImgMergeWindow *pImgMergeWindow = WinIMerge_CreateWindowless();
	if (pImgMergeWindow)
	{
		if (!pImgMergeWindow->OpenImages(filenames[0], filenames[1]))
		{
			std::wcerr << L"cmdidiff: could not open files. (" << filenameW[0] << ", " << filenameW[1] << L")" << std::endl;
			exit(1);
		}
		pImgMergeWindow->SaveDiffImageAs(1, L"diff.png");
		WinIMerge_DestroyWindow(pImgMergeWindow);
	}
#else
	FreeImage_Initialise();

	if (!buffer.OpenImages(2, filenames.data()))
	{
		std::cerr << "cmdidiff: could not open files. (" << filenames[0] << ", " << filenames[1] << ")" << std::endl;
		exit(1);
	}

	buffer.CompareImages();
	buffer.SaveDiffImageAs(1, "diff.png");
	buffer.CloseImages();
#endif

	return 0;
}


