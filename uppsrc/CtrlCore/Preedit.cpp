#include "CtrlCore.h"

#define LLOG(x) DLOG(x)

namespace Upp {

Rect Ctrl::GetPreeditPos()
{
	if(HasFocusDeep()) {
		Rect r = focusCtrl->GetPreedit();
		if(r.GetHeight() > 0) {
			r.Offset(focusCtrl->GetScreenView().TopLeft() - GetScreenRect().TopLeft());
			return r;
		}
	}
	return Null;
}

Font GetPreeditFont(int height)
{
	static int pheight = -1;
	static Font pfont;
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

struct PreeditCtrl : Ctrl {
	WString text;
	Font    font;

	virtual void Paint(Draw& w) {
		Size sz = GetSize();
		w.DrawRect(GetSize(), SWhite());
		w.DrawText(DPI(2), sz.cy - font.GetCy(), text, font, SBlack());
	}
	
	PreeditCtrl() { SetFrame(BlackFrame()); }
};

void Ctrl::ShowPreedit(const WString& text)
{
	Rect r = GetPreeditPos();
	PreeditCtrl& p = Single<PreeditCtrl>();
	p.text = text;
	p.font = GetPreeditFont(r.GetHeight());
	r.Offset(GetScreenRect().TopLeft());
	r.right = r.left + GetTextSize(text, p.font).cx + DPI(4);
	r.bottom = r.top + p.font.GetCy();
	p.SetRect(r);
	if(!p.IsOpen()) {
		LLOG("Open preedit " << r);
		p.PopUp(this, true, false, true);
	}
}

void Ctrl::HidePreedit()
{
	PreeditCtrl& p = Single<PreeditCtrl>();
	if(p.IsOpen()) {
		LLOG("Close preedit");
		p.Close();
	}
}

};