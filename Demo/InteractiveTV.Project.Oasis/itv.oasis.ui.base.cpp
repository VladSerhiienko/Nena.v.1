#include "app.precompiled.h"

#include "nena.bitmap.helper.h"
#include "itv.oasis.ui.base.h"

InteractiveTV::Ui::Base::Image::Image( )
{
	img_file = "";
}

void InteractiveTV::Ui::Base::Element::AcquireGlobals(
	)
{
	cross_context = &Oasis::GetForCurrentThread( )->Context;
	screen_size = &cross_context->Engine->Graphics.d3d.ActualRenderTargetSize;
	overlay = &cross_context->Engine->Graphics.d2d;
}


void InteractiveTV::Ui::Base::Element::CalculateActualPositionMetricTs(
	::Nena::Vector2 &metric
	)
{
	metric.x *= screen_size->Width * 0.5f;
	metric.y *= screen_size->Height * 0.5f;
}

void InteractiveTV::Ui::Base::Element::CalculateActualDimensionMetric(
	::Nena::Vector2 &metric
	)
{
	metric.x *= screen_size->Width;
	metric.y *= screen_size->Height;
}

void InteractiveTV::Ui::Base::Element::CalculateActualPositionMetricTs(
	::FLOAT &metric
	)
{
	metric *= min( screen_size->Width, screen_size->Height ) * 0.5f;
}

void InteractiveTV::Ui::Base::Element::CalculateActualDimensionMetric(
	::FLOAT &metric
	)
{
	metric *= min( screen_size->Width, screen_size->Height );
}

::FLOAT InteractiveTV::Ui::Base::Element::CalculateActualDistanceSquaredTo(
	Element *other
	)
{
	::D2D1::Matrix3x2F other_xform, this_xform;
	::Nena::Vector2 other_pos, this_pos;

	other->EvaluateTransformWs( other_xform );
	EvaluateTransformWs( this_xform );
	other_pos.x = other_xform._31;
	other_pos.y = other_xform._32;
	this_pos.x = this_xform._31;
	this_pos.y = this_xform._32;

	return ::Nena::Vector2::DistanceSquared(
		other_pos,
		this_pos
		);
}

::FLOAT InteractiveTV::Ui::Base::Element::CalculateActualDistanceTo(
	Element *other
	)
{
	::D2D1::Matrix3x2F other_xform, this_xform;
	::Nena::Vector2 other_pos, this_pos;

	other->EvaluateTransformWs( other_xform );
	EvaluateTransformWs( this_xform );
	other_pos.x = other_xform._31;
	other_pos.y = other_xform._32;
	this_pos.x = this_xform._31;
	this_pos.y = this_xform._32;

	return ::Nena::Vector2::Distance( 
		other_pos,
		this_pos
		);
}

void InteractiveTV::Ui::Base::Element::EvaluateTransformWs(
	::D2D1::Matrix3x2F &scene_translation
	)
{
	if ( element_ancestor )
	{
		::D2D1::Matrix3x2F parent_translation;
		element_ancestor->EvaluateTransformWs( 
			parent_translation
			);
		scene_translation = 
			::D2D1::Matrix3x2F::Rotation(
			element_angular_offset_os - element_angular_offset_ts
			) * ::D2D1::Matrix3x2F::Translation(
			element_position_ts.x * screen_size->Width * 0.5f,
			element_position_ts.y * screen_size->Height * 0.5f
			) * ::D2D1::Matrix3x2F::Rotation(
			element_angular_offset_ts
			) * parent_translation;
	}
	else scene_translation = 
		::D2D1::Matrix3x2F::Rotation(
		element_angular_offset_os
		) * ::D2D1::Matrix3x2F::Translation(
		screen_size->Width * 0.5f + element_position_ts.x * screen_size->Width * 0.5f,
		screen_size->Height * 0.5f + element_position_ts.y * screen_size->Height * 0.5f
		);
}

void InteractiveTV::Ui::Base::Element::GetDestImageSquare(
	::D2D1_RECT_F &square
	)
{
	auto dim = min(
		screen_size->Width,
		screen_size->Height
		);
	square = ::D2D1::RectF(
		element_dimensions.x * dim * -1.f,
		element_dimensions.x * dim * -1.f,
		element_dimensions.x * dim,
		element_dimensions.x * dim
		);
}

void InteractiveTV::Ui::Base::Element::GetDestImageRect(
	::D2D1_RECT_F &rect
	)
{
	rect = ::D2D1::RectF(
		element_dimensions.x * screen_size->Width * -1.f,
		element_dimensions.y * screen_size->Height * -1.f,
		element_dimensions.x * screen_size->Width,
		element_dimensions.y * screen_size->Height
		);
}

void InteractiveTV::Ui::Base::Element::GetDestEllipse(
	::D2D1_ELLIPSE &ellipse
	)
{
	ellipse = ::D2D1::Ellipse(
		D2D1::Point2F( ),
		element_dimensions.x * screen_size->Width,
		element_dimensions.y * screen_size->Height
		);
}
void InteractiveTV::Ui::Base::Element::GetDestCircle(
	::D2D1_ELLIPSE &ellipse
	)
{
	auto dim = min( 
		screen_size->Width, 
		screen_size->Height
		);
	ellipse = ::D2D1::Ellipse(
		D2D1::Point2F( ),
		element_dimensions.x * dim,
		element_dimensions.x * dim
		);
}

void InteractiveTV::Ui::Base::Element::CalculateActualPositionByOffsetAngle(
	::Nena::Vector2 const &origin,
	::Nena::Vector2 &target,
	::Nena::Vector2 const &offset,
	::FLOAT angle
	)
{
	::D2D1::Matrix3x2F 
		origin_translation,
		target_translation,
		target_transform,
		target_rotation;

	origin_translation = ::D2D1::Matrix3x2F::Translation( origin.x, origin.y );
	target_translation = ::D2D1::Matrix3x2F::Translation( offset.x, offset.y );
	target_rotation = ::D2D1::Matrix3x2F::Rotation( angle, ::D2D1::Point2F( ) );
	target_transform = target_translation * target_rotation *  origin_translation;
	target.x = target_transform._31;
	target.y = target_transform._32;
}

void InteractiveTV::Ui::Base::Element::CalculateActualPositionByOffsetXy(
	::Nena::Vector2 const &origin,
	::Nena::Vector2 &target,
	::Nena::Vector2 const &offset
	)
{
	::D2D1::Matrix3x2F
		origin_translation,
		target_translation,
		target_transform;

	origin_translation = ::D2D1::Matrix3x2F::Translation( origin.x, origin.y );
	target_translation = ::D2D1::Matrix3x2F::Translation( offset.x, offset.y );
	target_transform = origin_translation * target_translation;
	target.x = target_transform._31;
	target.y = target_transform._32;
}

void InteractiveTV::Ui::Base::Element::RestoreDeviceTransform(
	)
{
	overlay->Context->SetTransform(
		::D2D1::Matrix3x2F::Identity( )
		);
}

void InteractiveTV::Ui::Base::Image::CreateDeviceResources(
	)
{
	img_bitmap = nullptr;
	img_bitmap = ::Nena::Graphics::Resources::BitmapHelp::LoadFromFile(
		img_file
		);
}

void InteractiveTV::Ui::Base::Image::DrawFullscreen(
	)
{
	overlay->Context->DrawBitmap(
		img_bitmap.Get( ), NULL,
		img_opacity
		);
}

void InteractiveTV::Ui::Base::Image::DrawIcon(
	)
{
	::D2D1::Matrix3x2F icon_scene_translation_matrix;
	Element::EvaluateTransformWs(
		icon_scene_translation_matrix
		);
	::D2D1_RECT_F icon_desc_rect;
	Element::GetDestImageSquare(
		icon_desc_rect
		);
	overlay->Context->SetTransform(
		icon_scene_translation_matrix
		);
	overlay->Context->DrawBitmap(
		img_bitmap.Get( ),
		icon_desc_rect,
		img_opacity
		);
	Element::RestoreDeviceTransform(
		);
}

InteractiveTV::Ui::Base::Circle::Circle(
	)
{
}
void InteractiveTV::Ui::Base::Circle::SetRadius(
	::FLOAT radius
	)
{
	Element::element_dimensions.x = radius;
	Element::element_dimensions.y = radius;
}

void InteractiveTV::Ui::Base::Circle::CreateDeviceResources(
	)
{
	overlay->Context->CreateSolidColorBrush(
		circle_border_color, circle_border_brush.ReleaseAndGetAddressOf( )
		);
	overlay->Context->CreateSolidColorBrush(
		circle_fill_color, circle_fill_brush.ReleaseAndGetAddressOf( )
		);
}

void InteractiveTV::Ui::Base::Circle::DrawFillCircle(
	)
{
	::D2D1::Matrix3x2F translation;
	Element::EvaluateTransformWs(
		translation
		);
	::D2D1_ELLIPSE ellipse;
	Element::GetDestCircle(
		ellipse
		);
	::FLOAT actual_stroke_width = circle_stroke_width;
	Element::CalculateActualDimensionMetric(
		actual_stroke_width
		);
	overlay->Context->SetTransform(
		translation
		);
	overlay->Context->DrawEllipse(
		ellipse,
		circle_border_brush.Get( ),
		actual_stroke_width
		);
	overlay->Context->FillEllipse(
		ellipse,
		circle_fill_brush.Get( )
		);
	Element::RestoreDeviceTransform(
		);
}

void InteractiveTV::Ui::Base::Circle::DrawCircle(
	)
{
	::D2D1::Matrix3x2F translation;
	Element::EvaluateTransformWs(
		translation
		);
	::D2D1_ELLIPSE ellipse;
	Element::GetDestCircle(
		ellipse
		);
	overlay->Context->SetTransform(
		translation
		);
	overlay->Context->DrawEllipse(
		ellipse,
		circle_border_brush.Get( ),
		circle_stroke_width
		);
	Element::RestoreDeviceTransform(
		);
}

void InteractiveTV::Ui::Base::Circle::FillCircle(
	)
{
	::D2D1::Matrix3x2F translation;
	Element::EvaluateTransformWs(
		translation
		);
	::D2D1_ELLIPSE ellipse;
	Element::GetDestCircle(
		ellipse
		);
	overlay->Context->SetTransform(
		translation
		);
	overlay->Context->FillEllipse(
		ellipse,
		circle_fill_brush.Get( )
		);
	Element::RestoreDeviceTransform(
		);
}

void InteractiveTV::Ui::Base::Text::DrawCenter( )
{
	::D2D1::Matrix3x2F traslation;
	Element::EvaluateTransformWs(
		traslation
		);

	text_layout->GetMetrics( 
		&text_metrics 
		);

	text_origin.x = text_metrics.width * -0.5f;
	text_origin.y = text_metrics.height * -0.5f;

	overlay->Context->SetTransform( 
		traslation 
		);
	overlay->Context->DrawTextLayout(
		text_origin, 
		text_layout.Get(),
		text_brush.Get()
		);
	Element::RestoreDeviceTransform( 
		);

}

void InteractiveTV::Ui::Base::Text::DrawSnappedLeftTop( )
{
	text_layout->GetMetrics(
		&text_metrics
		);

	text_origin.x = 0;
	text_origin.y = 0;

	overlay->Context->DrawTextLayout(
		text_origin,
		text_layout.Get( ),
		text_brush.Get( )
		);
}

void InteractiveTV::Ui::Base::Text::DrawSnappedRightTop( )
{
	text_layout->GetMetrics(
		&text_metrics
		);

	text_origin.x = screen_size->Width - text_metrics.width;
	text_origin.y = 0;

	overlay->Context->DrawTextLayout(
		text_origin,
		text_layout.Get( ),
		text_brush.Get( )
		);
}

void InteractiveTV::Ui::Base::Text::DrawSnappedLeftBottom( )
{

	text_layout->GetMetrics(
		&text_metrics
		);

	text_origin.x = 0;
	text_origin.y = screen_size->Height - text_metrics.height;

	overlay->Context->DrawTextLayout(
		text_origin,
		text_layout.Get( ),
		text_brush.Get( )
		);
}

void InteractiveTV::Ui::Base::Text::DrawSnappedRightBottom( )
{

	text_layout->GetMetrics(
		&text_metrics
		);

	text_origin.x = screen_size->Width - text_metrics.width;
	text_origin.y = screen_size->Height - text_metrics.height;

	overlay->Context->DrawTextLayout(
		text_origin,
		text_layout.Get( ),
		text_brush.Get( )
		);
}

void InteractiveTV::Ui::Base::ProgressRing::CreateDeviceResources( )
{
	overlay->Context->CreateSolidColorBrush(
		progress_arc_color, progress_arc_brush.ReleaseAndGetAddressOf( )
		);
}

void InteractiveTV::Ui::Base::ProgressRing::Draw( )
{
	::D2D1_ELLIPSE circle;
	Element::GetDestCircle( circle );

	progress_value = max( progress_value, 0.0000f );
	progress_value = min( progress_value, 0.9999f );
	progress_arc_current_angle = 
		progress_value * (progress_arc_end_angle - progress_arc_start_angle)
		+ progress_arc_start_angle;

	auto progress_arc_current_angular_diff = progress_arc_current_angle - progress_arc_start_angle;
	auto progress_arc_size = progress_arc_current_angular_diff >= 180.0f
		? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL;

	::FLOAT actual_offset = element_dimensions.x;
	Element::CalculateActualDimensionMetric( 
		actual_offset 
		);
	::Nena::Vector2 local_actual_pos = element_position_ts;
	Element::CalculateActualPositionMetricTs( 
		local_actual_pos 
		);

	Element::CalculateActualPositionByOffsetAngle(
		local_actual_pos, temp_current_pos, { 0, actual_offset },
		progress_arc_current_angle
		);
	Element::CalculateActualPositionByOffsetAngle( 
		local_actual_pos, temp_start_pos, { 0, actual_offset },
		progress_arc_start_angle
		);

	overlay->D2DFactory->CreatePathGeometry(
		progress_arc_geometry.ReleaseAndGetAddressOf( )
		);
	progress_arc_geometry->Open(
		progress_arc_sink.ReleaseAndGetAddressOf( )
		);
	progress_arc_sink->SetFillMode(
		D2D1_FILL_MODE_WINDING
		);
	progress_arc_sink->BeginFigure(
		D2D1::Point2F( temp_start_pos.x, temp_start_pos.y ),
		D2D1_FIGURE_BEGIN_HOLLOW
		);
	progress_arc_segment = ::D2D1::ArcSegment(
		::D2D1::Point2F( temp_current_pos.x, temp_current_pos.y ),
		::D2D1::SizeF( circle.radiusX, circle.radiusY ),
		progress_arc_current_angular_diff,
		D2D1_SWEEP_DIRECTION_CLOCKWISE,
		progress_arc_size
		);
	progress_arc_sink->AddArc(
		progress_arc_segment
		);
	progress_arc_sink->EndFigure(
		D2D1_FIGURE_END_OPEN
		);

	if ( FAILED( progress_arc_sink->Close( ) ) ) return;
	progress_arc_sink = nullptr;

	::D2D1::Matrix3x2F traslation;
	Element::EvaluateTransformWs(
		traslation 
		);
	overlay->Context->SetTransform(
		traslation
		);
	::FLOAT actial_stroke_width = progress_arc_stroke_width;
	Element::CalculateActualDimensionMetric( 
		actial_stroke_width 
		);
	overlay->Context->DrawGeometry(
		progress_arc_geometry.Get( ),
		progress_arc_brush.Get(),
		actial_stroke_width
		);
	Element::RestoreDeviceTransform(
		);

}

void InteractiveTV::Ui::Base::SelectionRing::CreateDeviceResources( )
{
	//auto actual_radius = element_dimensions.x;
	//Element::CalculateActualDimensionMetric( actual_radius );

	//auto element_circle_length = 2.0f * DirectX::XM_PI * actual_radius;
	//auto element_circle_length_parteil = element_circle_length / 3.0f;

	selection_ring_stroke_style_props = ::D2D1::StrokeStyleProperties(
		::D2D1_CAP_STYLE_ROUND,
		::D2D1_CAP_STYLE_ROUND,
		::D2D1_CAP_STYLE_ROUND,
		::D2D1_LINE_JOIN_MITER, 
		10.0f,
		::D2D1_DASH_STYLE_DASH,
		//::D2D1_DASH_STYLE_CUSTOM,
		0.0f
		);

	overlay->D2DFactory->CreateStrokeStyle(
		selection_ring_stroke_style_props, NULL, 0u,
		selection_ring_stroke_style.ReleaseAndGetAddressOf( )
		);

	/*FLOAT dashes[ ] = 
	{ 
		element_circle_length_parteil * 0.8,
		element_circle_length_parteil * 0.2,
		element_circle_length_parteil * 0.8,
		element_circle_length_parteil * 0.2,
		element_circle_length_parteil * 0.8,
		element_circle_length_parteil * 0.2,
	};*/

	/*overlay->D2DFactory->CreateStrokeStyle(
		selection_ring_stroke_style_props, dashes, ARRAYSIZE( dashes ),
		selection_ring_stroke_style.ReleaseAndGetAddressOf( )
		);*/

}

void InteractiveTV::Ui::Base::SelectionRing::Draw( )
{
	::D2D1::Matrix3x2F translation;
	Element::EvaluateTransformWs(
		translation
		);
	::D2D1_ELLIPSE ellipse;
	Element::GetDestCircle(
		ellipse
		);
	overlay->Context->SetTransform(
		translation
		);
	overlay->Context->DrawEllipse(
		ellipse,
		selection_ring_brush.Get( ),
		selection_ring_stroke_width,
		selection_ring_stroke_style.Get( )
		);
	Element::RestoreDeviceTransform(
		);
}



