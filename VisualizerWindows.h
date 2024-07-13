#pragma once
namespace VisualizerWindows
{
	void RenderUI();
	extern bool Vsync;
	extern enum class threadSorting { unknown, sorting, sorted };
	extern threadSorting sortState;
}