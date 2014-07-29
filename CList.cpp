#include <stdio.h>
#include "CList.h"
int main()
{
	LIST_INFO *p_list = NULL;
	p_list = CList_List_Create();
	p_list->valid = TRUE;
	LIST_ITEM *p_item = NULL;
	UINT32 i = 0;
	UINT32 ret = FALSE;
	while(i<10)
	{
		p_item = CList_Item_Create();
		ret = CList_Item_SetBuff(p_item,(UINT8 *)&i,sizeof(UINT32));
		ret = CList_Add_Head(p_list,p_item);
		i ++;
	}
	printf("len %d\n",p_list->list_len);
	ret = CList_Traverse_Head(p_list);
	printf("len %d\n",ret);
	ret = CList_Traverse_Tail(p_list);
	printf("len %d\n",ret);

	/** remove **/
	while(ret)
	{
		ret = CList_Remove_Head(p_list);
		if(ret == TRUE)
		{
			printf("len %d\n",p_list->list_len);
		}
	}
	i = 0;
	while(i<10)
	{
		p_item = CList_Item_Create();
		ret = CList_Item_SetBuff(p_item,(UINT8 *)&i,sizeof(UINT32));
		ret = CList_Add_Head(p_list,p_item);
		i ++;
	}
	while(ret)
	{
		ret = CList_Remove_Tail(p_list);
		if(ret == TRUE)
		{
			printf("len %d\n",p_list->list_len);
		}
	}
	return 0;
}
