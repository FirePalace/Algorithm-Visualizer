#include "VisualizerWindows.h"
#include "imgui.h"
#include <string>
#include <random>
#include <chrono>
#include <stack>
#include <thread>
#include <semaphore>
#include <Windows.h>

namespace VisualizerWindows
{
	std::vector<int> arr;
	int a = 0;
	int b = 0;
	int key = 0;
	std::string sort = "";

	bool Vsync = true;
	bool sorting = false;
	bool inserting = false;

	threadSorting sortState = threadSorting::unknown;

	auto start_time = std::chrono::high_resolution_clock::now();
	auto end_time = std::chrono::high_resolution_clock::now();
	std::default_random_engine rng(std::chrono::system_clock::now().time_since_epoch().count());

	std::binary_semaphore smphSignalMainToThread{ 0 }, smphSignalThreadToMain{ 0 };

	std::thread thread;

	void ResetViewport(std::string sortType, int aInt, int bInt, int keyInt) {
		sort = sortType;
		a = aInt;
		b = bInt;
		key = keyInt;
	}

	void PopulateVectorWithRandomNumbers() {
		std::random_device randomNumbers;
		std::uniform_int_distribution<> distribution(2, 202);

		if (arr.empty()) {
			for (int i = 0; i <= 200; i++) {
				arr.push_back(distribution(randomNumbers));
			}
		}
		else {
			for (int i = 0; i <= 200; i++) {
				arr[i] = distribution(randomNumbers);
			}
		}
	}

	bool ExecuteBubbleSort(std::vector<int>& arr, int& i, int& j) {
		if (i < arr.size() - 1) {
			if (j < arr.size() - i - 1) {
				if (arr[j] > arr[j + 1]) {
					std::swap(arr[j], arr[j + 1]);
				}
				j++;
			}
			else {
				j = 0;
				i++;
			}

			return true;
		}
		end_time = std::chrono::high_resolution_clock::now();
		return false;
	}
	bool ExecuteInsertionSort(std::vector<int>& arr, int& i, int& j, int& key, bool& inserting) {
		if (i < arr.size()) {
			if (!inserting) {
				key = arr[i];
				j = i - 1;
				inserting = true;
			}

			if (j >= 0 && arr[j] > key) {
				arr[j + 1] = arr[j];
				j--;
			}
			else {
				arr[j + 1] = key;
				i++;
				inserting = false;
			}
			return true;
		}
		end_time = std::chrono::high_resolution_clock::now();
		return false;
	}
	bool ExecuteSelectionSort(std::vector<int>& arr, int& i, int& j, int& minIndex) {
		int n = arr.size();
		if (i < n - 1) {
			if (j < n) {
				if (arr[j] < arr[minIndex]) {
					minIndex = j;
				}
				j++;
			}
			else {
				if (minIndex != i) {
					std::swap(arr[i], arr[minIndex]);
				}
				i++;
				j = i + 1;
				minIndex = i;
			}
			return true;
		}
		end_time = std::chrono::high_resolution_clock::now();
		return false;
	}

	float MapIntToFloat(int int_value, int int_max) {
		return static_cast<float>(int_value) / static_cast<float>(int_max);
	}

	void Merge(std::vector<int>& arr, int left, int mid, int right) {
		int n1 = mid - left + 1;
		int n2 = right - mid;

		std::vector<int> L(n1);
		std::vector<int> R(n2);

		for (int i = 0; i < n1; i++) {
			smphSignalMainToThread.acquire();
			L[i] = arr[left + i];
			smphSignalThreadToMain.release();
		}
		for (int i = 0; i < n2; i++) {
			smphSignalMainToThread.acquire();
			R[i] = arr[mid + 1 + i];
			smphSignalThreadToMain.release();
		}

		int i = 0, j = 0, k = left;

		while (i < n1 && j < n2) {
			if (L[i] <= R[j]) {
				smphSignalMainToThread.acquire();
				arr[k] = L[i];
				smphSignalThreadToMain.release();
				i++;
			}
			else {
				smphSignalMainToThread.acquire();
				arr[k] = R[j];
				smphSignalThreadToMain.release();
				j++;
			}
			k++;
		}

		while (i < n1) {
			smphSignalMainToThread.acquire();
			arr[k] = L[i];
			smphSignalThreadToMain.release();
			i++;
			k++;
		}

		while (j < n2) {
			smphSignalMainToThread.acquire();
			arr[k] = R[j];
			smphSignalThreadToMain.release();
			j++;
			k++;
		}
	}

	void InternalMergeSort(std::vector<int>& arr, int left, int right) {
		if (left < right) {
			int mid = left + (right - left) / 2;

			InternalMergeSort(arr, left, mid);
			InternalMergeSort(arr, mid + 1, right);

			Merge(arr, left, mid, right);
		}
	}

	void ExecuteMergeSort(std::vector<int>& arr, int left, int right) {
		InternalMergeSort(arr, left, right);

		sortState = threadSorting::sorted;
	}

	int PartitionQuickSort(std::vector<int>& arr, int low, int high) {
		int pivot = arr[high];
		int i = (low - 1);

		for (int j = low; j < high; j++) {
			if (arr[j] <= pivot) {
				i++;

				smphSignalMainToThread.acquire();
				std::swap(arr[i], arr[j]);
				smphSignalThreadToMain.release();
			}
		}

		smphSignalMainToThread.acquire();
		std::swap(arr[i + 1], arr[high]);
		smphSignalThreadToMain.release();

		return (i + 1);
	}

	void InternalquickSort(std::vector<int>& arr, int low, int high) {
		if (low < high) {
			int pi = PartitionQuickSort(arr, low, high);

			InternalquickSort(arr, low, pi - 1);
			InternalquickSort(arr, pi + 1, high);
		}
	}
	void ExecuteQuickSort(std::vector<int>& arr, int low, int high)
	{
		InternalquickSort(arr, low, high);

		sortState = threadSorting::sorted;
	}

	bool IsArraySorted(const std::vector<int>& arr) {
		for (size_t i = 1; i < arr.size(); ++i) {
			if (arr[i - 1] > arr[i]) {
				return false;
			}
		}
		end_time = std::chrono::high_resolution_clock::now();
		return true;
	}
	bool ExecuteBogoSort(std::vector<int>& arr, std::default_random_engine& rng) {
		if (!IsArraySorted(arr)) {
			std::shuffle(arr.begin(), arr.end(), rng);
			return true;
		}
		return false;
	}

	void Heapify(std::vector<int>& arr, int n, int i) {
		int largest = i;
		int left = 2 * i + 1;
		int right = 2 * i + 2;

		if (left < n && arr[left] > arr[largest])
			largest = left;

		if (right < n && arr[right] > arr[largest])
			largest = right;

		if (largest != i) {
			smphSignalMainToThread.acquire();
			std::swap(arr[i], arr[largest]);
			smphSignalThreadToMain.release();

			Heapify(arr, n, largest);
		}
	}

	void ExecuteHeapSort(std::vector<int>& arr) {
		int n = arr.size();

		for (int i = n / 2 - 1; i >= 0; i--)
			Heapify(arr, n, i);

		for (int i = n - 1; i >= 0; i--) {
			smphSignalMainToThread.acquire();
			std::swap(arr[0], arr[i]);
			smphSignalThreadToMain.release();

			Heapify(arr, i, 0);
		}
		sortState = threadSorting::sorted;
	}

	void ExecuteCountingSort(std::vector<int>& arr) {
		if (arr.empty()) return;

		int maxElement = *std::max_element(arr.begin(), arr.end());
		std::vector<int> count(maxElement + 1, 0);

		for (int num : arr) {
			smphSignalMainToThread.acquire();
			count[num]++;
			smphSignalThreadToMain.release();
		}

		for (size_t i = 1; i < count.size(); ++i) {
			smphSignalMainToThread.acquire();
			count[i] += count[i - 1];
			smphSignalThreadToMain.release();
		}

		std::vector<int> output(arr.size());

		for (int i = arr.size() - 1; i >= 0; --i) {
			smphSignalMainToThread.acquire();
			output[count[arr[i]] - 1] = arr[i];
			smphSignalThreadToMain.release();
			count[arr[i]]--;
		}

		for (size_t i = 0; i < arr.size(); ++i) {
			smphSignalMainToThread.acquire();
			arr[i] = output[i];
			smphSignalThreadToMain.release();
		}
		sortState = threadSorting::sorted;
	}
	void DrawRectangles() {
		static ImVec4 whitef = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		static ImVec4 redf = ImVec4(1.0f, 0, 0, 1.0f);
		// static ImVec4 yellowf = ImVec4(1.0f, 1.0f, 0, 1.0f);

		const ImU32 white = ImColor(whitef);

		const ImU32 red = ImColor(redf);
		// const ImU32 yellow = ImColor(yellowf);
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		static int thickness = 1;

		const ImVec2 p = ImGui::GetCursorScreenPos();

		const float spacing = 4.0f;

		float x = p.x + 4.0f;
		float y = p.y + ImGui::GetWindowSize().y - 26;

		for (int i = 0; i < arr.size(); i++) {
			if (i == b) {
				draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y - (arr[i] * 2)), red);
			}
			else {
				draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y - (arr[i] * 2)), white);
			}
			x += spacing * 1.0f;
		}
	}
	void ResetAllSortingAttributes() {
		sorting = false;
		sort = "";
		end_time = std::chrono::high_resolution_clock::now();
		sortState = threadSorting::unknown;
	}

	void RenderUI() {
		if (arr.empty()) {
			PopulateVectorWithRandomNumbers();
		}

		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
		//Choose an Algorithm Window
		{
			ImGui::Begin("Choose an Algorithm");
			static int clicked = 0;

			if (ImGui::Button("Bubble Sort")) {
				clicked++;
				if (clicked & 1 && sort == "")
				{
					PopulateVectorWithRandomNumbers();
					ResetViewport("Bubble", 0, 0, 0);
					start_time = std::chrono::high_resolution_clock::now();
					clicked = 0;
				}
			}

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::Button("Slow")) {
				clicked++;
				if (clicked & 1)
				{
					Vsync = true;
					clicked = 0;
				}
			}
			if (ImGui::Button("Insertion Sort")) {
				clicked++;
				if (clicked & 1 && sort == "")
				{
					PopulateVectorWithRandomNumbers();
					ResetViewport("Insertion", 1, 0, 0);
					start_time = std::chrono::high_resolution_clock::now();
					clicked = 0;
				}
			}
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::Button("Fast")) {
				clicked++;
				if (clicked & 1)
				{
					Vsync = false;
					clicked = 0;
				}
			}
			if (ImGui::Button("Selection Sort")) {
				clicked++;
				if (clicked & 1 && sort == "")
				{
					PopulateVectorWithRandomNumbers();
					ResetViewport("Selection", 0, 1, 0);
					start_time = std::chrono::high_resolution_clock::now();
					clicked = 0;
				}
			}
			if (ImGui::Button("Merge Sort")) {
				clicked++;
				if (clicked & 1 && sort == "")
				{
					PopulateVectorWithRandomNumbers();
					ResetViewport("Merge", 0, 0, 0);
					start_time = std::chrono::high_resolution_clock::now();

					clicked = 0;
				}
			}
			if (ImGui::Button("Quick Sort")) {
				clicked++;
				if (clicked & 1 && sort == "")
				{
					PopulateVectorWithRandomNumbers();
					ResetViewport("Quick", 0, 0, 0);
					start_time = std::chrono::high_resolution_clock::now();

					clicked = 0;
				}
			}
			if (ImGui::Button("Bogo Sort")) {
				clicked++;
				if (clicked & 1 && sort == "")
				{
					PopulateVectorWithRandomNumbers();
					ResetViewport("Bogo", 0, 0, 0);
					start_time = std::chrono::high_resolution_clock::now();

					clicked = 0;
				}
			}
			if (ImGui::Button("Heap Sort")) {
				clicked++;
				if (clicked & 1 && sort == "")
				{
					PopulateVectorWithRandomNumbers();
					ResetViewport("Heap", 0, 0, 0);
					start_time = std::chrono::high_resolution_clock::now();

					clicked = 0;
				}
			}
			if (ImGui::Button("Counting Sort")) {
				clicked++;
				if (clicked & 1 && sort == "")
				{
					PopulateVectorWithRandomNumbers();
					ResetViewport("Counting", 0, 0, 0);
					start_time = std::chrono::high_resolution_clock::now();
					clicked = 0;
				}
			}

			ImGui::End();
		}

		//Viewport Window
		{
			ImGui::Begin("Viewport");
			ImGui::PushItemWidth(-ImGui::GetFontSize() * 15);

			if (sort != "") {
				if (sort == "Bubble") {
					sorting = ExecuteBubbleSort(arr, a, b);
				}
				else if (sort == "Insertion") {
					sorting = ExecuteInsertionSort(arr, a, b, key, inserting);
				}
				else if (sort == "Selection") {
					sorting = ExecuteSelectionSort(arr, a, b, key);
				}
				else if (sort == "Merge") {
					int n = arr.size() - 1;

					if (!sorting && sortState == threadSorting::unknown) {
						thread = std::thread(ExecuteHeapSort, std::ref(arr));
						smphSignalMainToThread.release();

						sortState = threadSorting::sorting;
						sorting = true;
					}

					smphSignalThreadToMain.acquire();
					smphSignalMainToThread.release();
				}
				else if (sort == "Quick") {
					int n = arr.size() - 1;

					if (!sorting && sortState == threadSorting::unknown) {
						thread = std::thread(ExecuteQuickSort, std::ref(arr), 0, n);
						smphSignalMainToThread.release();

						sortState = threadSorting::sorting;
						sorting = true;
					}

					smphSignalThreadToMain.acquire();
					smphSignalMainToThread.release();
				}
				else if (sort == "Bogo") {
					sorting = ExecuteBogoSort(arr, rng);
				}
				else if (sort == "Heap") {
					int n = arr.size() - 1;

					if (!sorting && sortState == threadSorting::unknown) {
						thread = std::thread(ExecuteHeapSort, std::ref(arr));
						smphSignalMainToThread.release();

						sortState = threadSorting::sorting;
						sorting = true;
					}

					smphSignalThreadToMain.acquire();
					smphSignalMainToThread.release();
				}
				else if (sort == "Counting") {
					int n = arr.size() - 1;

					if (!sorting && sortState == threadSorting::unknown) {
						thread = std::thread(ExecuteCountingSort, std::ref(arr));
						smphSignalMainToThread.release();

						sortState = threadSorting::sorting;
						sorting = true;
					}

					smphSignalThreadToMain.acquire();
					smphSignalMainToThread.release();
				}
			}

			DrawRectangles();

			//ProgressBar
			if (sort != "Bogo") {
				float progress = MapIntToFloat(a, arr.size());

				ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
				ImGui::Text("Progress Bar");
			}
			else {
				//Panicbutton if you want to exit Bogo Sort
				static int clicked = 0;
				if (ImGui::Button("Stop this Madness")) {
					clicked++;
					if (clicked & 1)
					{
						ResetAllSortingAttributes();
						clicked = 0;
					}
				}
			}

			if (!sorting) {
				if (end_time > start_time) {
					std::chrono::duration<double> elapsed_time = end_time - start_time;
					ImGui::Text("Execution Time: %.2fs\n", elapsed_time.count());
				}

				sort = "";
			}
			if (sortState == threadSorting::sorted) {
				ResetAllSortingAttributes();
				thread.join();
			}
			ImGui::PopItemWidth();
			ImGui::End();
		}
	}
}