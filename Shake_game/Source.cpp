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
	void rotateSprites(short unsigned int, sf::Sprite); 
	sf::Sprite activeBody();

	bool thisShake(sf::Vector2f);
	sf::Vector2f getPositionFirst();
	sf::Texture textureBody;
	sf::Texture textureHead;

private:
	std::vector <sf::Sprite> shake;
	sf::Sprite head;
};

//Game.hpp

class Game
{
public:
	Game();
	void run();
	sf::RenderWindow mWindow;
	sf::Sprite mFruit;
	void counterNull();
	void eatFruit(sf::Vector2f);

private:
	void processEvents();
	void update();
	void render();
	void handlePlayerInput(sf::Keyboard::Key, bool);
	void handlePlayerAcceleration(sf::Keyboard::Key);
	void createFruit();
	sf::Sprite turnOn();

private:
	sf::Texture textureFruit;
	sf::Texture textureBack;
	sf::Font font;
	sf::Text textFPS;
	sf::Text textCounter;
	sf::Sprite background;
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
bool turn = false;

Player::Player()
{
	if (!textureBody.loadFromFile("Media/Textures/body_shake.png") || !textureHead.loadFromFile("Media/Textures/head_shake.png"))
	{
		std::cout << "Error in loading textures!" << std::endl;// error...
	}
	head.setTexture(textureHead);
	head.setTextureRect(sf::IntRect(0, 0, 30, 30));

	head.setPosition(150.f, 150.f);
	shake.push_back(head);
}

bool Player::thisShake(sf::Vector2f thisPos) {
	for (sf::Sprite b : shake)
		if (b.getPosition() == thisPos) return true;
	return false;
}

void Player::moveShake(sf::Vector2f headPos)
{	
	if (headPos.x>=810)
		headPos.x = 0;
	if (headPos.x < 0)
		headPos.x = 780;
	if (headPos.y >= 600)
		headPos.y = 0;
	if (headPos.y < 0)
		headPos.y = 570;

	if (thisShake(headPos)) {
		shake.erase(shake.begin() + 1, shake.begin() + shake.size());
		game.counterNull();
	}

	sf::Vector2f lastPos = shake.back().getPosition();//сохраняем позицию последнего элемента, чтобы в случае сьеденного фрукта создать не его месте новый элемент

	head.setPosition(headPos);
	shake.insert(shake.begin(), head);//добавляем голову в новом месте
	shake.pop_back();//удаляем хвост
	if (shake.size()>1)
		shake[1].setTexture(textureBody);

	if (headPos == game.mFruit.getPosition())
		game.eatFruit(lastPos);
}

void Player::pushElement(sf::Vector2f lastPos) {
	sf::Sprite body;
	body.setPosition(lastPos);
	body.setTexture(textureBody);
	body.setTextureRect(sf::IntRect(0, 0, 30, 30));
	shake.push_back(body);
}

void Player::renderShake() {
	for (sf::Sprite b : shake) 
		game.mWindow.draw(b);
}

sf::Sprite Player::activeBody() {
	return shake[0];
}

void Player::rotateSprites(short unsigned int direct, sf::Sprite turnSprite){
	int max_i;

	if (turn == false)
		max_i = shake.size();
	else
		for (size_t i = 0; i < shake.size(); i++)
			if (shake[i].getPosition() == turnSprite.getPosition()) {
				max_i = i + 1;
				break;
			}
			else
				max_i = shake.size();

	switch (direct)
	{
	case 1:
		for (size_t i = 0; i < max_i; i++)
			shake[i].setTextureRect(sf::IntRect(90,0,30,30));
		break;
	case 2:
		for (size_t i = 0; i < max_i; i++)
			shake[i].setTextureRect(sf::IntRect(30, 0, 30, 30));
		break;
	case 3:
		for (size_t i = 0; i < max_i; i++)
			shake[i].setTextureRect(sf::IntRect(60, 0, 30, 30));
		break;
	case 4:
		for (size_t i = 0; i < max_i; i++)
			shake[i].setTextureRect(sf::IntRect(0, 0, 30, 30));
		break;
	}
}

sf::Vector2f Player::getPositionFirst() {
	return shake[0].getPosition();
}

//Game.cpp
Game::Game()
	: mWindow(sf::VideoMode(810, 600), "Shake game")
{	
	if (!textureBack.loadFromFile("Media/Textures/grass.png"))
	{
		std::cout << "Error in loading textures!" << std::endl;// error...
	}
	textureBack.setRepeated(true);
	background.setTexture(textureBack);
	background.setTextureRect(sf::IntRect(0, 0, 810, 600));
	background.setPosition(0.f,0.f);
	textFPS.setFont(font);
	textCounter.setFont(font);
	createFruit();
}

void Game::counterNull() {
	counterFruits = 0;
}

void Game::createFruit() {
	if (!textureFruit.loadFromFile("Media/Textures/cherry.png"))
	{
		std::cout << "Error in loading textures!" << std::endl;// error...
	}
	sf::Vector2f fruitPosition;
	do {
		srand(time(NULL));
		fruitPosition.x = 30 * (rand() % 26);
		fruitPosition.y = 30 * (rand() % 20);
	} while (player.thisShake(fruitPosition));

	mFruit.setTexture(textureFruit);
	mFruit.setPosition(fruitPosition);
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
		textCounter.setCharacterSize(32);
		textCounter.setFillColor(sf::Color(200,20,180));
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

int direct;

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed) {
	mIsMovingDown = false;
	mIsMovingUp = false;
	mIsMovingLeft = false;
	mIsMovingRight = false;
	if (key == sf::Keyboard::W) {
		mIsMovingUp = isPressed;
		direct = 1;
	}
	else if (key == sf::Keyboard::S){
		mIsMovingDown = isPressed;
		direct = 2;
	}
	else if (key == sf::Keyboard::A) {
		mIsMovingLeft = isPressed;
		direct = 3;
	}
	else if (key == sf::Keyboard::D) {
		mIsMovingRight = isPressed;
		direct = 4;
	}
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


sf::Sprite Game::turnOn() {
		turn = true;
		return player.activeBody();
}

int buf;
sf::Sprite turnSprite;

void Game::update()
{
	counter--;
	if (counter==0)
	{
		sf::Vector2f movement(player.getPositionFirst());
		if (direct)
		{
			if (buf != direct)
				turnSprite = turnOn();
			buf = direct;
		}
		if (mIsMovingUp) {
			movement.y -= PlayerSpeed;
			player.moveShake(movement);
			player.rotateSprites(direct,turnSprite);
		}
		if (mIsMovingDown) {
			movement.y += PlayerSpeed;
			player.moveShake(movement);
			player.rotateSprites(direct,turnSprite);
		}
		if (mIsMovingLeft) {
			movement.x -= PlayerSpeed;
			player.moveShake(movement);
			player.rotateSprites(direct,turnSprite);
		}
		if (mIsMovingRight) {
			movement.x += PlayerSpeed;
			player.moveShake(movement);
			player.rotateSprites(direct,turnSprite);
		}
		counter = acceleration ? step/3 : step ;
	}

}

void Game::render()
{
	mWindow.clear();
	mWindow.draw(background);
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