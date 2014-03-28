#include "InteractiveTV.h"


namespace Presentation
{
	void InteractiveTV::ProcessGesture(PXCGesture::Gesture *data)
	{
		if (Nena::Video::Utility::Equals(data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_LEFT))
			InteractiveTV::Get()->Talk.Msgs.push("event: gesture-swipe-left;"),
			std::cout << "> event: gesture-swipe-left (" << data->label << "/" << 
			InteractiveTV::Get()->Talk.Msgs.unsafe_size() << ")\n";
		else if (Nena::Video::Utility::Equals(data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_RIGHT))
			InteractiveTV::Get()->Talk.Msgs.push("event: gesture-swipe-right;"),
			std::cout << "> event: gesture-swipe-right (" << data->label << "/" <<
			InteractiveTV::Get()->Talk.Msgs.unsafe_size() << ")\n";
		else if (Nena::Video::Utility::Equals(data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_DOWN))
			InteractiveTV::Get()->Talk.Msgs.push("event: gesture-swipe-down;"),
			std::cout << "> event: gesture-swipe-down (" << data->label << "/" <<
			InteractiveTV::Get()->Talk.Msgs.unsafe_size() << ")\n";
		else if (Nena::Video::Utility::Equals(data->label, PXCGesture::Gesture::LABEL_NAV_SWIPE_UP))
			InteractiveTV::Get()->Talk.Msgs.push("event: gesture-swipe-up;"),
			std::cout << "> event: gesture-swipe-up (" << data->label << "/" <<
			InteractiveTV::Get()->Talk.Msgs.unsafe_size() << ")\n";
		else std::cout << "> event: (" << data->label << "/" <<
			InteractiveTV::Get()->Talk.Msgs.unsafe_size() << ")\n";
	}

	void InteractiveTV::GestureCallback::OnAlert(_In_ PXCGesture::Alert *alert)
	{
	}

	void InteractiveTV::GestureCallback::OnGesture(_In_ PXCGesture::Gesture *data)
	{
		using namespace Nena::Video;

		InteractiveTV::Get()->ProcessGesture(data);
	}

	void InteractiveTV::LaunchGestureTracking()
	{
		m_flow.run([this] { GestureTrackingRoutine(); });
	}

	void InteractiveTV::GestureTrackingRoutine()
	{
		pxcStatus sts;
		PXCSmartSPArray sps(2);
		PXCSmartArray<PXCImage> images;

		while (!concurrency::is_current_task_group_canceling() &&
			!concurrency::is_task_cancellation_requested())
		{
			sts = VideoSampler->SubmitReadTask(images.ReleaseRefs(), sps.ReleaseRef(0));
			if (Nena::Video::Utility::Failed(sts)) break;

			sts = GestureTracker.SubmitProcessTask(images, sps.ReleaseRef(1));
			if (Nena::Video::Utility::Failed(sts)) break;

			sts = sps.SynchronizeEx();
			if (Nena::Video::Utility::Failed(sts)) break;
		}
	}

	void InteractiveTV::CancelGestureTracking()
	{
		m_flow.cancel();
		m_flow.wait();
	}


}