/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning(disable: 4819)

#include <ImgDiffBuffer.hpp>

#include <FreeImagePlus.h>
#include <algorithm>
#include <gdkmm/general.h>
#include <gdkmm/pixbuf.h>
#include <gdkmm/rectangle.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>
#include <iostream>
#include <vector>

Glib::RefPtr<Gdk::Pixbuf> fipToGdkPixbuf(const fipImage &image);

class CImgWindow : public Gtk::Frame
{
	enum { MARGIN = 16 };

public:
    CImgWindow();
    ~CImgWindow();

    //	RECT GetWindowRect() const
    //	{
    //		RECT rc, rcParent;
    //		HWND hwndParent = GetParent(m_hWnd);
    //		::GetWindowRect(hwndParent, &rcParent);
    //		::GetWindowRect(m_hWnd, &rc);
    //		rc.left   -= rcParent.left;
    //		rc.top    -= rcParent.top;
    //		rc.right  -= rcParent.left;
    //		rc.bottom -= rcParent.top;
    //		return rc;
    //	}

    //	void SetWindowRect(const RECT& rc)
    //	{
    //		MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom -
    //rc.top, TRUE);
    //	}

    //	void SetFocus()
    //	{
    //		::SetFocus(m_hWnd);
    //	}

	/* Provides visible area in m_drawingArea coordinates. */
	RECT getViewRect() const;

	/* Convert from m_drawingArea to picture coordinates. */
    POINT ConvertDPtoLP(const POINT &p) const;

	/* Convert from picture coordinates to m_drawingArea coordinates */
    POINT ConvertLPtoDP(const POINT &p) const;

//	POINT GetCursorPos() const
//	{
//		POINT dpt;
//		::GetCursorPos(&dpt);
//		RECT rc;
//		::GetWindowRect(m_hWnd, &rc);
//		dpt.x -= rc.left;
//		dpt.y -= rc.top;
//		return ConvertDPtoLP(dpt.x, dpt.y);
//	}

//	bool IsFocused() const
//	{
//		return m_hWnd == GetFocus();
//	}

//	void ScrollTo(int x, int y, bool force = false)
//	{
//		SCROLLINFO sih{ sizeof SCROLLINFO, SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS };
//		SCROLLINFO siv{ sizeof SCROLLINFO, SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS };
//		GetScrollInfo(m_hWnd, SB_HORZ, &sih);
//		GetScrollInfo(m_hWnd, SB_VERT, &siv);

//		RECT rc;
//		GetClientRect(m_hWnd, &rc);

//		if (rc.right - rc.left < m_fip->getWidth() * m_zoom + MARGIN * 2)
//		{
//			if (force)
//			{
//				m_nHScrollPos = static_cast<int>(x * m_zoom + MARGIN - rc.right / 2);
//			}
//			else
//			{
//				if (x * m_zoom + MARGIN < m_nHScrollPos || m_nHScrollPos + rc.right < x * m_zoom + MARGIN)
//					m_nHScrollPos = static_cast<int>(x * m_zoom + MARGIN - rc.right / 2);
//			}
//			if (m_nHScrollPos < 0)
//				m_nHScrollPos = 0;
//			else if (m_nHScrollPos > sih.nMax - static_cast<int>(sih.nPage))
//				m_nHScrollPos = sih.nMax - sih.nPage;
//		}
//		if (rc.bottom - rc.top < m_fip->getHeight() * m_zoom + MARGIN * 2)
//		{
//			if (force)
//			{
//				m_nVScrollPos = static_cast<int>(y * m_zoom + MARGIN - rc.bottom / 2);
//			}
//			else
//			{
//				if (y * m_zoom + MARGIN < m_nVScrollPos || m_nVScrollPos + rc.bottom < y * m_zoom + MARGIN)
//					m_nVScrollPos = static_cast<int>(y * m_zoom + MARGIN - rc.bottom / 2);
//			}
//			if (m_nVScrollPos < 0)
//				m_nVScrollPos = 0;
//			else if (m_nVScrollPos > siv.nMax - static_cast<int>(siv.nPage))
//				m_nVScrollPos = siv.nMax - siv.nPage;
//		}

//		CalcScrollBarRange();
//		ScrollWindow(m_hWnd, sih.nPos - m_nHScrollPos, siv.nPos - m_nVScrollPos, NULL, NULL);
//		InvalidateRect(m_hWnd, NULL, FALSE);
//	}

//	void ScrollTo2(int lx, int ly, int dx, int dy)
//	{
//		SCROLLINFO sih{ sizeof SCROLLINFO, SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS };
//		SCROLLINFO siv{ sizeof SCROLLINFO, SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS };
//		GetScrollInfo(m_hWnd, SB_HORZ, &sih);
//		GetScrollInfo(m_hWnd, SB_VERT, &siv);

//		RECT rc;
//		GetClientRect(m_hWnd, &rc);

//		m_nHScrollPos = static_cast<int>(lx * m_zoom + MARGIN - dx);
//		if (m_nHScrollPos < 0)
//			m_nHScrollPos = 0;
//		else if (m_nHScrollPos > sih.nMax - static_cast<int>(sih.nPage))
//			m_nHScrollPos = sih.nMax - sih.nPage;
//		m_nVScrollPos = static_cast<int>(ly * m_zoom + MARGIN - dy);
//		if (m_nVScrollPos < 0)
//			m_nVScrollPos = 0;
//		else if (m_nVScrollPos > siv.nMax - static_cast<int>(siv.nPage))
//			m_nVScrollPos = siv.nMax - siv.nPage;

//		CalcScrollBarRange();
//		ScrollWindow(m_hWnd, sih.nPos - m_nHScrollPos, siv.nPos - m_nVScrollPos, NULL, NULL);
//		InvalidateRect(m_hWnd, NULL, FALSE);
//	}

//	RGBQUAD GetBackColor() const
//	{
//		return m_backColor;
//	}

//	void SetBackColor(RGBQUAD backColor)
//	{
//		m_backColor = backColor;
//		if (m_fip)
//		{
//			m_fip->setModified(true);
//			InvalidateRect(m_hWnd, NULL, TRUE);
//		}
//	}

//	bool GetUseBackColor() const
//	{
//		return m_useBackColor;
//	}

//	void SetUseBackColor(bool useBackColor)
//	{
//		m_useBackColor = useBackColor;
//		if (m_fip)
//		{
//			m_fip->setModified(true);
//			InvalidateRect(m_hWnd, NULL, TRUE);
//		}
//	}

    double GetZoom() const;
    void SetZoom(const double zoom);

    void Invalidate(bool erase = false);

    void SetImage(fipImage *pfip);

//	void SetCursor(HCURSOR hCursor)
//	{
//		m_hCursor = hCursor;
//	}

    POINT GetRectangleSelectionStart() const;
    bool SetRectangleSelectionStart(int x, int y, bool clamp = true);

    POINT GetRectangleSelectionEnd() const;
    bool SetRectangleSelectionEnd(int x, int y, bool clamp = true);

    bool SetRectangleSelection(
        int left, int top, int right, int bottom, bool clamp = true);
    RECT GetRectangleSelection() const;
    bool IsRectanlgeSelectionVisible() const;

    void DeleteRectangleSelection();

    const fipImage& GetOverlappedImage() const;

    void SetOverlappedImage(const fipImage& image);

    void StartDraggingOverlappedImage(const fipImage& image, const POINT& ptImage, const POINT& ptCursor);

    void RestartDraggingOverlappedImage(const POINT& ptCursor);

    void DragOverlappedImage(const POINT& ptCursor);

    void DeleteOverlappedImage();

	POINT GetOverlappedImagePosition() const
	{
		return m_ptOverlappedImage;
	}

    RECT GetOverlappedImageRect() const;

    void SetOverlappedImagePosition(const POINT& pt);

    void UpdateScrollBars();

private:

//	ATOM MyRegisterClass(HINSTANCE hInstance)
//	{
//		WNDCLASSEXW wcex = {0};
//		if (!GetClassInfoEx(hInstance, L"WinImgWindowClass", &wcex))
//		{
//			wcex.cbSize         = sizeof(WNDCLASSEX);
//			wcex.style			= CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
//			wcex.lpfnWndProc	= (WNDPROC)WndProc;
//			wcex.cbClsExtra		= 0;
//			wcex.cbWndExtra		= 0;
//			wcex.hInstance		= hInstance;
//			wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//			wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
//			wcex.lpszClassName	= L"WinImgWindowClass";
//		}
//		return RegisterClassExW(&wcex);
//	}

    void drawImage(const Cairo::RefPtr<Cairo::Context> &cr, const fipImage &image, const RECT &outputArea);

    void onScroll();

    bool onAreaDraw(const Cairo::RefPtr<Cairo::Context>& cr);

    void OnSize();

//	void OnHScroll(UINT nSBCode, UINT nPos)
//	{
//		SCROLLINFO si{ sizeof SCROLLINFO, SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS };
//		GetScrollInfo(m_hWnd, SB_HORZ, &si);
//		switch (nSBCode) {
//		case SB_LINEUP:
//			--m_nHScrollPos;
//			break;
//		case SB_LINEDOWN:
//			++m_nHScrollPos;
//			break;
//		case SB_PAGEUP:
//			m_nHScrollPos -= si.nPage;
//			break;
//		case SB_PAGEDOWN:
//			m_nHScrollPos += si.nPage;
//			break;
//		case SB_THUMBTRACK:
//			m_nHScrollPos = nPos;
//			break;
//		default: break;
//		}
//		CalcScrollBarRange();
//		ScrollWindow(m_hWnd, si.nPos - m_nHScrollPos, 0, NULL, NULL);
//	}

//	void OnVScroll(UINT nSBCode, UINT nPos)
//	{
//		SCROLLINFO si{ sizeof SCROLLINFO, SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS };
//		GetScrollInfo(m_hWnd, SB_VERT, &si);
//		switch (nSBCode) {
//		case SB_LINEUP:
//			--m_nVScrollPos;
//			break;
//		case SB_LINEDOWN:
//			++m_nVScrollPos;
//			break;
//		case SB_PAGEUP:
//			m_nVScrollPos -= si.nPage;
//			break;
//		case SB_PAGEDOWN:
//			m_nVScrollPos += si.nPage;
//			break;
//		case SB_THUMBTRACK:
//			m_nVScrollPos = nPos;
//			break;
//		default: break;
//		}
//		CalcScrollBarRange();
//		ScrollWindow(m_hWnd, 0, si.nPos - m_nVScrollPos, NULL, NULL);
//	}

//	void OnLButtonDown(UINT nFlags, int x, int y)
//	{
//		SetFocus();
//	}

//	void OnRButtonDown(UINT nFlags, int x, int y)
//	{
//		SetFocus();
//	}

//	void OnMouseWheel(UINT nFlags, short zDelta)
//	{
//		if (!(nFlags & MK_CONTROL))
//		{
//			RECT rc;
//			GetClientRect(m_hWnd, &rc);
//			if (!(nFlags & MK_SHIFT))
//			{
//				if (rc.bottom - rc.top < m_fip->getHeight() * m_zoom + MARGIN * 2)
//				{
//					SCROLLINFO si{ sizeof SCROLLINFO, SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS };
//					GetScrollInfo(m_hWnd, SB_VERT, &si);
//					m_nVScrollPos += - zDelta / (WHEEL_DELTA / 16);
//					CalcScrollBarRange();
//					ScrollWindow(m_hWnd, 0, si.nPos - m_nVScrollPos, NULL, NULL);
//				}
//			}
//			else
//			{
//				if (rc.right - rc.left < m_fip->getWidth() * m_zoom + MARGIN * 2)
//				{
//					SCROLLINFO si{ sizeof SCROLLINFO, SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS };
//					GetScrollInfo(m_hWnd, SB_HORZ, &si);
//					m_nHScrollPos += - zDelta / (WHEEL_DELTA / 16);
//					CalcScrollBarRange();
//					ScrollWindow(m_hWnd, si.nPos - m_nHScrollPos, 0, NULL, NULL);
//				}
//			}
//		}
//		else
//		{
//			SetZoom(m_zoom + (zDelta > 0 ? 0.1 : -0.1));
//		}
//	}

//	void OnSetFocus(HWND hwndOld)
//	{
//		InvalidateRect(m_hWnd, NULL, TRUE);
//	}

//	void OnKillFocus(HWND hwndNew)
//	{
//		InvalidateRect(m_hWnd, NULL, TRUE);
//	}

//	LRESULT OnWndMsg(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
//	{
//		switch (iMsg)
//		{
//		case WM_PAINT:
//			OnPaint();
//			break;
//		case WM_ERASEBKGND:
//			return TRUE;
//		case WM_HSCROLL:
//			OnHScroll((UINT)(LOWORD(wParam) & 0xff), (int)(unsigned short)HIWORD(wParam) | ((LOWORD(wParam) & 0xff00) << 8)); // See 'case WM_HSCROLL:' in CImgMergeWindow::ChildWndProc()
//			break;
//		case WM_VSCROLL:
//			OnVScroll((UINT)(LOWORD(wParam) & 0xff), (int)(unsigned short)HIWORD(wParam) | ((LOWORD(wParam) & 0xff00) << 8)); // See 'case WM_VSCROLL:' in CImgMergeWindow::ChildWndProc()
//			break;
//		case WM_LBUTTONDOWN:
//			OnLButtonDown((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
//			break;
//		case WM_RBUTTONDOWN:
//			OnRButtonDown((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
//			break;
//		case WM_MOUSEWHEEL:
//			OnMouseWheel(GET_KEYSTATE_WPARAM(wParam), GET_WHEEL_DELTA_WPARAM(wParam));
//			break;
//		case WM_SETFOCUS:
//			OnSetFocus((HWND)wParam);
//			break;
//		case WM_KILLFOCUS:
//			OnKillFocus((HWND)wParam);
//			break;
//		case WM_COMMAND:
//			PostMessage(GetParent(m_hWnd), iMsg, wParam, lParam);
//			break;
//		case WM_SIZE:
//			OnSize((UINT)wParam, LOWORD(lParam), HIWORD(lParam));
//			break;
//		case WM_SETCURSOR:
//			::SetCursor(m_hCursor ? m_hCursor : LoadCursor(nullptr, IDC_ARROW));
//			break;
//		default:
//			return DefWindowProc(hwnd, iMsg, wParam, lParam);
//		}
//		return 0;
//	}

    void CalcScrollBarRange();

    void DrawXorBar(
        const Cairo::RefPtr<Cairo::Context>& cr,
        int x1,
        int y1,
        int width,
        int height);

    void DrawXorRectangle(
        const Cairo::RefPtr<Cairo::Context>& cr,
        int left,
        int top,
        int width,
        int height,
        int lineWidth);

    //	static LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam,
    //LPARAM lParam)
    //	{
    //		if (iMsg == WM_NCCREATE)
    //			SetWindowLongPtr(hwnd, GWLP_USERDATA,
    //reinterpret_cast<LONG_PTR>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams));
    //		CImgWindow *pImgWnd = reinterpret_cast<CImgWindow
    //*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)); 		LRESULT lResult =
    //pImgWnd->OnWndMsg(hwnd, iMsg, wParam, lParam); 		return lResult;
    //	}

	int getVScrollPos() const;
	int getHScrollPos() const;

	Gtk::ScrolledWindow m_scrolledWindow;
	Gtk::DrawingArea m_drawingArea;
	fipImage *m_fip;
	fipImage m_fipOverlappedImage;
	POINT m_ptOverlappedImage;
	POINT m_ptOverlappedImageCursor;
	double m_zoom;
//	bool m_useBackColor;
//	RGBQUAD m_backColor;
	bool m_visibleRectangleSelection;
	POINT m_ptSelectionStart;
	POINT m_ptSelectionEnd;
//	HCURSOR m_hCursor;
};
