#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdio.h>

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/System/Vector2.hpp>

int main()
{
	int wWidth = 1280;
	int wHeight = 720;

	sf::Font myFont;
	sf::Text mainText;
	std::vector<std::string> shapeNames;

	typedef struct CircleStr {
		sf::CircleShape shape;
		std::string name;
		sf::Vector2f speed;
		bool isDraw;
		sf::FloatRect bbox;
		sf::Text text;
	};

	typedef struct RectangleStr {
		sf::RectangleShape shape;
		std::string name;
		sf::Vector2f speed;
		bool isDraw;
		sf::FloatRect bbox;
		sf::Text text;
	};

	std::vector<CircleStr*> circles;
	std::vector<RectangleStr*> rectangles;

	std::fstream configFile;
	configFile.open("config.txt", std::ios::in);

	// Get window size, font type and shapes info from configuration file
	if (configFile.is_open()) {
		size_t i = 0;
		std::string line;
		while (std::getline(configFile, line)) {
			std::stringstream ss(line);

			std::string word;
			while (ss >> word) {
				if (word == "Window") {
					ss >> word;
					wWidth = std::stoi(word);

					ss >> word;
					wHeight = std::stoi(word);
				}

				if (word == "Font") {
					ss >> word;
					if (!myFont.loadFromFile(word)) {
						std::cerr << "Could not load font!\n";
						exit(-1);
					}
					ss >> word;
					mainText.setFont(myFont);
					mainText.setString("Shapes");
					mainText.setCharacterSize(std::stoi(word));

					ss >> word;
					int r = std::stoi(word);

					ss >> word;
					int g = std::stoi(word);

					ss >> word;
					int b = std::stoi(word);

					mainText.setFillColor(sf::Color(r, g, b));
					mainText.setPosition(0, wHeight - (float)mainText.getCharacterSize()); // piksel olarak karakter size, 24 dönüyor, font size gibi

				}

				if (word == "Circle") {
					CircleStr* circle = new CircleStr;

					ss >> word;

					circle->text.setFont(myFont);
					circle->text.setString(word);
					circle->text.setCharacterSize(12);
					circle->name = word;

					ss >> word;
					float x = std::stof(word);

					ss >> word;
					float y = std::stof(word);

					circle->shape.setPosition(sf::Vector2f(x, y));

					ss >> word;
					float speedX = std::stof(word);

					ss >> word;
					float speedY = std::stof(word);

					circle->speed = sf::Vector2f(speedX, speedY);

					ss >> word;
					int r = std::stoi(word);

					ss >> word;
					int g = std::stoi(word);

					ss >> word;
					int b = std::stoi(word);

					circle->shape.setFillColor(sf::Color(r, g, b));

					ss >> word;
					float radius = std::stof(word);


					circle->shape.setRadius(radius);
					circle->isDraw = true;
					circle->shape.setPointCount(32);


					circle->bbox.left = circle->shape.getPosition().x;
					circle->bbox.top = circle->shape.getPosition().y;
					circle->bbox.width = circle->shape.getRadius() * 2;
					circle->bbox.height = circle->shape.getRadius() * 2;

					circle->text.setPosition(circle->shape.getPosition());

					circles.push_back(circle);
				}

				if (word == "Rectangle") {
					RectangleStr* rectangle = new RectangleStr;

					ss >> word;
					rectangle->text.setFont(myFont);
					rectangle->text.setString(word);
					rectangle->text.setCharacterSize(12);
					rectangle->name = word;

					ss >> word;
					float x = std::stof(word);

					ss >> word;
					float y = std::stof(word);

					rectangle->shape.setPosition(sf::Vector2f(x, y));

					ss >> word;
					float speedX = std::stof(word);

					ss >> word;
					float speedY = std::stof(word);

					rectangle->speed = sf::Vector2f(speedX, speedY);

					ss >> word;
					int r = std::stoi(word);

					ss >> word;
					int g = std::stoi(word);

					ss >> word;
					int b = std::stoi(word);

					rectangle->shape.setFillColor(sf::Color(r, g, b));

					ss >> word;
					float width = std::stof(word);

					ss >> word;
					float height = std::stof(word);


					rectangle->shape.setSize(sf::Vector2f(width, height));
					rectangle->isDraw = true;


					rectangle->bbox.left = rectangle->shape.getPosition().x;
					rectangle->bbox.top = rectangle->shape.getPosition().y;
					rectangle->bbox.width = rectangle->shape.getSize().x;
					rectangle->bbox.height = rectangle->shape.getSize().y;

					rectangle->text.setPosition(rectangle->shape.getPosition());

					rectangles.push_back(rectangle);
				}
			}

			i++;
		}
		configFile.close();
	}


	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "COMP4300 Assignment 1!");
	window.setFramerateLimit(60);

	sf::FloatRect windowBbox(0, 0, wWidth, wHeight);

	// initialize IMGUI and create a clock used for its internal timing
	ImGui::SFML::Init(window);
	sf::Clock deltaClock;

	// scale the imgui ui by a given factor, does not affect text size
	ImGui::GetStyle().ScaleAllSizes(1.0f);

	while (window.isOpen()) {

		sf::Event event;

		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(window, event);

			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			if (event.type == sf::Event::KeyPressed)
			{
				std::cout << "Key Pressed with code = " << event.key.code << "\n";
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
				if (event.key.code == sf::Keyboard::X) {}
				{
					// reverse circle x speed
					for (auto e : circles) {
						e->speed.x *= -1;
					}
					for (auto e : rectangles) {
						e->speed.x *= -1;
					}
				}
			}
		}

		// update imgui for this freame with the time that the last frame took
		ImGui::SFML::Update(window, deltaClock.restart());

		// draw the UI

		
		shapeNames.clear();
		for (auto e : circles)
		{
			shapeNames.push_back(e->text.getString());
		}
		for (auto e : rectangles)
		{
			shapeNames.push_back(e->text.getString());
		}

		ImGui::Begin("Debug Panel");
		ImGui::Text("Parameters of Shapes");

		static const char* current_item = "";
		if (ImGui::BeginCombo("Shapes", current_item)) {
			for (int n = 0; n < 6; n++)
			{
				bool is_selected = (current_item == shapeNames.at(n)); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(shapeNames.at(n).c_str(), is_selected)) {
					current_item = shapeNames.at(n).c_str();
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)}
				}
			}
			ImGui::EndCombo();
		}

		// temporary shape variables
		float radius;
		sf::Vector2f size;
		float r;
		float g;
		float b;
		char* tempName;


		for (auto e : circles) {
			radius = e->shape.getRadius();
			r = e->shape.getFillColor().r;
			g = e->shape.getFillColor().g;
			b = e->shape.getFillColor().b;
			tempName = &(e->name[0]);

			if (e->text.getString() == current_item) {
				std::string name = "Draw " + e->text.getString();
				ImGui::Checkbox(name.c_str(), &e->isDraw);
				ImGui::PushItemWidth(450);
				ImGui::SliderFloat("Radius", &radius, 0.0f, 300.0f);
				ImGui::PushItemWidth(220);
				ImGui::SliderFloat("##", &e->speed.x, -6.0f, 6.0f);
				ImGui::SameLine();
				ImGui::SliderFloat("Velocity", &e->speed.y, -6.0f, 6.0f);
				ImGui::PushItemWidth(150);
				ImGui::SliderFloat("##r", &r, 0, 255.0f);
				ImGui::SameLine();
				ImGui::SliderFloat("##g", &g, 0, 255.0f);
				ImGui::SameLine();
				ImGui::SliderFloat("Color", &b, 0, 255.0f);
				ImGui::InputText("Name", tempName, 50);
			}

			e->shape.setRadius(radius);
			e->bbox.width = 2 * radius;
			e->bbox.height = 2 * radius;
			e->shape.setFillColor(sf::Color(r, g, b));
			e->text.setString(tempName);
			e->name = tempName;
		}

		for (auto e : rectangles) {
			size = e->shape.getSize();
			r = e->shape.getFillColor().r;
			g = e->shape.getFillColor().g;
			b = e->shape.getFillColor().b;
			tempName = &(e->name[0]);

			if (e->text.getString() == current_item) {
				std::string name = "Draw " + e->text.getString();
				ImGui::Checkbox(name.c_str(), &e->isDraw);
				ImGui::PushItemWidth(220);
				ImGui::SliderFloat("##Sizex", &size.x, 0.0f, 300.0f);
				ImGui::SameLine();
				ImGui::SliderFloat("Size", &size.y, 0.0f, 300.0f);
				ImGui::PushItemWidth(220);
				ImGui::SliderFloat("##", &e->speed.x, -6.0f, 6.0f);
				ImGui::SameLine();
				ImGui::SliderFloat("Velocity", &e->speed.y, -6.0f, 6.0f);
				ImGui::PushItemWidth(150);
				ImGui::SliderFloat("##r", &r, 0, 255.0f);
				ImGui::SameLine();
				ImGui::SliderFloat("##g", &g, 0, 255.0f);
				ImGui::SameLine();
				ImGui::SliderFloat("Color", &b, 0, 255.0f);
				ImGui::InputText("Name", tempName, 50);
			}

			e->shape.setSize(size);
			e->bbox.width = size.x;
			e->bbox.height = size.y;
			e->shape.setFillColor(sf::Color(r, g, b));
			e->text.setString(tempName);
			e->name = tempName;
		}


		ImGui::End();

		// check collision
		for (auto e : circles) {
			if (e->bbox.left < 0 || (e->bbox.left + e->bbox.width) > wWidth) {
				e->speed.x *= -1;

			}
			if (e->bbox.top < 0 || (e->bbox.top + e->bbox.height) > wHeight) {
				e->speed.y *= -1;
			}
		}

		for (auto e : rectangles) {
			if (e->bbox.left < 0 || (e->bbox.left + e->bbox.width) > wWidth) {
				e->speed.x *= -1;

			}
			if (e->bbox.top < 0 || (e->bbox.top + e->bbox.height) > wHeight) {
				e->speed.y *= -1;
			}
		}

		
		for (auto e : circles) {
			e->shape.setPosition(e->shape.getPosition().x + e->speed.x, e->shape.getPosition().y + e->speed.y);
			e->bbox.left = e->shape.getPosition().x;
			e->bbox.top = e->shape.getPosition().y;
			e->text.setPosition(e->shape.getPosition().x + e->shape.getRadius(), e->shape.getPosition().y + e->shape.getRadius());
			e->text.setOrigin(e->text.getLocalBounds().left + e->text.getLocalBounds().width / 2, e->text.getLocalBounds().top + e->text.getLocalBounds().height / 2);
		}

		for (auto e : rectangles) {
			e->shape.setPosition(e->shape.getPosition().x + e->speed.x, e->shape.getPosition().y + e->speed.y);
			e->bbox.left = e->shape.getPosition().x;
			e->bbox.top = e->shape.getPosition().y;
			e->text.setPosition(e->shape.getPosition().x + (e->shape.getSize().x / 2.0f), e->shape.getPosition().y + (e->shape.getSize().y / 2.0f));
			e->text.setOrigin(e->text.getLocalBounds().left + e->text.getLocalBounds().width / 2, e->text.getLocalBounds().top + e->text.getLocalBounds().height / 2);
		}

		// Draw 
		window.clear();

		for (auto e : circles) {
			if (e->isDraw)
			{
				window.draw(e->shape);
				window.draw(e->text);
			}
		}

		for (auto e : rectangles) {
			if (e->isDraw)
			{
				window.draw(e->shape);
				window.draw(e->text);
			}
		}

	    window.draw(mainText);

		ImGui::SFML::Render(window); // draw the ui last so it's on top;

		window.display();
	}
	return 0;
}


