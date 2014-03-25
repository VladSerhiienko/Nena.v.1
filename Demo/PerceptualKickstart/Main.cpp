
#include "util_render.h"
#include "util_pipeline.h"

int main(int argc, char* argv [])
{
	UtilPipeline pipeline;
	pipeline.EnableImage(PXCImage::COLOR_FORMAT_RGB32);
	pipeline.EnableImage(PXCImage::COLOR_FORMAT_DEPTH);
	pipeline.Init();
	UtilRender color_render(L"Color Stream");
	UtilRender depth_render(L"Depth Stream");
	for (;;)
	{
		if (!pipeline.AcquireFrame(true)) break;
		PXCImage *color_image = pipeline.QueryImage(PXCImage::IMAGE_TYPE_COLOR);
		PXCImage *depth_image = pipeline.QueryImage(PXCImage::IMAGE_TYPE_DEPTH);
		if (!color_render.RenderFrame(color_image)) break;
		if (!depth_render.RenderFrame(depth_image)) break;

		pipeline.ReleaseFrame();
	}
	pipeline.Close();
	return 0;
}

