#include "BboxData.h"
#include "TextFileOps.h"
#include <fstream>
#include <string>

string BboxData::modelNames[] = { "" };
string BboxData::attrNames[] = { "" };

BboxData::BboxData(const char setCsvName[])
{
	csvName = static_cast<string>(setCsvName);

	if (TextFileOps::inst().fetchDelimitedSlice(getInArchive(), csvName, modelNames, k_num_model, false, 2) == k_num_model &&
		TextFileOps::inst().fetchDelimitedSlice(getInArchive(), csvName, attrNames, k_num_attr, true, 2) == k_num_attr)
	{
		numColumns = k_num_attr;
		bStaticVarsInitialized = true;
	}
}

BboxData::BboxData()
{
	bStaticVarsInitialized = false;
}

bool BboxData::getBStaticVarsInitialized()
{
	return bStaticVarsInitialized;
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

bool BboxData::bCheckArchive(BboxData &newBbox)
{
	bool bUpdate = false;

	for (int i = 0; i < k_num_model && !bUpdate; ++i) //&& !bUpdate will terminate the loop after first mismatch
		bUpdate = bCheckArchiveRow(modelNames[i], attrNames, bboxData[i], bboxData[i]); //Until bReadModelFiles() is coded
		//bUpdate = bCheckArchiveRow(modelNames[i], attrNames, newBbox.bboxData[i], bboxData[i]);

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
