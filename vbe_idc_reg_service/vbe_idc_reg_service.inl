

template<typename MAP_TYPE, typename TYPE>
inline void CVbeIdcRegService::InsertSession(MAP_TYPE& mapSession, TYPE* session, CMcpHandler* mcpHandle)
{
	session->m_pRegServiceHandler = (CVbeRegServiceHandler*)mcpHandle;
	session->m_pRegServiceHandler->m_arrUserSession.Insert(session);
	session->m_nRegSessionID = m_nSessionID;

	SetDscTimer(session, EN_SESSION_TIMEOUT_VALUE);

	mapSession.DirectInsert(m_nSessionID++, session);
}

template<typename MAP_TYPE>
inline CVbeIdcRegService::IUserSession* CVbeIdcRegService::EraseSession(MAP_TYPE& mapSession, ACE_UINT32 sessionID)
{
	auto pSession = mapSession.Erase(sessionID);

	if (pSession)
	{
		CancelDscTimer(pSession);
		pSession->m_pRegServiceHandler->m_arrUserSession.Erase(pSession);
		return pSession;
	}

}
