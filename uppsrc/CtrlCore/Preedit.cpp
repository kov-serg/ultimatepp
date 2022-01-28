#include "CtrlCore.h"

#define LLOG(x) DLOG(x)

namespace Upp {

struct PreeditCtrl : Ctrl {
	WString text;
	Font    font;
	Ctrl   *owner = NULL;

	virtual void Paint(Draw& w) {
		Size sz = GetSize();
		DDUMP(font);
		w.DrawRect(GetSize(), SWhite());
		w.DrawText(DPI(2), sz.cy - font.GetCy(), text, font, SBlack());
	}
	
	PreeditCtrl() { SetFrame(BlackFrame()); }
};

Rect Ctrl::GetPreeditScreenRect()
{ // preedit position relative to window rect (client area in win32), zero width
	if(HasFocusDeep()) {
		Point p = focusCtrl->GetPreedit();
		DDUMP(focusCtrl->GetPreeditFont());
		DDUMP(focusCtrl->GetPreeditFont().GetCy());
		if(!IsNull(p)) {
			p += focusCtrl->GetScreenView().TopLeft();
			return RectC(p.x, p.y - 1, 0, focusCtrl->GetPreeditFont().GetCy() + 1);
		}
	}
	return Null;
}

Point Ctrl::GetPreedit()
{
	if(HasFocus()) {
		Rect r = GetCaret();
		if(r.GetHeight() > 0)
			return r.TopRight();
	}
	return Null;
}

Font Ctrl::GetPreeditFont()
{
	static int pheight = -1;
	static Font pfont;
	if(!focusCtrl)
		return StdFont();
	int height = max(focusCtrl->GetCaret().GetHeight(), DPI(7));
	if(height != pheight) {
		pheight = height;
		while(pheight > 0) {
			pfont = StdFont(height);
			if(pfont.GetCy() < pheight)
				break;
			height--;
		}
		if(height == 0)
			pfont = StdFont();
	}
	return pfont;
}

void Ctrl::SyncPreedit()
{
	PreeditCtrl& p = Single<PreeditCtrl>();
	if(p.owner == this && focusCtrl) {
		Rect r = GetPreeditScreenRect();
		p.font = focusCtrl->GetPreeditFont();
		r.right = r.left + GetTextSize(p.text, p.font).cx + DPI(4);
		p.SetRect(r);
	}
}

void Ctrl::ShowPreedit(const WString& text)
{
	PreeditCtrl& p = Single<PreeditCtrl>();
	p.text = text;
	p.owner = this;
	SyncPreedit();
	if(!p.IsOpen())
		p.PopUp(this, true, false, true);
	p.Refresh();
}

void Ctrl::HidePreedit()
{
	PreeditCtrl& p = Single<PreeditCtrl>();
	if(p.IsOpen()) {
		p.Close();
		p.owner = NULL;
	}
}

};