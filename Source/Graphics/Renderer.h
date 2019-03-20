#pragma once

class CViewport;

class CRenderer
{
public:
	CRenderer();
	~CRenderer();
	
	void Draw(CViewport const* const viewport);
	void Clear();

};
