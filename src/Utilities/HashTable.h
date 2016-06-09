// HashTable.h: interface for the HashTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HASHTABLE_H__28C8E59E_9980_478E_B40B_2D37F41FA886__INCLUDED_)
#define AFX_HASHTABLE_H__28C8E59E_9980_478E_B40B_2D37F41FA886__INCLUDED_

#include "ImageStructs.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class HashEntry
{
public:
	HashEntry(){Occupied = false; ent = NULL; index = -1;}
	ColorEntry * ent;
	int index;
	bool Occupied;
};

class HashTable  
{
private:
	int Hash(colort c, ColorEntry * ent)
	{
		int hash;

		hash = c;
		hash = (hash << 1) ^ (int)ent ^ hash;
		hash &= 8191;

		return hash;
	}

public:

	int Find(colort c, ColorEntry * ent)
	{
		int hash = Hash(c,ent);
		while(table[hash].Occupied)
		{
			if(table[hash].ent->c == c && table[hash].ent->next == ent)
				return table[hash].index;
			hash++;
			hash &= 8191;
		}
		return -1;
	}

	void Clear()
		{for(int i = 0; i < 8192; i++) table[i] = HashEntry();}

	void InitTable(ColorEntry ColorMap[4096], int size)
	{
		int i;
		ColorEntry * ce;
		for(i = 0, ce = ColorMap;i < size;i++, ce++)
		{
			InsertEntry(ce,i);
		}
	}

	void InsertEntry(ColorEntry * ce, int i)
	{
		int hash;

		hash = Hash(ce->c, ce->next);

		while(table[hash].Occupied)
		{
			hash++;
			hash &= 8191;
		}

		table[hash].Occupied = true;
		table[hash].ent = ce;
		table[hash].index = i;
	}

	HashEntry table[8192];
};

#endif // !defined(AFX_HASHTABLE_H__28C8E59E_9980_478E_B40B_2D37F41FA886__INCLUDED_)
