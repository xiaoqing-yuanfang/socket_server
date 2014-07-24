#include <iostream>
#include "common.h"
#include "CLogger.h"
#include "CDbManager.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define MANAGER_SERVICE_PROTOCOL_MAGIC (0xB9E6D4F2)
typedef struct interface_servicemodule_rulemanager_req
{
	UINT32 magic; /** 0xb9e6d4f2 **/
	UINT16 msg_type;
	UINT16 msg_subtype;
	UINT32 msg_id;
	UINT32 msg_len;
}MANAGER_SERVICE_REQ,*P_MANAGER_SERVICE_REQ;
typedef struct interface_servicemodule_rulemanager_ack
{
	UINT32 magic; /** 0xb9e6d4f2 **/
	UINT16 msg_type;
	UINT16 msg_subtype;
	UINT16 return_code;
	UINT16 return_subcode;
}MANAGER_SERVICE_ACK,*P_MANAGER_SERVICE_ACK;
typedef enum manager_service_error_type
{
	MANAGER_SERVICE_OK = 0x00,
	MANAGER_SERVICE_TRANSPORT_DATA_ERROR = 0x01,
}MANAGER_SERVICE_ERROR_TYPE;
typedef enum manager_service_error_subtype
{
	MANAGER_SERVICE_MSG_FORMAT_ERROR = 0x0100,
	MANAGER_SERVICE_XML_FORMAT_ERROR = 0x0101,
	MANAGER_SERVICE_XML_LACK_FIELD_ERROR = 0x0102,
	MANAGER_SERVICE_SOCKET_ERROR = 0x0200,
	MANAGER_SERVICE_BUFFER_ERROR = 0x0300,
}MANAGER_SERVICE_ERROR_SUBTYPE;
typedef union BIG_LITTLE_ENDIAN_TEST
{
	UINT32 i;
	UINT8 c;
}BIG_LITTLE_ENDIAN_TEST;

class ACE_TaskBase
{
	private:
		CLogger m_logger;
		bool m_stopped;
		bool m_wouldStop;
		UINT8 *m_tempDir;
		UINT8 *m_ruleInputDir;
		pthread_t m_handleThread;
	public:
		ACE_TaskBase();
		~ACE_TaskBase();
		void Init();
		void svc();
		void Start();
		void Close();
};
class CSocketReadThread:ACE_TaskBase
{
	private:
		UINT8 *m_listenIp;
		UINT16 m_listenPort;
		UINT32 m_connFd;
		sockaddr_in m_sockaddrManager;
		UINT32 m_socketFd;
	public:
		CSocketReadThread();
		~CSocketReadThread();

		void Init(UINT32 connFd);
		void Start();
		void Close();
};

class CRuleManager
{
	public:
		CRuleManager()
		{
		};
		~CRuleManager()
		{
			close(m_socketManager);
		};
        void Init(UINT32 interface_type,char* listen_ip, UINT16 listen_port,UINT8 *file_name);
        void Run();
		void Stop()
		{

		};
	private:
		bool m_stopped;
		bool m_wouldStop;
		CLogger m_logger;
		CDbManager m_dbManager;
		struct sockaddr_in m_sockaddr_manager;
		struct sockaddr_in m_sockaddr_client;
		UINT32 m_interfaceType;
		UINT32 m_socketManager;
};