#include <SDL.h>
#include <random>
#include <ranges>
#include <algorithm>
#include<iostream>
#undef main;

void Initialize() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());

	}
	SDL_Window* win = SDL_CreateWindow("Algorithm Visualizer",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1000, 600, 0);
}
int main() {
	std::random_device randomNumbers;
	std::uniform_int_distribution<> distribution(1, 100);
	std::vector<int> v;


	Initialize();
	
	while (true) {

	}

	return 0;
}