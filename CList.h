#include "common.h"
#include <stdio.h>
#include <memory.h>
#include <malloc.h>

typedef struct LIST_ITEM
{
	BOOL valid;
	UINT8 *buff;
	UINT32 buff_len;
	struct LIST_ITEM *prev;
	struct LIST_ITEM *next;
}LIST_ITEM;
typedef struct LIST_INFO
{
	BOOL valid;
	UINT32 list_len;
	struct LIST_ITEM  *buff_head;
	struct LIST_ITEM  *buff_tail;
}LIST_INFO;
UINT32 CList_Add_Head(LIST_INFO *list,LIST_ITEM *item)
{
	if(list == NULL || item == NULL)
	{
		return FALSE;
	}
	if(item->valid != TRUE || list->valid != TRUE)
	{
		return FALSE;
	}
	if(list->buff_head == NULL && list->buff_tail == NULL)
	{
		list->buff_head = item;
		list->buff_tail = item;
		item->next = NULL;
		item->prev = NULL;
		list->list_len += 1;
	}else
	{
		item->next = list->buff_head;
		item->prev = NULL;
		list->buff_head->prev = item;
		list->buff_head = item;
		list->list_len += 1;
	}
	return TRUE;
}
UINT32 CList_Add_Tail(LIST_INFO *list,LIST_ITEM *item)
{
	if(list == NULL || item == NULL)
	{
		return FALSE;
	}
	if(item->valid != TRUE || list->valid != TRUE)
	{
		return FALSE;
	}
	if(list->buff_head == NULL || list->buff_tail == NULL)
	{
		list->buff_head = item;
		list->buff_tail = item;
		item->next = NULL;
		item->prev = NULL;
		list->list_len += 1;
	}else
	{
		item->next = NULL;
		item->prev = list->buff_tail;
		list->buff_tail->next = item;
		list->buff_tail = item;
		list->list_len += 1;
	}
	return TRUE;
}
UINT32 CList_Remove_Head(LIST_INFO *list)
{
	if(list == NULL)
	{
		return FALSE;
	}
	if(list->valid != TRUE)
	{
		return FALSE;
	}
	if(list->buff_head == NULL || list->buff_tail == NULL)
	{
		return FALSE;
	}
	else if(list->buff_head == list->buff_tail)
	{
		if(list->buff_head->valid)
		{
			free(list->buff_head->buff);		
		}
		free(list->buff_head);
		list->buff_head = NULL;
		list->buff_tail = NULL;
		list->list_len -= 1;
	}
	else
	{
		if(list->buff_head->valid)
		{
			free(list->buff_head->buff);		
		}
		LIST_ITEM *p_head = list->buff_head;
		list->buff_head = p_head->next;
		list->buff_head->prev = NULL;
		free(p_head);
		list->list_len -= 1;
	}
	return TRUE;
}
UINT32 CList_Remove_Tail(LIST_INFO *list)
{
	if(list == NULL)
	{
		return FALSE;
	}
	if(list->valid != TRUE)
	{
		return FALSE;
	}
	if(list->buff_head == NULL || list->buff_tail == NULL)
	{
		return FALSE;
	}
	else if(list->buff_head == list->buff_tail)
	{
		if(list->buff_head->valid)
		{
			free(list->buff_head->buff);		
		}
		free(list->buff_head);
		list->buff_head = NULL;
		list->buff_tail = NULL;
		list->list_len -= 1;
	}
	else
	{
		if(list->buff_tail->valid)
		{
			free(list->buff_tail->buff);		
		}
		LIST_ITEM *p_tail = list->buff_tail;
		list->buff_tail = p_tail->prev;
		list->buff_tail->next  = NULL;
		free(p_tail);
		list->list_len -= 1;
	}
	return TRUE;
}
LIST_ITEM *CList_Get_Head(LIST_INFO *list)
{
	return list->buff_head;
}
LIST_ITEM *CList_Get_Tail(LIST_INFO *list)
{
	return list->buff_tail;
}
LIST_INFO *CList_List_Create()
{
	LIST_INFO *list = (LIST_INFO *)malloc(sizeof(LIST_INFO));
	if(list == NULL)
	{
		return FALSE;
	}
	memset(list,0,sizeof(LIST_INFO));
	list->valid = TRUE;
	return list;
}
LIST_ITEM *CList_Item_Create()
{
	LIST_ITEM *item = (LIST_ITEM *)malloc(sizeof(LIST_ITEM));
	if(item == NULL)
	{
		return FALSE;
	}
	memset(item,0,sizeof(LIST_ITEM));
	item->valid = TRUE;
	return item;
}
UINT32 CList_Item_SetBuff(LIST_ITEM *item, UINT8 *buff, UINT32 buff_len)
{
	if(item == NULL || buff == NULL || buff_len == 0)
	{
		return FALSE;
	}
	item->buff  = (UINT8 *)malloc(sizeof(UINT8));
	if(item->buff  == NULL)
	{
		return FALSE;
	}
	memcpy(item->buff,buff,buff_len);
	item->buff_len = buff_len;
	return TRUE;
}
UINT32 CList_List_Destroy(LIST_INFO *list)
{
	if(list == NULL)
	{
		return TRUE;
	}
	if(list->buff_head == NULL && list->buff_tail == NULL)
	{
		free(list);	
	}
	LIST_ITEM *list_head = list->buff_head;
	while(list_head)
	{
		CList_Remove_Head(list);
		list_head = list->buff_head;
	}
	free(list);
	return TRUE;
}
UINT32 CList_Traverse_Head(LIST_INFO *p_list)
{
	if(p_list == NULL)
	{
		return 0;
	}
	if(p_list->buff_head == NULL || p_list->buff_tail == NULL)
	{	
		return 0;
	}
	UINT32 len = 0;

	LIST_ITEM *p_cur = p_list->buff_head;
	while(p_cur)
	{
		printf("%d\n",*(UINT32 *)p_cur->buff);
		len += 1;
		p_cur = p_cur->next;
	}
	return len;
}
UINT32 CList_Traverse_Tail(LIST_INFO *p_list)
{
	if(p_list == NULL)
	{
		return 0;
	}
	if(p_list->buff_head == NULL || p_list->buff_tail == NULL)
	{	
		return 0;
	}
	UINT32 len = 0;

	LIST_ITEM *p_cur = p_list->buff_tail;
	while(p_cur)
	{
		printf("%d\n",*(UINT32 *)p_cur->buff);
		len += 1;
		p_cur = p_cur->prev;
	}
	return len;
}
