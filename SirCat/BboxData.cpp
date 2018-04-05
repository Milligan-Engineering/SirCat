#include "BboxData.h"
#include <fstream>
#include <string>

using namespace std;

bool BboxData::bArchiveObjMade = false;
wstring BboxData::modelNames[] = { L"" };
wstring BboxData::attrNames[] = { L"" };

bool BboxData::bMakeBboxObjArchive(const wstring csvName)
{
	bool bSuccess = false;

	if (!bArchiveObjMade)
	{
		wstring *headers[2] = { modelNames, attrNames };
		const int sliceSize[2] = { k_num_model, k_num_attr };
		const bool sliceIsRow[2] = { false, true };
		const int numSlice[2] = { 1, 1 };

		Archive::csvName = csvName;
		numColumns = k_num_attr;
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

bool BboxData::bCheckArchive(BboxData &newBbox, wstring &badRowName, wstring &badColName, wstring &badNewVal,
	wstring &badArchiveVal)
{
	bool bUpdate = false;
	int j;

	for (int i = 0; i < k_num_model; ++i)
	{
		if (bUpdate = bCheckArchiveRow(modelNames[i], attrNames, bboxData[i], bboxData[i], j)) //Single = is intentional
			//^^^^^^Until bReadModelFiles() is coded
		//if (bUpdate = bCheckArchiveRow(modelNames[i], attrNames, newBbox.bboxData[i], bboxData[i], j))
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
