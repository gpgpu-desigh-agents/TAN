//
// MIT license
//
// Copyright (c) 2019 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "stdafx.h"
#include "FileUtility.h"
#include "StringUtility.h"

#include <fstream>
#include <iostream>

#define MAX_SIZE_STRING  16384
#define MAX_BLOCK_CHUNK  12000

int main(int argc, char* argv[])
{
	if (argc > 3)
	{
		std::cout << "Too many parameters" << std::endl;

		return 1;
	}

	if (argc <= 1)
	{
		std::cout << "Not enough parameters" << std::endl;

		return 1;
	}

	std::string kernelFileFullName = argv[1];
	auto kernelFileExtension = getFileExtension(kernelFileFullName);

	if(!compareIgnoreCase(kernelFileExtension, "cl"))
	{
		std::cout << "File is not a .cl file" << std::endl;

		return 1;
	}

	std::wifstream clKernelStream(kernelFileFullName);

	if(clKernelStream.fail())
	{
		std::cout << "Could not open .cl file" << std::endl;

		return 1;
	}

	auto fileName = getFileNameWithExtension(kernelFileFullName);
	fileName.resize(fileName.length() - 3); //skip extension

	std::string outputFileName(
		argc > 2
		    ? argv[2]
			: "CLKernel_" + fileName + ".h"
		);

	auto path2File(getPath2File(outputFileName));

	//std::cout << "current path: [" << getCurrentDirectory() << "]" << std::endl;
	//std::cout << "PTF: " << path2File << std::endl;

	if(path2File.length() && !checkDirectoryExist(path2File))
	{
		if(!createPath(path2File))
		{
			std::cout << "Could not create path " << path2File << std::endl;

			return 1;
		}
	}

	std::wofstream outputStream(outputFileName);

	if(outputStream.fail())
	{
		std::cout << "Could not open output file " << outputFileName << std::endl;

		return 1;
	}

	std::cout << "Outputing: " << outputFileName << std::endl;

	std::wstring clKernelSource(
		(std::istreambuf_iterator<wchar_t>(clKernelStream)),
		std::istreambuf_iterator<wchar_t>()
		);

	std::wstring headerdoc =
	    L"#pragma once\n"
		"#include <string>\n"
		"// This file is generated by CLKenelProprocessor, any changes made in this file will be lost\n"
		;
	outputStream << headerdoc << std::endl;

	// Since MS has a maximum size for string literal, we need to check the file size
	size_t incrementChunk = MAX_BLOCK_CHUNK;

	size_t partitionCount = 0;
	std::wstring concatenateSource;
	std::wstring fileNameWide = toWideString(fileName);

	for
	(
		size_t blockIndex = 0;
		blockIndex < clKernelSource.length();
		blockIndex += incrementChunk, ++partitionCount
	)
	{
		if((clKernelSource.size() - blockIndex) < MAX_BLOCK_CHUNK)
		{
			incrementChunk = clKernelSource.size() - blockIndex;
		}
		else
		{
			incrementChunk = MAX_BLOCK_CHUNK;
		}

		outputStream
			<< L"const std::string " << fileNameWide << partitionCount
			<< L" = R\"(" << clKernelSource.substr(blockIndex, incrementChunk) << L")\";"
			<< std::endl;

		if(incrementChunk < MAX_BLOCK_CHUNK)
		{
			concatenateSource += fileNameWide + std::to_wstring(partitionCount);
		}
		else
		{
			concatenateSource += fileNameWide + std::to_wstring(partitionCount) + L"+";
		}
	}

	outputStream
	    << "const std::string " << fileNameWide << "_Str = " << concatenateSource << ";" << std::endl
		<< "static const char* " << fileNameWide << " = &" << fileNameWide << "_Str[0u];" << std::endl
		<< "const size_t " << fileNameWide << "Count = " << fileNameWide << "_Str.size();" << std::endl
		;

	outputStream.flush();

	return 0;
}