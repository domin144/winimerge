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
	CImgWindow() :
		m_fip(nullptr),
		m_ptOverlappedImage {},
		m_ptOverlappedImageCursor {},
		m_zoom(1.0),
		// m_useBackColor(false),
		// m_backColor {0xff, 0xff, 0xff, 0xff},
		m_visibleRectangleSelection(false) //,
		// m_hCursor(nullptr)
	{
		add(m_scrolledWindow);
		m_scrolledWindow.add(m_drawingArea);

		m_scrolledWindow.get_hadjustment()->signal_value_changed().connect(
			sigc::mem_fun(this, &CImgWindow::onScroll));
		m_scrolledWindow.get_vadjustment()->signal_value_changed().connect(
			sigc::mem_fun(this, &CImgWindow::onScroll));
		m_drawingArea.signal_draw().connect(
			sigc::mem_fun(this, &CImgWindow::onAreaDraw));

		m_drawingArea.show();
		m_scrolledWindow.show();
		show();
//		MyRegisterClass(hInstance);
//		m_hWnd = CreateWindowExW(0, L"WinImgWindowClass", NULL, WS_CHILD | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE,
//			0, 0, 0, 0, hWndParent, NULL, hInstance, this);
//		return m_hWnd ? true : false;
	}

	~CImgWindow()
	{
	}

	bool Destroy()
	{
		m_fip = nullptr;
		hide();
		return true;
	}

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
//		MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
//	}

//	void SetFocus()
//	{
//		::SetFocus(m_hWnd);
//	}

	/* Provides visible area in m_drawingArea coordinates. */
	RECT getViewRect() const;

	/* Convert from m_drawingArea to picture coordinates. */
	POINT ConvertDPtoLP(const POINT &p) const
	{
		const RECT view = getViewRect();
		POINT lp;

		if (view.get_width() < m_fip->getWidth() * m_zoom + MARGIN * 2)
			lp.x = static_cast<int>((p.x - MARGIN) / m_zoom);
		else
			lp.x = static_cast<int>(
				(p.y - (view.get_width() / 2 - m_fip->getWidth() / 2 * m_zoom))
				/ m_zoom);
		if (view.get_height() < m_fip->getHeight() * m_zoom + MARGIN * 2)
			lp.y = static_cast<int>((p.y - MARGIN) / m_zoom);
		else
			lp.y = static_cast<int>(
				(p.y - (view.get_height() / 2 - m_fip->getHeight() / 2 * m_zoom))
				/ m_zoom);
		return lp;
	}

	/* Convert from picture coordinates to m_drawingArea coordinates */
	POINT ConvertLPtoDP(const POINT &p) const
	{
		const int viewWidth = static_cast<int>(
			m_scrolledWindow.get_hadjustment()->get_page_size());
		const int viewHeight = static_cast<int>(
			m_scrolledWindow.get_vadjustment()->get_page_size());
		POINT result;

		if (viewWidth > m_fip->getWidth() * m_zoom + MARGIN * 2)
			result.x =
				static_cast<int>((viewWidth - m_fip->getWidth() * m_zoom) / 2);
		else
			result.x = MARGIN;
		if (viewHeight > m_fip->getHeight() * m_zoom + MARGIN * 2)
			result.y = static_cast<int>(
				(viewHeight - m_fip->getHeight() * m_zoom) / 2);
		else
			result.y = MARGIN;
		result.x += static_cast<int>(p.x * m_zoom);
		result.y += static_cast<int>(p.y * m_zoom);
		return result;
	}

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

	double GetZoom() const
	{
		return m_zoom;
	}

	void SetZoom(const double zoom)
	{
		double oldZoom = m_zoom;
		m_zoom = zoom;
		if (m_zoom < 0.1)
			m_zoom = 0.1;
		int newVScrollPos = static_cast<int>(getVScrollPos() / oldZoom * m_zoom);
		int newHScrollPos = static_cast<int>(getHScrollPos() / oldZoom * m_zoom);
		if (m_fip)
		{
			const int viewWidth = static_cast<int>(m_scrolledWindow.get_vadjustment()->get_page_size());
			const int viewHeight = static_cast<int>(m_scrolledWindow.get_vadjustment()->get_page_size());
			const int width  = static_cast<int>(m_fip->getWidth()  * m_zoom) + MARGIN * 2;
			const int height = static_cast<int>(m_fip->getHeight() * m_zoom) + MARGIN * 2;
			newHScrollPos = std::clamp(newHScrollPos, 0, width - viewWidth);
			newVScrollPos = std::clamp(newVScrollPos, 0, height - viewHeight);
			CalcScrollBarRange();
			m_scrolledWindow.get_vadjustment()->set_value(newVScrollPos);
			m_scrolledWindow.get_hadjustment()->set_value(newHScrollPos);
		}
	}

	void Invalidate(bool erase = false)
	{
//		InvalidateRect(m_hWnd, NULL, erase);
	}

	void SetImage(fipImage *pfip)
	{
		m_fip = pfip;
		m_visibleRectangleSelection = false;
		m_ptSelectionStart = {};
		m_ptSelectionEnd   = {};
		CalcScrollBarRange();
	}

//	void SetCursor(HCURSOR hCursor)
//	{
//		m_hCursor = hCursor;
//	}

	POINT GetRectangleSelectionStart() const
	{
		return m_ptSelectionStart;
	}

	bool SetRectangleSelectionStart(int x, int y, bool clamp = true)
	{
		if (!m_fip)
			return false;
		if (clamp)
		{
			m_ptSelectionStart.x = std::clamp(x, 0, static_cast<int>(m_fip->getWidth()));
			m_ptSelectionStart.y = std::clamp(y, 0, static_cast<int>(m_fip->getHeight()));
		}
		else
		{
			m_ptSelectionStart.x = x;
			m_ptSelectionStart.y = y;
		}
		m_visibleRectangleSelection = true;
		return true;
	}

	POINT GetRectangleSelectionEnd() const
	{
		return m_ptSelectionEnd;
	}

	bool SetRectangleSelectionEnd(int x, int y, bool clamp = true)
	{
		if (!m_fip)
			return false;
		if (clamp)
		{
			m_ptSelectionEnd.x = std::clamp(x, 0, static_cast<int>(m_fip->getWidth()));
			m_ptSelectionEnd.y = std::clamp(y, 0, static_cast<int>(m_fip->getHeight()));
		}
		else
		{
			m_ptSelectionEnd.x = x;
			m_ptSelectionEnd.y = y;
		}
		return true;
	}

	bool SetRectangleSelection(int left, int top, int right, int bottom, bool clamp = true)
	{
		SetRectangleSelectionStart(left, top, clamp);
		return SetRectangleSelectionEnd(right, bottom, clamp);
	}

	RECT GetRectangleSelection() const
	{
		int left    = (std::min)(m_ptSelectionStart.x, m_ptSelectionEnd.x);
		int top     = (std::min)(m_ptSelectionStart.y, m_ptSelectionEnd.y);
		int right   = (std::max)(m_ptSelectionStart.x, m_ptSelectionEnd.x);
		int bottom  = (std::max)(m_ptSelectionStart.y, m_ptSelectionEnd.y);
		return {left, top, right - left, bottom - top};
	}

	bool IsRectanlgeSelectionVisible() const
	{
		return m_visibleRectangleSelection;
	}

	void DeleteRectangleSelection()
	{
		m_ptSelectionStart = {};
		m_ptSelectionEnd   = {};
		m_visibleRectangleSelection = false;
	}

	const fipImage& GetOverlappedImage() const
	{
		return m_fipOverlappedImage;
	}

	void SetOverlappedImage(const fipImage& image)
	{
		m_fipOverlappedImage = image;
		m_ptOverlappedImage = {};
		m_ptOverlappedImageCursor = {};
	}

	void StartDraggingOverlappedImage(const fipImage& image, const POINT& ptImage, const POINT& ptCursor)
	{
		m_fipOverlappedImage = image;
		m_ptOverlappedImage = ptImage;
		m_ptOverlappedImageCursor = ptCursor;
	}

	void RestartDraggingOverlappedImage(const POINT& ptCursor)
	{
		m_ptOverlappedImageCursor = ptCursor;
	}

	void DragOverlappedImage(const POINT& ptCursor)
	{
		m_ptOverlappedImage.x += ptCursor.x - m_ptOverlappedImageCursor.x;
		m_ptOverlappedImage.y += ptCursor.y - m_ptOverlappedImageCursor.y;
		m_ptOverlappedImageCursor = ptCursor;
	}

	void DeleteOverlappedImage()
	{
		m_fipOverlappedImage.clear();
	}

	POINT GetOverlappedImagePosition() const
	{
		return m_ptOverlappedImage;
	}

	RECT GetOverlappedImageRect() const
	{
		return { m_ptOverlappedImage.x, m_ptOverlappedImage.y,
			static_cast<int>(m_fipOverlappedImage.getWidth()),
			static_cast<int>(m_fipOverlappedImage.getHeight()) };
	}

	void SetOverlappedImagePosition(const POINT& pt)
	{
		m_ptOverlappedImage = pt;
	}

	void UpdateScrollBars()
	{
		CalcScrollBarRange();
	}

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

	void drawImage(const Cairo::RefPtr<Cairo::Context> &cr, const fipImage &image, const RECT &outputArea)
	{
		Glib::RefPtr<Gdk::Pixbuf> pixbufToBeScaled = fipToGdkPixbuf(image);

		Glib::RefPtr<Gdk::Pixbuf> pixbuf = pixbufToBeScaled->scale_simple(
			outputArea.get_width(),
			outputArea.get_height(),
			Gdk::INTERP_BILINEAR);
		Gdk::Cairo::set_source_pixbuf(cr, pixbuf, outputArea.get_x(), outputArea.get_y());
		cr->paint();
	}

	void onScroll()
	{
		const auto view = getViewRect();
		m_drawingArea.queue_draw_area(
			view.get_x(), view.get_y(), view.get_width(), view.get_height());
	}

	bool onAreaDraw(const Cairo::RefPtr<Cairo::Context>& cr)
	{
		Glib::RefPtr<Gtk::StyleContext> style = get_style_context();
		const int viewWidth = static_cast<int>(
			m_scrolledWindow.get_hadjustment()->get_page_size());
		const int viewHeight = static_cast<int>(
			m_scrolledWindow.get_vadjustment()->get_page_size());

		cr->set_source_rgb(206, 215, 230);
		cr->paint();
		style->render_background(cr, 0, 0, viewWidth, viewHeight);

		if (m_fip)
		{
			if (m_fip->isValid())
			{
				const POINT pt = ConvertLPtoDP({0, 0});
				RECT rcImg = {
					pt.x,
					pt.y,
					static_cast<int>(m_fip->getWidth() * m_zoom),
					static_cast<int>(m_fip->getHeight() * m_zoom)};

				const RECT view = getViewRect();
				const POINT ptTmpLT =
					ConvertDPtoLP({view.get_x(), view.get_y()});
				const POINT ptTmpRB = ConvertDPtoLP(
					{view.get_x() + static_cast<int>(viewWidth + 1 * m_zoom),
					 view.get_y() + static_cast<int>(viewHeight + 1 * m_zoom)});
				const POINT ptSubLT = {
					std::clamp<int>(ptTmpLT.x, 0, m_fip->getWidth()),
					std::clamp<int>(ptTmpLT.y, 0, m_fip->getHeight())};
				const POINT ptSubRB = {
					std::clamp<int>(ptTmpRB.x, 0, m_fip->getWidth()),
					std::clamp<int>(ptTmpRB.y, 0, m_fip->getHeight())};
				const POINT ptSubLTDP = ConvertLPtoDP(ptSubLT);
				const POINT ptSubRBDP = ConvertLPtoDP(ptSubRB);
				rcImg = {ptSubLTDP.x,
						 ptSubLTDP.y,
						 ptSubRBDP.x - ptSubLTDP.x,
						 ptSubRBDP.y - ptSubLTDP.y};
				fipImage fipSubImage;
				m_fip->copySubImage(
					fipSubImage, ptSubLT.x, ptSubLT.y, ptSubRB.x, ptSubRB.y);
				std::cout << "rcImg: (x, y, width, height) = (" << rcImg.get_x()
						  << ", " << rcImg.get_y() << ", " << rcImg.get_width()
						  << ", " << rcImg.get_height() << ")" << std::endl;
				drawImage(cr, fipSubImage, rcImg);
			}

			if (m_visibleRectangleSelection)
			{
				const RECT rcSelectionL = GetRectangleSelection();
				const POINT ptLT =
					ConvertLPtoDP({rcSelectionL.get_x(), rcSelectionL.get_y()});
				const POINT ptRB = ConvertLPtoDP(
					{rcSelectionL.get_x() + rcSelectionL.get_width(),
					 rcSelectionL.get_y() + rcSelectionL.get_height()});
				const RECT rcSelection = {
					ptLT.x, ptLT.y, ptRB.x - ptLT.x, ptRB.y - ptLT.y};
				if (rcSelection.get_width() == 0
					|| rcSelection.get_height() == 0)
				{
					DrawXorBar(
						cr,
						rcSelection.get_x(),
						rcSelection.get_y(),
						rcSelection.get_width() + 1,
						rcSelection.get_height() + 1);
				}
				else
				{
					DrawXorRectangle(
						cr,
						rcSelection.get_x(),
						rcSelection.get_y(),
						rcSelection.get_width(),
						rcSelection.get_height(),
						1);
				}
			}

			if (m_fipOverlappedImage.isValid())
			{
				const POINT pt = ConvertLPtoDP(m_ptOverlappedImage);
				RECT rcImg = {
					pt.x,
					pt.y,
					static_cast<int>(m_fipOverlappedImage.getWidth() * m_zoom),
					static_cast<int>(
						m_fipOverlappedImage.getHeight() * m_zoom)};
				drawImage(cr, m_fipOverlappedImage, rcImg);
				DrawXorRectangle(
					cr,
					rcImg.get_x(),
					rcImg.get_y(),
					rcImg.get_width(),
					rcImg.get_height(),
					1);
			}
		}
		return true;
	}

	void OnSize()
	{
		CalcScrollBarRange();
	}

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

	void CalcScrollBarRange()
	{
		if (m_fip)
		{
			m_drawingArea.set_size_request(
				static_cast<int>(m_fip->getWidth() * m_zoom) + 2 * MARGIN,
				static_cast<int>(m_fip->getHeight() * m_zoom) + 2 * MARGIN);
		}
		else
		{
			m_drawingArea.set_size_request();
		}
	}

	void DrawXorBar(const Cairo::RefPtr<Cairo::Context> &cr, int x1, int y1, int width, int height)
	{
		cr->save();
		static const std::array<std::uint8_t, 4 * 2 * 2> patternData = {
			{0xff,
			 0xff,
			 0xff,
			 0x00,
			 0x00,
			 0x00,
			 0x00,
			 0x00,
			 0x00,
			 0x00,
			 0x00,
			 0xff,
			 0xff,
			 0xff,
			 0x00,
			 0x00}};

		auto patternPixBuf = Gdk::Pixbuf::create_from_data(
			patternData.data(), Gdk::COLORSPACE_RGB, false, 8, 2, 2, 8);

		Gdk::Cairo::set_source_pixbuf(cr, patternPixBuf, x1, y1);
		cr->set_operator(
			static_cast<Cairo::Operator>(CAIRO_OPERATOR_DIFFERENCE));
		cr->rectangle(x1, y1, width, height);
		cr->fill();

		cr->restore();
	}

	void DrawXorRectangle(const Cairo::RefPtr<Cairo::Context> &cr, int left, int top, int width, int height, int lineWidth)
	{
		int right = left + width;
		int bottom = top + height;
		DrawXorBar(cr, left                 , top                   , width    , lineWidth);
		DrawXorBar(cr, left                 , bottom - lineWidth + 1, width    , lineWidth);
		DrawXorBar(cr, left                 , top                   , lineWidth, height);
		DrawXorBar(cr, right - lineWidth + 1, top                   , lineWidth, height);
	}

//	static LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
//	{
//		if (iMsg == WM_NCCREATE)
//			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams));
//		CImgWindow *pImgWnd = reinterpret_cast<CImgWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
//		LRESULT lResult = pImgWnd->OnWndMsg(hwnd, iMsg, wParam, lParam);
//		return lResult;
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
