#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include <cstdlib> // srand и rand
#include <ctime> // содержит time()
#include <vector>


class Player
{
public:
	Player();
	void pushElement(sf::Vector2f);
	void moveShake(sf::Vector2f);
	void renderShake();
	bool thisShake(sf::Vector2f);
	sf::Vector2f getPositionFirst();

private:
	std::vector <sf::RectangleShape> shake;
	sf::RectangleShape head;
};


//Game.hpp

class Game
{
public:
	Game();
	void run();
	sf::RenderWindow mWindow;
	sf::CircleShape mFruit;
	void counterNull();
	void eatFruit(sf::Vector2f);

private:
	void processEvents();
	void update();
	void render();
	void handlePlayerInput(sf::Keyboard::Key, bool);
	void handlePlayerAcceleration(sf::Keyboard::Key);
	void createFruit();

private:
	sf::Font font;
	sf::Text textFPS;
	sf::Text textCounter;
	Player player;
	const int step = 18;
	int counter = step;
	int counterFruits = 0;
	bool mIsMovingUp = false;
	bool mIsMovingDown = false;
	bool mIsMovingLeft = false;
	bool mIsMovingRight = false;
	bool acceleration = false;
	const float PlayerSpeed = 30;
	const sf::Time TimePerFrame = sf::seconds(1.f / 60.f);
};

Game game;

Player::Player()
{
	head.setSize(sf::Vector2f(30, 30));
	head.setFillColor(sf::Color::Green);
	head.setPosition(150.f, 150.f);
	shake.push_back(head);
}

bool Player::thisShake(sf::Vector2f thisPos) {
	for (sf::RectangleShape b : shake)
		if (b.getPosition() == thisPos) return true;
	return false;
}

void Player::moveShake(sf::Vector2f headPos)
{	
	if (headPos.x>=840)
		headPos.x = 0;
	if (headPos.x < 0)
		headPos.x = 810;
	if (headPos.y >= 600)
		headPos.y = 0;
	if (headPos.y < 0)
		headPos.y = 570;

	if (thisShake(headPos))
		shake.erase(shake.begin()+1,shake.begin()+shake.size());

	sf::Vector2f lastPos = shake.back().getPosition();//сохраняем позицию последнего элемента, чтобы в случае сьеденного фрукта создать не его месте новый элемент

	head.setPosition(headPos);
	shake.insert(shake.begin(), head);//добавляем голову в новом месте
	shake.pop_back();//удаляем хвост
	if (shake.size()>1)
	{
		shake[1].setOutlineThickness(2);
		shake[1].setOutlineColor(sf::Color(250, 150, 100));
	}
	if (headPos == game.mFruit.getPosition())
		game.eatFruit(lastPos);
}

void Player::pushElement(sf::Vector2f lastPos) {
	sf::RectangleShape body;
	body.setSize(sf::Vector2f(30, 30));
	body.setPosition(lastPos);
	body.setFillColor(sf::Color::Green);
	body.setOutlineThickness(2);
	body.setOutlineColor(sf::Color(250, 150, 100));
	shake.push_back(body);
}

void Player::renderShake() {
	for (sf::RectangleShape b : shake) 
		game.mWindow.draw(b);
}

sf::Vector2f Player::getPositionFirst() {
	return shake[0].getPosition();
}


//Game.cpp
Game::Game()
	: mWindow(sf::VideoMode(810, 600), "Shake game")
{
	textFPS.setFont(font);
	textCounter.setFont(font);
	createFruit();
}

void Game::counterNull() {
	counterFruits = 0;
}

void Game::createFruit() {
	sf::Vector2f fruitPosition;
	do {
		srand(time(NULL));
		fruitPosition.x = 30 * (rand() % 26);
		fruitPosition.y = 30 * (rand() % 20);
	} while (player.thisShake(fruitPosition));

	mFruit.setRadius(15.f);
	mFruit.setPosition(fruitPosition);
	mFruit.setFillColor(sf::Color::Red);
}

void Game::eatFruit(sf::Vector2f lastPos) {
	createFruit();
	counterFruits++;
	player.pushElement(lastPos);
	mWindow.draw(mFruit);
}

void Game::run() {
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	if (!font.loadFromFile("Media/Fonts/13741.otf"))
	{
		std::cout << "Error in loading font!" << std::endl;// error...
	}

	while (mWindow.isOpen())
	{
		processEvents();
		timeSinceLastUpdate += clock.restart();
		std::stringstream stream;
		stream << std::fixed << std::setprecision(0) << 1 / timeSinceLastUpdate.asSeconds();//Вычисляем фпс, в setprecision устанавливаем количество знаков после запятой
		std::string fps = stream.str();
		textFPS.setString(fps);
		textFPS.setCharacterSize(24);
		textFPS.setFillColor(sf::Color::Magenta);
		std::string fruits = std::to_string(counterFruits);
		textCounter.setString(fruits);
		textCounter.setCharacterSize(24);
		textCounter.setFillColor(sf::Color::Yellow);
		textCounter.setPosition(750,0);

		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;
			processEvents();
			update();
		}
		render();
	}
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed) {
	mIsMovingDown = false;
	mIsMovingUp = false;
	mIsMovingLeft = false;
	mIsMovingRight = false;
	if (key == sf::Keyboard::W)
		mIsMovingUp = isPressed;
	else if (key == sf::Keyboard::S)
		mIsMovingDown = isPressed;
	else if (key == sf::Keyboard::A)
		mIsMovingLeft = isPressed;
	else if (key == sf::Keyboard::D)
		mIsMovingRight = isPressed;
	acceleration = true;
}

void Game::handlePlayerAcceleration(sf::Keyboard::Key key) {
	acceleration = false;
}

void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;
		case sf::Event::KeyReleased:
			handlePlayerAcceleration(event.key.code);
			break;
		case sf::Event::Closed:
			mWindow.close();
			break;
		}
	}
}

void Game::update()
{
	sf::Vector2f movement(player.getPositionFirst());
	counter--;
	if (counter==0)
	{
		if (mIsMovingUp) {
			movement.y -= PlayerSpeed;
			player.moveShake(movement);
		}
		if (mIsMovingDown) {
			movement.y += PlayerSpeed;
			player.moveShake(movement);
		}
		if (mIsMovingLeft) {
			movement.x -= PlayerSpeed;
			player.moveShake(movement);
		}
		if (mIsMovingRight) {
			movement.x += PlayerSpeed;
			player.moveShake(movement);
		}
		counter = acceleration ? step/3 : step ;
	}

}

void Game::render()
{
	mWindow.clear();
	player.renderShake();
	mWindow.draw(mFruit);
	mWindow.draw(textFPS);
	mWindow.draw(textCounter);
	mWindow.display();
}

int main()
{
	game.run();
}