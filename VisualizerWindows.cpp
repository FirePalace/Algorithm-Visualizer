#include "VisualizerWindows.h"
#include "imgui.h"
#include <string>
#include <random>
#include <ranges>

namespace visualizerWindows 
{ 
    std::vector<int> randomNumberVector;
    int a = 0;
    int b = 0;
    std::string sort;

    void ResetViewport(std::string sortType) {
        sort = sortType;
        a = 0;
        b = 0;
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
    void ExecuteBubbleSort() {
              
        if (randomNumberVector[a] > randomNumberVector[b]) {
            std::swap(randomNumberVector[a], randomNumberVector[b]);
        }
        
        if (b != randomNumberVector.size() -1 ){
            b++;
        }
        else {
            b = 0;
            if (a != randomNumberVector.size() - 1) {
                a++;
            }
            else {
                sort = "";
            }
        }
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
                ResetViewport("Bubble");
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
            static int thickness = 1.0;
            
            static ImVec4 colf = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
           
            const ImVec2 p = ImGui::GetCursorScreenPos();
            const ImU32 col = ImColor(colf);
            const float spacing = 4.0f;
            
            

            float x = p.x + 4.0f;
            ImGui::GetWindowSize();
            float y = p.y + ImGui::GetWindowSize().y -26;
            
            if (sort == "Bubble") {
                ExecuteBubbleSort();
            }
             
            for (int i = 0; i < randomNumberVector.size(); i++) {
                
                // Vertical line (faster than AddLine, but only handle integer thickness)
                draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y - (randomNumberVector[i] * 2)), col);
                x += spacing * 1.0f; 
            }
                                         
            //draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + 1, y + 1), col);                                      x += sz;            // Pixel (faster than AddLine)

            ImGui::PopItemWidth();
            ImGui::End();
        }
        
        

	}
}