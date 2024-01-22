#include "Game.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

Game::Game()
{
	init();

}

void Game::init()
{
	
	m_window.create(sf::VideoMode(1280, 720), "Bells Balls");
	m_window.setFramerateLimit(60);

	spawnPlayer();
	

	m_title.setFont(m_font);
	m_title.setString("Game Menu");
	m_title.setCharacterSize(40);
	m_title.setFillColor(sf::Color::White);
	centerText(m_title, m_window.getSize().y * 0.2);

	m_thighScore.setFont(m_font);
	m_thighScore.setString("High Score: " + std::to_string(m_highScore)); 
	m_thighScore.setCharacterSize(20);
	m_thighScore.setFillColor(sf::Color::White);
	centerText(m_thighScore, m_window.getSize().y * 0.4);

	m_playButton.setFont(m_font);
	m_playButton.setString("Play");
	m_playButton.setCharacterSize(30);
	m_playButton.setFillColor(sf::Color::White);
	centerText(m_playButton, m_window.getSize().y * 0.5);


	m_exitButton.setFont(m_font);
	m_exitButton.setString("Exit");
	m_exitButton.setCharacterSize(30);
	m_exitButton.setFillColor(sf::Color::White);
	centerText(m_exitButton, m_window.getSize().y * 0.6);

	m_livesleft.setFont(m_font);
	m_livesleft.setString("Lives: " + std::to_string(m_lives));
	m_livesleft.setCharacterSize(30);
	m_livesleft.setFillColor(sf::Color::White);
	centerText(m_livesleft, 10.0f);

}
void Game::run()
{
	
	
	while (m_running)
	{
		if (m_mainmenu)
		{
			mainMenu();
			renderMenu();
		}
		else
		{
			m_entities.update();
			if (m_currentFrame % 60 == 0)
			{
				sEnemySpawner();
			}
			if (m_lives <= 0)
			{
				m_lives = 3;
				m_livesleft.setString("Lives: " + std::to_string(m_lives));
				m_mainmenu = true;
				for (auto& e : m_entities.getEntities("score"))
				{
					e->cScore->score = 0;
					e->cScore->updateText();
				}
				for (auto& e : m_entities.getEntities("enemy"))
				{
					e->destroy();
				}
			}
			sLifespan();
			sMovement();
			sCollision();
			sUserInput();
			sRender();

			m_currentFrame++;
		}
	}
	
}

void Game::spawnPlayer()
{
	auto entity = m_entities.addEntity("player");
	

	entity->cTransform = std::make_shared<CTransform>(Vec2(200.0f, 200.0f), Vec2(10.0f, 10.0f),0.0f);

	entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

	entity->cInput = std::make_shared<CInput>();

	if (!m_font.loadFromFile("Resources/OpenSans-Bold.ttf")) {
		std::cerr << "Error loading font" << std::endl;
	}
	auto text = m_entities.addEntity("score");

	text->cScore = std::make_shared<CScore>(0);

	text->cScore->text.setFont(m_font);
	text->cScore->text.setCharacterSize(24);
	text->cScore->text.setPosition(10.0f, 10.0f); 
	text->cScore->text.setFillColor(sf::Color::White);
	text->cScore->updateText();

	m_player = entity;
}
void Game::spawnEnemy()
{
	auto e = m_entities.addEntity("enemy");
	

	float maxX = static_cast<float>(m_window.getSize().x - 2 * 32.0f);
	float maxY = static_cast<float>(m_window.getSize().y - 2 * 32.0f);

	
	float randomX = static_cast<float>(rand() % static_cast<int>(maxX)) + 32.0f;
	float randomY = static_cast<float>(rand() % static_cast<int>(maxY)) + 32.0f;

	float randomVX = static_cast<float>(rand() % 201 - 100) / 10.0f; 
	float randomVY = static_cast<float>(rand() % 201 - 100) / 10.0f;

	float randomR = static_cast<float>(rand() % 101) / 10.0f + 20.0f; // Generates a random float between 20 and 30

	float randomV = static_cast<float>(rand() % 51) / 10.0f + 3.0f; // Generates a random float between 3 and 8

	int randomC1 = rand() % 256;
	int randomC2 = rand() % 256;
	int randomC3 = rand() % 256;

	int randomO1 = rand() % 256;
	int randomO2 = rand() % 256;
	int randomO3 = rand() % 256;

	e->cTransform = std::make_shared<CTransform>(Vec2(randomX, randomY), Vec2(randomVX, randomVY), 0.0f);
	e->cShape = std::make_shared<CShape>(randomR, randomV, sf::Color(randomC1, randomC2, randomC3), sf::Color(randomO1, randomO2, randomO3), 4.0f);
	
	

	m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	auto b = m_entities.addEntity("bullet");
	float ex = entity->cTransform->pos.x;
	float ey = entity->cTransform->pos.y;
	float tx = target.x;
	float ty = target.y;
	Vec2 nomb = Vec2((tx - ex), (ty - ey)).norm(); // normalized bullet

	b->cTransform = std::make_shared<CTransform>(Vec2(ex,ey), nomb *= 15.0f, 0.0f);
	b->cShape = std::make_shared<CShape>(10.0f, 20, sf::Color(0, 0, 255), sf::Color(0, 255, 0), 2.0f);
	b->cLifespan = std::make_shared<CLifespan>(30);
}

void Game::sMovement()
{
	const float windowWidth = m_window.getSize().x;
	const float windowHeight = m_window.getSize().y;

	if (m_player->cInput->up)
	{
		if (m_player->cTransform->pos.y - m_player->cTransform->velocity.y >= (m_player->cShape->circle.getRadius()))  // Check upper boundary
		{
			m_player->cTransform->pos.y -= m_player->cTransform->velocity.y;
		}
	}
	if (m_player->cInput->left)
	{
		if (m_player->cTransform->pos.x - m_player->cTransform->velocity.x >= (m_player->cShape->circle.getRadius()))  // Check left boundary
		{
			m_player->cTransform->pos.x -= m_player->cTransform->velocity.x;
		}
	}
	if (m_player->cInput->down)
	{
		if (m_player->cTransform->pos.y + m_player->cTransform->velocity.y < windowHeight - (m_player->cShape->circle.getRadius()))  // Check lower boundary
		{
			m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
		}
	}
	if (m_player->cInput->right)
	{
		if (m_player->cTransform->pos.x + m_player->cTransform->velocity.x < windowWidth - (m_player->cShape->circle.getRadius()))  // Check right boundary
		{
			m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
		}
	}
	for (auto& e : m_entities.getEntities("bullet"))
	{
		e->cTransform->pos.x += e->cTransform->velocity.x;
		e->cTransform->pos.y += e->cTransform->velocity.y;
	}
	for (auto& e : m_entities.getEntities("enemy"))
	{
		e->cTransform->pos.x += e->cTransform->velocity.x;
		e->cTransform->pos.y += e->cTransform->velocity.y;
		if (e->cTransform->velocity.y < 0)
		{
			if (e->cTransform->pos.y - e->cTransform->velocity.y <= (e->cShape->circle.getRadius()))  // Check upper boundary
			{
				e->cTransform->velocity.y *= -1;
			}
		}
		if (e->cTransform->velocity.x < 0)
		{
			if (e->cTransform->pos.x - e->cTransform->velocity.x <= (e->cShape->circle.getRadius()))  // Check left boundary
			{
				e->cTransform->velocity.x *= -1;
			}
		}
		if (e->cTransform->velocity.y > 0)
		{
			if (e->cTransform->pos.y + e->cTransform->velocity.y >= windowHeight - (e->cShape->circle.getRadius())/2)  // Check lower boundary
			{
				e->cTransform->velocity.y *= -1;
			}
		}
		if (e->cTransform->velocity.x > 0)
		{
			if (e->cTransform->pos.x + e->cTransform->velocity.x >= windowWidth - (e->cShape->circle.getRadius())/2)  // Check right boundary
			{
				e->cTransform->velocity.x *= -1;
			}
		}
	}

}
void Game::sLifespan()
{
	for (auto& e : m_entities.getEntities("bullet"))
	{
		if (e->cLifespan->remaining > 0)
		{
			sf::Color fillColor = e->cShape->circle.getFillColor();
			sf::Color outColor = e->cShape->circle.getOutlineColor();
			
			int fred = fillColor.r;
			int fgreen = fillColor.g;
			int fblue = fillColor.b;

			int ored = outColor.r;
			int ogreen = outColor.g;
			int oblue = outColor.b;

			int opacity = 255 -  ((1-(static_cast<float>(e->cLifespan->remaining) /e->cLifespan->total)) * 255);
			
			e->cShape->circle.setFillColor(sf::Color(fred,fgreen,fblue, opacity));
			e->cShape->circle.setOutlineColor(sf::Color(ored,ogreen,oblue, opacity));
			e->cLifespan->remaining--;

		}
		if (e->cLifespan->remaining == 0)
		{
			e->destroy();
		}
	}
}
void Game::sCollision()
{
	for (auto& p : m_entities.getEntities("bullet"))
	{
		float playerRadius = m_player->cShape->circle.getRadius();
		float px = p->cTransform->pos.x;
		float py = p->cTransform->pos.y;
		float pr = p->cShape->circle.getRadius();
		for (auto& e : m_entities.getEntities("enemy"))
		{
			float ex = e->cTransform->pos.x;
			float ey = e->cTransform->pos.y;
			float er = e->cShape->circle.getRadius();
			if (Vec2(px, py).dist(Vec2(ex, ey)) < (er + pr) * (er + pr))
			{
				e->destroy();

				m_entities.getEntities("score")[0]->cScore->score+=100;
				m_entities.getEntities("score")[0]->cScore->updateText();

				if (m_entities.getEntities("score")[0]->cScore->score > m_highScore)
				{
					m_highScore = m_entities.getEntities("score")[0]->cScore->score;
					m_thighScore.setString("High Score: " + std::to_string(m_highScore));
				}
			}
		}
	}
	for (auto& p : m_entities.getEntities("player"))
	{
		
		float px = p->cTransform->pos.x;
		float py = p->cTransform->pos.y;
		float pr = p->cShape->circle.getRadius();
		for (auto& e : m_entities.getEntities("enemy"))
		{
			float ex = e->cTransform->pos.x;
			float ey = e->cTransform->pos.y;
			float er = e->cShape->circle.getRadius();
			if (Vec2(px, py).dist(Vec2(ex, ey)) < (er + pr) * (er + pr))
			{
				e->destroy();
				m_lives--;
				m_livesleft.setString("Lives: " + std::to_string(m_lives));

				if (m_lives == 0)
				{
					p->destroy();
				}
			}
		}
	}
}
void Game::sEnemySpawner()
{
	spawnEnemy();
}

void Game::sRender()
{
	m_window.clear();


	m_player->cShape->circle.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);

	m_player->cTransform->angle += 5.0f;
	m_player->cShape->circle.setRotation(m_player->cTransform->angle);
	
	

	for(auto&e: m_entities.getEntities("enemy"))
	{
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
		e->cTransform->angle += 5.0f;
		e->cShape->circle.setRotation(m_player->cTransform->angle);
		m_window.draw(e->cShape->circle);
	}
	for (auto& e : m_entities.getEntities("bullet"))
	{
		
			e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
			m_window.draw(e->cShape->circle);
		
	}
	for (auto& e : m_entities.getEntities("player"))
	{
		m_window.draw(m_player->cShape->circle);
	}
	m_window.draw(m_entities.getEntities("score")[0]->cScore->text);
	m_window.draw(m_livesleft);
	m_window.display();
}
void Game::mainMenu() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			m_window.close();
		}
		else if (event.type == sf::Event::MouseButtonPressed) {
			
			if (m_playButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
				
				m_mainmenu = false;
				spawnPlayer();
			}

			
			if (m_exitButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
				m_window.close(); 
			}
		}
		else if (event.type == sf::Event::MouseMoved) {
			handleButtonHover(event.mouseMove.x, event.mouseMove.y);
		}
	}
}
void Game::renderMenu()
{
	m_window.clear();

	
	m_window.draw(m_title);
	m_window.draw(m_thighScore);
	m_window.draw(m_playButton);
	m_window.draw(m_exitButton);

	m_window.display();
}
void Game::centerText(sf::Text& text, float centerY) {
	float centerX = (m_window.getSize().x - text.getGlobalBounds().width) / 2.f;
		text.setPosition(centerX, centerY);
}

void Game::handleButtonHover(float mouseX, float mouseY) {
	
	if (m_playButton.getGlobalBounds().contains(mouseX, mouseY)) {
		m_playButton.setFillColor(sf::Color::Yellow);
	}
	else {
		m_playButton.setFillColor(sf::Color::White);
	}

	 
	if (m_exitButton.getGlobalBounds().contains(mouseX, mouseY)) {
		m_exitButton.setFillColor(sf::Color::Yellow);
	}
	else {
		m_exitButton.setFillColor(sf::Color::White);
	}
}

void Game::sUserInput()
{
	sf::Event event;

	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}
		
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = true; break;
			case sf::Keyboard::A:
				m_player->cInput->left = true; break;
			case sf::Keyboard::S:
				m_player->cInput->down = true; break;
			case sf::Keyboard::D:
				m_player->cInput->right = true; break;

			default:
				break;
			}
		}
		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = false; break;
			case sf::Keyboard::A:
				m_player->cInput->left = false; break;
			case sf::Keyboard::S:
				m_player->cInput->down = false; break;
			case sf::Keyboard::D:
				m_player->cInput->right = false; break;

			default:
				break;
			}
		}
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left && m_player->isActive())
			{
				
				spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
			}
		}
	}
}
