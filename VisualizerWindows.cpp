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
    int callEveryNFrame = 0;

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
        
        if (a != randomNumberVector.size() - 1) {
            a++;
        }
        else {
            a = 0;
            if (b != randomNumberVector.size() - 1) {
                b++;
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
            
            static ImVec4 whitef = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            static ImVec4 redf = ImVec4(1.0f, 0, 0, 1.0f);
            static ImVec4 yellowf = ImVec4(1.0f, 1.0f, 0, 1.0f);
           
            const ImVec2 p = ImGui::GetCursorScreenPos();
            const ImU32 white = ImColor(whitef);
            const ImU32 red = ImColor(redf);
            const ImU32 yellow = ImColor(yellowf);


            const float spacing = 4.0f;
            
            

            float x = p.x + 4.0f;
            ImGui::GetWindowSize();
            float y = p.y + ImGui::GetWindowSize().y -26;
            
            if (sort == "Bubble" /*&& callEveryNFrame == 60*/) {
                ExecuteBubbleSort();
            }
             
            for (int i = 0; i < randomNumberVector.size(); i++) {
               
                    if (i == a) {
                        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y - (randomNumberVector[i] * 2)), yellow);
                        x += spacing * 1.0f;
                    }
                    else if (i == b) {
                        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y - (randomNumberVector[i] * 2)), red);
                        x += spacing * 1.0f;
                    }
                    else {
                        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y - (randomNumberVector[i] * 2)), white);
                        x += spacing * 1.0f;
                    }
                
                
            }

            if (callEveryNFrame == 60) {
                
                callEveryNFrame = 0;
            }
            else {
                callEveryNFrame++;
            }
            
                                        
            ImGui::PopItemWidth();
            ImGui::End();
        }
        
        

	}
}