#ifndef STRICT //Enforce strict definitions of Windows data types
	#define STRICT
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN //Exclude rarely-used stuff from Windows headers
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "BboxData.h"
#include "..\TextFileOps\TextFileOps.h"
#include <fstream>
#include <string>
#include <Windows.h>

using namespace std;

bool BboxData::bArchiveObjMade = false;
wstring BboxData::modelNames[] = { L"" };
wstring BboxData::attrNames[] = { L"" };

BboxData::BboxData()
{
	numColumns = k_num_attr;
}

bool BboxData::bMakeBboxObjArchive(const wstring csvName)
{
	bool bSuccess = false;

	if (!bArchiveObjMade) //Only one archive can be made because this gets set to true inside
	{
		//Parameters to pass for slicing out headers that are member arrays
		wstring *headers[2] = { modelNames, attrNames };
		const int sliceSize[2] = { k_num_model, k_num_attr };
		const bool sliceIsRow[2] = { false, true };
		const int numSlice[2] = { 1, 1 };

		Archive::csvName = csvName;

		if (bMakeObjArchive(2, headers, sliceSize, sliceIsRow, numSlice))
		{
			bArchiveObjMade = true;
			bSuccess = true;
		}
	}

	return bSuccess;
}

bool BboxData::bReadModelFiles()
{
	bool bSuccess = false;

	if (bUnpackModels() && bDecompileModels())
	{
		HANDLE hFind;
		WIN32_FIND_DATA FindFileData;

		hFind = FindFirstFileW(static_cast<LPCWSTR>(L".\\models\\*.qc"), &FindFileData); //Begin fetching model file names

		if (hFind != INVALID_HANDLE_VALUE)
		{
			for (int i = 0; i < k_num_model; ++i) //Collect attributes for each model in bboxData
			{
				size_t dotIndex = static_cast<wstring>(FindFileData.cFileName).find_last_of(L"."); //Where to trim file extension

				if (static_cast<wstring>(FindFileData.cFileName).substr(static_cast<size_t>(0), dotIndex) != modelNames[i]
					&& !static_cast<bool>(hFind) && i < k_num_model - 1 && GetLastError() != ERROR_NO_MORE_FILES)
				{
					bSuccess = false; //Model filename did not match expected modelNames[i] or FindNextFileW failed early
					break;
				}
				else
				{
					wifstream modelFile;

					modelFile.open(static_cast<wstring>(L".\\models\\") + static_cast<wstring>(FindFileData.cFileName));

					if (!modelFile.fail())
					{
						WCHAR searchResult[1][MAX_PATH];
						wstring searchTerm = static_cast<wstring>(L"$hbox");
						int elementsToCopy[k_num_attr] = { 2, 3, 4, 5, 6, 7, 11 };
						int charPos = 1; //Will skip first char (a space) when reading searchResult[0] char by char later
						int spaceDelimitedElement = 0; //Tracks the current space-delimited string element in searchResult[0]

						if (TextFileOps::inst().parseTextFile(searchTerm, modelFile, searchResult, 1) != 1)
						{
							bSuccess = false;
							break;
						}

						for (int j = 0; j < k_num_attr; ++j) //Collect each attribute for this model
						{
							wstring bboxDatumBuilder;

							do
							{
								if (searchResult[0][charPos] == L' ')
									++spaceDelimitedElement;

								if (spaceDelimitedElement == elementsToCopy[j] && searchResult[0][charPos] != L' ')
									bboxDatumBuilder += searchResult[0][charPos]; //Build bbox datum at the element of interest

								++charPos;

								if (spaceDelimitedElement > elementsToCopy[j] || searchResult[0][charPos] == L'\0')
									break;
							} while (true);

							bboxData[i][j] = bboxDatumBuilder;
						}

						bSuccess = true;
						FindNextFileW(hFind, &FindFileData); //Fetch next model file name
						modelFile.close();
					}
				}
			}

			FindClose(hFind);
		}
	}

	return bSuccess;
}

bool BboxData::bUnpackModels()
{
	bool bSuccess = false;

	//******Call other program to do this or implement someone else's code******
	bSuccess = true; //Set to true until function body is coded

	return bSuccess;
}

bool BboxData::bDecompileModels()
{
	bool bSuccess = false;

	//******Call other program to do this or implement someone else's code******
	bSuccess = true; //Set to true until function body is coded

	return bSuccess;
}

bool BboxData::bCheckArchive(BboxData &newBbox, wstring &badRowName, wstring &badColName, wstring &badNewVal,
	wstring &badArchiveVal)
{
	bool bUpdate = false;
	int j;

	for (int i = 0; i < k_num_model; ++i)
	{
		if (bUpdate = bCheckArchiveRow(newBbox.bboxData[i], bboxData[i], j)) //Single = is intentional
		{
			badRowName = modelNames[i];
			badColName = attrNames[j];
			badNewVal = newBbox.bboxData[i][j];
			badArchiveVal = bboxData[i][j];
			break; //Terminate the loop after first mismatch
		}
	}

	return bUpdate;
}

void BboxData::readArchive()
{
	for (int i = 0; i < k_num_model; ++i)
		readArchiveRow(bboxData[i], i + 2);
}

bool BboxData::bWriteArchiveFile(BboxData &newBbox)
{
	bool bSuccess = false;

	getOutArchive().open(csvName);

	if (!getOutArchive().fail())
	{
		writeArchiveFileRow(attrNames);
		getOutArchive() << endl;

		for (int i = 0; i < k_num_model; ++i)
		{
			getOutArchive() << modelNames[i];
			writeArchiveFileRow(newBbox.bboxData[i]);
			getOutArchive() << endl;
		}

		bSuccess = true;
		getOutArchive().close();
	}

	return bSuccess;
}
