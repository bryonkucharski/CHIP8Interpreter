/*
Bryon Kucharski
September 5th, 2017

CHIP8 Intrepreter

main.cpp 

-handles SFML graphics window, initalizes the CHIP8 CPU, emulates clock cycles.

*/

#include <iostream>
#include <ctime>
#include <cstdlib>
#include "cpu.h"
#include <thread>
#include <bitset>
#include <sstream>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

//const variables for drawing the SFML window
const int SCALE_FACTOR = 15;
const int WIDTH = 64 * SCALE_FACTOR + (10 * SCALE_FACTOR);
const int HEIGHT = 32 * SCALE_FACTOR;

//const variables for drawing the side panel
const int BAR_WIDTH = (HEIGHT / 22);
const int COLUMN_HEIGHT = (HEIGHT / 22); //20 items to draw and 2 bars to draw


std::vector<sf::RectangleShape> pixels;
sf::SoundBuffer bf;
sf::Sound beepSound;
sf::Font font;
sf::Color LIGHT_BLUE(135, 206, 250, 255);
sf::Clock cycleTimer;


void updateDisplay(byte **disp, RenderWindow* window);
void drawTextDisplay(byte **disp);

void drawSidePanel(sf::RenderWindow * window, byte * registers, word pc, byte I, byte delay, byte sound, word stackTop);
void updateKeys(CPU *chip8);

int main() {
	

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "CHIP8");
	//window.setFramerateLimit(60);
	bf.loadFromFile("sounds/beep.wav");
	beepSound.setBuffer(bf);
	if (!font.loadFromFile("fonts/Roboto-Regular.ttf")) {
		return EXIT_SUCCESS;
	}

	CPU* cpu = new CPU;

	cpu->initCPU();

	cpu->loadROM("roms/WIPEOFF");

//	cpu->printROM();

	byte **display = cpu->getScreen();
	byte* registers;
	cycleTimer.restart();
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		//the time will vary from computer to computer. It will also vary if using debug or release SFML dlls 
		//In my testing, I found that this timer works best for the release dlls and no timer works best for the debug dlls
		if (cycleTimer.getElapsedTime().asSeconds() > (0.005)) { 
			//main cycle
			cpu->cycle();

			if (cpu->getSoundTimer()) {
				beepSound.play();
			}

			updateKeys(cpu);
			cycleTimer.restart();
		}
		//drawing
		window.clear();
		drawSidePanel(
			&window,
			cpu->getRegisters(),
			cpu->getPC(),
			cpu->getRegisterI(),
			cpu->getDelayTimer(),
			cpu->getSoundTimer(),
			cpu->getStackTop()
		);

		if (cpu->getScreenFlag()) {
			display = cpu->getScreen();
		}
		//updates and draws display
		updateDisplay(display, &window);
		
		window.display();
	}
	
	return 0;
}

//Iterates through a 2D array, draws squares where the 2d array is set to 1
void updateDisplay(byte **disp, sf::RenderWindow * window) {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 64; j++) {
			if (disp[i][j] == 1) {
				sf::RectangleShape newRect;
				newRect.setPosition(j*SCALE_FACTOR, i*SCALE_FACTOR);
				newRect.setFillColor(sf::Color::White);
				newRect.setSize(sf::Vector2f(1* SCALE_FACTOR,1* SCALE_FACTOR));
				window->draw(newRect);
			}
			
		}
		
	}
	
}

//draws 15 registers, I register, PC, delay/sound timer, and the top of the stack to the SFML window
void drawSidePanel(sf::RenderWindow * window, byte * registers,  word pc, byte I, byte delay, byte sound, word stackTop) {
/*
	//draws the outline of the sidepanel
	RectangleShape topbar;
	topbar.setPosition(64 * SCALE_FACTOR, 0);
	topbar.setSize(Vector2f(10 * SCALE_FACTOR, BAR_WIDTH));
	topbar.setFillColor(LIGHT_BLUE);
	window->draw(topbar);
	
	RectangleShape bottombar;
	bottombar.setPosition(64 * SCALE_FACTOR, HEIGHT - BAR_WIDTH);
	bottombar.setSize(Vector2f(10 * SCALE_FACTOR, BAR_WIDTH));
	bottombar.setFillColor(LIGHT_BLUE);
	window->draw(bottombar);
	
	RectangleShape leftbar;
	leftbar.setPosition(64 * SCALE_FACTOR,0);
	leftbar.setSize(Vector2f(BAR_WIDTH, HEIGHT));
	leftbar.setFillColor(LIGHT_BLUE);
	window->draw(leftbar);

	RectangleShape rightbar;
	rightbar.setPosition(WIDTH - BAR_WIDTH, 0);
	rightbar.setSize(Vector2f(BAR_WIDTH, HEIGHT));
	rightbar.setFillColor(LIGHT_BLUE);
	window->draw(rightbar);
	*/
	//15 registers
	Text text;
	text.setFont(font);
	text.setCharacterSize(1*SCALE_FACTOR);
	for (int i = 1; i < 17; i++) {
		Text text;
		text.setFont(font);
		text.setCharacterSize(1 * SCALE_FACTOR);

		std::stringstream ss;
		ss << "R" << (i - 1) << ": 0x" << hex << (int)*(registers + i);
		text.setString(ss.str());
		text.setPosition((64 * SCALE_FACTOR) + BAR_WIDTH, (i*COLUMN_HEIGHT));
		text.setFillColor(Color::Red);

		window->draw(text);
	}

	std::stringstream ss_pc, ss_I, ss_delay, ss_sound, ss_stack;
	Text text_pc, text_I, text_delay, text_sound,text_stack;

	//draw pc
	text_pc.setFont(font);
	text_pc.setCharacterSize(1 * SCALE_FACTOR);
	ss_pc << "PC: 0x" << hex << int(pc);
	text_pc.setString(ss_pc.str());
	text_pc.setPosition((64 * SCALE_FACTOR) + BAR_WIDTH, (17*COLUMN_HEIGHT));
	text_pc.setFillColor(Color::Red);
	window->draw(text_pc);

	//draw I register
	text_I.setFont(font);
	text_I.setCharacterSize(1 * SCALE_FACTOR);
	ss_I << "I: 0x" << hex << int(I);
	text_I.setString(ss_I.str());
	text_I.setPosition((64 * SCALE_FACTOR) + BAR_WIDTH, (18 * COLUMN_HEIGHT));
	text_I.setFillColor(Color::Red);
	window->draw(text_I);

	//draw stack
	text_stack.setFont(font);
	text_stack.setCharacterSize(1 * SCALE_FACTOR);
	ss_stack << "Stack: 0x" << hex << int(stackTop);
	text_stack.setString(ss_stack.str());
	text_stack.setPosition((64 * SCALE_FACTOR) + BAR_WIDTH, (19 * COLUMN_HEIGHT));
	text_stack.setFillColor(Color::Red);
	window->draw(text_stack);

	//draw delay timer
	text_delay.setFont(font);
	text_delay.setCharacterSize(1 * SCALE_FACTOR);
	ss_delay << "delay: " << int(delay);
	text_delay.setString(ss_delay.str());
	text_delay.setPosition((64 * SCALE_FACTOR) + BAR_WIDTH, (20 * COLUMN_HEIGHT));
	text_delay.setFillColor(Color::Red);
	window->draw(text_delay);

	//draw sound timer
	text_sound.setFont(font);
	text_sound.setCharacterSize(1 * SCALE_FACTOR);
	ss_sound << "sound: " << int(sound);
	text_sound.setString(ss_sound.str());
	text_sound.setPosition((64 * SCALE_FACTOR) + BAR_WIDTH, (21 * COLUMN_HEIGHT));
	text_sound.setFillColor(Color::Red);
	window->draw(text_sound);

}

//not used - displays 1 and 0s to the console. Used to debug the SFML screen
void drawTextDisplay(byte **disp) {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 64; j++) {
			cout << (int)disp[i][j] << " ";
		}
		cout << endl;
	}
}

void updateKeys(CPU *chip8) {
	chip8->updateKey(0, sf::Keyboard::isKeyPressed(sf::Keyboard::X));
	chip8->updateKey(1, sf::Keyboard::isKeyPressed(sf::Keyboard::Num1));
	chip8->updateKey(2, sf::Keyboard::isKeyPressed(sf::Keyboard::Num2));
	chip8->updateKey(3, sf::Keyboard::isKeyPressed(sf::Keyboard::Num3));
	chip8->updateKey(4, sf::Keyboard::isKeyPressed(sf::Keyboard::Q));
	chip8->updateKey(5, sf::Keyboard::isKeyPressed(sf::Keyboard::W));
	chip8->updateKey(6, sf::Keyboard::isKeyPressed(sf::Keyboard::E));
	chip8->updateKey(7, sf::Keyboard::isKeyPressed(sf::Keyboard::A));
	chip8->updateKey(8, sf::Keyboard::isKeyPressed(sf::Keyboard::S));
	chip8->updateKey(9, sf::Keyboard::isKeyPressed(sf::Keyboard::D));
	chip8->updateKey(10, sf::Keyboard::isKeyPressed(sf::Keyboard::Z));
	chip8->updateKey(11, sf::Keyboard::isKeyPressed(sf::Keyboard::C));
	chip8->updateKey(12, sf::Keyboard::isKeyPressed(sf::Keyboard::Num4));
	chip8->updateKey(13, sf::Keyboard::isKeyPressed(sf::Keyboard::R));
	chip8->updateKey(14, sf::Keyboard::isKeyPressed(sf::Keyboard::F));
	chip8->updateKey(15, sf::Keyboard::isKeyPressed(sf::Keyboard::V));
	

}