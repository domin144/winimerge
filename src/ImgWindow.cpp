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

#include <ImgWindow.hpp>

Glib::RefPtr<Gdk::Pixbuf> fipToGdkPixbuf(const fipImage &image)
{
	const int bitsPerSample = 8;
	const int bytesPerSample = bitsPerSample / 8;
	auto result = Gdk::Pixbuf::create(
				Gdk::COLORSPACE_RGB, true, bitsPerSample, image.getWidth(), image.getHeight());
	std::uint8_t *const resultPixels = result->get_pixels();
	for (int y = 0; y < image.getHeight(); ++y)
	{
		for (int x = 0; x < image.getWidth(); ++x)
		{
			std::uint8_t* const resultPixel =
					&resultPixels
					[y * result->get_rowstride()
					+ x * bytesPerSample * result->get_n_channels()];
			RGBQUAD color{};
			image.getPixelColor(x, image.getHeight() - y - 1, &color);
			resultPixel[0] = color.rgbRed;
			resultPixel[1] = color.rgbGreen;
			resultPixel[2] = color.rgbBlue;
			resultPixel[3] = 0xff;
		}
	}
	return result;
}

CImgWindow::CImgWindow() :
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

CImgWindow::~CImgWindow()
{
}

RECT CImgWindow::getViewRect() const
{
    const int viewWidth = static_cast<int>(
                m_scrolledWindow.get_hadjustment()->get_page_size());
    const int viewHeight = static_cast<int>(
                m_scrolledWindow.get_vadjustment()->get_page_size());
    return {getHScrollPos(), getVScrollPos(), viewWidth, viewHeight};
}

POINT CImgWindow::ConvertDPtoLP(const POINT &p) const
{
    const RECT view = getViewRect();
    POINT lp;

    if (view.get_width() < m_fip->getWidth() * m_zoom + MARGIN * 2)
        lp.x = static_cast<int>((p.x - MARGIN) / m_zoom);
    else
        lp.x = static_cast<int>(
                    (p.x - (view.get_width() / 2 - m_fip->getWidth() / 2 * m_zoom))
                    / m_zoom);
    if (view.get_height() < m_fip->getHeight() * m_zoom + MARGIN * 2)
        lp.y = static_cast<int>((p.y - MARGIN) / m_zoom);
    else
        lp.y = static_cast<int>(
                    (p.y - (view.get_height() / 2 - m_fip->getHeight() / 2 * m_zoom))
                    / m_zoom);
    return lp;
}

POINT CImgWindow::ConvertLPtoDP(const POINT &p) const
{
    const RECT view = getViewRect();
    POINT result;

    if (view.get_width() > m_fip->getWidth() * m_zoom + MARGIN * 2)
        result.x = static_cast<int>(
                    (view.get_width() - m_fip->getWidth() * m_zoom) / 2);
    else
        result.x = MARGIN;
    if (view.get_height() > m_fip->getHeight() * m_zoom + MARGIN * 2)
        result.y = static_cast<int>(
                    (view.get_height() - m_fip->getHeight() * m_zoom) / 2);
    else
        result.y = MARGIN;
    result.x += static_cast<int>(p.x * m_zoom);
    result.y += static_cast<int>(p.y * m_zoom);
    return result;
}

double CImgWindow::GetZoom() const
{
    return m_zoom;
}

void CImgWindow::SetZoom(const double zoom)
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

void CImgWindow::Invalidate(bool erase)
{
    //		InvalidateRect(m_hWnd, NULL, erase);
}

void CImgWindow::SetImage(fipImage *pfip)
{
    m_fip = pfip;
    m_visibleRectangleSelection = false;
    m_ptSelectionStart = {};
    m_ptSelectionEnd   = {};
    CalcScrollBarRange();
}

POINT CImgWindow::GetRectangleSelectionStart() const
{
    return m_ptSelectionStart;
}

bool CImgWindow::SetRectangleSelectionStart(int x, int y, bool clamp)
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

POINT CImgWindow::GetRectangleSelectionEnd() const
{
    return m_ptSelectionEnd;
}

bool CImgWindow::SetRectangleSelectionEnd(int x, int y, bool clamp)
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

bool CImgWindow::SetRectangleSelection(int left, int top, int right, int bottom, bool clamp)
{
    SetRectangleSelectionStart(left, top, clamp);
    return SetRectangleSelectionEnd(right, bottom, clamp);
}

RECT CImgWindow::GetRectangleSelection() const
{
    int left    = (std::min)(m_ptSelectionStart.x, m_ptSelectionEnd.x);
    int top     = (std::min)(m_ptSelectionStart.y, m_ptSelectionEnd.y);
    int right   = (std::max)(m_ptSelectionStart.x, m_ptSelectionEnd.x);
    int bottom  = (std::max)(m_ptSelectionStart.y, m_ptSelectionEnd.y);
    return {left, top, right - left, bottom - top};
}

bool CImgWindow::IsRectanlgeSelectionVisible() const
{
    return m_visibleRectangleSelection;
}

void CImgWindow::DeleteRectangleSelection()
{
    m_ptSelectionStart = {};
    m_ptSelectionEnd   = {};
    m_visibleRectangleSelection = false;
}

const fipImage &CImgWindow::GetOverlappedImage() const
{
    return m_fipOverlappedImage;
}

void CImgWindow::SetOverlappedImage(const fipImage &image)
{
    m_fipOverlappedImage = image;
    m_ptOverlappedImage = {};
    m_ptOverlappedImageCursor = {};
}

void CImgWindow::StartDraggingOverlappedImage(const fipImage &image, const POINT &ptImage, const POINT &ptCursor)
{
    m_fipOverlappedImage = image;
    m_ptOverlappedImage = ptImage;
    m_ptOverlappedImageCursor = ptCursor;
}

void CImgWindow::RestartDraggingOverlappedImage(const POINT &ptCursor)
{
    m_ptOverlappedImageCursor = ptCursor;
}

void CImgWindow::DragOverlappedImage(const POINT &ptCursor)
{
    m_ptOverlappedImage.x += ptCursor.x - m_ptOverlappedImageCursor.x;
    m_ptOverlappedImage.y += ptCursor.y - m_ptOverlappedImageCursor.y;
    m_ptOverlappedImageCursor = ptCursor;
}

void CImgWindow::DeleteOverlappedImage()
{
    m_fipOverlappedImage.clear();
}

RECT CImgWindow::GetOverlappedImageRect() const
{
    return { m_ptOverlappedImage.x, m_ptOverlappedImage.y,
                static_cast<int>(m_fipOverlappedImage.getWidth()),
                static_cast<int>(m_fipOverlappedImage.getHeight()) };
}

void CImgWindow::SetOverlappedImagePosition(const POINT &pt)
{
    m_ptOverlappedImage = pt;
}

void CImgWindow::UpdateScrollBars()
{
    CalcScrollBarRange();
}

void CImgWindow::drawImage(const Cairo::RefPtr<Cairo::Context> &cr, const fipImage &image, const RECT &outputArea)
{
    Glib::RefPtr<Gdk::Pixbuf> pixbufToBeScaled = fipToGdkPixbuf(image);

    Glib::RefPtr<Gdk::Pixbuf> pixbuf = pixbufToBeScaled->scale_simple(
                outputArea.get_width(),
                outputArea.get_height(),
                Gdk::INTERP_BILINEAR);
    Gdk::Cairo::set_source_pixbuf(cr, pixbuf, outputArea.get_x(), outputArea.get_y());
    cr->paint();
}

void CImgWindow::onScroll()
{
    const auto view = getViewRect();
    m_drawingArea.queue_draw_area(
                view.get_x(), view.get_y(), view.get_width(), view.get_height());
}

bool CImgWindow::onAreaDraw(const Cairo::RefPtr<Cairo::Context> &cr)
{
    Glib::RefPtr<Gtk::StyleContext> style = get_style_context();
    const RECT view = getViewRect();

    cr->set_source_rgb(206, 215, 230);
    cr->paint();
    style->render_background(cr, 0, 0, view.get_width(), view.get_height());

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

            const POINT ptTmpLT =
                    ConvertDPtoLP({view.get_x(), view.get_y()});
            const POINT ptTmpRB = ConvertDPtoLP(
                        {view.get_x()
                         + static_cast<int>(view.get_width() + 1 * m_zoom),
                         view.get_y()
                         + static_cast<int>(view.get_height() + 1 * m_zoom)});
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
//            std::cout << "rcImg: (x, y, width, height) = (" << rcImg.get_x()
//                      << ", " << rcImg.get_y() << ", " << rcImg.get_width()
//                      << ", " << rcImg.get_height() << ")" << std::endl;
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

void CImgWindow::OnSize()
{
    CalcScrollBarRange();
}

void CImgWindow::CalcScrollBarRange()
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

void CImgWindow::DrawXorBar(const Cairo::RefPtr<Cairo::Context> &cr, int x1, int y1, int width, int height)
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

void CImgWindow::DrawXorRectangle(const Cairo::RefPtr<Cairo::Context> &cr, int left, int top, int width, int height, int lineWidth)
{
    int right = left + width;
    int bottom = top + height;
    DrawXorBar(cr, left                 , top                   , width    , lineWidth);
    DrawXorBar(cr, left                 , bottom - lineWidth + 1, width    , lineWidth);
    DrawXorBar(cr, left                 , top                   , lineWidth, height);
    DrawXorBar(cr, right - lineWidth + 1, top                   , lineWidth, height);
}

int CImgWindow::getHScrollPos() const
{
    return static_cast<int>(m_scrolledWindow.get_hadjustment()->get_value());
}

int CImgWindow::getVScrollPos() const
{
    return static_cast<int>(m_scrolledWindow.get_vadjustment()->get_value());
}
