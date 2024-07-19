#pragma once
namespace VisualizerWindows
{
	void RenderUI();
	extern bool Vsync;
	extern bool showQuitPopup;
	extern enum class threadSorting { unknown, sorting, sorted };
	extern threadSorting sortState;
}