#pragma once

struct stNode
{
	stNode()
	{
		_dValueG = 0;
		_iValueH = 0;
		_dValueF = 0;
		pParent = NULL;
	}
	int _iX;
	int _iY;
	double _dValueG;
	int _iValueH;
	double _dValueF;
	stNode* pParent;
};

class CList
{
public:
	struct stListNode
	{
		stNode* _data;
		stListNode* _Prev;
		stListNode* _Next;
	};

public:
	CList();
	~CList();
	void Push(stNode* data);
	stNode* Pop();
	void Sort();

	void Clear();
	bool Search(int X, int Y, stNode** out);

	stListNode* GetStartNode();
	stListNode* GetEndNode();

	
private:

	void Swap(stListNode* node, stListNode* node2);
	int _size ;
	stListNode* head;
	stListNode* tail;
};