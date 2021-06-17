#include "vbe_idc_user_manager.h"


#include "dsc/dsc_log.h"

ACE_INT32 CVbeIdcUserManager::Init()
{
	// TODO ����
	CDscString strStoragePathName;
	CDscString strBlockIdStackPathName;
	CDscString strPageCachePathName;
	CDscString strPageHeadCachePathName;
	bool bDirectIO = true;

	auto nReturnCode = m_dbUserInfo.OpenStorage(strStoragePathName, strBlockIdStackPathName, strPageCachePathName, strPageHeadCachePathName, bDirectIO);

	if (nReturnCode)
	{
		DSC_RUN_LOG_ERROR("open storage failed %d", nReturnCode);

		return nReturnCode;
	}

	return 0;
}

bool CVbeIdcUserManager::LockUser(ACE_UINT64 nHashKey, ACE_UINT32 nLockKey)
{
	auto userState = GetUserState(nHashKey);

	if (!userState)
	{
		// �ϲ������û�Ӧ��ȷ���û��ڱ���cache�д��ڣ�ȷ�����ﲻ���ߵ�
		DSC_RUN_LOG_ERROR("try to lock not exist user! (%llu)", nHashKey);
		return false;
	}

	CVbeIdcUserInfo userinfo;

	if (m_dbUserInfo.Read(userinfo, userState->m_dbRecord))
	{
		DSC_RUN_LOG_ERROR("read user failed! (user hash key=%llu, m_nPageID=%u, m_nPageBlockID=%u, m_nNumberStamp=%u)", 
			nHashKey, userState->m_dbRecord.m_nPageID, userState->m_dbRecord.m_nPageBlockID, userState->m_dbRecord.m_nNumberStamp);

		return false;
	}

	if (userinfo.m_bLocked)
	{
		return false;
	}
	
	userinfo.m_bLocked = true;
	userinfo.m_nLockKey = nLockKey;

	m_dbUserInfo.Update(userinfo, userState->m_dbRecord);

	return true;
}

void CVbeIdcUserManager::UnlockUser(ACE_UINT64 nHashKey)
{
	auto userState = GetUserState(nHashKey);

	if (userState)
	{
		CVbeIdcUserInfo userinfo;

		if (m_dbUserInfo.Read(userinfo, userState->m_dbRecord))
		{
			DSC_RUN_LOG_ERROR("read user failed! (user hash key=%llu, m_nPageID=%u, m_nPageBlockID=%u, m_nNumberStamp=%u)",
				nHashKey, userState->m_dbRecord.m_nPageID, userState->m_dbRecord.m_nPageBlockID, userState->m_dbRecord.m_nNumberStamp);
		}
		else
		{
			userinfo.m_bLocked = false;
			m_dbUserInfo.Update(userinfo, userState->m_dbRecord);
		}
	}
}

bool CVbeIdcUserManager::CheckLock(ACE_UINT64 nHashKey, ACE_UINT32 nLockKey)
{
	auto userState = GetUserState(nHashKey);

	if (userState)
	{
		CVbeIdcUserInfo userinfo;

		if (m_dbUserInfo.Read(userinfo, userState->m_dbRecord))
		{
			DSC_RUN_LOG_ERROR("read user failed! (user hash key=%llu, m_nPageID=%u, m_nPageBlockID=%u, m_nNumberStamp=%u)",
				nHashKey, userState->m_dbRecord.m_nPageID, userState->m_dbRecord.m_nPageBlockID, userState->m_dbRecord.m_nNumberStamp);
		}
		else
		{
			if (userinfo.m_bLocked == true && userinfo.m_nLockKey == nLockKey)
			{
				return true;
			}
		}
	}

	return false;
}

void CVbeIdcUserManager::PutUserInfo(CVbeIdcUserInfo& userinfo, ACE_UINT32 nCoin)
{
	auto userState = GetUserState(userinfo.m_nHashKey);

	if (userState)
	{
		m_dbUserInfo.Update(userinfo, userState->m_dbRecord);
	}
	else
	{
		SBS::SbsRecord record;
		if (m_dbUserInfo.Insert(record, userinfo) == 0)
		{
			userState = DSC_THREAD_TYPE_NEW(CUserState) CUserState;
			userState->m_nCoin = nCoin;
			userState->m_dbRecord = record;

			m_mapUserState[userinfo.m_nHashKey] = userState;
		}
	}
}

ACE_INT32 CVbeIdcUserManager::GetUserInfo(ACE_UINT64 nHashKey, CVbeIdcUserInfo& userinfo)
{
	auto userState = GetUserState(nHashKey);

	if (userState)
	{
		CVbeIdcUserInfo userinfo;

		if (m_dbUserInfo.Read(userinfo, userState->m_dbRecord))
		{
			DSC_RUN_LOG_ERROR("read user failed! (user hash key=%llu, m_nPageID=%u, m_nPageBlockID=%u, m_nNumberStamp=%u)",
				nHashKey, userState->m_dbRecord.m_nPageID, userState->m_dbRecord.m_nPageBlockID, userState->m_dbRecord.m_nNumberStamp);
		}
		else
		{
			return 0;
		}
	}
	return -1;
}

ACE_UINT32 CVbeIdcUserManager::AllocLockKey()
{
	return m_nLockKey++;
}

CVbeIdcUserManager::CUserState* CVbeIdcUserManager::GetUserState(ACE_UINT64 nHashKey)
{
	auto it = m_mapUserState.find(nHashKey);

	if (it != m_mapUserState.end())
	{
		return it->second;
	}
	
	return nullptr;
}
