#include "app.precompiled.h"
#include "nena.animation.h"

#include "itv.oasis.web.h"
#include "itv.oasis.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_UI_BASE_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_UI_BASE_INCLUDED__

namespace InteractiveTV
{
	namespace Ui
	{
		namespace Base
		{

			struct Element;
			struct Image;
			struct Circle;
			struct Text;
			struct ProgressRing;
			struct SelectionRing;
		}
	}
}

struct InteractiveTV::Ui::Base::Element
{
	::FLOAT element_angular_offset_os = 0.0f;
	::FLOAT element_angular_offset_ts = 0.0f;
	::Nena::Vector2 element_position_ts = { 0.0f, 0.0f };
	::Nena::Vector2 element_dimensions = { 0.0f, 0.0f };
	::Nena::Graphics::OverlayResources *overlay = nullptr;
	::InteractiveTV::Oasis::Shared *cross_context = nullptr;
	::Nena::Graphics::Resources::Size *screen_size = nullptr;
	::InteractiveTV::Ui::Base::Element *element_ancestor = nullptr;

	void AcquireGlobals( );
	void RestoreDeviceTransform( );
	void EvaluateTransformWs( ::D2D1::Matrix3x2F &scene_translation );
	void GetDestImageSquare( ::D2D1_RECT_F &dest_rect );
	void GetDestImageRect( ::D2D1_RECT_F &dest_rect );
	void GetDestCircle( ::D2D1_ELLIPSE &ellipse );
	void GetDestEllipse( ::D2D1_ELLIPSE &ellipse );
	void CalculateActualPositionMetricTs( ::Nena::Vector2 &metric );
	void CalculateActualDimensionMetric( ::Nena::Vector2 &metric );
	void CalculateActualPositionMetricTs( ::FLOAT &metric );
	void CalculateActualDimensionMetric( ::FLOAT &metric );
	::FLOAT CalculateActualDistanceSquaredTo( Element * );
	::FLOAT CalculateActualDistanceTo( Element * );

	static void CalculateActualPositionByOffsetAngle(
		::Nena::Vector2 const &origin,
		::Nena::Vector2 &target,
		::Nena::Vector2 const &offset,
		::FLOAT angle
		);
	static void CalculateActualPositionByOffsetXy(
		::Nena::Vector2 const &origin,
		::Nena::Vector2 &target,
		::Nena::Vector2 const &offset
		);
};

struct InteractiveTV::Ui::Base::Image : public Element
{
	::FLOAT img_opacity = 1.0f;
	::std::string img_file = "";
	::Nena::Graphics::Resources::Direct2DBitmap img_bitmap = nullptr;

	Image( );
	void CreateDeviceResources( );
	void DrawFullscreen( );
	void DrawIcon( );
};

struct InteractiveTV::Ui::Base::Circle : public Element
{
	::FLOAT circle_stroke_width = 0.0f;
	::FLOAT circle_stroke_opacity = 1.0f;
	::D2D1_COLOR_F circle_fill_color = ::D2D1::ColorF( ::D2D1::ColorF::Black );
	::D2D1_COLOR_F circle_border_color = ::D2D1::ColorF( ::D2D1::ColorF::Black );
	::Nena::Graphics::Resources::Direct2DSolidColorBrush circle_fill_brush = nullptr;
	::Nena::Graphics::Resources::Direct2DSolidColorBrush circle_border_brush = nullptr;

	Circle( );
	void SetRadius( ::FLOAT radius );
	void CreateDeviceResources( );
	void DrawFillCircle( );
	void DrawCircle( );
	void FillCircle( );
};

struct InteractiveTV::Ui::Base::Text : public Element
{
	::std::wstring text = L"";
	::D2D1_POINT_2F text_origin;
	::Nena::Graphics::Resources::DirectWriteTextMetrics text_metrics;
	::Nena::Graphics::Resources::DirectWriteTextFormat text_format = nullptr;
	::Nena::Graphics::Resources::DirectWriteTextLayout text_layout = nullptr;
	::Nena::Graphics::Resources::Direct2DSolidColorBrush text_brush = nullptr;

	void DrawCenter( );
	void DrawSnappedLeftTop( );
	void DrawSnappedRightTop( );
	void DrawSnappedLeftBottom( );
	void DrawSnappedRightBottom( );
};

struct InteractiveTV::Ui::Base::ProgressRing : public Element
{
	::Nena::Graphics::Resources::Direct2DSolidColorBrush progress_arc_brush = nullptr;
	::D2D1_COLOR_F progress_arc_color = ::D2D1::ColorF( ::D2D1::ColorF::Black );
	::D2D1_ARC_SEGMENT progress_arc_segment;
	::FLOAT progress_arc_stroke_width = 1.0f;
	::FLOAT progress_arc_current_angle = 0.0f;
	::FLOAT progress_arc_start_angle = 90.0f;
	::FLOAT progress_arc_end_angle = 450.0f;
	::FLOAT progress_arc_angle = 0.0f;
	::FLOAT progress_value = 0.0f;

	void CreateDeviceResources( );
	void Draw( );

private:

	::Nena::Graphics::Resources::Direct2DPathGeometry progress_arc_geometry;
	::Nena::Graphics::Resources::Direct2DGeometrySink progress_arc_sink;
	::Nena::Vector2 temp_current_pos;
	::Nena::Vector2 temp_start_pos;
	::Nena::Vector2 temp_end_pos;
};

struct InteractiveTV::Ui::Base::SelectionRing : public Element
{
	::Nena::Graphics::Resources::Direct2DSolidColorBrush selection_ring_brush = nullptr;
	::Microsoft::WRL::ComPtr<ID2D1StrokeStyle> selection_ring_stroke_style = nullptr;
	::D2D1_STROKE_STYLE_PROPERTIES selection_ring_stroke_style_props;
	::FLOAT selection_ring_stroke_opacity = 1.0f;
	::FLOAT selection_ring_stroke_width = 0.01f;

	void CreateDeviceResources( );
	void Draw( );
};

#endif //!__NENA_INTERACTIVE_TV_OASIS_UI_BASE_INCLUDED__
