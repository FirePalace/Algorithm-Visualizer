#include "VisualizerWindows.h"
#include "imgui.h"
#include <string>
#include <random>
#include <ranges>
#include <chrono>


namespace visualizerWindows 
{ 
    std::vector<int> randomNumberVector;
    int a = 0;
    int b = 0;
    int key = 0;
    std::string sort = "";
    
    bool sorting = false;
    bool inserting = false;

    auto start_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    
   
   
    void ResetViewport(std::string sortType, int aInt, int bInt, int keyInt) {
        sort = sortType;
        a = aInt;
        b = bInt;
        key = keyInt;
        
    }

    void PopulateVectorWithRandomNumbers() {
        std::random_device randomNumbers;
        std::uniform_int_distribution<> distribution(1, 200);

        if (randomNumberVector.empty()) {
            for (int i = 0; i <= 200; i++) {
                randomNumberVector.push_back(distribution(randomNumbers));
            }
        }
        else {
            for (int i = 0; i <= 200; i++) {
                randomNumberVector[i] = distribution(randomNumbers);
            }
        }
        
    }
    

    bool ExecuteBubbleSort(std::vector<int>& arr, int& i, int& j) {
                 
        
       if (i < arr.size() - 1) {
           if (j < arr.size() - i - 1) {
               if (arr[j] > arr[j + 1]) {
                   std::swap(arr[j], arr[j + 1]);
               }
               ++j;
           }
           else {
               j = 0;
               ++i;
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
                ++j;
            }
            else {
                if (minIndex != i) {
                    std::swap(arr[i], arr[minIndex]);
                }
                ++i;
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
      
	void RenderUI() {

        if (randomNumberVector.empty()) {
            PopulateVectorWithRandomNumbers();
        }
       
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
        //Choose an Algorithm Window
        {
                   
            ImGui::Begin("Choose an Algorithm");
            static int clicked = 0;

            if (ImGui::Button("Bubble Sort"))
                clicked++;
            if (clicked & 1)
            { 
                PopulateVectorWithRandomNumbers();
                ResetViewport("Bubble",0,0,0);
                start_time = std::chrono::high_resolution_clock::now();
                clicked = 0;
            }

            if (ImGui::Button("Insertion Sort"))
                clicked++;
            if (clicked & 1)
            {
                PopulateVectorWithRandomNumbers();
                ResetViewport("Insertion",1,0,0);
                start_time = std::chrono::high_resolution_clock::now();
                clicked = 0;
            }
            if (ImGui::Button("Selection Sort"))
                clicked++;
            if (clicked & 1)
            {
                PopulateVectorWithRandomNumbers();
                ResetViewport("Selection", 0, 1, 0);
                start_time = std::chrono::high_resolution_clock::now();
                clicked = 0;
            }
            ImGui::End();

        }

        //Viewport Window
        {
           
            ImGui::Begin("Viewport");
            ImGui::PushItemWidth(-ImGui::GetFontSize() * 15);

            ImDrawList* draw_list = ImGui::GetWindowDrawList();

           
            static float sz = 36.0f;
            static int thickness = 1;
            
            static ImVec4 whitef = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            static ImVec4 redf = ImVec4(1.0f, 0, 0, 1.0f);
            static ImVec4 yellowf = ImVec4(1.0f, 1.0f, 0, 1.0f);
           
            const ImVec2 p = ImGui::GetCursorScreenPos();
            const ImU32 white = ImColor(whitef);
            const ImU32 red = ImColor(redf);
            const ImU32 yellow = ImColor(yellowf);

            const float spacing = 4.0f;
            
            float x = p.x + 4.0f;
            float y = p.y + ImGui::GetWindowSize().y -26;
            if (sort != "") {
                if (sort == "Bubble") {
                    sorting = ExecuteBubbleSort(randomNumberVector, a, b);
                }
                else if(sort == "Insertion"){
                    sorting = ExecuteInsertionSort(randomNumberVector, a, b, key, inserting);
                }
                if (sort == "Selection") {
                    sorting = ExecuteSelectionSort(randomNumberVector, a, b, key);
                }

            }
             
            for (int i = 0; i < randomNumberVector.size(); i++) {
               
                    
                    if (i == b) {
                        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y - (randomNumberVector[i] * 2)), red);
                        
                    }
                    else {
                        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y - (randomNumberVector[i] * 2)), white);
                        
                    }    
                    x += spacing * 1.0f;
            }
           
            //ProgressBar
            float progress = mapIntToFloat(a, randomNumberVector.size());

            ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Text("Progress Bar");
            
            

            if (!sorting) {
                std::chrono::duration<double> elapsed_time = end_time - start_time;
                ImGui::Text("Execution Time: %.2fs\n", elapsed_time.count());
                sort = "";
            }
            
                         
            ImGui::PopItemWidth();
            ImGui::End();
        }
	}
}