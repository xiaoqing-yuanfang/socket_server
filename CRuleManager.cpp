#include "CRuleManager.h"
#include <pthread.h>
#include <signal.h>

extern UINT32 debug_level;
UINT32 manager_service_isbigendian()
{
	/**
	 * 1 big
	 * 0 little
	 */
	BIG_LITTLE_ENDIAN_TEST a;
	memset(&a,0,sizeof(BIG_LITTLE_ENDIAN_TEST));
	a.c = 1;
	if(a.i == 1)
	{
		return 0;
	}else
	{
		return 1;
	}
	return 0;
}
UINT16 manager_service_get_return_code(UINT16 return_subcode)
{
	/*
	 * retrun return code from return_subcode 
	 */
	int ret = 0;
	ret = manager_service_isbigendian();
	if(ret == 1)
	{
		ret = *(UINT8 *)&return_subcode;
	}else
	{
		ret = *((UINT8 *)&return_subcode+1);
	}
	return (UINT16)ret;
}
UINT32 manager_service_rcv_data(UINT32 sockfd,UINT8 *p_buff,UINT32 buff_len)
{
	if(p_buff == NULL)
	{
		return MANAGER_SERVICE_BUFFER_ERROR;
	}
	if(buff_len == 0)
	{
		return MANAGER_SERVICE_OK;
	}
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(sockfd,&readfds);
	int ret = 0;
	int recv_bytes = 0;
	int bytes_left = buff_len;
	while(bytes_left > 0)
	{
		ret = select(sockfd+1,&readfds,0,0,0);	
		if(ret >0)
		{
			/** recv **/
			recv_bytes = recv(sockfd,p_buff,bytes_left,0);
			if(debug_level == 1)
			{
				printf("manager_service_rev_data want_bytes:%d real_recv_bytes %d\n",bytes_left,recv_bytes);
			}
			if(recv_bytes <= 0)
			{
				/** sock failed **/
				return MANAGER_SERVICE_SOCKET_ERROR;
			}else
			{
				bytes_left -= recv_bytes;
				p_buff += recv_bytes;
			}
		}
	}
	return MANAGER_SERVICE_OK;
}
UINT32 manager_service_send_data(UINT32 sockfd,UINT8 *p_buff,UINT32 buff_len)
{
	if(p_buff == NULL)
	{
		return MANAGER_SERVICE_BUFFER_ERROR;
	}
	fd_set writefds;
	FD_ZERO(&writefds);
	FD_SET(sockfd,&writefds);
	int ret = 0;
	int send_bytes = 0;
	int bytes_left = buff_len;
	while(bytes_left > 0)
	{
		ret = select(sockfd+1,0,&writefds,0,0);	
		if(ret >0 && FD_ISSET(sockfd,&writefds))
		{
			/** recv **/
			send_bytes = send(sockfd,p_buff,bytes_left,MSG_NOSIGNAL);
			if(send_bytes <= 0)
			{
				/** sock failed **/
				return MANAGER_SERVICE_SOCKET_ERROR;
			}else
			{
				bytes_left -= send_bytes;
				p_buff += send_bytes;
			}
		}
	}
	return MANAGER_SERVICE_OK;
}
void manager_service_req_ntoh(MANAGER_SERVICE_REQ *req)
{
	req->magic == ntohl(req->magic);
	req->msg_id == ntohl(req->msg_id);
	req->msg_len = ntohl(req->msg_len);
	req->msg_type = ntohs(req->msg_type);
	req->msg_subtype = ntohs(req->msg_subtype);
}
void manager_service_ack_hton(MANAGER_SERVICE_ACK *ack)
{
	ack->magic = htonl(ack->magic);
	ack->msg_type = htons(ack->msg_type);
	ack->msg_subtype = htons(ack->msg_subtype);
	ack->return_code = htons(ack->return_code);
	ack->return_subcode = htons(ack->return_subcode);
}
UINT32 manager_service_msg_format_valid(MANAGER_SERVICE_REQ *req)
{
	if(req == NULL)
	{
		return MANAGER_SERVICE_BUFFER_ERROR;
	}
	//if(req->magic != MANAGER_SERVICE_PROTOCOL_MAGIC)
	//{
	//	return MANAGER_SERVICE_MSG_FORMAT_ERROR;
	//}
	return MANAGER_SERVICE_OK;
}
UINT32 manager_service_xml_valid(UINT8 *xml_data)
{
	if(xml_data == NULL)
	{
		return MANAGER_SERVICE_BUFFER_ERROR;
	}
	return MANAGER_SERVICE_OK;
}
UINT32 manager_service_rcv_deal(UINT32 sockfd)
{
	if(sockfd <= 0)
	{
		return MANAGER_SERVICE_SOCKET_ERROR;
	}
	while(1)
	{
		MANAGER_SERVICE_REQ  *buf_head = (MANAGER_SERVICE_REQ *)malloc(sizeof(MANAGER_SERVICE_REQ));
		if(buf_head == NULL)
		{
			return MANAGER_SERVICE_BUFFER_ERROR;
		}else
		{
		    memset(buf_head,0,sizeof(MANAGER_SERVICE_REQ));
		}
		int ret = 0;
		/* recv head */
		ret = manager_service_rcv_data(sockfd,(UINT8 *)buf_head,sizeof(MANAGER_SERVICE_REQ));
		if(ret != MANAGER_SERVICE_OK)
		{
			free(buf_head);
			/*
			 * log 
			 */
			return ret;
		}
		manager_service_req_ntoh(buf_head);
		UINT32 body_len = buf_head->msg_len;	
		UINT8 *buf_body = (UINT8 *)malloc(body_len);
		
		if(buf_body == NULL)
		{
			free(buf_head);
			return MANAGER_SERVICE_BUFFER_ERROR;
		}else
		{
		    memset(buf_body,0,sizeof(body_len));
		}
		/** recv body **/
		ret = manager_service_rcv_data(sockfd,(UINT8 *)buf_body,body_len);
		if(ret != MANAGER_SERVICE_OK)
		{
			free(buf_head);
			free(buf_body);
			/*
			 * log 
			 */
			return ret;
		}
		/*
		 * valid 
		 */
		ret = manager_service_msg_format_valid(buf_head);
		if(ret != MANAGER_SERVICE_OK)
		{
			free(buf_head);
			free(buf_body);
			/*
			 * log 
			 */
			return ret;
		}
		ret = manager_service_xml_valid(buf_body);
		if(ret != MANAGER_SERVICE_OK)
		{
			free(buf_head);
			free(buf_body);
			/*
			 * log 
			 */
			return ret;
		}
		/*
		 * send clietns return code 
		 */
		MANAGER_SERVICE_ACK *buf_ack = (MANAGER_SERVICE_ACK *)malloc(sizeof(MANAGER_SERVICE_ACK));
		if(buf_ack == NULL)
		{
			free(buf_head);
			free(buf_body);
			free(buf_ack);
			return MANAGER_SERVICE_BUFFER_ERROR;
		}else
		{
		    memset(buf_ack,0,sizeof(MANAGER_SERVICE_ACK));
		}
		buf_ack->magic = MANAGER_SERVICE_PROTOCOL_MAGIC;
		buf_ack->msg_type = buf_head->msg_type;
		buf_ack->msg_subtype = buf_head->msg_subtype;
		buf_ack->return_subcode = ret;
		buf_ack->return_code = manager_service_get_return_code((UINT16)ret);
		manager_service_ack_hton(buf_ack);
		ret = manager_service_send_data(sockfd,(UINT8 *)buf_ack,sizeof(MANAGER_SERVICE_ACK));
		if(ret != 0)
		{
			free(buf_head);
			free(buf_body);
			free(buf_ack);
			return MANAGER_SERVICE_SOCKET_ERROR;
		}
		/*
		 * rule managemnet 
		 */

		/*
		 * store acl to database 
		 */
		
		/*
		 * free malloc buffer
		 */
		free(buf_head);
		free(buf_body);
		free(buf_ack);
	}
};
void *thread_deal_manager_service(void *arg)
{
	UINT32 ret = 0;
	UINT32 sockfd = *(UINT32*)arg;
	ret = manager_service_rcv_deal(sockfd);
	if(ret != MANAGER_SERVICE_OK)
	{
		if(debug_level == 1)
		{
			printf("error %d\n",ret);
		}
	}
	close(sockfd);
	return NULL;
}
void CRuleManager::Run()
{
	if(INTERFACE_TYPE_FILE == m_interfaceType)
	{
	}
	else if(INTERFACE_TYPE_SOCKET == m_interfaceType)
	{
		int ret = bind(m_socketManager,(struct sockaddr*)&m_sockaddr_manager,sizeof(m_sockaddr_manager));
		assert(ret != -1);
		ret = listen(m_socketManager,MAX_CONN);
		assert(ret != -1);
		while(1)
		{
			socklen_t sockaddr_client_len = sizeof(m_sockaddr_client);
			int m_connfd = accept(m_socketManager,(struct sockaddr *)&m_sockaddr_client,&sockaddr_client_len);
			if(m_connfd < 0)
			{
				/* logger */
			}
			else
			{
				/* get client info 
				 * and deal with this conn
				 * */
				pthread_t tid;
				pthread_create(&tid,NULL,thread_deal_manager_service,&m_connfd);

			}
		}
	}
};
void CRuleManager::Init(UINT32 interface_type,char* listen_ip, UINT16 listen_port,UINT8 *file_name)
{
	if(INTERFACE_TYPE_FILE == interface_type)
	{
		m_interfaceType = 	INTERFACE_TYPE_FILE ;
	}
	else if(INTERFACE_TYPE_SOCKET == interface_type)
	{
		m_interfaceType = 	INTERFACE_TYPE_SOCKET;
		bzero(&m_sockaddr_manager,sizeof(m_sockaddr_manager));
		bzero(&m_sockaddr_client,sizeof(m_sockaddr_client));
		m_sockaddr_manager.sin_family = AF_INET; 
		inet_pton(AF_INET,(const char *)listen_ip,&m_sockaddr_manager.sin_addr);
		m_sockaddr_manager.sin_port = htons(listen_port);
		m_socketManager = socket(AF_INET,SOCK_STREAM,0);
		assert(m_socketManager >0);
	}
};
ACE_TaskBase::ACE_TaskBase()
{
}
ACE_TaskBase::~ACE_TaskBase()
{
}
void ACE_TaskBase::Init()
{
}
void ACE_TaskBase::Start()
{
}
void ACE_TaskBase::Close()
{
}
void ACE_TaskBase::svc()
{
}

CSocketReadThread::CSocketReadThread()
{
}
CSocketReadThread::~CSocketReadThread()
{
}
void CSocketReadThread::Init(UINT32 connFd)
{
    this->m_connFd = connFd;
}
void CSocketReadThread::Start()
{
}
void CSocketReadThread::Close()
{
    close(this->m_connFd);
}