// FileExplorerTreeCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "OrderBookVI.h"
#include "FileExplorerTreeCtrl.h"


// CFileExplorerTreeCtrl

IMPLEMENT_DYNAMIC(CFileExplorerTreeCtrl, CMFCShellTreeCtrl)

CFileExplorerTreeCtrl::CFileExplorerTreeCtrl()
{
}

CFileExplorerTreeCtrl::~CFileExplorerTreeCtrl()
{
}

void CFileExplorerTreeCtrl::SetFileExt(CString strExt)
{
	CString pattern=_T("|");
	
	int iPos = strExt.Find(pattern), iStart = 0;
	while (iPos != -1){
		m_FileExt.Add(strExt.Mid(iStart, iPos - iStart));
		iStart = iPos + pattern.GetLength();
		iPos = strExt.Find(pattern, iStart);
	}
	if (iStart != strExt.GetLength())m_FileExt.Add(strExt.Mid(iStart));
}


bool CFileExplorerTreeCtrl::MatchExtension(CString filename)
{
	int num=m_FileExt.GetCount();
	if (num==0) return TRUE;
	
	CString strTemp=_T("");
	int pos=filename.ReverseFind('.');
	if(pos-1)strTemp=filename.Mid(pos+1);
	else
		return FALSE;
	int i;for(i=0;i<num;i++){
		if(!strTemp.CollateNoCase(m_FileExt[i]))
			return TRUE;
	}return FALSE;
}

HRESULT CFileExplorerTreeCtrl::EnumObjects(HTREEITEM hParentItem, LPSHELLFOLDER pParentFolder, LPITEMIDLIST pidlParent){
	ASSERT_VALID(this);//
	ASSERT_VALID(afxShellManager);
	
	LPENUMIDLIST pEnum = NULL;
	HRESULT hr = pParentFolder->EnumObjects(NULL, m_dwFlags, &pEnum);
	if (FAILED(hr) || pEnum == NULL){
		return hr;
	}
	
	LPITEMIDLIST pidlTemp;DWORD dwFetched = 1;
	// Enumerate the item's PIDLs:
	while (pEnum->Next(1, &pidlTemp, &dwFetched) == S_OK && dwFetched){
		TVITEM tvItem;
		ZeroMemory(&tvItem, sizeof(tvItem));
		// Fill in the TV_ITEM structure for this item:
		tvItem.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;
		// AddRef the parent folder so it's pointer stays valid:
		pParentFolder->AddRef();
		// Put the private information in the lParam:
		LPAFX_SHELLITEMINFO pItem = (LPAFX_SHELLITEMINFO)GlobalAlloc(GPTR, sizeof(AFX_SHELLITEMINFO));
		ENSURE(pItem != NULL);

		pItem->pidlRel = pidlTemp;
		pItem->pidlFQ =((CWinAppEx *)AfxGetApp())->GetShellManager()->ConcatenateItem(pidlParent, pidlTemp);
		pItem->pParentFolder = pParentFolder;
		tvItem.lParam = (LPARAM)pItem;
		
		CString strItem = OnGetItemText(pItem);
		tvItem.pszText = strItem.GetBuffer(strItem.GetLength());
		tvItem.iImage = OnGetItemIcon(pItem, FALSE);
		tvItem.iSelectedImage = OnGetItemIcon(pItem, TRUE);

		// Determine if the item has children:
		DWORD dwAttribs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER | SFGAO_DISPLAYATTRMASK | SFGAO_CANRENAME | SFGAO_FILESYSANCESTOR;
		pParentFolder->GetAttributesOf(1, (LPCITEMIDLIST*) &pidlTemp, &dwAttribs);
		
		tvItem.cChildren = (dwAttribs & (SFGAO_HASSUBFOLDER | SFGAO_FILESYSANCESTOR));
		// Determine if the item is shared:
		if (dwAttribs & SFGAO_SHARE){
			tvItem.mask |= TVIF_STATE;
			tvItem.stateMask |= TVIS_OVERLAYMASK;
			tvItem.state |= INDEXTOOVERLAYMASK(1); //1 is the index for the shared overlay image
		}

		if (dwAttribs & SFGAO_GHOSTED){
			tvItem.mask |= LVIF_STATE;
			tvItem.stateMask |= LVIS_CUT;
			tvItem.state |= LVIS_CUT;
		}
		
		if (dwAttribs & SFGAO_FOLDER){}
		else{
			if(!MatchExtension(strItem))
				continue;
		}// Fill in the TV_INSERTSTRUCT structure for this item:
		
		TVINSERTSTRUCT tvInsert;
		tvInsert.item = tvItem;
		tvInsert.hInsertAfter = TVI_LAST;
		tvInsert.hParent = hParentItem;
		InsertItem(&tvInsert);
		dwFetched = 0;
	}

	pEnum->Release();
	return S_OK;
}

BOOL CFileExplorerTreeCtrl::GetItemPathName(CString& strPath, HTREEITEM htreeItem)
{
	ASSERT_VALID(this);
	strPath.Empty();
	if (htreeItem == NULL)
		htreeItem = GetSelectedItem();
	if (htreeItem == NULL)
		return FALSE;

	LPAFX_SHELLITEMINFO pItem = (LPAFX_SHELLITEMINFO) GetItemData(htreeItem);
	if (pItem == NULL || pItem->pidlFQ == NULL || pItem->pidlRel == NULL)
		return FALSE;
	LPSHELLFOLDER lpShellFolder = NULL;
	
	HRESULT hRes;
	
	if (pItem->pParentFolder == NULL){
		hRes = SHGetDesktopFolder(&lpShellFolder);
		
		if (FAILED(hRes))
			return FALSE;
		GetName(lpShellFolder,pItem->pidlRel,SHGDN_FORPARSING,strPath);
		lpShellFolder->Release();
	}else{
		GetName(pItem->pParentFolder,pItem->pidlRel,SHGDN_FORPARSING,strPath);
	}
	return TRUE;
}

BOOL CFileExplorerTreeCtrl::GetName (LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, DWORD dwFlags, CString &strName)
{
	BOOL bSuccess=TRUE;
	STRRET str;
	if (NOERROR==lpsf->GetDisplayNameOf(lpi,dwFlags, &str)){
		LPTSTR lpStr;
		StrRetToStr(&str,lpi,&lpStr);
		strName = lpStr;
		CoTaskMemFree(lpStr);
	}else
		bSuccess = FALSE;
	return bSuccess;
}

BEGIN_MESSAGE_MAP(CFileExplorerTreeCtrl, CMFCShellTreeCtrl)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CFileExplorerTreeCtrl 메시지 처리기입니다.

void CFileExplorerTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CMFCShellTreeCtrl::OnLButtonDblClk(nFlags, point);
	
	CString strPath;
	GetItemPathName(strPath, this->GetSelectedItem());
	if (strPath.GetLength() >= 4) {
		CString ext = strPath.Mid(strPath.GetLength()-4);

		// TODO : Open File When *.csv
		if (ext == L".csv") {
			AfxGetApp()->OpenDocumentFile((LPCTSTR)strPath);
		}
	}
}
