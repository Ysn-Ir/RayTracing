#include<iostream>
#include<vector>
#include<SFML/Graphics.hpp>
#include<random>
#include"consts.hpp"
using namespace defs;
int strsize = 0;
int resize = rsize;



//direction vector
sf::Vector2f vec;
//defining functions  
float Getrandomfloat();  // generate random float
bool getIntersection(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);  //checking intersection between two verctors

std::vector<float> scene;
//class lines
class line {
public:

	sf::Vertex lines[2];
	line(float x1, float y1, float x2, float y2) {
		lines[0].position = sf::Vector2f(x1, y1);
		lines[1].position = sf::Vector2f(x2, y2);

		updateColor();
	}
	void setpos(int i, sf::Vector2f pos) {
		lines[i].position = pos;
	}
	std::vector<sf::Vector2f> getpos() {
		std::vector<sf::Vector2f> vec = { lines[0].position,lines[1].position };
		return vec;
	}
	void updateColor() {
		// Calculate the distance from lines[0] to lines[1] (the intersection point)
		float dx = lines[1].position.x - lines[0].position.x;
		float dy = lines[1].position.y - lines[0].position.y;
		float distance = std::sqrt(dx * dx + dy * dy);

		// Define a maximum distance for full darkness (black)
		float maxDistance = w; // Adjust this value as needed
		float intensity = std::min(distance / maxDistance, 1.0f);

		// Update colors based on the distance (brighter when closer to the object)
		sf::Uint8 colorValue = static_cast<sf::Uint8>(255 * (1.0f - intensity));
		lines[0].color = sf::Color(255, 255, 255); // Start color (white)
		lines[1].color = sf::Color(colorValue, colorValue, colorValue, colorValue); // End color (gray to black)
	}
};

//object class with multiple rays
class obj {
public:
	sf::CircleShape cir;
	std::vector<line> rays;
	obj() {
		cir.setRadius(rad);
		cir.setOrigin(cir.getPosition() + sf::Vector2f(rad,rad));
	}
	void updaterays() {
		rays.clear();  // Clear existing rays before updating
		for (int i = 0; i < rsize; i++) {
			float angle = 2.f * PI * i / rsize;  // Convert loop index to angle in radians
			sf::Vector2f startPos = cir.getPosition();
			sf::Vector2f direction = sf::Vector2f(cosf(angle), sinf(angle));  // Direction vector for the ray
			sf::Vector2f endPos = startPos + direction * 100.0f;  // Define ray length (e.g., 150.0f)

			rays.push_back(line(startPos.x, startPos.y, endPos.x, endPos.y));  // Create and store the ray
		}
	}

};

//Main Function
  int main() {

  // creating the window

    sf::RenderWindow window(sf::VideoMode(w, l), "ray"); 

	// creating the obj

	obj Obj;
  //creating walls

	 line* walls[SIZE+4];
	 for (int i = 0; i < SIZE; i++) {
		 walls[i] = new line(Getrandomfloat() * w, Getrandomfloat() * l, Getrandomfloat() * w, Getrandomfloat() * l);
	 }
	 walls[SIZE + 0 ] = new line(0, 0, w, 0);
	 walls[SIZE + 1 ] = new line(w, 0, w, l);
	 walls[SIZE + 2 ] = new line(0, l, w, l);
	 walls[SIZE + 3 ] = new line(0, l, 0, 0);

      //creating vector to check the intersection between the walls and the rays

      std::vector<sf::Vector2f> pos = {};
      line ray(w / 2 - 50, l / 2, w / 2 + 50, l / 2);

//main loop
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
					break;
				}
			}
		}
	
     //temp

		//changing direction

		sf::Vector2f mpos = (sf::Vector2f)sf::Mouse::getPosition(window);
		

		//moving the object
		Obj.cir.setPosition(mpos);
		//update the rays 
		Obj.updaterays();
       //checking intersection
		for (int j = strsize; j < resize; j++) {
			float dist = INFINITY;
			bool inter = 0;
			sf::Vector2f invec = vec;
			for (int i = 0; i < SIZE+4; i++) {
				
				if (getIntersection(Obj.rays[j].getpos().at(0).x, Obj.rays[j].getpos().at(0).y, Obj.rays[j].getpos().at(1).x, Obj.rays[j].getpos().at(1).y,
					walls[i]->getpos().at(0).x, walls[i]->getpos().at(0).y, walls[i]->getpos().at(1).x, walls[i]->getpos().at(1).y)) {
					float temp = sqrtf(pow(Obj.rays[j].getpos().at(0).x - vec.x, 2) + pow(Obj.rays[j].getpos().at(0).y - vec.y, 2));
					if (dist > temp) {
						dist = temp;
						invec = vec;
					}
					inter = 1;
					
				}
				

			}
			if (inter)
				Obj.rays[j].setpos(1, invec);
			Obj.rays[j].updateColor();
		}
		//rendering
		window.clear();
		for (int i = 0; i < SIZE; i++)
			window.draw(walls[i]->lines, 2, sf::Lines);
		for (int i = strsize; i < resize; i++)
			window.draw(Obj.rays[i].lines, 2, sf::Lines);
		window.draw(Obj.cir);
		window.display();
		
	}

	for (int i = 0; i < SIZE+4; i++) {
		delete walls[i];
	}

	return 0;
}

float Getrandomfloat() {
	std::random_device rd;
	std::mt19937 gen(rd());

	// Define the range for the random float numbers

	std::uniform_real_distribution<float> dis(0.0, 1.0); // For range [0.0, 1.0]

	return dis(gen);

}
bool getIntersection(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
	float denom = (y3 - y4) * (x1 - x2) - (x4 - x3) * (y2 - y1);

	// Lines are parallel
	if (denom == 0) {
		return false;
	}	

	float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
	float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;

	// Check if the intersection point is within both line segments
		// Calculate the intersection point using parameter t
	if (u < 1 && u > 0 && t > 0) {
		vec.x = x1 + t * (x2 - x1);
		vec.y = y1 + t * (y2 - y1);
		return true;
	}
	else return 0;
}