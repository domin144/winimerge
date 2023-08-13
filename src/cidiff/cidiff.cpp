#ifdef USE_WINIMERGELIB
#include "../WinIMergeLib/WinIMergeLib.h"
#else
#include "ImgDiffBuffer.hpp"
#endif
#include <array>
#include <string>
#include <filesystem>
#include <boost/nowide/args.hpp>
#include <boost/nowide/iostream.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
#ifndef USE_WINIMERGELIB
	CImgDiffBuffer buffer;
#endif

	boost::nowide::args a(argc, argv);

	if (argc < 3)
	{
		boost::nowide::cerr << "usage: cmdidiff image_file1 image_file2" << std::endl;
		exit(1);
	}

	std::array<std::filesystem::path, 2> filenames{
		std::filesystem::u8path(argv[1]),
		std::filesystem::u8path(argv[2])};

#ifdef USE_WINIMERGELIB
	IImgMergeWindow *pImgMergeWindow = WinIMerge_CreateWindowless();
	if (pImgMergeWindow)
	{
		if (!pImgMergeWindow->OpenImages(filenames[0], filenames[1]))
		{
			boost::nowide::cerr
				<< "cmdidiff: could not open files. ("
				<< filenames[0].u8string() << ", "
				<< filenames[1].u8string() << ")" << std::endl;
			exit(1);
		}
		pImgMergeWindow->SaveDiffImageAs(1, "diff.png");
		WinIMerge_DestroyWindow(pImgMergeWindow);
	}
#else
	FreeImage_Initialise();

	if (!buffer.OpenImages(2, filenames.data()))
	{
		boost::nowide::cerr
			<< "cmdidiff: could not open files. ("
			<< filenames[0].u8string() << ", "
			<< filenames[1].u8string() << ")" << std::endl;
		exit(1);
	}

	buffer.CompareImages();
	buffer.SaveDiffImageAs(1, "diff.png");
	buffer.CloseImages();
#endif

	return 0;
}
