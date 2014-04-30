#include "app.precompiled.h"
#include "itv.oasis.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_WEB_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_WEB_INCLUDED__

namespace InteractiveTV
{
	namespace Project
	{
		struct Oasis::Web : public Oasis::Object
		{
			struct AppBase
			{
				typedef ::std::vector<Oasis::Web::AppBase *> List;
				friend ::InteractiveTV::Project::Oasis::Web;
				friend ::InteractiveTV::Project::Oasis;

				AppBase(Oasis::Web::AppBase *master);
				virtual ~AppBase();

				void CreateCoreObjects();
				void DestroyCoreObjects();
				void SetName(Oasis::String name);

				int Width = 0, Height = 0;
				Awesomium::WebView *View = nullptr;
				Awesomium::WebSession *Session = nullptr;
				Awesomium::WebPreferences SessionPreferencies;

				Oasis::Web::AppBase *Master = nullptr;
			};

			Web();
			~Web();
			void Init();
			void Quit();
			void OnFrameMove();

		private:

			friend InteractiveTV::Project::Oasis;
			friend InteractiveTV::Project::Oasis::Web::AppBase;
			static Oasis::Web *GetForCurrentThread();

			Oasis::Web::AppBase::List m_states;
			Awesomium::WebCore *m_core = nullptr;
			Awesomium::WebConfig m_core_configuration;

		private:

			static Oasis::Web::AppBase::List s_kids;
		};
	}
}

#endif // !__NENA_INTERACTIVE_TV_OASIS_WEB_INCLUDED__
