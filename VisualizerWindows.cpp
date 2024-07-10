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
    enum class threadSorting {
        unknown, sorting, sorted
    };
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
        std::uniform_int_distribution<> distribution(1, 200);

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
    
    float mapIntToFloat(int int_value, int int_max) {
        return static_cast<float>(int_value) / static_cast<float>(int_max);
    }
    
    void merge(std::vector<int>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        std::vector<int> L(n1);
        std::vector<int> R(n2);

        for (int i = 0; i < n1; i++)
            L[i] = arr[left + i];
        for (int i = 0; i < n2; i++)
            R[i] = arr[mid + 1 + i];

        int i = 0, j = 0, k = left;

        while (i < n1 && j < n2) {
            if (L[i] <= R[j]) {
                arr[k] = L[i];
                i++;
            }
            else {
                arr[k] = R[j];
                j++;
            }
            k++;
        }

        while (i < n1) {
            arr[k] = L[i];
            i++;
            k++;
        }

        while (j < n2) {
            arr[k] = R[j];
            j++;
            k++;
        }
    }

    

    void mergeSort(std::vector<int>& arr, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;

            mergeSort(arr, left, mid);
            mergeSort(arr, mid + 1, right);

            merge(arr, left, mid, right);
        }
    }
    
    

    
    int partitionQuickSort(std::vector<int>& arr, int low, int high) {
        int pivot = arr[high]; 
        int i = (low - 1);     
        //smphSignalThreadToMain.release();
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

   

    void internalquickSort(std::vector<int>& arr, int low, int high) {
          
        if (low < high) {
           
            int pi = partitionQuickSort(arr, low, high);
            
            internalquickSort(arr, low, pi - 1);
            internalquickSort(arr, pi + 1, high);
        }
       
        
        
    }
    void quickSort(std::vector<int>& arr, int low, int high) 
    {
        internalquickSort(arr, low, high);
        OutputDebugStringW(L"Done");
        sortState = threadSorting::sorted;
    }
    
    bool isArraySorted(const std::vector<int>& arr) {
        for (size_t i = 1; i < arr.size(); ++i) {
            if (arr[i - 1] > arr[i]) {
                return false;
            }
        }
        end_time = std::chrono::high_resolution_clock::now();
        return true;
    }
    bool bogoSortStep(std::vector<int>& arr, std::default_random_engine& rng) {
        if (!isArraySorted(arr)) {
            std::shuffle(arr.begin(), arr.end(), rng);
            return true; 
        }
        return false; 
    }

    void DrawRectangles() {

        static ImVec4 whitef = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        static ImVec4 redf = ImVec4(1.0f, 0, 0, 1.0f);
      //  static ImVec4 yellowf = ImVec4(1.0f, 1.0f, 0, 1.0f);

        const ImU32 white = ImColor(whitef); 
       
        const ImU32 red = ImColor(redf); 
        // const ImU32 yellow = ImColor(yellowf);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
 
        static int thickness = 1;
      
        const ImVec2 p = ImGui::GetCursorScreenPos();
            

        const float spacing = 4.0f;
            
            
        float x = p.x + 4.0f;
        float y = p.y + ImGui::GetWindowSize().y -26;
       

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
                if (clicked & 1)
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
                if (clicked & 1)
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
                if (clicked & 1)
                {
                    PopulateVectorWithRandomNumbers();
                    ResetViewport("Selection", 0, 1, 0);
                    start_time = std::chrono::high_resolution_clock::now();
                    clicked = 0;
                }
            }
            if (ImGui::Button("Merge Sort")) {
                clicked++;
                if (clicked & 1)
                {
                    PopulateVectorWithRandomNumbers();
                    ResetViewport("Merge", 0, 0, 0);
                    start_time = std::chrono::high_resolution_clock::now();
                   
                    clicked = 0;
                }
            }
            if (ImGui::Button("Quick Sort")) {
                clicked++;
                if (clicked & 1)
                {
                    PopulateVectorWithRandomNumbers();
                    ResetViewport("Quick", 0, 0, 0);
                    start_time = std::chrono::high_resolution_clock::now();

                    clicked = 0;
                }
            }
            if (ImGui::Button("Bogo Sort")) {
                clicked++;
                if (clicked & 1)
                {
                    PopulateVectorWithRandomNumbers();
                    ResetViewport("Bogo", 0, 0, 0);
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
                else if(sort == "Insertion"){
                    sorting = ExecuteInsertionSort(arr, a, b, key, inserting);
                }
                else if (sort == "Selection") {
                    sorting = ExecuteSelectionSort(arr, a, b, key);
                }
                else if (sort == "Merge") {
                    
                    mergeSort(arr, 0, arr.size() - 1);
                    sorting = false;
                    
                }
                else if (sort == "Quick") {
                    int n = arr.size() - 1;
                    
                    
                    if (!sorting && sortState == threadSorting::unknown) {
                        
                        thread = std::thread(quickSort, std::ref(arr), 0, n);
                        smphSignalMainToThread.release();
                        
                        sortState = threadSorting::sorting;
                        sorting = true;
                    }
              
                   smphSignalThreadToMain.acquire();
                   smphSignalMainToThread.release();
                }
                else if (sort == "Bogo") {
                    sorting = bogoSortStep(arr, rng);
                }

            }
      
            DrawRectangles();

           
            //ProgressBar
            if (sort != "Bogo") {
                float progress = mapIntToFloat(a, arr.size());

                ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
                ImGui::Text("Progress Bar");
            }

            if (!sorting) {

                if (end_time > start_time) {
                    std::chrono::duration<double> elapsed_time = end_time - start_time;
                    ImGui::Text("Execution Time: %.2fs\n", elapsed_time.count());
                }

                sort = "";
            }
               
            ImGui::PopItemWidth();
            ImGui::End();
            
            if (sortState == threadSorting::sorted) {
                sorting = false;
                sort = "";
                sortState = threadSorting::unknown;
                      
                thread.join();

            }
        }
	}
    
}