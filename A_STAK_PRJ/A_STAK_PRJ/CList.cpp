#include <iostream>
#include "CList.h"

CList::CList()
{
	head = new stListNode;
	tail = new stListNode;

	head->_data = NULL;
	head->_Prev = NULL;
	head->_Next = tail;
	tail->_data = NULL;
	tail->_Next = NULL;
	tail->_Prev = head;

	_size = 0;
}

CList::~CList()
{
	delete head;
	delete tail;
}

CList::stListNode* CList::GetStartNode()
{
	return head->_Next;
}

CList::stListNode* CList::GetEndNode()
{
	return tail;
}

void CList::Push(stNode* data)
{
	stListNode* node = new stListNode;
	node->_data = data;
	
	node->_data = data;
	node->_Prev = head;
	node->_Next = head->_Next;

	head->_Next->_Prev = node;
	head->_Next = node;

	/*node->_Next = head->_Next;
	head->_Next = node;*/
		
	_size++;
	return;
}

stNode* CList::Pop()
{
	stListNode* ret = head->_Next;
	if (ret == tail) //ежеж ╨С ╟м
		return NULL;

	/*head->_Next = ret->_Next;*/
	ret->_Prev->_Next = ret->_Next;
	ret->_Next->_Prev = ret->_Prev;

	stNode* retData = ret->_data;
	_size -= 1;

	delete ret;
	return retData;
}

void CList::Sort()
{
	stListNode* tmpHead = head;
	stListNode* swapNode;
	
	if (head->_Next == tail)
		return;
	//while (tmpHead->_Next != tail)
	//{
	//	if (tmpHead->_data->_dValueF > tmpHead->_Next->_data->_dValueF)
	//	{			
	//		swapNode = tmpHead->_Next;
	//		tmpHead->_Next = tmpHead->_Next->_Next;
	//		swapNode->_Next = head->_Next;
	//		head->_Next = tmpHead = swapNode;
	//		continue;
	//	}
	//	tmpHead = tmpHead->_Next;
	//}
	while (tmpHead->_Next != tail)
	{
		swapNode = tmpHead->_Next;
		if (swapNode->_Next == tail)
			break;
		while (swapNode != head && swapNode->_data->_dValueF > swapNode->_Next->_data->_dValueF)
		{
			Swap(swapNode, swapNode->_Next);

			swapNode = swapNode->_Prev;
		}
		tmpHead = tmpHead->_Next;
	}
}

void CList::Swap(stListNode* node, stListNode* node2)
{
	stNode data = *node->_data;
	*node->_data = *node2->_data;
	*node2->_data = data;
}

void CList::Clear()
{
	
	while (head->_Next != tail)
	{
		Pop();
	}
}

bool CList::Search(int X, int Y, stNode** out)
{
	stListNode* tmp = head->_Next;
	while (tmp != tail)
	{
		if (tmp->_data->_iX == X &&
			tmp->_data->_iY == Y)
		{
			*out = tmp->_data;
			return false;
		}
		tmp = tmp->_Next;
	}
	out = NULL;
	return true;
}