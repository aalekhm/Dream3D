#ifndef WCOMBOBOX_H
#define WCOMBOBOX_H

#include "Engine/UI/widgetdef.h"
#include "Engine/UI/WContainer.h"
#include "Engine/UI/WTextField.h"
#include "Engine/UI/WListBox.h"

struct WComboBox : public WContainer {
	
	public:
		WComboBox();
		~WComboBox();

		static H_WND __stdcall Create(const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y, int width, int height, H_WND hwndParent, HMENU hMenu, LPVOID lpParam);

		LRESULT		OnSendMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		void			addItem(LISTBOX_ITEM* item);
		void			insertItemAt(int pos, LISTBOX_ITEM* item);
		void			removeItemAt(int pos);
		int				getSelectedIndex();
		void			setSelectedIndex(int pos);
		const char*		getSelectedString();
		const char*		getStringAt(int pos);
		int				getSize();
		void				addItem(const char* sItem);
		int				findString(const char* str);
		int				findStringExact(const char* strToFind);
		int				getStringLengthAtPos(int iIndex);

		void				addDefaultTestItems();

		const char*	getText();
	private:
		enum ID_COMBOBOX {
			ID_BUTTON = 0,
			ID_TEXTFIELD,
			ID_LISTBOX
		};

		enum ComboBoxState {
			NORMAL,
			READONLY
		};

		virtual void onCreateEx(LPVOID lpVoid);
		virtual void	onUpdate();
		virtual void	onRender();

		virtual void	onMouseWheelEx(WPARAM wParam, LPARAM lParam);
		virtual void	onKeyBDown(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void	onKeyBUp(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void	onMessage(H_WND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		void			setReadOnly(bool bRd);
		bool			getReadOnly();
		
		WIDGET*			m_ComboBoxWidget;
		WTextField*		m_cbTextField;
		WButton*		m_cbButton;
		WListBox*		m_cbListBox;

		ComboBoxState	mState;

		int				m_MaxLineWidth;
		int				m_CurrentIndex;
		
		int				m_mainX;
		int				m_minX;
		int				m_maxX;

		int				m_mainY;
		int				m_minY;
		int				m_maxY;

		unsigned int	TB_HEIGHT;
};

#endif