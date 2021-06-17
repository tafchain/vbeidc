#include "vbe_idc/vbe_idc_hls/vbe_idc_propose_manager.h"
#include "dsc/dsc_log.h"

CVbeIdcProposeManager::CVbeIdcProposeManager()
{
}

CVbeIdcProposeManager::~CVbeIdcProposeManager()
{
}

ACE_INT32 CVbeIdcProposeManager::Init()
{
	// TODO ²ÎÊý
	CDscString strStoragePathName;
	CDscString strBlockIdStackPathName;
	CDscString strPageCachePathName;
	CDscString strPageHeadCachePathName;
	bool bDirectIO = true;

	auto nReturnCode = m_dbPropose.OpenStorage(strStoragePathName, strBlockIdStackPathName, strPageCachePathName, strPageHeadCachePathName, bDirectIO);

	if (nReturnCode)
	{
		DSC_RUN_LOG_ERROR("open storage failed %d", nReturnCode);

		return nReturnCode;
	}

	return 0;
	return ACE_INT32();
}

ACE_INT32 CVbeIdcProposeManager::InsertPropose(SBS::SbsRecord& record, CStorageProposeItem& item)
{
	return m_dbPropose.Insert(record, item);
}

ACE_INT32 CVbeIdcProposeManager::DeletePropose(SBS::SbsRecord& record)
{
	return m_dbPropose.DeleteBlock(record);
}
