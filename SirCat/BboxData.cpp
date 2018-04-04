#ifndef STRICT //Enforce strict definitions of Windows data types
	#define STRICT
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN //Exclude rarely-used stuff from Windows headers
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "BboxData.h"
#include "TextFileOps.h"
#include <fstream>
#include <string>
#include <Windows.h>

wstring BboxData::modelNames[] = { L"" };
wstring BboxData::attrNames[] = { L"" };

BboxData::BboxData()
{
	bStaticVarsInitialized = false;
}

BboxData::BboxData(const WCHAR setCsvName[])
{
	csvName = static_cast<wstring>(setCsvName);

	if (TextFileOps::inst().fetchDelimitedSlice(getInArchive(), csvName, modelNames, k_num_model, false, 2) == k_num_model &&
		TextFileOps::inst().fetchDelimitedSlice(getInArchive(), csvName, attrNames, k_num_attr, true, 2) == k_num_attr)
	{
		numColumns = k_num_attr;
		bStaticVarsInitialized = true;
	}
}

bool BboxData::bReadModelFiles()
{
	bool bParseSuccess;

	if (bUnpackModels() && bDecompileModels())
	{
		//******Parse model files******
		bParseSuccess = true;
	}

	return bParseSuccess;
}

bool BboxData::bUnpackModels()
{
	bool bSuccess;

	//******Call other program to do this or implement someone else's code******
	bSuccess = true; //Set to true until function body is coded

	return bSuccess;
}

bool BboxData::bDecompileModels()
{
	bool bSuccess;

	//******Call other program to do this or implement someone else's code******
	bSuccess = true; //Set to true until function body is coded

	return bSuccess;
}

bool BboxData::bCheckArchive(BboxData &newBbox, wstring &badRowName, wstring &badColName, wstring &badNewVal, wstring &badAVal)
{
	bool bUpdate = false;
	int j;

	for (int i = 0; i < k_num_model; ++i) //&& !bUpdate will terminate the loop after first mismatch
	{
		if (bUpdate = bCheckArchiveRow(modelNames[i], attrNames, bboxData[i], bboxData[i], j)) //Single = is intentional
			//^^^^^^Until bReadModelFiles() is coded
		//if (bUpdate = bCheckArchiveRow(modelNames[i], attrNames, newBbox.bboxData[i], bboxData[i], j))
		{
			badRowName = modelNames[i];
			badColName = attrNames[j];
			badNewVal = newBbox.bboxData[i][j];
			badAVal = bboxData[i][j];
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
	bool bWriteSuccess = false;

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

		bWriteSuccess = true;
		getOutArchive().close();
	}

	return bWriteSuccess;
}
