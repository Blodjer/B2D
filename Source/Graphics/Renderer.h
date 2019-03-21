#pragma once

class CViewport;

class CRenderer
{
public:
	CRenderer();
	~CRenderer();
	
    void PreRender();
    void PostRender();

private:
	void Clear();

};
