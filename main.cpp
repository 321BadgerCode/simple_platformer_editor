#include <iostream>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>

using namespace std;

typedef unsigned char u8;

const float PLAYER_SPEED = 200.0f;
const float JUMP_HEIGHT = 250.0f;
const float GRAVITY = 500.0f;
const float GROUND_HEIGHT = 50.0f;
const sf::Vector2f PLAYER_SIZE(50.0f, 50.0f);
const sf::Vector2f WINDOW_SIZE(800.0f, 600.0f);

enum class Direction { TOP = 0b0001, BOTTOM = 0b0010, LEFT = 0b0100, RIGHT = 0b1000 };
u8 getCollisionDirection(const sf::RectangleShape& player, const sf::RectangleShape& rectangle) {
	sf::Vector2f playerPosition = player.getPosition();
	sf::Vector2f playerSize = player.getSize();
	sf::Vector2f rectanglePosition = rectangle.getPosition();
	sf::Vector2f rectangleSize = rectangle.getSize();

	float playerTop = playerPosition.y;
	float playerBottom = playerPosition.y + playerSize.y;
	float playerLeft = playerPosition.x;
	float playerRight = playerPosition.x + playerSize.x;

	float rectangleTop = rectanglePosition.y;
	float rectangleBottom = rectanglePosition.y + rectangleSize.y;
	float rectangleLeft = rectanglePosition.x;
	float rectangleRight = rectanglePosition.x + rectangleSize.x;

	float verticalDistance = (playerTop + playerBottom) / 2 - (rectangleTop + rectangleBottom) / 2;
	float horizontalDistance = (playerLeft + playerRight) / 2 - (rectangleLeft + rectangleRight) / 2;

	float minDistanceX = (playerSize.x + rectangleSize.x) / 2;
	float minDistanceY = (playerSize.y + rectangleSize.y) / 2;

	u8 ret = 0b0000;
	if (abs(horizontalDistance) < minDistanceX && abs(verticalDistance) < minDistanceY) {
		float offsetX = minDistanceX - abs(horizontalDistance);
		float offsetY = minDistanceY - abs(verticalDistance);

		if (offsetX < offsetY) {
			if (horizontalDistance > 0) {
				ret |= (int)Direction::RIGHT;
			}
			else {
				ret |= (int)Direction::LEFT;
			}
		}
		else {
			if (verticalDistance > 0) {
				ret |= (int)Direction::TOP;
			}
			else {
				ret |= (int)Direction::BOTTOM;
			}
		}
	}

	return ret;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE.x, WINDOW_SIZE.y), "Simple Platformer Editor");

	sf::RectangleShape player(PLAYER_SIZE);
	player.setFillColor(sf::Color::Green);
	player.setPosition(WINDOW_SIZE.x / 2, WINDOW_SIZE.y - GROUND_HEIGHT - PLAYER_SIZE.y);

	sf::RectangleShape ground(sf::Vector2f(WINDOW_SIZE.x, GROUND_HEIGHT));
	ground.setFillColor(sf::Color::Blue);
	ground.setPosition(0, WINDOW_SIZE.y - GROUND_HEIGHT);

	sf::Clock clock;
	float velocityY = 0.0f;
	bool isOnGround = true;
	u8 collisionDirection = 0b0000;

	sf::Vector2f startPoint;
	vector<sf::RectangleShape> rectangles;
	bool isDragging = false;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			rectangles.push_back(sf::RectangleShape());
			startPoint = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			sf::RectangleShape& rectangle = rectangles.back();
			rectangle.setPosition(startPoint);
			rectangle.setSize(sf::Vector2f(0, 0));
			isDragging = true;
		}

		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
			sf::Vector2f endPoint = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			sf::Vector2f size = endPoint - startPoint;
			rectangles.back().setSize(size);
			isDragging = false;
		}

		float deltaTime = clock.restart().asSeconds();

		sf::Vector2f movement(0.0f, 0.0f);

		if (collisionDirection&(int)Direction::BOTTOM && (velocityY < 0 && !isOnGround)) {
			velocityY = -velocityY;
		}
		else if (collisionDirection&(int)Direction::TOP && velocityY > 0) {
			velocityY = 0.0f;
		}

		if (!(collisionDirection&(int)Direction::LEFT) && sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			movement.x -= PLAYER_SPEED * deltaTime;
		}
		if (!(collisionDirection&(int)Direction::RIGHT) && sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			movement.x += PLAYER_SPEED * deltaTime;
		}
		if ((collisionDirection&(int)Direction::TOP || isOnGround) && (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space))) {
			velocityY = -sqrt(2.0f * JUMP_HEIGHT * GRAVITY);
			isOnGround = false;
		}

		if (!isOnGround || !(collisionDirection&(int)Direction::TOP)) {
			velocityY += GRAVITY * deltaTime;
		}

		movement.y += velocityY * deltaTime;

		player.move(movement);

		if (player.getPosition().y + PLAYER_SIZE.y >= WINDOW_SIZE.y - GROUND_HEIGHT) {
			player.setPosition(player.getPosition().x, WINDOW_SIZE.y - GROUND_HEIGHT - PLAYER_SIZE.y);
			velocityY = 0.0f;
			isOnGround = true;
		}

		window.clear();
		window.draw(player);
		window.draw(ground);
		collisionDirection = 0b0000;
		for (const sf::RectangleShape& rectangle : rectangles) {
			collisionDirection |= getCollisionDirection(rectangle, player);
			window.draw(rectangle);
		}
		window.display();
	}

	return 0;
}