#pragma once


// CFileExplorerTreeCtrl

class CFileExplorerTreeCtrl : public CMFCShellTreeCtrl
{
	DECLARE_DYNAMIC(CFileExplorerTreeCtrl)

public:
	CFileExplorerTreeCtrl();
	virtual ~CFileExplorerTreeCtrl();
	
	virtual HRESULT EnumObjects(HTREEITEM hParentItem, LPSHELLFOLDER pParentFolder, LPITEMIDLIST pidlParent);
	CStringArray m_FileExt;
	bool MatchExtension(CString filename);
	BOOL GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, DWORD dwFlags, CString &strName);
	void SetFileExt(CString strExt);
	BOOL GetItemPathName(CString& strPath, HTREEITEM htreeItem);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


