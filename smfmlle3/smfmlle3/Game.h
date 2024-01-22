#ifndef GAME_H
#define GAME_H

#include "Entity.h"
#include "EntityManager.h"

#include <SFML/Graphics.hpp>

struct PlayerConfig {};
struct EnemyConfig {};
struct BulletConfig {};

class Game
{
private:

	sf::RenderWindow	m_window;
	EntityManager		m_entities;
	sf::Font			m_font;
	PlayerConfig		m_playerConfig;
	EnemyConfig			m_enemyConfig;
	BulletConfig		m_BulletConfig;
	sf::Clock			m_deltaClock;
	int					m_score = 0;
	int					m_currentFrame = 0;
	bool				m_running = true;
	bool				m_mainmenu = true;
	int					m_highScore = 0;
	sf::Text			m_title;
	sf::Text			m_playButton;
	sf::Text			m_exitButton;
	sf::Text			m_thighScore;
	sf::Text			m_livesleft;
	int					m_lives = 3;
	sf::Text			m_text;
	

	std::shared_ptr<Entity> m_player;
	
	int					m_lastEnemySpawnTime = 0;
	
public:

	Game();
	
	void init();
	void run();
	void setPaused(bool paused);
	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> e);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target);
	void spawnSpecialWeapon(std::shared_ptr<Entity> entity);
	void sMovement();
	void sLifespan();
	void sCollision();
	void sEnemySpawner();
	void sRender();
	void sUserInput();
	
	void mainMenu();
	void renderMenu();
	void centerText(sf::Text& text, float centerY);
	void handleButtonHover(float mouseX, float mouseY);
};

#endif 
