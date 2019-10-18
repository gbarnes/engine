#include "GfxRenderDoc.h"
#include "Vendor/RenderDoc/renderdoc_app.h"#
#include "inc_common.h"
#include "Core/Config.h"

namespace Dawn
{
	namespace RenderDoc
	{
		RENDERDOC_API_1_1_2 *rdoc_api = NULL;

		void InitRenderDocHook()
		{
			bool bShouldHook = false;
			Config::GetBool("Engine", "HookRenderdoc", &bShouldHook);
			if (!bShouldHook)
				return;

			// At init, on windows
			if (HMODULE mod =  LoadLibraryA("renderdoc.dll"))
			{
				pRENDERDOC_GetAPI RENDERDOC_GetAPI =
					(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
				int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&rdoc_api);
				assert(ret == 1);
			}
		}

		void StartFrameCapture()
		{
			if (rdoc_api) 
				rdoc_api->StartFrameCapture(NULL, NULL);
		}

		void EndFrameCapture()
		{
			if (rdoc_api)
				rdoc_api->EndFrameCapture(NULL, NULL);
		}
	}
}