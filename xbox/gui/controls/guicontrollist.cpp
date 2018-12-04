#include "guicontrollist.h"
#include "..\xboxgui.h"
#include "..\guimessage.h"
#include "..\..\input\sysxboxinput.h"
#include "..\utils\stringutils.h"

using namespace std;

#define LINE_SPACING 30
#define ITEMS_PERPAGE 10 //TODO: Calculate items per page by ( pListItem->Height() / CGUIControl::Height() )

#define CONTROL_LEBEL_ITEM 2

CGUIListItem::CGUIListItem(std::string strName, bool bIsDirectory)
{
	m_strName = strName;
	m_bIsDirectory = bIsDirectory;
}

CGUIListItem::~CGUIListItem()
{
}

CGUIControlList::CGUIControlList(int iControlID, int iWindowID, int iPosX, int iPosY, int iWidth, int iHeight, std::string strFont, DWORD dwColor, DWORD dwSelectedColor)
: CGUIControl(iControlID, iWindowID, iPosX, iPosY, iWidth, iHeight)
{
	m_iCursor = 0;
	m_iOffset = 0;
	m_pFont = g_XBoxGUI.GetFontManager().GetFont(strFont);
	m_dwColor = dwColor;
	m_dwSelectedColor = dwSelectedColor;
}

CGUIControlList::~CGUIControlList()
{
}

void CGUIControlList::Render()
{
	int iYPos = m_iPosY;
	int iSize = ITEMS_PERPAGE; //TODO: Calculate items per page by ( pListItem->Height() / CGUIControl::Height() )

	if(m_vecItems.size() < ITEMS_PERPAGE)
		iSize = m_vecItems.size();

	for(int i = m_iOffset; i < iSize+m_iOffset; i++)
	{
		CGUIListItem* pListItem = m_vecItems[i];
		if(pListItem)
		{
			if(m_pFont)
			{
				if(i == m_iCursor && HasFocus())
					m_pFont->Render(m_iPosX, iYPos, 15, m_dwSelectedColor, pListItem->GetName());
				else
					m_pFont->Render(m_iPosX, iYPos, 15, m_dwColor, pListItem->GetName());
			}
			iYPos += LINE_SPACING;
		}
	}

	// TODO: Don't send this each frame, only when updated
	// Update our selection label
	string strLabel = CStringUtils::IntToString(m_iCursor+1) + " of " + CStringUtils::IntToString(m_vecItems.size());
	CGUIMessage msg(GUI_MSG_SET_LABEL, GetID(), CONTROL_LEBEL_ITEM, strLabel);
	g_XBoxGUI.SendMessage(msg);

	CGUIControl::Render();
}

bool CGUIControlList::OnKey(int iKey)
{
	if(iKey == K_XBOX_DPAD_DOWN)
	{
		if(m_iCursor+1 < (int)m_vecItems.size())
		{
			m_iCursor++;

			if(m_iCursor >= ITEMS_PERPAGE+m_iOffset)
				m_iOffset++;
		}
		return true;
	}

	if(iKey == K_XBOX_DPAD_UP)
	{
		if((m_iCursor > 0))
		{
			m_iCursor--;

			if(m_iCursor < m_iOffset)
			{
				if(m_iOffset > 0)
					m_iOffset--;
			}
		}
		return true;
	}

	if(iKey == K_XBOX_A)
	{
		CGUIMessage msg(GUI_MSG_CLICKED, GetParentID(), GetID());
		g_XBoxGUI.SendMessage(msg);
		return true;
	}

	return CGUIControl::OnKey(iKey);
}

bool CGUIControlList::AddItem(CGUIListItem* pItem)
{
	m_vecItems.push_back(pItem);

	return true;
}

CGUIListItem* CGUIControlList::GetSelectedItem()
{
	CGUIListItem* pListItem = m_vecItems[m_iCursor];
	
	if(pListItem)
		return pListItem;

	return NULL;
}

int CGUIControlList::GetSize()
{
	return m_vecItems.size();
}

void CGUIControlList::FreeResources()
{
	for(int i = 0; i < (int)m_vecItems.size(); i++)
	{
		CGUIListItem* pListItem = NULL;
		pListItem = m_vecItems[i];

		if(pListItem)
			delete pListItem;
	}

	m_vecItems.clear();

	CGUIControl::FreeResources();
}