/*

Copyright (C) 2018  Daisuke Kamiyama (https://www.mimikaki.net)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
/*

APurgeManager
#693

*/

//#693
//★未レビュー・未テスト（未使用）

#include "APurgeManager.h"
#include "ABase.h"
#include "../Wrapper.h"

APurgeManager*	APurgeManager::sPurgeManager = NULL;

/**
*/
class APurgeManagerData
{
  public:
	AFileAcc	mPurgeFolder;
	AUniqIDArray			mArray_UniqID;
	AHashArray<const void*>		mArray_KeyMemoryPtr;
	AArray<AByteCount>		mArray_ByteCount;
	AObjectArray<AFileAcc>	mArray_PurgeFile;
};

/**
*/
APurgeManager::APurgeManager() : mPurgeManagerData(NULL)
{
	sPurgeManager = this;
	//
	mPurgeManagerData = new APurgeManagerData();
	//
	AFileAcc	tempFolder;
	AFileWrapper::GetTempFolder(tempFolder);
	mPurgeManagerData->mPurgeFolder.SpecifyUniqChildFile(tempFolder,"PurgeManager");
	mPurgeManagerData->mPurgeFolder.CreateFolder();
}

/**
*/
APurgeManager::~APurgeManager()
{
	//
	delete mPurgeManagerData;
}

/**
*/
void	APurgeManager::Purge( const void* inKeyMemoryPtr, void* inPurgeMemoryPtr, const AByteCount inByteCount )
{
	AIndex	index = mPurgeManagerData->mArray_UniqID.AddItem();
	AUniqID	uniqID = mPurgeManagerData->mArray_UniqID.Get(index);
	mPurgeManagerData->mArray_KeyMemoryPtr.Add(inKeyMemoryPtr);
	mPurgeManagerData->mArray_ByteCount.Add(inByteCount);
	//
	AText	filename;
	filename.SetNumber(uniqID.val);
	AFileAcc	purgeFile;
	purgeFile.SpecifyUniqChildFile(mPurgeManagerData->mPurgeFolder,filename);
	purgeFile.CreateFile();
	//
	mPurgeManagerData->mArray_PurgeFile.GetObject(mPurgeManagerData->mArray_PurgeFile.AddNewObject()).CopyFrom(purgeFile);
	//
	AText	data;
	data.AddFromTextPtr((AConstUCharPtr)inPurgeMemoryPtr,inByteCount);
	purgeFile.WriteFile(data);
	//
	AMemoryWrapper::Free(inPurgeMemoryPtr);
}

/**
*/
void*	APurgeManager::RestoreFromPurge( const void* inKeyMemoryPtr )
{
	AIndex	index = mPurgeManagerData->mArray_KeyMemoryPtr.Find(inKeyMemoryPtr);
	AFileAcc	purgeFile = mPurgeManagerData->mArray_PurgeFile.GetObjectConst(index);
	//
	AText	data;
	purgeFile.ReadTo(data);
	//
	AByteCount	byteCount = mPurgeManagerData->mArray_ByteCount.Get(index);
	void*	ptr = AMemoryWrapper::Malloc(byteCount);
	data.CopyToTextPtr((AUCharPtr)ptr,byteCount,0,byteCount);
	//
	purgeFile.DeleteFile();
	mPurgeManagerData->mArray_UniqID.Delete1Item(index);
	mPurgeManagerData->mArray_KeyMemoryPtr.Delete1(index);
	mPurgeManagerData->mArray_ByteCount.Delete1(index);
	mPurgeManagerData->mArray_PurgeFile.Delete1Object(index);
	//
	return ptr;
}

/**
*/
void	APurgeManager::DeletePurge( const void* inKeyMemoryPtr )
{
	AIndex	index = mPurgeManagerData->mArray_KeyMemoryPtr.Find(inKeyMemoryPtr);
	AFileAcc	purgeFile = mPurgeManagerData->mArray_PurgeFile.GetObjectConst(index);
	//
	purgeFile.DeleteFile();
	mPurgeManagerData->mArray_UniqID.Delete1Item(index);
	mPurgeManagerData->mArray_KeyMemoryPtr.Delete1(index);
	mPurgeManagerData->mArray_ByteCount.Delete1(index);
	mPurgeManagerData->mArray_PurgeFile.Delete1Object(index);
}




