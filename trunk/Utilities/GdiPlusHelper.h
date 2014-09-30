#pragma once

namespace Gdiplus
{
	class GraphicsStateHelper
	{
	private:
		TextRenderingHint _oldHint;
		SmoothingMode _oldSmoothingMode;
	public:
		GraphicsStateHelper(void) {};
		~GraphicsStateHelper(void) {};

		void SetTextRenderingHint( Graphics* g, TextRenderingHint newHint )
		{
			_oldHint = g->GetTextRenderingHint();
			g->SetTextRenderingHint(newHint);
		}

		void RestoreTextRenderingHint( Graphics* g )
		{
			g->SetTextRenderingHint(_oldHint);
		}

		void SetSmoothingMode( Graphics* g, SmoothingMode newHint )
		{
			_oldSmoothingMode = g->GetSmoothingMode();
			g->SetSmoothingMode(newHint);
		}

		void RestoreSmoothingMode( Graphics* g )
		{
			g->SetSmoothingMode(_oldSmoothingMode);
		}
	};
}

